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

	return TRUE;
}

VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
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
}

int visual_thread_free (VisThread *thread)
{
	visual_log_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	return visual_mem_free (thread);
}

void *visual_thread_join (VisThread *thread)
{
	void *result;

	if (thread == NULL)
		return NULL;

	if (pthread_join (&thread->thread, &result) < 0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while joining thread");

		return NULL;
	}

	return result;
}

void visual_thread_exit (void *retval)
{
	pthread_exit (retval);
}

void visual_thread_yield (void)
{
	sched_yield ();
}

void visual_thread_set_priority (VisThread *thread, VisThreadPriority priority)
{
	
}

VisMutex *visual_mutex_new (void)
{
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	return mutex;
}

int visual_mutex_lock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_lock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;

	return VISUAL_OK;
}

int visual_mutex_trylock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_trylock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE;

	return VISUAL_OK;
}

int visual_mutex_unlock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (pthread_mutex_unlock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;

	return VISUAL_OK;
}

int visual_mutex_free (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	return visual_mem_free (mutex);
}

/**
 * @}
 */

