
#include "DeleteThread.h"


DeleteThread::DeleteThread( nodeClass* inNodeToDelete ) :
	EgThread( 1, false ) {
	mNodeToDelete = inNodeToDelete;
}

		
void DeleteThread::SpendTime() {
	if ( mNodeToDelete )
		delete mNodeToDelete;
	Stop();
}
