#include "XDynArray.h"




XDynArray::XDynArray( long inRecSize ) {

	mRecSize		= inRecSize;
	mNumElements	= 0;
}




void* XDynArray::operator[] ( const long inIndex ) {
	long len;
	
	if ( inIndex >= 0 ) {
		len = length();
		if ( inIndex >= mNumElements ) {
			mNumElements = inIndex + 1;
			Append( NULL, ( mNumElements ) * mRecSize - len );
		}
			
		return (void*) ( mBuf + inIndex * mRecSize + 1 ); }
	else
		return NULL;
}
			

