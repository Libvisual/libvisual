
#ifndef _EGTHREAD_
#define _EGTHREAD_

#include "XPtrList.h"


class EgThread {
	
	private:
		static XPtrList				sThreadList;

		char						mOwnedElsewhere;
		char						mStopped;	
		unsigned long int			mResumeTime;
		unsigned long int			mDelay;			
	
	protected:
		unsigned long int			mLastExecuted;
		
		// Callback when this thread is stopped
		virtual void				StopSelf();
	
	public:
		
		//	Post:	This thread is contstructed and SpendTime() will be called every
		//			<inTimeDelay> milliseconds.  When the thread is stopped, it will later
		//			be trash collected (and deleted) if <inOwnedElsewhere> is false.
									EgThread( long inTimeDelay, int inOwnedElsewhere );

		virtual 					~EgThread();
		
		//	Post:	SpendTime() is longer called and the StopSelf() callback is be called
		//			and this thread will be deleted at a later time if this thread isn't owned elsewhere.
		void						Stop();
		
		//	Post:	Returns true if this thread had been terminated.
		bool						IsStopped()			{ return mStopped;	}
		
		// 	Post:	Will not get a SpendTime() until inSleepTime miliseconds from the present
		void						Sleep( long inSleepTime );	
		
		virtual void				Suspend();
		virtual void				Resume();
		
		// Post:	Overide this to run one iteration of your thread. Upon entering, <mLastExecuted> will contain
		//			the present time in milliseconds.
		virtual void				SpendTime() = 0;
		
		// Post:	OS specific class must call DoThreads repeatedly with the current time (in milliseconds)
		static void					DoThreads();
		
		// Post:	Calling this will stop all running threads.  Typically used in low memory situations
		static void					StopAllThreads();
};

#endif
