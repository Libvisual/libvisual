#include "EgThread.h"


#ifdef EG_MAC
#include <Events.h>
#define __millisTime		::TickCount() * 17
#else
#include <windows.h>
#define __millisTime		timeGetTime()
#endif

XPtrList	EgThread::sThreadList;


EgThread::EgThread( long inTimeDelay, int inOwnedElsewhere ) :
	mOwnedElsewhere( inOwnedElsewhere ),
	mStopped( false ),
	mDelay( inTimeDelay ),
	mLastExecuted( 0 ),
	mResumeTime( 0 ) {
	
	sThreadList.Add( this );
}


EgThread::~EgThread() {

	sThreadList.Remove( this );
}



void EgThread::DoThreads() {
	long		time = __millisTime;
	EgThread*	threadPtr;
	int			i, n = sThreadList.Count();
	
	for ( i = 1; i <= n; i++ ) {
		threadPtr = (EgThread*) sThreadList.Fetch( i );
		if ( ! threadPtr -> mStopped ) {
			if ( time >= threadPtr -> mResumeTime ) {
				threadPtr -> mLastExecuted = time;
				threadPtr -> mResumeTime = time + threadPtr -> mDelay;
				threadPtr -> SpendTime();
			} }
		else {
			sThreadList.RemoveElement( i );
			if ( ! threadPtr -> mOwnedElsewhere )
				delete threadPtr;
			i--;
			n--;
		}
	}
}


void EgThread::Sleep( long inSleepTime ) {
	mResumeTime = __millisTime + inSleepTime;
}


void EgThread::StopAllThreads() {
	EgThread*	threadPtr;
	int			i, n = sThreadList.Count();
	
	// Stop all the threads first
	for ( i = 1; i <= n; i++ ) {
		threadPtr = (EgThread*) sThreadList.Fetch( i );
		threadPtr -> Stop();
	}

	// Then delete each one we own
	for ( i = 1; i <= n; i++ ) {
		threadPtr = (EgThread*) sThreadList.Fetch( i );	
		if ( ! threadPtr -> mOwnedElsewhere )
			delete threadPtr;
	}	
	
	sThreadList.RemoveAll();
}


void EgThread::Suspend() {
	mResumeTime = 0xFFFFFFFF;
}


void EgThread::Resume() {
	mResumeTime = 0;
}
		
		
void EgThread::Stop() {

	if ( ! mStopped ) {
		mStopped = true;
		StopSelf();
	}
}


void EgThread::StopSelf() {


}

