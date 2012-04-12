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

#ifdef VISUAL_THREAD_MODEL_POSIX

#include "gettext.h"
#include <pthread.h>

struct _VisThread {
    pthread_t thread;
};

struct _VisMutex {
    pthread_mutex_t mutex;
};

static int is_initialized = FALSE;

int visual_thread_initialize (void)
{
    is_initialized = TRUE;

    return TRUE;
}

int visual_thread_is_initialized (void)
{
    return is_initialized;
}

int visual_thread_is_supported (void)
{
    return TRUE;
}

VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
    VisThread *thread = NULL;
    pthread_attr_t attr;
    int result;

    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);

    thread = visual_mem_new0 (VisThread, 1);

    pthread_attr_init(&attr);

    if (joinable)
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
    else
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    result = pthread_create (&thread->thread, &attr, func, data);

    pthread_attr_destroy (&attr);

    if (result != 0) {
        visual_log (VISUAL_LOG_ERROR, _("Error while creating thread"));

        visual_mem_free (thread);

        return NULL;
    }

    return thread;
}

int visual_thread_free (VisThread *thread)
{
    visual_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

    return visual_mem_free (thread);
}

void *visual_thread_join (VisThread *thread)
{
    void *result = NULL;

    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);
    visual_return_val_if_fail (thread != NULL, NULL);

    if (pthread_join (thread->thread, &result) < 0) {
        visual_log (VISUAL_LOG_ERROR, _("Error while joining thread"));

        return NULL;
    }

    return result;
}

void visual_thread_exit (void *retval)
{
    visual_return_if_fail (visual_thread_is_initialized ());

    pthread_exit (retval);
}

void visual_thread_yield (void)
{
    visual_return_if_fail (visual_thread_is_initialized ());

    sched_yield ();
}

VisMutex *visual_mutex_new (void)
{
    VisMutex *mutex;

    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);

    mutex = visual_mem_new0 (VisMutex, 1);

    pthread_mutex_init (&mutex->mutex, NULL);

    return mutex;
}

int visual_mutex_free (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    return visual_mem_free (mutex);
}

int visual_mutex_lock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    if (pthread_mutex_lock (&mutex->mutex) < 0)
        return -VISUAL_ERROR_MUTEX_LOCK_FAILURE;

    return VISUAL_OK;
}

int visual_mutex_trylock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    return (pthread_mutex_trylock (&mutex->mutex) == 0);
}

int visual_mutex_unlock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    if (pthread_mutex_unlock (&mutex->mutex) < 0)
        return -VISUAL_ERROR_MUTEX_UNLOCK_FAILURE;

    return VISUAL_OK;
}

#endif /* VISUAL_THREAD_MODEL_POSIX */
