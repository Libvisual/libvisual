
#ifndef _DELETETHREAD_
#define _DELETETHREAD_

#include "EgThread.h"


class DeleteThread : protected EgThread {

	protected:
		nodeClass*					mNodeToDelete;
		
	public:
									DeleteThread( nodeClass* inNodeToDelete );
		
		virtual void				SpendTime() = 0;
};

#endif