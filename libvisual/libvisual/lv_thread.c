#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_log.h"
#include "lv_thread.h"

/* FIXME add more threading backends here:
 * glib			(could proof to be a nice fallback when needed)
 * native windows	(says nuff)
 * what is used on Mac os X ?
 */


/**
 * @defgroup VisThread VisThread
 * @{
 */

/**
 * Is used to check if threading is supported. When threading is used this should always
 * be checked, it's possible to disable threads from within the code, so no #ifdefs should
 * be used.
 *
 * @return TRUE if threading is supported or FALSE if not.
 */
int visual_thread_is_supported ()
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	return TRUE;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return FALSE;
#endif
#else
	return FALSE;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Creates a new VisThread that is used in threading.
 *
 * @param func The threading function.
 * @param data The private data that is send to the threading function.
 * @param joinable Flag that contains whatever the thread can be joined or not.
 *
 * @return A newly allocated VisThread, or NULL on failure.
 */
VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	VisThread *thread;
	pthread_attr_t attr;
	int res;

	thread = visual_mem_new0 (VisThread, 1);

	pthread_attr_init(&attr);

	if (joinable == TRUE)
		pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
	else
		pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	res = pthread_create (&thread->thread, &attr, func, data);

	pthread_attr_destroy (&attr);

	if (res != 0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while creating thread");

		visual_mem_free (thread);

		return NULL;
	}

	return thread;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return NULL;
#endif 
#else
	return NULL;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * After a VisThread is not needed anylonger it needs to be freed using this function.
 *
 * @param thread The VisThread that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_THREAD_NULL, -VISUAL_ERROR_THREAD_NO_THREADING or
 *	error values returned by visual_mem_free on failure.
 */
int visual_thread_free (VisThread *thread)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	return visual_mem_free (thread);
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Joins a VisThread with another.
 *
 * @param thread The VisThread that is about to be joined.
 *
 * @return Possible result that was passed to visual_thread_exit as retval or
 *	NULL.
 */
void *visual_thread_join (VisThread *thread)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	void *result;

	if (thread == NULL)
		return NULL;

	if (pthread_join (thread->thread, &result) < 0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while joining thread");

		return NULL;
	}

	return result;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return NULL;
#endif
#else
	return NULL;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Exits a VisThread, this will terminate the thread.
 *
 * @param retval The return value that is catched by the visual_thread_join function.
 */
void visual_thread_exit (void *retval)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	pthread_exit (retval);
#else /* !VISUAL_THREAD_MODEL_POSIX */

#endif
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Yield the current VisThread so another gets time.
 */
void visual_thread_yield ()
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	sched_yield ();
#else /* !VISUAL_THREAD_MODEL_POSIX */
	
#endif
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Creates a new VisMutex that is used to do thread locking so data can be synchronized.
 *
 * @return A newly allocated VisMutex that can be used with the visual_mutex_lock and
 *	visual_mutex_unlock functions or NULL on failure.
 */
VisMutex *visual_mutex_new ()
{
#ifdef VISUAL_HAVE_THREADS
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	return mutex;
#else
	return NULL;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Locks a VisMutex, with the VisMutex locks checked right, only one thread can access the area at once.
 *	This will block if the thread is already locked.
 *
 * @param mutex Pointer to the VisMutex to register the lock.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_LOCK_FAILURE or
 *	-VISUAL_ERROR_THREAD_NO_THREADING on failure.
 */
int visual_mutex_lock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_lock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;

	return VISUAL_OK;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Tries to lock a VisMutex, however instead of visual_mutex_lock it does not block on failure. but returns
 * instead with -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE as error value.
 *
 * @param mutex Pointer to the VisMutex that needs to be locked.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE or
 *	-VISUAL_ERROR_THREAD_NO_THREADING on failure.
 */
int visual_mutex_trylock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_trylock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE;

	return VISUAL_OK;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * Unlocks a VisMutex so other threads that use the same lock can now enter the critical area.
 *
 * @param mutex Pointer to the VisMutex that is unlocked.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE or
 *	-VISUAL_ERROR_THREAD_NO_THREADING on failure.
 */
int visual_mutex_unlock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_unlock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;

	return VISUAL_OK;
#else /* !VISUAL_THREAD_MODEL_POSIX */
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

/* FIXME this needs to go, somehow, we can't start mallocing freeing for every lock/unlock we do. */
/**
 * A VisMutex is allocated to have more flexibility with the actual thread backend. Thus they need
 * to be freed as well.
 *
 * @param mutex Pointer to the VisMutex that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL or -VISUAL_ERROR_THREAD_NO_THREADING on failure.
 */
int visual_mutex_free (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	return visual_mem_free (mutex);
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

/**
 * @}
 */

