#ifndef _CLIPBOARD_
#define _CLIPBOARD_


#include "XLongList.h"
#include "XStrList.h"

class Clipboard {


	public:
									Clipboard();
									
		//	Post:	Empties the clipboard.
		void						Clear();

		//	Post:	Adds a chuck of data to this clipboard, with a flavor tag indicating the kind of data it is.
		void						Append( UtilStr& inData, long inDataFlavor );
		void						Append( void* inData, long inLen, long inDataFlavor );

		//	Post:	Loads the data at the <inIndex>th slot to <outData> and <outDataFlavor>.
		//	Post:	<true> is returned iff the fetch was successful
		//	Note:	1 is the first item in the clipboard (ie, "one-indexing" is used)
		bool						Fetch( long inIndex, long& outDataFlavor ) const;	
		bool						Fetch( long inIndex, UtilStr& outData, long& outDataFlavor ) const;	

		//	Designed to be used publicly by any application.
		static Clipboard			sAppClipboard;
		
	protected:
		XLongList					mFlavorList;
		XStrList					mDataList;


};

#endif

