#ifndef _CSTRNODE_
#define _CSTRNODE_


#include "nodeClass.h"
#include "UtilStr.h"

#define VAL(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))

class CStrNode : public nodeClass, public UtilStr {

	public:
								CStrNode( nodeClass* inParent, char* inStr );
								CStrNode( nodeClass* inParent );

		enum 					{ sClassID = VAL('S','t','r','N') };

		virtual void			ReadFrom( CEgIStream* inStream );
		virtual void			WriteTo( CEgOStream* inStream );

};

#undef VAL

#endif
