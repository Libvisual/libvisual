#include "CEgOStub.h"



CEgOStub::CEgOStub() :
	mBytesPut( 0 ),
	CEgOStream() {

}





void CEgOStub::PutBlock( const void* , long numBytes ) {
	mBytesPut += numBytes;
}





