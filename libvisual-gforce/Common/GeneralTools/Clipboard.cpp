#include "Clipboard.h"


Clipboard Clipboard::sAppClipboard;


Clipboard::Clipboard() :
	mFlavorList( cOrderImportant ), 
	mDataList( cDuplicatesAllowed, cOrderImportant ) {
}



void Clipboard::Clear() {

	mDataList.RemoveAll();
	mFlavorList.RemoveAll();
}



void Clipboard::Append( UtilStr& inData, long inDataFlavor ) {

	Append( inData.getCStr(), inData.length(), inDataFlavor );
}


void Clipboard::Append( void* inData, long inLen, long inDataFlavor ) {

	mFlavorList.Add( inDataFlavor );
	mDataList.Add( inData, inLen );
}


bool Clipboard::Fetch( long inIndex, long& outDataFlavor ) const {
	return mFlavorList.Fetch( inIndex, &outDataFlavor );
}


bool Clipboard::Fetch( long inIndex, UtilStr& outData, long& outDataFlavor ) const {

	if ( mFlavorList.Fetch( inIndex, &outDataFlavor ) )
		return mDataList.Fetch( inIndex, outData );
	else
		return false;
}

