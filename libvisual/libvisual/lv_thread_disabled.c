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

int visual_thread_initialize (void)
{
    return FALSE;
}

int visual_thread_is_initialized (void)
{
    return FALSE;
}

int visual_thread_is_supported (void)
{
    return FALSE;
}

VisThread *visual_thread_create (VisThreadFunc func, void *data, int joinable)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return NULL;
}

int visual_thread_free (VisThread *thread)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return -VISUAL_ERROR_THREAD_NOT_SUPPORTED;
}

void *visual_thread_join (VisThread *thread)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return NULL;
}

void visual_thread_exit (void *retval)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");
}

void visual_thread_yield (void)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");
}

VisMutex *visual_mutex_new (void)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return NULL;
}

int visual_mutex_free (VisMutex *mutex)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return -VISUAL_ERROR_THREAD_NOT_SUPPORTED;
}

int visual_mutex_lock (VisMutex *mutex)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return -VISUAL_ERROR_THREAD_NOT_SUPPORTED;
}

int visual_mutex_trylock (VisMutex *mutex)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return -VISUAL_ERROR_THREAD_NOT_SUPPORTED;
}

int visual_mutex_unlock (VisMutex *mutex)
{
    visual_log (VISUAL_LOG_ERROR, "Threading is not supported");

    return -VISUAL_ERROR_THREAD_NOT_SUPPORTED;
}
