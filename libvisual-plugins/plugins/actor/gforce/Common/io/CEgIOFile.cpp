#include "CEgIOFile.h"


#include "CEgFileSpec.h"


long CEgIOFile::sCreatorType = 0x3f3f3f3f;


CEgIOFile::CEgIOFile( int inDoTrunc, long inOBufSize ) :
	mDoTrunc( inDoTrunc ) {
	mDoTrunc = inDoTrunc;
	mOBufSize = inOBufSize;

	if ( mOBufSize < 100 )
		mOBufSize = 100;
}




CEgIOFile::~CEgIOFile() {

	close();
}



#ifdef EG_MAC

#include <Files.h>
#include <Errors.h>

#define 	__OSWOpen( specPtr )			short int refNum;																									\
											short err = ::FSpCreate( (FSSpec*) inSpecPtr -> OSSpec(), sCreatorType, inSpecPtr -> GetType(), -1 );				\
											if ( err != ::noErr && err != dupFNErr ) {																			\
												throwErr( cCreateErr );																							\
												CEgOStream::mOSErr = err; }																						\
											else {																												\
												CEgOStream::mOSErr = ::FSpOpenDF( (FSSpec*) specPtr -> OSSpec(), fsRdWrPerm, &refNum );							\
												if ( CEgOStream::mOSErr == ::noErr )																			\
													mFile = refNum;																								\
											}


#define 	__OSWrite( srcePtr, ioBytes )	CEgOStream::mOSErr = ::FSWrite( mFile, &ioBytes, srcePtr );			\
											if ( CEgOStream::mOSErr != ::noErr ) 								\
												throwErr( cWriteErr );




#endif









#if defined(EG_WIN16) || defined(UNIX_X)

#include <stdio.h>

#define 	__OSWOpen( specPtr )			mFile = (long) fopen( (char*) (specPtr -> OSSpec()), "w+b" );

#define 	__OSWrite( srcePtr, ioBytes )	unsigned long int wrote = fwrite( srcePtr, 1, ioBytes, (FILE*) mFile );				\
											if ( wrote > 0 || ioBytes == 0 )										\
												ioBytes = wrote;													\
											else {																	\
												throwErr( cWriteErr );												\
											}
#endif





#ifdef EG_WIN32

#ifndef EG_ZINC
#include <time.h>		// Otherwise a weird compile err deep in windows.h occurs
#endif
#include <windows.h>



#define 	__OSWOpen( specPtr )			mFile = (long) ::CreateFile( (char*) (specPtr -> OSSpec()), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0 );	\
											if ( ((void*) mFile) == INVALID_HANDLE_VALUE ) {																		\
												mFile = 0;																										\
												mOSErr = ::GetLastError();																							\
											}



#define 	__OSWrite( srcePtr, ioBytes )	DWORD wrote; 															\
											bool ok = ::WriteFile( (void*) mFile, srcePtr, ioBytes, &wrote, 0 );	\
											if ( ! ok || ioBytes != wrote )	{										\
												throwErr( cWriteErr );												\
												mOSErr = ::GetLastError();											\
											}
#endif



void CEgIOFile::open( const CEgFileSpec* inSpecPtr ) {

	close();
	throwErr( cNoErr );

	if ( inSpecPtr ) {
		if ( mDoTrunc )
			inSpecPtr -> Delete();

		__OSWOpen( inSpecPtr )
	}

	if ( mFile == 0 ) {
		#if EG_MAC
		if ( mOSErr == fnfErr )
		#elif defined(UNIX_X)
		  if (0)   /* FIXME */
		#else
		if ( mOSErr == ERROR_FILE_NOT_FOUND )
		#endif
			throwErr( cFileNotFound );
		else
			throwErr( cOpenErr );
	}
}






void CEgIOFile::PutBlock( const void* inSrce, long numBytes ) {

	CEgIFile::skip( numBytes );										// Keep mPos up to date

	// If we don't want to exceed our buffer limit...
	if ( numBytes + (long) mOBuf.length() > mOBufSize ) {			// Uh oh, we actually  have to write to disk
		// Get rid of what we have waiting first
		flush();

		if ( numBytes > mOBufSize /4  && noErr() ) {
			__OSWrite( inSrce, numBytes ) }
		else
			CEgOStream::PutBlock( inSrce, numBytes ); }
	else
		CEgOStream::PutBlock( inSrce, numBytes );
}


/*

void CEgIOFile::PutBlock( const void* inSrce, long numBytes ) {

	CEgIFile::skip( numBytes );										// Keep mPos up to date
	if ( numBytes + (long) mOBuf.length() >= cMaxOBufSize )			// Uh oh, we actually  have to write to disk
		flush();													// Dump our buf to disk

	while ( numBytes > cMaxOBufSize ) {								// We do this loop crap because to avoid code duplication/maintainance and
		PutBlock( inSrce, cMaxOBufSize );							// to avoid giving OSWrite a 0 srce ptr (which is perfectly valid from a skip call)
		if ( inSrce )
			inSrce = (char*) inSrce + cMaxOBufSize;
		numBytes -= cMaxOBufSize;
	}

	CEgOStream::PutBlock( inSrce, numBytes );
}
*/






void CEgIOFile::flush() {
	long ioBytes = mOBuf.length();

	if ( ! mFile )
		throwErr( cNotOpen );
	else if ( ioBytes > 0 && noErr() ) {
		__OSWrite( mOBuf.getCStr(), ioBytes )
		if ( noErr() ) {
			invalidateBuf();							// Invalidate read buffer
			mOBuf.Wipe();								// We're done with the out buffer
		}
	}
}




long CEgIOFile::size() {
	flush();

	return CEgIFile::size();
}



void CEgIOFile::seek( long inPos ) {

	if ( noErr() ) {
		flush();										// Write any pending data

		if ( noErr() ) {
			CEgIFile::seek( inPos );
			diskSeek( inPos );
		}
	}
}






void CEgIOFile::close() {

	if ( is_open() ) {
		flush();

		CEgIFile::close();
	}
}



