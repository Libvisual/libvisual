
#include "ResourceFile.h"

#include "EgOSUtils.h"
#include "CEgFileSpec.h"


ResourceFile::ResourceFile() :
	mFile( false ) {

	mLargestSerialNum = cRezBaseID;
	mHead = NULL;
}


ResourceFile::~ResourceFile() {

	deleteRezRefs();
}



CEgErr ResourceFile::open( CEgFileSpec* inSpec )  {
	deleteRezRefs();
	
	mFile.open( inSpec );	
	
	if ( mFile.noErr() ) {
		if ( mFile.size() > cMasterJumpPos ) {						// If opening existing
			mFile.seek( 8 );
			mLargestSerialNum = mFile.GetLong();
			readRezRefs();	}
		else {														// If making new
			mFile.seek( 0 );
			mFile.PutLong( cCurFileVersion );						// Resource File version number
			mFile.PutLong( cCurFileVersion );						// Resource File version number
			mLargestSerialNum = cRezBaseID;
			mFile.PutLong( mLargestSerialNum );						// Largest serial num in use
			for ( int i = 0; i < cMasterJumpPos; i++ ) 
				mFile.PutByte( 0 );
			mFile.flush();
		}
	}
	
	return mFile;													// Return any err that occured
}


RezRefNum ResourceFile::IssueNewRefNum() {
	mLargestSerialNum++;
	mFile.seek( 8 );
	mFile.PutLong( mLargestSerialNum ); 
	return mLargestSerialNum;
}


void ResourceFile::readRezRefs() {
	long			nextPos, thisPos;
	RezT*			rezPtr;
	long			fileSize = mFile.size();
	
	deleteRezRefs();
	
	mFile.seek( cMasterJumpPos );
	nextPos = mFile.GetLong();
	
	while ( mFile.noErr() && nextPos > 0 ) {
		thisPos = nextPos;
		mFile.seek( thisPos );
		if ( mFile.GetLong() == cRezFlag && nextPos < fileSize ) {
			nextPos = mFile.GetLong();
			
			rezPtr = new RezT;
			rezPtr -> mPos 				= thisPos;
			rezPtr -> mNext 			= mHead;
			rezPtr -> mSerialNum		= mFile.GetLong();
			rezPtr -> mPhysicalSize		= mFile.GetLong();
			rezPtr -> mLogicalSize		= mFile.GetLong(); 
			
			if ( mFile.noErr() ) {
				if ( rezPtr -> mSerialNum > mLargestSerialNum )
					mLargestSerialNum = rezPtr -> mSerialNum;
				
				mHead = rezPtr; }
			else {
				mFile.throwErr( cNoErr );
				mFile.throwErr( cResHeaderCorrupt );
				delete rezPtr;							// Abort new ref
			} }
		else {
			mFile.throwErr( cNoErr );
			mFile.throwErr( cResHeaderCorrupt );
		}
			
		EgOSUtils::SpinCursor();
	}
	
}





void ResourceFile::deleteRezRefs() {
	RezT* 	nextPtr, *rezPtr = mHead;
	
	while ( rezPtr ) {
		nextPtr = rezPtr -> mNext;
		delete rezPtr;
		rezPtr = nextPtr;
	}
	mHead = NULL;
}



RezT* ResourceFile::FetchRez( RezRefNum inSerialNum, RezT** outPrevRez, long inVersNum ) {
	RezT*	prevPtr	= NULL;
	RezT* 	retPtr	= NULL;
	RezT*	rezPtr;

	if ( inVersNum < 1 )
		inVersNum = 1;
		
	if ( mHead ) {
		if ( mHead -> mSerialNum == inSerialNum )
			inVersNum--;
		if ( inVersNum == 0 )									// If we found it
			retPtr = mHead;
		else if ( mHead -> mNext ) {
			prevPtr = mHead;
			rezPtr = mHead -> mNext;
			while ( rezPtr && ! retPtr  ) {
				if ( rezPtr -> mSerialNum == inSerialNum )		// Newest ones apprear first
					inVersNum--;
				if ( inVersNum == 0 )							// If we found it
					retPtr = rezPtr;
				else {
					prevPtr	= rezPtr;
					rezPtr	= rezPtr -> mNext;
				}
			} 
		}
	}
	
	if ( outPrevRez )
		*outPrevRez = prevPtr;
		
	return retPtr;
}



void ResourceFile::ReleaseExtraVersions( RezRefNum inSerialNum, long inMaxNum ) {
	RezT* rezPtr, *prevPtr;
	
	do {
		rezPtr = FetchRez( inSerialNum, &prevPtr, inMaxNum );
		if ( rezPtr ) {
			releaseRez( rezPtr, prevPtr );
			EgOSUtils::SpinCursor();
		}
	} while ( rezPtr );
}








CEgErr ResourceFile::GetResInfo( RezRefNum inRefNum, long* outSize ) {
	RezT*	rezPtr	= FetchRez( inRefNum, NULL, 1 );	
		
	if ( rezPtr )  {
		if ( outSize )
			*outSize = rezPtr -> mLogicalSize;
		return cNoErr; }
	else if ( inRefNum <= 0 )
		return cResourceInvalid;
	else {
		if ( outSize )
			*outSize = 0;
		return cResourceNotFound;
	}
}


CEgErr ResourceFile::GetRes( RezRefNum inRefNum, UtilStr& outData ) {
	RezT*	rezPtr	= FetchRez( inRefNum, NULL, 1 );	
		
	mFile.throwErr( cNoErr );
	
	if ( rezPtr ) {
		mFile.seek( rezPtr -> mPos + cRezHeaderSize );
		outData.Assign( mFile, rezPtr -> mLogicalSize ); 
		return mFile; }
	else if ( inRefNum <= 0 )
		return cResourceInvalid;
	else
		return cResourceNotFound;
}



CEgErr ResourceFile::GetRes( RezRefNum inRefNum, CEgIStream& outStream ) {
	RezT*	rezPtr	= FetchRez( inRefNum, NULL, 1 );	
	
	mFile.throwErr( cNoErr );
	
	if ( rezPtr ) {
		mFile.seek( rezPtr -> mPos + cRezHeaderSize );
		outStream.Assign( &mFile, rezPtr -> mLogicalSize );
		return mFile; }
	else if ( inRefNum <= 0 )
		return cResourceInvalid;
	else
		return cResourceNotFound;
}



CEgErr ResourceFile::SetRes( RezRefNum inRefNum, const UtilStr* inData, RezWriteMode inMode, unsigned short inSlop ) {
	void*		srce = NULL;
	long		len = 0;
	
	if ( inData ) {
		srce = inData -> getCStr();
		len = inData -> length();
	}
		
	return SetRes( inRefNum, srce, len, inMode, inSlop );
}



CEgErr ResourceFile::SetRes( RezRefNum inRefNum, const void* inPtr, unsigned long inSize, RezWriteMode inMode, unsigned short inSlop ) {
	RezT*	rezPtr = ( inMode == cReplaceMode ) ? FetchRez( inRefNum, NULL, 1 ) : NULL;
	CEgErr	retErr;
	bool	addNew = true;

	
	mFile.throwErr( cNoErr );
	
	if ( inRefNum <= 0 || inRefNum > mLargestSerialNum )
		return cResourceInvalid;
	else {
		if ( rezPtr ) {
			if ( rezPtr -> mPhysicalSize >= inSize ) {
				addNew = false;
				mFile.seek( rezPtr -> mPos + cRezHeaderSize - 4 );		// Seek LogicalSize Pos.  WARNING: Subject to change on header change
				rezPtr -> mLogicalSize = inSize; 
			}
		}
		
		if ( addNew ) {
			mFile.seekEnd();
			
			rezPtr = new RezT;
			
			rezPtr -> mSerialNum		= inRefNum;
			rezPtr -> mLogicalSize		= inSize;
			rezPtr -> mPhysicalSize		= inSize + inSlop;
			rezPtr -> mPos				= mFile.tell();
			rezPtr -> mNext				= mHead;
			
			mFile.PutLong( cRezFlag );
			mFile.PutLong( 0 );
			mFile.PutLong( rezPtr -> mSerialNum );
			mFile.PutLong( rezPtr -> mPhysicalSize );
		}
		
		mFile.PutLong( rezPtr -> mLogicalSize );
		mFile.PutBlock( inPtr, rezPtr -> mLogicalSize );
		mFile.CEgOStream::skip( rezPtr -> mPhysicalSize - rezPtr -> mLogicalSize );
		mFile.flush();									// We want to make sure we catch any disk errors
		
		if ( mFile.noErr() ) {							// If successful
			if ( addNew ) {
				if ( mHead )
					mFile.seek( mHead -> mPos + 4 );	// Move to NextRezFilePtr
				else
					mFile.seek( cMasterJumpPos );
				mFile.PutLong( rezPtr -> mPos ); 		// Finish the file link
				mFile.flush();
				mHead = rezPtr;							// Finish Inserting the new Rez item in the list
			} 
		}
		
		if ( mFile.noErr() ) {
			if ( inMode == cReplaceMode || inMode == cSafeReplaceMode )
				ReleaseExtraVersions( inRefNum, 3 );
		}
		
		return mFile;									// Return any err
	}
}



void ResourceFile::ReleaseRes( RezRefNum inRefNum ) {
	CEgErr	delErr;
	
	do {
		delErr = RevertRes( inRefNum );
	} while ( delErr.noErr() );
}



CEgErr ResourceFile::RevertRes( RezRefNum inRefNum ) {
	RezT*			prevPtr;
	RezT*			rezPtr = FetchRez( inRefNum, &prevPtr, 1 );
	
	return releaseRez( rezPtr, prevPtr );
}




CEgErr ResourceFile::releaseRez( RezT* rezPtr, RezT* prevPtr ) {
	unsigned long	nextRezPos;
	CEgErr			retErr;
	
	mFile.throwErr( cNoErr );
	
	if ( rezPtr ) {
		mFile.seek( rezPtr -> mNext ? rezPtr -> mNext -> mPos + 4: cMasterJumpPos );
		nextRezPos = prevPtr ? prevPtr -> mPos: 0;							// We denote end of link as 0
		if ( mFile.noErr() )
			mFile.PutLong( nextRezPos );
		if ( mFile.noErr() ) {
			if ( prevPtr )
				prevPtr -> mNext = rezPtr -> mNext;
			else
				mHead = rezPtr -> mNext;
			delete rezPtr; }
		else
			retErr = mFile; }
	else
		retErr.throwErr( cResourceNotFound );
	
	return retErr;
}




		
int	ResourceFile::CalcPercentGarbage() {
	RezT*		rezPtr	= mHead;
	long		used 	= cMasterJumpPos + 200;
	long		percent;
	
	while ( rezPtr ) {											// Newest come first
		used += rezPtr -> mPhysicalSize + cRezHeaderSize;
		rezPtr = rezPtr -> mNext;
	}
	
	mFile.throwErr( cNoErr );
	
	percent = 100 - (100 * used) / mFile.size();

	if ( percent < 0 )
		percent = 0;
		
	if ( mFile.noErr() )
		return percent;
	else
		return 0;
}




CEgErr ResourceFile::Duplicate( CEgFileSpec& inDestSpec ) {
	CEgErr			err;
	RezT*			rezPtr = mHead;
	UtilStr		theData;
	unsigned short	slop;
	ResourceFile	newRF;
	
	err = newRF.open( &inDestSpec );
	
	while ( rezPtr && err.noErr() ) {
		if ( ! newRF.FetchRez( rezPtr -> mSerialNum, NULL, 1 ) ) {			// Don't write old resource versions
			err = GetRes( rezPtr -> mSerialNum, theData );
			if ( err.noErr() ) {
				slop = rezPtr -> mPhysicalSize - rezPtr -> mLogicalSize;
				
				// Maintain what serial nums have been issued
				if ( newRF.mLargestSerialNum < rezPtr -> mSerialNum )
					newRF.mLargestSerialNum = rezPtr -> mSerialNum;
					
				err = newRF.SetRes( rezPtr -> mSerialNum, &theData, cReplaceMode, slop );
			}
			EgOSUtils::SpinCursor();
		}
			
		rezPtr = rezPtr -> mNext;
	}
	
	// Flush what serial num we're up to to disk
	if ( err.noErr() )
		newRF.IssueNewRefNum();
	
	return err;
}




void ResourceFile::Search( UtilStr& inSearchStr, void* inProcArg, bool inCaseSensitive, AddRefHitFcnT inAddHitFcn ) {
	SearchArgT	info = { this, inProcArg, inAddHitFcn };
	
	mFile.throwErr( cNoErr );
	mFile.Search( inSearchStr, &info, inCaseSensitive, sFilePosHitFcn );
}


long ResourceFile::sFilePosHitFcn( void* inProcArg, long inFilePos ) {
	SearchArgT*	info = (SearchArgT*) inProcArg;
	
	return info -> thisPtr -> filePosHitFcn( info -> refHitProcArg, inFilePos, info -> addRefHitFcn );
}

	
long ResourceFile::filePosHitFcn( void* inProcArg, long inFilePos, AddRefHitFcnT inRefHitFcn ) {
	RezT*		rezPtr		= mHead;
	RezT*		hitPtr		= NULL;
	long		skipBytes	= 0;
	
	while ( rezPtr && ! hitPtr ) {
		if ( inFilePos > rezPtr -> mPos + cRezHeaderSize ) 
			if ( inFilePos < rezPtr -> mPos + rezPtr -> mLogicalSize ) 
				hitPtr = rezPtr;
		rezPtr = rezPtr -> mNext;
	}
	
	if ( hitPtr ) {
		skipBytes = hitPtr -> mPhysicalSize - ( inFilePos - hitPtr -> mPos );
		rezPtr = FetchRez( hitPtr -> mSerialNum, NULL, 1 );				
		if ( rezPtr == hitPtr ) { 											// Make sure the hit was in a non-backup Rez
			if ( ! inRefHitFcn( inProcArg, hitPtr -> mSerialNum ) )
				skipBytes = -1;												// Send flag to stop search
		}
	}
	
	return skipBytes;
}





CEgErr ResourceFile::EmergencyRecover( XLongList& inSearchList ) {
	CEgOStream		temp;
	RFRecoverParamT	param;
	long			marker = cRezFlag;
	UtilStr		str( &marker, 4 );
	
	mFile.throwErr( cNoErr );

	// Setup the param for searching for the Rez flag marker
	param.mOrigSize			= mFile.size();
	param.mSearchList		= &inSearchList;
	param.mSourceRF			= this;
	param.mErr.throwErr( cNoErr );
	
	// Search this RF's file at a low level for cRezFlags, recovering data blocks
	mFile.Search( str, &param, true, RecoverRezFcn );
	
	// Update to disk the highest ref num issued
	IssueNewRefNum();
		
	return param.mErr;
} 
		
	



long ResourceFile::RecoverRez( long inPos, XLongList& inSearchList, long inOrigSize, CEgErr& outErr ) {
	RezRefNum	rezNum;
	UtilStr	data;
	long 		physSize, logSize, pos;
	
	outErr.throwErr( cNoErr );
	
	// Skip the cRezFlag
	mFile.throwErr( cNoErr );
	mFile.seek( inPos + 4 );

	// If the pos of the next rez looks suspicious, then abort
	pos = mFile.GetLong();
	if ( pos < 0 || pos > inOrigSize )
		return 4;
		
	// If the resource ID looks suspicious, then abort
	rezNum = mFile.GetLong();
	if ( rezNum < 0 || rezNum > 10000000 )
		return 8;
		
	// If the logical or physical size look suspicious, then abort
	physSize 	= mFile.GetLong();
	logSize		= mFile.GetLong();
	if ( logSize > physSize || inPos + logSize > inOrigSize || inPos + physSize > inOrigSize || physSize < 0 || logSize < 0 )
		return 16;
			
	// See if we're looking for this rez.  If we are, write a new copy of the rez
	if ( inSearchList.FindIndexOf( rezNum ) > 0 ) {
	
		// Read the res's data into a buffer
		mFile.Read( data, logSize );	
		
		if ( mFile.noErr() ) {
			// Make sure we don't issue this rez again
			if ( mLargestSerialNum < rezNum )
				mLargestSerialNum = rezNum;
				
			// Write a fresh rez in this resource file
			outErr = SetRes( rezNum, &data ); }
		else
			outErr.throwErr( cNoErr );
	}
	
	// This is the offset after this rez we should resume looking...
	return physSize + 16;
}





long ResourceFile::RecoverRezFcn( void* inProcArg, long inFilePos ) {
	RFRecoverParamT*	param = (RFRecoverParamT*) inProcArg;
	long skip;
	
	// Stop the file search if we go past the initial size of the resource file
	if ( inFilePos < param -> mOrigSize ) {
	
		// Suck the rez info and data from the temp stream.
		skip = param -> mSourceRF -> RecoverRez( inFilePos, *param -> mSearchList, param -> mOrigSize, param -> mErr );
	
		// If there was no errors, resume the file search
		if ( param -> mErr.noErr() )
			return skip;
	}
			
			
	return -1;
}

