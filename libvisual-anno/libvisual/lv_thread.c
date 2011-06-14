/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_thread.c,v 1.19 2006/02/13 20:54:08 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include "lvconfig.h"

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

#include "lv_log.h"
#include "lv_thread.h"

/* FIXME add more threading backends here:
 * native windows	(says nuff)
 */


typedef struct _ThreadFuncs ThreadFuncs;

typedef VisThread *(*ThreadFuncCreate)(VisThreadFunc func, void *data, int joinable);
typedef int (*ThreadFuncFree)(VisThread *thread);
typedef void *(*ThreadFuncJoin)(VisThread *thread);
typedef void (*ThreadFuncExit)(void *retval);
typedef void (*ThreadFuncYield)(void);

typedef VisMutex *(*MutexFuncNew)(void);
typedef int (*MutexFuncFree)(VisMutex *mutex);
typedef int (*MutexFuncInit)(VisMutex *mutex);
typedef int (*MutexFuncLock)(VisMutex *mutex);
typedef int (*MutexFuncTrylock)(VisMutex *mutex);
typedef int (*MutexFuncUnlock)(VisMutex *mutex);

struct _ThreadFuncs {
	ThreadFuncCreate	thread_create;
	ThreadFuncFree		thread_free;
	ThreadFuncJoin		thread_join;
	ThreadFuncExit		thread_exit;
	ThreadFuncYield		thread_yield;

	MutexFuncNew		mutex_new;
	MutexFuncFree		mutex_free;
	MutexFuncInit		mutex_init;
	MutexFuncLock		mutex_lock;
	MutexFuncTrylock	mutex_trylock;
	MutexFuncUnlock		mutex_unlock;
};

/* Internal variables */
static int __lv_thread_initialized = FALSE;
static int __lv_thread_supported = FALSE;
static int __lv_thread_enabled = TRUE;
static ThreadFuncs __lv_thread_funcs;

/* Posix implementation */
static VisThread *thread_create_posix (VisThreadFunc func, void *data, int joinable);
static int thread_free_posix (VisThread *thread);
static void *thread_join_posix (VisThread *thread);
static void thread_exit_posix (void *retval);
static void thread_yield_posix (void);

static VisMutex *mutex_new_posix (void);
static int mutex_free_posix (VisMutex *mutex);
static int mutex_init_posix (VisMutex *mutex);
static int mutex_lock_posix (VisMutex *mutex);
static int mutex_trylock_posix (VisMutex *mutex);
static int mutex_unlock_posix (VisMutex *mutex);

/* Windows32 implementation */
static VisThread *thread_create_win32 (VisThreadFunc func, void *data, int joinable);
static int thread_free_win32 (VisThread *thread);
static void *thread_join_win32 (VisThread *thread);
static void thread_exit_win32 (void *retval);
static void thread_yield_win32 (void);

static VisMutex *mutex_new_win32 (void);
static int mutex_free_win32 (VisMutex *mutex);
static int mutex_init_win32 (VisMutex *mutex);
static int mutex_lock_win32 (VisMutex *mutex);
static int mutex_trylock_win32 (VisMutex *mutex);
static int mutex_unlock_win32 (VisMutex *mutex);

/* GThread implementation */
static VisThread *thread_create_gthread (VisThreadFunc func, void *data, int joinable);
static int thread_free_gthread (VisThread *thread);
static void *thread_join_gthread (VisThread *thread);
static void thread_exit_gthread (void *retval);
static void thread_yield_gthread (void);

static VisMutex *mutex_new_gthread (void);
static int mutex_free_gthread (VisMutex *mutex);
static int mutex_init_gthread (VisMutex *mutex);
static int mutex_lock_gthread (VisMutex *mutex);
static int mutex_trylock_gthread (VisMutex *mutex);
static int mutex_unlock_gthread (VisMutex *mutex);


/**
 * @defgroup VisThread VisThread
 * @{
 */

/**
 * Initializes the VisThread subsystem. This function needs to be called before VisThread can be used. Also
 * this function is called from within visual_init().
 * 
 * @return TRUE if initialized, FALSE if not initialized.
 */
int visual_thread_initialize ()
{
	__lv_thread_initialized = TRUE;

#ifdef VISUAL_HAVE_THREADS

#ifdef VISUAL_THREAD_MODEL_POSIX
	__lv_thread_supported = TRUE;
	
	__lv_thread_funcs.thread_create = thread_create_posix;
	__lv_thread_funcs.thread_free = thread_free_posix;
	__lv_thread_funcs.thread_join = thread_join_posix;
	__lv_thread_funcs.thread_exit = thread_exit_posix;
	__lv_thread_funcs.thread_yield = thread_yield_posix;

	__lv_thread_funcs.mutex_new = mutex_new_posix;
	__lv_thread_funcs.mutex_free = mutex_free_posix;
	__lv_thread_funcs.mutex_init = mutex_init_posix;
	__lv_thread_funcs.mutex_lock = mutex_lock_posix;
	__lv_thread_funcs.mutex_trylock = mutex_trylock_posix;
	__lv_thread_funcs.mutex_unlock = mutex_unlock_posix;

	return TRUE;
#elif defined(VISUAL_THREAD_MODEL_WIN32) /* !VISUAL_THREAD_MODEL_POSIX */
	__lv_thread_supported = TRUE;

	__lv_thread_funcs.thread_create = thread_create_win32;
	__lv_thread_funcs.thread_free = thread_free_win32;
	__lv_thread_funcs.thread_join = thread_join_win32;
	__lv_thread_funcs.thread_exit = thread_exit_win32;
	__lv_thread_funcs.thread_yield = thread_yield_win32;

	__lv_thread_funcs.mutex_new = mutex_new_win32;
	__lv_thread_funcs.mutex_free = mutex_free_win32;
	__lv_thread_funcs.mutex_init = mutex_init_win32;
	__lv_thread_funcs.mutex_lock = mutex_lock_win32;
	__lv_thread_funcs.mutex_trylock = mutex_trylock_win32;
	__lv_thread_funcs.mutex_unlock = mutex_unlock_win32;

	return TRUE;
#elif defined(VISUAL_THREAD_MODEL_GTHREAD2) /* !VISUAL_THREAD_MODEL_WIN32 */
	__lv_thread_supported = TRUE;

	__lv_thread_funcs.thread_create = thread_create_gthread;
	__lv_thread_funcs.thread_free = thread_free_gthread;
	__lv_thread_funcs.thread_join = thread_join_gthread;
	__lv_thread_funcs.thread_exit = thread_exit_gthread;
	__lv_thread_funcs.thread_yield = thread_yield_gthread;

	__lv_thread_funcs.mutex_new = mutex_new_gthread;
	__lv_thread_funcs.mutex_free = mutex_free_gthread;
	__lv_thread_funcs.mutex_init = mutex_init_gthread;
	__lv_thread_funcs.mutex_lock = mutex_lock_gthread;
	__lv_thread_funcs.mutex_trylock = mutex_trylock_gthread;
	__lv_thread_funcs.mutex_unlock = mutex_unlock_gthread;

	return TRUE;
#else /* !VISUAL_THREAD_MODEL_GTHREAD2 */
	return FALSE;
#endif 
#else
	return FALSE;
#endif /* VISUAL_HAVE_THREADS */

}

/**
 * Request if VisThread is initialized or not. This function should not be confused with visual_thread_is_supported().
 *
 * @return TRUE if initialized, FALSE if not initialized.
 */
int visual_thread_is_initialized ()
{
	return __lv_thread_initialized;
}

/**
 * Enable or disable threading support. This can be used to disallow threads, which might be needed in some environments.
 * 
 * @see visual_thread_is_enabled
 *
 * @param enabled TRUE to enable threads, FALSE to disable threads.
 */
void visual_thread_enable (int enabled)
{
	__lv_thread_enabled = enabled > 0 ? TRUE : FALSE;
}

/**
 * Request if threads are enabled or not. This function should not be confused with visual_thread_is_supported().
 *
 * @return TRUE if enabled, FALSE if disabled.
 */
int visual_thread_is_enabled (void)
{
	return __lv_thread_enabled;
}

/**
 * Is used to check if threading is supported. When threading is used this should always
 * be checked, it's possible to disable threads from within the code, so no \#ifdefs should
 * be used.
 *
 * @return TRUE if threading is supported or FALSE if not.
 */
int visual_thread_is_supported ()
{
	return __lv_thread_supported;
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
	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.thread_create (func, data, joinable);
}

/**
 * After a VisThread is not needed anylonger it needs to be freed using this function.
 *
 * @param thread The VisThread that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_THREAD_NULL or
 *	error values returned by visual_mem_free on failure.
 */
int visual_thread_free (VisThread *thread)
{
	visual_log_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	if (visual_thread_is_supported () == FALSE) {
		visual_log (VISUAL_LOG_WARNING, _("Tried freeing thread memory while threading is not supported, simply freeing mem"));

		return visual_mem_free (thread);
	}

	return __lv_thread_funcs.thread_free (thread);
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
	visual_log_return_val_if_fail (thread != NULL, NULL);

	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.thread_join (thread);
}

/**
 * Exits a VisThread, this will terminate the thread.
 *
 * @param retval The return value that is catched by the visual_thread_join function.
 */
void visual_thread_exit (void *retval)
{
	visual_log_return_if_fail (visual_thread_is_initialized () != FALSE);
	visual_log_return_if_fail (visual_thread_is_supported () != FALSE);
	visual_log_return_if_fail (visual_thread_is_enabled () != FALSE);
	
	return __lv_thread_funcs.thread_exit (retval);
}

/**
 * Yield the current VisThread so another gets time.
 */
void visual_thread_yield ()
{
	visual_log_return_if_fail (visual_thread_is_initialized () != FALSE);
	visual_log_return_if_fail (visual_thread_is_supported () != FALSE);
	visual_log_return_if_fail (visual_thread_is_enabled () != FALSE);
	
	return __lv_thread_funcs.thread_yield ();
}

/**
 * Creates a new VisMutex that is used to do thread locking so data can be synchronized.
 *
 * @return A newly allocated VisMutex that can be used with the visual_mutex_lock and
 *	visual_mutex_unlock functions or NULL on failure.
 */
VisMutex *visual_mutex_new ()
{
	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.mutex_new ();
}

/**
 * A VisMutex is allocated to have more flexibility with the actual thread backend. Thus they need
 * to be freed as well.
 *
 * @param mutex Pointer to the VisMutex that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL on failure.
 */
int visual_mutex_free (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (visual_thread_is_supported () == FALSE) {
		visual_log (VISUAL_LOG_WARNING, _("Tried freeing mutex memory while threading is not supported, simply freeing mem"));

		return visual_mem_free (mutex);
	}

	return __lv_thread_funcs.mutex_free (mutex);
}

/**
 * A VisMutex that has not been allocated using visual_mutex_new () can be initialized using this function. You can
 * use non allocated VisMutex variables in this context by giving a reference to them.
 *
 * @param mutex Pointer to the VisMutex which needs to be initialized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL,  -VISUAL_ERROR_THREAD_NOT_INTIALIZED,
 * 	-VISUAL_ERROR_THREAD_NOT_SUPPORTED or -VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_init (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);
	
	return __lv_thread_funcs.mutex_init (mutex);
}

/**
 * Locks a VisMutex, with the VisMutex locks checked right, only one thread can access the area at once.
 *	This will block if the thread is already locked.
 *
 * @param mutex Pointer to the VisMutex to register the lock.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_LOCK_FAILURE,
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED,
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_lock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_lock (mutex);
}

/**
 * Tries to lock a VisMutex, however instead of visual_mutex_lock it does not block on failure. but returns
 * instead with -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE as error value.
 *
 * @param mutex Pointer to the VisMutex that needs to be locked.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE,
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED or
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_trylock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_trylock (mutex);
}

/**
 * Unlocks a VisMutex so other threads that use the same lock can now enter the critical area.
 *
 * @param mutex Pointer to the VisMutex that is unlocked.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MUTEX_NULL, -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE.
 *	-VISUAL_ERROR_THREAD_NOT_INITIALIZED, -VISUAL_ERROR_THREAD_NOT_SUPPORTED or 
 *	-VISUAL_ERROR_THREAD_NOT_ENABLED on failure.
 */
int visual_mutex_unlock (VisMutex *mutex)
{
	visual_log_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_log_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_log_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_log_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_unlock (mutex);
}

/**
 * @}
 */


/* Native implementations */

/* Posix implementation */
static VisThread *thread_create_posix (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;
#ifdef VISUAL_THREAD_MODEL_POSIX

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
		visual_log (VISUAL_LOG_CRITICAL, _("Error while creating thread"));

		visual_mem_free (thread);

		return NULL;
	}
#endif

	return thread;
}

static int thread_free_posix (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_posix (VisThread *thread)
{
	void *result = NULL;
#ifdef VISUAL_THREAD_MODEL_POSIX

	if (pthread_join (thread->thread, &result) < 0) {
		visual_log (VISUAL_LOG_CRITICAL, _("Error while joining thread"));

		return NULL;
	}
#endif

	return result;
}

static void thread_exit_posix (void *retval)
{
#ifdef VISUAL_THREAD_MODEL_POSIX
	pthread_exit (retval);
#endif
}

static void thread_yield_posix ()
{
#ifdef VISUAL_THREAD_MODEL_POSIX
	sched_yield ();
#endif
}


static VisMutex *mutex_new_posix ()
{
	VisMutex *mutex;
#ifdef VISUAL_THREAD_MODEL_POSIX

	mutex = visual_mem_new0 (VisMutex, 1);

	pthread_mutex_init (&mutex->mutex, NULL);
#endif

	return mutex;
}

static int mutex_free_posix (VisMutex *mutex)
{
	return visual_mem_free (mutex);
}

static int mutex_init_posix (VisMutex *mutex)
{
	visual_mem_set (mutex, 0, sizeof (VisMutex));
#ifdef VISUAL_THREAD_MODEL_POSIX

	pthread_mutex_init (&mutex->mutex, NULL);
#endif

	return VISUAL_OK;
}

static int mutex_lock_posix (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_POSIX
	if (pthread_mutex_lock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;
#endif

	return VISUAL_OK;
}

static int mutex_trylock_posix (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_POSIX
	if (pthread_mutex_trylock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE;
#endif

	return VISUAL_OK;
}

static int mutex_unlock_posix (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_POSIX
	if (pthread_mutex_unlock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;
#endif

	return VISUAL_OK;
}

/* Windows32 implementation */
static VisThread *thread_create_win32 (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;
#ifdef VISUAL_THREAD_MODEL_WIN32
	
	thread = visual_mem_new0 (VisThread, 1);

	thread->thread = CreateThread (NULL, 0, func, (PVOID) data, 0, &thread->threadId);

	if (thread == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while creating thread");

		visual_mem_free (thread);

		return NULL;
	}
/*
	printf("Waiting for thread to finish...\n");
	if (WaitForSingleObject(a_thread, INFINITE) != WAIT_OBJECT_0) {
		perror("Thread join failed");
		exit(EXIT_FAILURE);
	}
*/
	// Retrieve the code returned by the thread.
	//     GetExitCodeThread(a_thread, &thread_result);
#endif

	return thread;
}

static int thread_free_win32 (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_win32 (VisThread *thread)
{
	void *result = NULL;
#ifdef VISUAL_THREAD_MODEL_WIN32
	DWORD thread_result;

	if (WaitForSingleObject(thread->thread, INFINITE) != WAIT_OBJECT_0) {
		visual_log (VISUAL_LOG_CRITICAL, "Error while joining thread");

		return NULL;
	}

	GetExitCodeThread(thread->thread, &thread_result);

	result = (void *) thread_result;
#endif

	return result;
}

static void thread_exit_win32 (void *retval)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static void thread_yield_win32 ()
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}


static VisMutex *mutex_new_win32 ()
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static int mutex_free_win32 (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static int mutex_init_win32 (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static int mutex_lock_win32 (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static int mutex_trylock_win32 (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

static int mutex_unlock_win32 (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_WIN32

#endif
}

/* GThread implementation */
static VisThread *thread_create_gthread (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;
#ifdef VISUAL_THREAD_MODEL_GTHREAD2

	thread = visual_mem_new0 (VisThread, 1);

	thread->thread = g_thread_create (func, data, joinable, NULL);

	if (thread->thread == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, _("Error while creating thread"));

		visual_mem_free (thread);

		return NULL;
	}
#endif

	return thread;
}

static int thread_free_gthread (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_gthread (VisThread *thread)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	gpointer result;

	visual_log_return_val_if_fail (thread->thread != NULL, NULL);

	result = g_thread_join (thread->thread);
	
	return result;
#else
	return NULL;
#endif
}

static void thread_exit_gthread (void *retval)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	g_thread_exit (retval);
#endif
}

static void thread_yield_gthread ()
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	g_thread_yield ();
#endif
}


static VisMutex *mutex_new_gthread ()
{
	VisMutex *mutex;
	
	mutex = visual_mem_new0 (VisMutex, 1);
	
#ifdef VISUAL_THREAD_MODEL_GTHREAD2	
	mutex->static_mutex_used = FALSE;

	mutex->mutex = g_mutex_new ();
#endif

	return mutex;
}

static int mutex_free_gthread (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	visual_log_return_val_if_fail (mutex->mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	g_mutex_free (mutex->mutex);
#endif

	return visual_mem_free (mutex);
}

static int mutex_init_gthread (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	mutex->static_mutex_used = TRUE;

	g_static_mutex_init (&mutex->static_mutex);
#endif

	return VISUAL_OK;
}

static int mutex_lock_gthread (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_lock (&mutex->static_mutex);
	else
		g_mutex_lock (mutex->mutex);
#endif

	return VISUAL_OK;
}

static int mutex_trylock_gthread (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_trylock (&mutex->static_mutex);
	else
		g_mutex_trylock (mutex->mutex);
#endif

	return VISUAL_OK;
}

static int mutex_unlock_gthread (VisMutex *mutex)
{
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_unlock (&mutex->static_mutex);
	else
		g_mutex_unlock (mutex->mutex);
#endif

	return VISUAL_OK;
}

