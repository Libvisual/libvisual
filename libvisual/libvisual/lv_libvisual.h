/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_libvisual.h,v 1.11 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_LIBVISUAL_H
#define _LV_LIBVISUAL_H

#include <libvisual/lv_param.h>

/**
 * @defgroup Libvisual Libvisual
 * @{
 */

VISUAL_BEGIN_DECLS

/**
 * Indicates at which version the API is.
 */
#define VISUAL_API_VERSION	4000

/**
 * Gives the libvisual version.
 *
 * @return A const char containing the libvisual version.
 */
const char *visual_get_version (void);

/**
 * Gives the libvisual API verison. Can be used to compare against the
 * compile time VISUAL_API_VERSION to validate if the API is at the right version.
 *
 * @return A const integer equal to VISUAL_API_VERSION.
 */
int visual_get_api_version (void);

/**
 * Returns a pointer to the libvisual global VisParamContainer.
 *
 * @return A pointer to the libvisual global VisParamContainer.
 */
VisParamContainer *visual_get_params (void);

/**
 * Initialize libvisual. Sets up a plugin registry, register the program name and such.
 *
 * @param argc Pointer to an int containing the number of arguments within argv or NULL.
 * @param argv Pointer to a list of strings that make up the argument vector or NULL.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED,
 *	-VISUAL_ERROR_LIBVISUAL_NO_REGISTRY or error values returned by visual_init_path_add () on failure.
 */
int visual_init (int *argc, char ***argv);

/*
 * Tells whether Libvisual is (correctly) initialized.
 *
* @return TRUE if is it is initialized, FALSE otherwise.
 */
int visual_is_initialized (void);

/**
 * Quits libvisual, destroys all the plugin registries.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED on failure.
 */
int visual_quit (void);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_LIBVISUAL_H */
