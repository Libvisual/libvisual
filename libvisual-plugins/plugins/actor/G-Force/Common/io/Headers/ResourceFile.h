
#ifndef _RESOURCEFILE_
#define _RESOURCEFILE_

#include "CEgIOFile.h"

#include "XLongList.h"

//  See end of this file for documentation.


class ResourceFile;


struct RezT {		// Purpose: linked list element that represents a record/resource located on disk.
	long				mSerialNum;				// Unique ID number
	long				mLogicalSize;
	long				mPhysicalSize;
	long				mPos;					// Where in the file still resource is located.
	RezT*				mNext;					// Ptr to next item in the linked list.
};





typedef long RezRefNum;			


enum RezWriteMode {
	cSafeMode,									// This write mode keeps the previous version of the resource so it can be recovered using RevertRes()
												// cSafeMode is useful for very saving very important data.
	cSafeReplaceMode,							// This is the same as cSafeMode except that versions of the resource remains (accessable by RevertRes)
	cReplaceMode								// Overwrites the previous version of the resource (if it exists)
};


typedef bool (*AddRefHitFcnT)(void* inProcArg, RezRefNum inRefNum );


class ResourceFile {


	public:
								ResourceFile();
		virtual 				~ResourceFile();
		
		//	Post:	Opens and loads the resource block headers from the resource block file <inSpec>.  
		//	Note:	If ANY error but cResHeaderCorrupt is returned, open must be called AGAIN before any subsequent to this.
		//			If cResHeaderCorrupt is returned, this ResourceFile can still be used (in hopes that the client can retrieve any surviving blocks via GetRes). 
		//	Note:	See EmergencyRecover()
		//	Note:	Any file/stream err can be returned, including cResHeaderCorrupt
		CEgErr					open( CEgFileSpec* inSpec );
				
		//	Post:	Issues a unique "ticket"/serial number for a new "resource block" in the file.  This ticked is then used in SetRes() to store data blocks.
		//			Later, when the block needs to be retrieved, this ticket is used to claim/fetch the block.
		//	Note:	Serial number between 1 and 255 inclusive are intended for client direct access (ie, they do not need to be issued).  For example, a client
		//			that needs to store some master header table for his file would use, say, 25 because he needs a way to "know" where the master table is.
		RezRefNum				IssueNewRefNum();	

		//	Post:	Associates (and saves) the incoming data block with a resource reference number issued by ::IssueNewRefNum().  if <inMode> is cReplaceMode, if there existed
		//			a resource block with an identical <inRefNum>, it is overwritten.  If <inMode> is cSafeMode, if there existed a resource block with an identical <inRefNum>,
		//			it is kept around and can be recovered using ::RevertRes()
		//	Note:	Any file/stream err can be returned, including cResourceInvalid
		CEgErr					SetRes( RezRefNum inRefNum, const void* inData, unsigned long inSize, RezWriteMode inMode = cReplaceMode, unsigned short inSlop = 0 );
		CEgErr					SetRes( RezRefNum inRefNum, const UtilStr* inData, RezWriteMode inMode = cReplaceMode, unsigned short inSlop = 0 );

		//	Post:	Fetches/retieves the data block identified by <inRefNum> and places it in <outData>.
		//	Note:	Any file/stream err can be returned, including cResourceNotFound or cResourceInvalid
		CEgErr					GetRes( RezRefNum inRefNum, CEgIStream& outData );
		CEgErr					GetRes( RezRefNum inRefNum, UtilStr& outData );

		//	Post:	Gives some info about the block (ie, the block logical size) to the caller if <outSize> != 0
		//	Note:	Possible errors returned: cResourceNotFound, cResourceInvalid
		CEgErr					GetResInfo( RezRefNum inRefNum, long* outSize );
		
		//	Post:	This attempts to revert to an earlier version of the block reffed by <inRefNum> (assuming the black was saved in cSafeMode).
		//			If successful, the most recent version of the block reffed by <inRefNum> is "forgotten" and the 2nd most recent version
		//			will be used in future uses of <inRefNum>.
		//	Note:	Any file/stream err can be returned, including cResourceNotFound
		CEgErr					RevertRes( RezRefNum inRefNum );
		
		//	Post:	Tells this that you no longer need the blocked reffed by <inRefNum>.  ReleaseRes() can be thought of as "free( ptr )" and SetRes() 
		//			can be thought of like "ptr = malloc()".
		//	Note:	CALL THIS when you're done with resource blocks or else they will be "file leaked" and will be uselessly toted around FOREVER!
		void					ReleaseRes( RezRefNum inRefNum );		
		
		//	Post:	This duplicates this resource block file to <inDestSpec>.
		//	Note:	Garbage collection is effectly performed to the destination file.
		//	Note:	Any valid file/stream error can be returned.
		CEgErr					Duplicate( CEgFileSpec& inDestSpec );
		
		//	Post:	Attempts to reconstruct the master header of this resource file.  A new
		//			ResourceFile is created using the given spec and the recovered resources are placed in the RF.
		//	Note:	This is useful if cResHeaderCorrupt is returned from open().
		CEgErr					EmergencyRecover( XLongList& inSearchList );
		
		//	Post:	Returns the percent data garbage (ie, data blocks released) within this file.
		//	Note:	0 to 100 inclusive is returned.
		int						CalcPercentGarbage();
		
		//	Post:	Accessor function to close the file this resource file mgr uses.
		//	Note:	open() must be called if this file is to be used again after close()
		void					close()												{ mFile.close();	}
		
		//	Pre:	<inAddHitFcn> is a callback fcn that handles hits during the search, with <inProgArg> given as its argument
		//	Post:	Perfomrs a search for <inSearchStr> within all valid resource blocks.  For each hit, the callback <inAddHitFcn>() is called.
		void					Search( UtilStr& inSearchStr, void* inProgArg, bool inCaseSensitive, AddRefHitFcnT inAddHitFcn );


	protected:
		CEgIOFile				mFile;
		long					mLargestSerialNum;
		
		RezT*					mHead;				// This holds our linked list of RezT, our list of records that represent the records on disk.
													// In this list, if there are two or more elements with the same serial/ID rez num, the record appearing
													//   first in the list is the newest one. 
				
		enum {	
			cCurFileVersion		= 0x1111,
			cRezBaseID			= 255,
			cRezHeaderSize		= 20,
			cMasterJumpPos		= 64,
			cRezFlag			= 'GkkG'
		};
		
		struct SearchArgT  { 
			ResourceFile*	thisPtr;
			void*			refHitProcArg;
			AddRefHitFcnT	addRefHitFcn;
		};
		
		void					ReleaseExtraVersions( RezRefNum inSerialNum, long inMaxNum );
		
		static long				sFilePosHitFcn( void* inProcArg, long inFilePos );
		long					filePosHitFcn( void* inProcArg, long inFilePos, AddRefHitFcnT inRefHitFcn );
		
		void					readRezRefs();
		void					deleteRezRefs();
		RezT*					FetchRez( RezRefNum inSerialNum, RezT** outPrevRez, long inVersNum );
		
		CEgErr					releaseRez( RezT* inRez, RezT* prevPtr );
	
	
		// Used in EmergencyRecover()
		long					RecoverRez( long inPos, XLongList& inSearchList, long inOrigSize, CEgErr& outErr );
		static long				RecoverRezFcn( void* inProcArg, long inFilePos );

	
};
		



// Used for emergency recovert--used in EmergencyRecover()
struct RFRecoverParamT{
	long				mOrigSize;
	ResourceFile*		mSourceRF;
	XLongList*			mSearchList;
	CEgErr				mErr;
};



/*
The format of a ResourceFile is actually simple:
(Note: All integers are LS Byte first in file)

File Offset					Type			Description

0..63						Reserved 		(not used)
64..67						Long			file pos of first data record  (0 if no recs)



For a given data record:


Record Offset				Type			Description

0..3						Long			Constant Record marker, cRezFlag, for emergency corruption recover ability of a ResourceFile
4..7						Long			File pos of next data record (they are singly linked). 0 is last record.
8..11						Long			Serial Number of that data record (ie, the resource number, or "RezID")
12..15						Long			Physical size of data record
16..19						Long 			Logical size of data record
20..20+Lgl Size				Data			The resource data
21+Lgl Size..20+Phys Size	Junk			
*/



#endif
		



