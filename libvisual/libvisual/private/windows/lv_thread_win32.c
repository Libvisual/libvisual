/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include <windows.h>


struct _VisThread {
    HANDLE thread;
    DWORD threadId;
};

struct _VisMutex {
    HANDLE handle;
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
    // FIXME: What to do with the joinable flag?

    VisThread *thread;

    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);

    thread = visual_mem_new0 (VisThread, 1);

    thread->thread = CreateThread (NULL, 0, func, (PVOID) data, 0, &thread->threadId);

    if (thread == NULL) {
        visual_log (VISUAL_LOG_ERROR, "Error while creating thread");

        visual_mem_free (thread);

        return NULL;
    }

    return thread;
}

int visual_thread_free (VisThread *thread)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (thread != NULL, -VISUAL_ERROR_THREAD_NULL);

    return visual_mem_free (thread);
}

void *visual_thread_join (VisThread *thread)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);
    visual_return_val_if_fail (thread != NULL, NULL);

    void *result = NULL;
    DWORD thread_result;

    if (WaitForSingleObject(thread->thread, INFINITE) != WAIT_OBJECT_0) {
        visual_log (VISUAL_LOG_ERROR, "Error while joining thread");

        return NULL;
    }

    // FIXME: This is wrong (see thread_exit_win32() comments)
    GetExitCodeThread(thread->thread, &thread_result);

    result = (void *) thread_result;

    return result;
}

void visual_thread_exit (void *retval)
{
    visual_return_if_fail (visual_thread_is_initialized ());

    ExitThread (0);
}

void visual_thread_yield (void)
{
    visual_return_if_fail (visual_thread_is_initialized ());

    SwitchToThread ();
}

VisMutex *visual_mutex_new (void)
{
    VisMutex *mutex;
    HANDLE handle;

    visual_return_val_if_fail (visual_thread_is_initialized (), NULL);

    handle = CreateMutex (NULL, FALSE, NULL);
    if (!handle) {
        return NULL;
    }

    mutex = visual_mem_new0 (VisMutex, 1);
    mutex->handle = handle;

    return mutex;
}

int visual_mutex_free (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    CloseHandle (mutex->handle);

    return visual_mem_free (mutex);
}

int visual_mutex_lock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    return (WaitForSingleObject (mutex->handle, INFINITE) == WAIT_OBJECT_0);
}

int visual_mutex_trylock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    return (WaitForSingleObject (mutex->handle, 0) == WAIT_OBJECT_0);
}

int visual_mutex_unlock (VisMutex *mutex)
{
    visual_return_val_if_fail (visual_thread_is_initialized (), -VISUAL_ERROR_THREAD_NOT_INITIALIZED);
    visual_return_val_if_fail (mutex != NULL, -VISUAL_ERROR_MUTEX_NULL);

    ReleaseMutex (mutex->handle);

    return 0;
}
