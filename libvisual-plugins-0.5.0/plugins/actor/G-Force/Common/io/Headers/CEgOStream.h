#ifndef _CEGOSTREAM_
#define _CEGOSTREAM_

#include "CEgErr.h"


#include "UtilStr.h"


class CEgOStream : public virtual CEgErr {

	
	protected:

		
	public:
										CEgOStream();
	
		
		// Client fcns...
		void							PutLong( long inNum );
		void							PutShort( signed short int inNum );
		void							PutByte( unsigned char inChar );
		virtual void					PutBlock( const void* inPtr, long byteSize );
		void							PutBlock( CEgIStream& inStream, long inBytes );
		void							Write( const char* inStr );
		void							Write( const UtilStr* inStr );
		void							Writeln( const UtilStr& inStr );
		void							Writeln( const char* inStr = 0 );
		
		virtual void					skip( long inBytes );
		
		UtilStr							mOBuf;
		
		virtual void					Reset();

};

#endif
