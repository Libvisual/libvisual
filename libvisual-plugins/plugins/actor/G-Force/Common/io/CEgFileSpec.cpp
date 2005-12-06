#include "CEgFileSpec.h"

#include "CEgIOFile.h"

#ifdef EG_WIN
#include <stdio.h>

#ifndef GetCurrentDirectory
#include <windows.h>
#endif

#define DIRSEP '\\'

#endif

#ifdef UNIX_X
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define DIRSEP '/'
#endif

#include "EgOSUtils.h"


#ifdef EG_MAC
#include <Resources.h>
#include <Files.h>
#endif


CEgFileSpec::CEgFileSpec() {
	
	Assign( 0, 0 );
}


CEgFileSpec::CEgFileSpec( const char* inFileName, long inType  ) {
	Assign( 0, 0 );
	AssignPathName( inFileName );
	SetType( inType );
}




CEgFileSpec::CEgFileSpec( const CEgFileSpec& inSpec ) {
	
	Assign( inSpec );
}




void CEgFileSpec::Assign( const CEgFileSpec& inSpec ) {

	Assign( inSpec.OSSpec(), inSpec.GetType() );
}





void CEgFileSpec::Assign( const void* inOSSpecPtr, long inType ) {
	mFileType = inType;
	
	#ifdef EG_MAC
	mSpecData.Assign( (char*) inOSSpecPtr, sizeof( FSSpec ) + 32 );
	#endif
	#if EG_WIN || defined(UNIX_X)	
	mSpecData.Assign( (char*) inOSSpecPtr );
	#endif
}


void CEgFileSpec::AssignPathName( const char* inPathName ) {

	if ( inPathName ) {
		#ifdef EG_MAC
		FSSpec	spec;
		UtilStr path( inPathName );
		::FSMakeFSSpec( 0, 0, path.getPasStr(), &spec );
		//__path2fss( inPathName, &spec );
		Assign( &spec, mFileType );
		#endif
		#if defined(EG_WIN) || defined(UNIX_X)	
		Assign( inPathName, 0 );
		#endif
	}	
}


void CEgFileSpec::AssignFolder( const char* inFolderName ) {
	
	#ifdef EG_MAC
	CInfoPBRec 	pb;
	FSSpec		spec;
	Str255		name;
	
	UtilStr path;
	if ( inFolderName[ 0 ] != ':' )
		path.Append( ':' );
	path.Append( inFolderName );
	path.copyTo( name, 255 );
	pb.hFileInfo.ioNamePtr		= name;
	pb.hFileInfo.ioVRefNum		= ( (FSSpec*) EgOSUtils::sAppSpec.OSSpec() ) -> vRefNum;
	pb.hFileInfo.ioDirID		= ( (FSSpec*) EgOSUtils::sAppSpec.OSSpec() ) -> parID;
	pb.hFileInfo.ioFDirIndex	= 0;

	::PBGetCatInfoSync( &pb );
	spec.vRefNum	= pb.hFileInfo.ioVRefNum;
	spec.parID		= pb.hFileInfo.ioDirID;
	spec.name[0]	= 0;
	Assign( &spec, 0 );
	#endif
	
	#if defined(EG_WIN) || defined(UNIX_X)
	mSpecData.Assign( (char*) EgOSUtils::sAppSpec.OSSpec() );
#if 0
	/* FIXME why the DIRSEP at the start */
	if ( inFolderName[ 0 ] != DIRSEP && mSpecData.getChar( mSpecData.length() ) != DIRSEP )
		mSpecData.Append( DIRSEP );
#endif
	mSpecData.Append( inFolderName );
	if ( mSpecData.getChar( mSpecData.length() ) != DIRSEP )
		mSpecData.Append( DIRSEP );
	#endif
}
	




long CEgFileSpec::GetType() const {

	#ifdef EG_MAC
	return mFileType;
	#endif
	
	#if defined(EG_WIN) || defined(UNIX_X)
	unsigned long i, j, len, type = 0;

	len = mSpecData.length();
	i = mSpecData.FindLastInstanceOf( '.' );
	#if defined(EG_WIN)
	j = mSpecData.FindLastInstanceOf( '\\' );
        #elif defined(UNIX_X)
	j = mSpecData.FindLastInstanceOf( '/' );
	#endif

	if ( i > 0 && len - i <= 3 && j < i ) {
		for ( j = i; j <= len; j++ )
			type = ( type << 8 ) | mSpecData.getChar( j );
	}
	
	return type;
	#endif	
	
}





void CEgFileSpec::SetType( long inType ) {
	mFileType = inType;
}





void CEgFileSpec::Delete() const {
	
	if ( OSSpec() ) {
		#ifdef EG_MAC
		::FSpDelete( (FSSpec*) mSpecData.getCStr() );
		#endif
		#if defined(EG_WIN)
		remove( mSpecData.getCStr() );
		#endif
		#if defined(UNIX_X)
		unlink( mSpecData.getCStr() );
		#endif
	}
}






void CEgFileSpec::Rename( const UtilStr& inNewName ) {

	
	#ifdef EG_MAC
	inNewName.copyTo( ((FSSpec*) OSSpec()) -> name, 32 );
	#endif
	
	#if defined(EG_WIN) || defined(UNIX_X)
	int pos;
	pos = mSpecData.FindLastInstanceOf( DIRSEP );
	mSpecData.Trunc( mSpecData.length() - pos );
	mSpecData.Append( inNewName );
	#endif
}



void CEgFileSpec::MakeUnique() {
	UtilStr origName, newName;
	long i;
		
	// Make a copy of the original file name	
	GetFileName( origName );
	#if EG_MAC
	origName.Keep( 29 );
	#endif
	origName.Append( " " );
		
	// Once this spec isn't taken/used, then we're done	
	for ( i = 1; i < 10000 && Exists(); i++ ) {
		newName.Assign( origName );
		newName.Append( i );
		Rename( newName );
	}
}


void CEgFileSpec::GetFileName( UtilStr& outFileName ) const {
	int 	pos;
	
	outFileName.Wipe();
		
	if ( OSSpec() ) {
		#ifdef EG_MAC
		outFileName.Assign( ((FSSpec*) OSSpec()) -> name );
		#endif
		#if defined(EG_WIN) || defined(UNIX_X)
		
		// If it's a folder, temporarily remove the end '\'
		long len = mSpecData.length();
		bool isFolder = mSpecData.getChar( len ) == DIRSEP;
		if ( isFolder ) {
			pos = mSpecData.FindPrevInstanceOf( len - 1, DIRSEP ); 
			outFileName.Assign( mSpecData.getCStr() + pos, len - pos - 1 );
			}
		else {
			pos = mSpecData.FindLastInstanceOf( DIRSEP );
			outFileName.Assign( mSpecData.getCStr() + pos );
		}
		#endif
	}
	
	pos = outFileName.FindLastInstanceOf( '.' );	// Find where ext begins
	
	if ( pos > 0 )
		outFileName.Keep( pos - 1 );				// Strip extension
}






void CEgFileSpec::SaveAs( const CEgFileSpec& inDestSpec ) const {
	
	if ( OSSpec() && inDestSpec.OSSpec() ) {
		#ifdef EG_MAC
		::FSpExchangeFiles ( (FSSpec*) OSSpec(), (FSSpec*) inDestSpec.OSSpec() );
		Delete();
		#endif
		#if defined(EG_WIN) || defined(UNIX_X)
		inDestSpec.Delete();
		rename( (char*) OSSpec(), (char*) inDestSpec.OSSpec() );
		#endif
	}
}



int CEgFileSpec::Exists() const {
	CEgIFile	  iFile;

	#if EG_MAC
	OSErr				err;
	HFileInfo			pb;
	FSSpec* 			spec = (FSSpec*) OSSpec();
	UtilStr				fname;
	
	// Prep a str to receive un updated name change
	fname.Dim( 40 );
	fname.Assign( spec -> name );
	
	// See if we have a file
	pb.ioDirID 		= spec -> parID;
	pb.ioVRefNum	= spec -> vRefNum;
	pb.ioNamePtr 	= fname.getPasStr();
	pb.ioFDirIndex	= 0;
	err = ::PBGetCatInfoSync( (CInfoPBRec*) &pb );
	if ( err == noErr )
		return 1;
	
	// See if we have a dir	
	pb.ioDirID 		= spec -> parID;
	pb.ioVRefNum	= spec -> vRefNum;
	pb.ioFDirIndex	= -1;
	err = ::PBGetCatInfoSync( (CInfoPBRec*) &pb );
	if ( err == noErr && fname.compareTo( spec -> name, false ) == 0 )
		return 2;
	#endif
	
	#if EG_WIN
	long attribs = ::GetFileAttributes( (char*) OSSpec() );

	if ( attribs & FILE_ATTRIBUTE_DIRECTORY )
		return 2;
	else if ( attribs & FILE_ATTRIBUTE_NORMAL )
		return 1;
	#endif

	#ifdef UNIX_X
	struct stat statdata;
	if (stat((char *) OSSpec(), &statdata) != 0)
		return 0;
	if (S_ISDIR(statdata.st_mode))
		return 2;
	else if (S_ISREG(statdata.st_mode))
		return 1;
	#endif

	return 0;
}



CEgErr CEgFileSpec::Duplicate( const CEgFileSpec& inDestSpec ) const {
	CEgIOFile	oFile;
	CEgIFile	iFile;

	// Open the source and destination files
	iFile.open( this );
	iFile.seek( 0 );
	if ( ! iFile.noErr() ) 
		oFile.open( &inDestSpec );
	
	long		pos		= 0, numBytes;
	long		size	= iFile.size();
	CEgErr		err;
	
	// Copy the file in 50k chunks
	while ( pos < size && oFile.noErr() && iFile.noErr() ) {
		numBytes = 50000;
		if ( numBytes + pos > size )
			numBytes = size - pos;
		oFile.CEgOStream::PutBlock( iFile, numBytes );
		pos += numBytes;
	}
	
	if ( iFile.noErr() )
		err = oFile;
	else
		err = iFile;
		
	return err;
}




void CEgFileSpec::ChangeExt( const char* inExt ) {
	UtilStr	fileName;
	long		len;
	
	GetFileName( fileName );
	fileName.Append( '.' );
	len = fileName.length();
	fileName.Append( inExt );
	fileName.Keep( len + 3 );
	Rename( fileName );
	
}





const void* CEgFileSpec::OSSpec() const {

	if ( mSpecData.length() > 0 )
		return mSpecData.getCStr();
	else
		return 0;
}
