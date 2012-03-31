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

#include "config.h"
#include "lv_thread.h"
#include "lv_common.h"
#include "gettext.h"

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
#ifdef VISUAL_THREAD_MODEL_POSIX
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
#endif

/* Windows32 implementation */
#ifdef VISUAL_THREAD_MODEL_WIN32
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
#endif

/* GThread implementation */
#ifdef VISUAL_THREAD_MODEL_GTHREAD2
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
#endif

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

int visual_thread_is_initialized ()
{
	return __lv_thread_initialized;
}

void visual_thread_enable (int enabled)
{
	__lv_thread_enabled = enabled > 0 ? TRUE : FALSE;
}

int visual_thread_is_enabled (void)
{
	return __lv_thread_enabled;
}

int visual_thread_is_supported ()
{
	return __lv_thread_supported;
}

VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.thread_create (func, data, joinable);
}
int visual_thread_free (VisThread *thread)
{
	visual_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

	if (visual_thread_is_supported () == FALSE) {
		visual_log (VISUAL_LOG_WARNING, _("Tried freeing thread memory while threading is not supported, simply freeing mem"));

		return visual_mem_free (thread);
	}

	return __lv_thread_funcs.thread_free (thread);
}

void *visual_thread_join (VisThread *thread)
{
	visual_return_val_if_fail (thread != NULL, NULL);

	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.thread_join (thread);
}

void visual_thread_exit (void *retval)
{
	visual_return_if_fail (visual_thread_is_initialized () != FALSE);
	visual_return_if_fail (visual_thread_is_supported () != FALSE);
	visual_return_if_fail (visual_thread_is_enabled () != FALSE);

	__lv_thread_funcs.thread_exit (retval);
}

void visual_thread_yield ()
{
	visual_return_if_fail (visual_thread_is_initialized () != FALSE);
	visual_return_if_fail (visual_thread_is_supported () != FALSE);
	visual_return_if_fail (visual_thread_is_enabled () != FALSE);

	__lv_thread_funcs.thread_yield ();
}

VisMutex *visual_mutex_new ()
{
	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, NULL);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, NULL);

	return __lv_thread_funcs.mutex_new ();
}

int visual_mutex_free (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	if (visual_thread_is_supported () == FALSE) {
		visual_log (VISUAL_LOG_WARNING, _("Tried freeing mutex memory while threading is not supported, simply freeing mem"));

		return visual_mem_free (mutex);
	}

	return __lv_thread_funcs.mutex_free (mutex);
}

int visual_mutex_init (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_init (mutex);
}

int visual_mutex_lock (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_lock (mutex);
}

int visual_mutex_trylock (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_trylock (mutex);
}

int visual_mutex_unlock (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	visual_return_val_if_fail (visual_thread_is_initialized () != FALSE, -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
	visual_return_val_if_fail (visual_thread_is_supported () != FALSE, -VISUAL_ERROR_THREAD_NOT_SUPPORTED);
	visual_return_val_if_fail (visual_thread_is_enabled () != FALSE, -VISUAL_ERROR_THREAD_NOT_ENABLED);

	return __lv_thread_funcs.mutex_unlock (mutex);
}


/* Native implementations */

#ifdef VISUAL_THREAD_MODEL_POSIX

/* Posix implementation */
static VisThread *thread_create_posix (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;

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
		visual_log (VISUAL_LOG_ERROR, _("Error while creating thread"));

		visual_mem_free (thread);

		return NULL;
	}

	return thread;
}

static int thread_free_posix (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_posix (VisThread *thread)
{
	void *result = NULL;

	if (pthread_join (thread->thread, &result) < 0) {
		visual_log (VISUAL_LOG_ERROR, _("Error while joining thread"));

		return NULL;
	}

	return result;
}

static void thread_exit_posix (void *retval)
{
	pthread_exit (retval);
}

static void thread_yield_posix ()
{
	sched_yield ();
}


static VisMutex *mutex_new_posix ()
{
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	pthread_mutex_init (&mutex->mutex, NULL);

	return mutex;
}

static int mutex_free_posix (VisMutex *mutex)
{
	return visual_mem_free (mutex);
}

static int mutex_init_posix (VisMutex *mutex)
{
	visual_mem_set (mutex, 0, sizeof (VisMutex));

	pthread_mutex_init (&mutex->mutex, NULL);

	return VISUAL_OK;
}

static int mutex_lock_posix (VisMutex *mutex)
{
	if (pthread_mutex_lock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;

	return VISUAL_OK;
}

static int mutex_trylock_posix (VisMutex *mutex)
{
	if (pthread_mutex_trylock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE;

	return VISUAL_OK;
}

static int mutex_unlock_posix (VisMutex *mutex)
{
	if (pthread_mutex_unlock (&mutex->mutex) < 0)
		return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;

	return VISUAL_OK;
}

#endif // VISUAL_THREAD_MODEL_POSIX

/* Windows32 implementation */
#ifdef VISUAL_THREAD_MODEL_WIN32

static VisThread *thread_create_win32 (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;

	thread = visual_mem_new0 (VisThread, 1);

	thread->thread = CreateThread (NULL, 0, func, (PVOID) data, 0, &thread->threadId);

	if (thread == NULL) {
		visual_log (VISUAL_LOG_ERROR, "Error while creating thread");

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

	return thread;
}

static int thread_free_win32 (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_win32 (VisThread *thread)
{
	void *result = NULL;
	DWORD thread_result;

	if (WaitForSingleObject(thread->thread, INFINITE) != WAIT_OBJECT_0) {
		visual_log (VISUAL_LOG_ERROR, "Error while joining thread");

		return NULL;
	}

	GetExitCodeThread(thread->thread, &thread_result);

	result = (void *) thread_result;

	return result;
}

static void thread_exit_win32 (void *retval)
{
}

static void thread_yield_win32 ()
{
}


static VisMutex *mutex_new_win32 ()
{
    return 0;
}

static int mutex_free_win32 (VisMutex *mutex)
{
    return 0;
}

static int mutex_init_win32 (VisMutex *mutex)
{
    return 0;
}

static int mutex_lock_win32 (VisMutex *mutex)
{
    return 0;
}

static int mutex_trylock_win32 (VisMutex *mutex)
{
    return 0;
}

static int mutex_unlock_win32 (VisMutex *mutex)
{
    return 0;
}

#endif /* VISUAL_THREAD_MODEL_WIN32 */

/* GThread implementation */
#ifdef VISUAL_THREAD_MODEL_GTHREAD2

static VisThread *thread_create_gthread (VisThreadFunc func, void *data, int joinable)
{
	VisThread *thread = NULL;

	thread = visual_mem_new0 (VisThread, 1);

	thread->thread = g_thread_create (func, data, joinable, NULL);

	if (thread->thread == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Error while creating thread"));

		visual_mem_free (thread);

		return NULL;
	}

	return thread;
}

static int thread_free_gthread (VisThread *thread)
{
	return visual_mem_free (thread);
}

static void *thread_join_gthread (VisThread *thread)
{
	gpointer result;

	visual_return_val_if_fail (thread->thread != NULL, NULL);

	result = g_thread_join (thread->thread);

	return result;
}

static void thread_exit_gthread (void *retval)
{
	g_thread_exit (retval);
}

static void thread_yield_gthread ()
{
	g_thread_yield ();
}


static VisMutex *mutex_new_gthread ()
{
	VisMutex *mutex;

	mutex = visual_mem_new0 (VisMutex, 1);

	mutex->static_mutex_used = FALSE;

	mutex->mutex = g_mutex_new ();

	return mutex;
}

static int mutex_free_gthread (VisMutex *mutex)
{
	visual_return_val_if_fail (mutex->mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

	g_mutex_free (mutex->mutex);

	return visual_mem_free (mutex);
}

static int mutex_init_gthread (VisMutex *mutex)
{
	mutex->static_mutex_used = TRUE;

	g_static_mutex_init (&mutex->static_mutex);

	return VISUAL_OK;
}

static int mutex_lock_gthread (VisMutex *mutex)
{
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_lock (&mutex->static_mutex);
	else
		g_mutex_lock (mutex->mutex);

	return VISUAL_OK;
}

static int mutex_trylock_gthread (VisMutex *mutex)
{
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_trylock (&mutex->static_mutex);
	else
		g_mutex_trylock (mutex->mutex);

	return VISUAL_OK;
}

static int mutex_unlock_gthread (VisMutex *mutex)
{
	if (mutex->static_mutex_used == TRUE)
		g_static_mutex_unlock (&mutex->static_mutex);
	else
		g_mutex_unlock (mutex->mutex);

	return VISUAL_OK;
}

#endif // VISUAL_THREAD_MODEL_GTHREAD2
