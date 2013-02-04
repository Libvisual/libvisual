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
 * Functions that want to handle libvisual errors must match this signature. The standard
 * libvisual error handler aborts the program after an error by raise(SIGTRAP). If it's
 * desired to override this use visual_set_error_handler to set your own error handler.
 *
 * @see visual_set_error_handler
 *
 * @arg priv Private field to be used by the client. The library will never touch this.
 */
typedef void (*VisErrorHandlerFunc) (const char *error_msg, void *priv);

LV_BEGIN_DECLS

/**
 * Raise a libvisual error. With the standard error handler this will
 * do a raise(SIGTRAP). You can set your own error handler function using the
 * visual_error_set_handler.
 *
 * @see visual_error_set_handler
 */
LV_API void visual_error_raise (const char *error_msg);

/**
 * Sets the error handler callback. By using this function you
 * can override libvisual it's default error handler.
 *
 * @param handler The error handler which you want to use
 *      to handle libvisual errors.
 * @param priv Optional private data which could be needed in the
 *      error handler that has been set.
 */
LV_API void visual_error_set_handler (VisErrorHandlerFunc handler, void *priv);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_ERROR_H */
