/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

#ifndef _LV_ERROR_H
#define _LV_ERROR_H

#include <libvisual/lv_defines.h>

/**
 * @defgroup VisError VisError
 * @{
 */

/**
 * Function signature and type for error handlers.
 *
 * @see visual_error_set_handler()
 *
 * @param error_msg Error message
 * @param priv      Optional private data passed to handler on invocation
 */
typedef void (*VisErrorHandlerFunc) (const char *error_msg, void *priv);

LV_BEGIN_DECLS

/**
 * Raises an error in LV.
 *
 * @note By default, LV will raise the SIGTRAP signal on POSIX systems.
 *
 * @see visual_error_set_handler()
 */
LV_API void visual_error_raise (const char *error_msg);

/**
 * Sets the error handler callback.
 *
 * Use this to override LV's default error handler.
 *
 * @param handler Custom error handler
 * @param priv    Optional private data to pass to handler on invocation
 */
LV_API void visual_error_set_handler (VisErrorHandlerFunc handler, void *priv);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_ERROR_H */
