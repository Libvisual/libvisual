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
	#else
	return FALSE;
	#endif
#else
	return FALSE
#endif
}

/* FIXME still encapsulate code in the MODEL_POSIX stuff */
VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
#ifdef VISUAL_HAVE_THREADS
	#ifdef VISUAL_THREAD_MODEL_POSIX
	VisThread *thread;
	int res;

	thread = visual_mem_new0 (VisThread, 1);

	/* FIXME implement joinable */
	res = pthread_create (&thread->thread, NULL, func, data);

	if (res != 0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while creating thread");

		visual_mem_free (thread);

		return NULL;
	}

	return thread;
	#else
	return NULL
	#endif
#else
	return NULL
#endif
}

int visual_thread_free (VisThread *thread)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	return visual_mem_free (thread);
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
}

void *visual_thread_join (VisThread *thread)
{
#ifdef VISUAL_HAVE_THREADS
	void *result;

	if (thread == NULL)
		return NULL;

	if (pthread_join (thread->thread, &result) < 0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while joining thread");

		return NULL;
	}

	return result;
#else
	return NULL
#endif
}

void visual_thread_exit (void *retval)
{
#ifdef VISUAL_HAVE_THREADS
	pthread_exit (retval);
#endif
}

void visual_thread_yield (void)
{
#ifdef VISUAL_HAVE_THREADS
	sched_yield ();
#endif
}

void visual_thread_set_priority (VisThread *thread, VisThreadPriority priority)
{
#ifdef VISUAL_HAVE_THREADS

#endif
}

VisMutex *visual_mutex_new (void)
{
#ifdef VISUAL_HAVE_THREADS
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	return mutex;
#else
	return NULL;
#endif
}

int visual_mutex_lock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_lock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;

	return VISUAL_OK;
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
}

int visual_mutex_trylock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_trylock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE;

	return VISUAL_OK;
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
}

int visual_mutex_unlock (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_unlock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;

	return VISUAL_OK;
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
}

int visual_mutex_free (VisMutex *mutex)
{
#ifdef VISUAL_HAVE_THREADS
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	return visual_mem_free (mutex);
#else
	return -VISUAL_ERROR_THREAD_NO_THREADING;
#endif
}

/**
 * @}
 */

