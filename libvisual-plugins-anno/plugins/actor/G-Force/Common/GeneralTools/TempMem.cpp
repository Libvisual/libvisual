// TempMem.cpp

#include "TempMem.h"

TempMem::TempMem() {

	mDimSize = 0;
}


TempMem::~TempMem() {

}


char* TempMem::Dim( long inBytes ) {

	if ( inBytes > mDimSize ) {
		mDimSize = 0;

		mDimSize = inBytes;
	}

	return mTemp.Dim( inBytes );
}


