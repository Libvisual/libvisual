#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_log.h"
#include "lv_thread.h"


/**
 * @defgroup VisThread VisThread
 * @{
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

int visual_thread_free (VisThread *thread)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	return visual_mem_free (thread);
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif /* VISUAL_HAVE_THREADS */
}

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

void visual_thread_exit (void *retval)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	pthread_exit (retval);
#else /* !VISUAL_THREAD_MODEL_POSIX */

#endif
#endif /* VISUAL_HAVE_THREADS */
}

void visual_thread_yield (void)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX
	sched_yield ();
#else /* !VISUAL_THREAD_MODEL_POSIX */
	
#endif
#endif /* VISUAL_HAVE_THREADS */
}

/* FIXME implement some kind of sched priority someway */
void visual_thread_set_priority (VisThread *thread, VisThreadPriority priority)
{
#ifdef VISUAL_HAVE_THREADS
#ifdef VISUAL_THREAD_MODEL_POSIX

#else /* !VISUAL_THREAD_MODEL_POSIX */

#endif
#endif /* VISUAL_HAVE_THREADS */
}

VisMutex *visual_mutex_new (void)
{
#ifdef VISUAL_HAVE_THREADS
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	return mutex;
#else
	return NULL;
#endif /* VISUAL_HAVE_THREADS */
}

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

