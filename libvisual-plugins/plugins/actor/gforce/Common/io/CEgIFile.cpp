#include "CEgIFile.h"

#include "CEgFileSpec.h"


#include "EgOSUtils.h"






#ifdef EG_MAC

#include <Files.h>
#include <Errors.h>

#define __OSROpen( specPtr )			short int refNum;																\
										mOSErr = ::FSpOpenDF( (FSSpec*) specPtr -> OSSpec(), fsRdPerm, &refNum );		\
										if ( mOSErr == ::noErr )														\
											mFile = refNum;

#define	__OSClose						mOSErr = ::FSClose( mFile );													\
										if ( mOSErr != ::noErr )														\
											throwErr( cCloseErr );


#define __OSRead( destPtr, ioBytes )	OSErr err = ::FSRead( mFile, &ioBytes, destPtr );								\
										if ( err != ::noErr && err != eofErr ) {										\
											throwErr( cReadErr );														\
											mOSErr = err;																\
										}
#endif





#if defined(EG_WIN16) || defined(UNIX_X)

#include <stdio.h>

#define __OSROpen( specPtr )			mFile = (long) fopen( (char*) (specPtr -> OSSpec()), "rb" );			\


#define	__OSClose						if ( fclose( (FILE*) mFile ) != 0 ) {									\
											throwErr( cCloseErr );																							\
										}


#define __OSRead( destPtr, ioBytes )	unsigned long outRead = fread( destPtr, 1, ioBytes, (FILE*) mFile );			\
										if ( outRead > 0 || ioBytes == 0 ) 										\
											ioBytes = outRead;													\
										else {																	\
        ioBytes = 0; \
	if (feof((FILE *) mFile)) { \
	   clearerr((FILE *) mFile);  \
        } else if (ferror((FILE *) mFile)) { \
	   clearerr((FILE *) mFile);							   throwErr( cReadErr ); \
	} \
      }
#endif




#ifdef EG_WIN32


#ifndef EG_ZINC
#include <time.h>		// Otherwise a weird compile err deep in windows.h occurs
#endif
#include <windows.h>


#define __OSROpen( specPtr )			mFile = (long) ::CreateFile( (char*) (specPtr -> OSSpec()), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );	\
										if ( ((void*) mFile) == INVALID_HANDLE_VALUE ) {														\
											mFile = 0;																						\
											mOSErr = ::GetLastError();																			\
										}

#define	__OSClose						if ( ! ::CloseHandle( (void*) mFile ) ) {																\
											throwErr( cCloseErr );																				\
											mOSErr = ::GetLastError();																			\
										}


#define __OSRead( destPtr, ioBytes )	DWORD outRead;																							\
										if ( ::ReadFile( (void*) mFile, destPtr, ioBytes, &outRead, 0 ) ) 										\
											ioBytes = outRead;																					\
										else {																									\
											throwErr( cReadErr );																				\
											mOSErr = ::GetLastError();																			\
										}

#endif




CEgIFile::CEgIFile( unsigned short int inBufSize ) :
	CEgIStream( inBufSize ),
	mFile( 0 ) {

}






CEgIFile::~CEgIFile() {

	close();
}











void CEgIFile::close() {

	if ( is_open() ) {
		__OSClose

		mFile = 0;
		invalidateBuf();
	}
}





void CEgIFile::open( const CEgFileSpec* inSpec ) {

	close();
	throwErr( cNoErr );
	mPos = 0;

	if ( inSpec ) {
		__OSROpen( inSpec )
	}

	if ( mFile == 0 ) {
		#if EG_MAC
		if ( mOSErr == fnfErr )
		#elif EG_WIN
		if ( mOSErr == ERROR_FILE_NOT_FOUND || mOSErr == ERROR_PATH_NOT_FOUND )
		#else
		  if (0) /* FIXME */
		#endif
			throwErr( cFileNotFound );
		else
			throwErr( cOpenErr );
	}

}



void CEgIFile::open( const char* inFileName ) {
	CEgFileSpec fileSpec( inFileName );

	open( &fileSpec );
}



long CEgIFile::size() {
	long retSize = 0;
	long curPos = tell();

	if ( mFile ) {

		#ifdef EG_MAC
		::GetEOF( mFile, &retSize );
		#endif

		#ifdef EG_WIN16
		if ( fseek( (FILE*) mFile, 0, SEEK_CUR ) == 0 )
			retSize = ftell( (FILE*) mFile );
		#endif

		#ifdef EG_WIN32
		retSize = ::SetFilePointer( (void*) mFile, 0, 0, FILE_END );
		#endif
	}

	if ( curPos >= 0 && curPos <= retSize )
		seek( curPos );

	return retSize;
}



void CEgIFile::seek( long inPos ) {

	mNextPtr = getCStr() + inPos - mBufPos;
	mPos = inPos;
}




long CEgIFile::tell() {

	return mPos;
}



void CEgIFile::seekEnd() {

	seek( size() );
}



void CEgIFile::diskSeek( long inPos ) {

	if ( noErr() && mFile ) {

		#ifdef EG_MAC
		mOSErr = ::SetFPos( mFile, fsFromStart, inPos );
		if ( mOSErr != ::noErr )
			throwErr( cSeekErr );
		#endif

		#ifdef EG_WIN16
		fseek( (FILE*) mFile, inPos, SEEK_SET );
		if ( ferror( (FILE*) mFile ) )
			throwErr( cSeekErr );
		#endif

		#ifdef EG_WIN32
		if ( ::SetFilePointer( (void*) mFile, inPos, 0, FILE_BEGIN ) != inPos )
			throwErr( cSeekErr );
		#endif
	}
}



void CEgIFile::fillBlock( unsigned long inStartPos, void* destPtr, long& ioBytes ) {

	if ( ! mFile )
		throwErr( cNotOpen );

	diskSeek( inStartPos );

	if ( noErr() && ioBytes > 0 ) {

		__OSRead( destPtr, ioBytes )


		if ( noErr() && ioBytes <= 0 )
			throwErr( cEOFErr );
	}

}



void CEgIFile::Search( UtilStr& inSearchStr, void* inProcArg, bool inCaseSensitive, AddHitFcnT inAddHitFcn ) {
	unsigned char*	buf = new unsigned char[ cSearchBufSize ];
	unsigned char*	curPtr, *endPtr;
	unsigned char 	srchChar, srchCharLC, c;
	unsigned long	strLen = inSearchStr.length();
	unsigned long	bufLen, bufPos = 0, fileSize = size();

	srchChar	= inSearchStr.getChar( 1 );
	if ( srchChar >= 'a' && srchChar <= 'z' )
		srchChar -= 32;
	srchCharLC	= srchChar + 32;

	while ( noErr() && bufPos + strLen < fileSize ) {
		EgOSUtils::SpinCursor();
		seek( bufPos );
		bufLen = GetBlock( buf, cSearchBufSize );
		if ( bufLen >= strLen ) {
			curPtr 		= buf;
			endPtr 		= buf + bufLen - strLen;
			while ( curPtr <= endPtr ) {
				c = *curPtr;
				if ( (c == srchChar) || (c == srchCharLC) ) {
					if ( UtilStr::StrCmp( inSearchStr.getCStr(), (char*) curPtr, strLen, inCaseSensitive ) == 0 )  {
						long reqSkip = inAddHitFcn( inProcArg, bufPos + curPtr - buf );			// Add hit and If client told us to abort search...
						if ( reqSkip < 0 ) {
							curPtr = endPtr;													// Exit inner loop
							bufPos = fileSize; }												// Exit block reading loop
						else
							curPtr += reqSkip;
					}
				}
				curPtr++;
			}
			bufPos += curPtr - buf + 1;
		}
	}

	delete []buf;
}
