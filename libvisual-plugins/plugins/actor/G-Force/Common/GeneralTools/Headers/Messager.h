
#ifndef _MESSAGER_
#define _MESSAGER_


#include "XPtrList.h"




class Messager {

	private:
		
		XPtrList						mListeners;
		bool							mIsListening;
		bool							mAllowBroadcasting;
		
		static XPtrList					sAllMessagers;
		
		
	public:	
										Messager();
										~Messager();
										
		void							StartListening();
		void							StopListening();
		bool							IsListening()								  { return mIsListening; }
		
		void							SetAllowBroadcasting( bool inAllowed );
		
		//	Post:	Returns true if message was handled.
		virtual bool					ListenToMsg( long inMessage, long inArgs[] );
		
		void							AddListener( Messager* inListener );
		void							CopyListeners( Messager* inSource ); 
		void							BroadcastMessage( long inMsg, long inArgs[] );
		
		static void						BroadcastGlobalMessage( long inMessage, long inArgs[] );

};

#endif

