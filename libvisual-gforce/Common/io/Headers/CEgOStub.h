#ifndef _CEGOSTUB_
#define _CEGOSTUB_

#include "CEgOStream.h"

//#include "Eg_Consts.h"


class CEgOStub : public CEgOStream {
		
	protected:
		
	public:
										CEgOStub();
										
		virtual void					PutBlock( const void* inSrce, long numBytes );

		long							mBytesPut;


};

#endif