#include "Messager.h"




XPtrList Messager::sAllMessagers;


Messager::Messager() {
	mAllowBroadcasting = true;
	
	sAllMessagers.Add( this );
	StartListening();
}


Messager::~Messager() {
	int			i = 1;
	Messager*	msger;
	
	StopListening();
	sAllMessagers.Remove( this );
		
	while ( mListeners.Fetch( i, (void**) &msger ) ) {
		i++;
		msger -> mListeners.Remove( this );
	}
}





		
void Messager::BroadcastGlobalMessage( long inMsg, long inArgs[] ) {
	Messager*	msger;
	int			i = 1;
	
	while ( sAllMessagers.Fetch( i, (void**) &msger ) ) {
		i++;
		if ( msger -> IsListening() )
			msger -> ListenToMsg( inMsg, inArgs );
	}
}




void Messager::BroadcastMessage( long inMsg, long inArgs[] ) {
	int			i = 1;
	Messager*	msger;
	
	if ( mAllowBroadcasting && inMsg ) {
		while ( mListeners.Fetch( i, (void**) &msger ) ) {
			i++;
			if ( msger -> IsListening() )
				msger -> ListenToMsg( inMsg, inArgs );
		}
	}
}


void Messager::StartListening() {
	mIsListening = true;
}


void Messager::StopListening() {
	mIsListening = false;
}



void Messager::SetAllowBroadcasting( bool inAllowed ) {
	mAllowBroadcasting = inAllowed;
}



void Messager::AddListener( Messager* inListener ) {

	if ( inListener ) {
		if ( mListeners.FindIndexOf( inListener ) == 0 ) {
			mListeners.Add( inListener );
			inListener -> mListeners.Add( this );
		}
	}
}



void Messager::CopyListeners( Messager* inSource ) {
	int i = 0;
	Messager* msger;
	
	if ( inSource ) {
		while ( inSource -> mListeners.Fetch( i, (void**) &msger ) ) {
			i++;
			AddListener( msger );
		}
	}
}



bool Messager::ListenToMsg( long , long []  ) {
	return false;
}


