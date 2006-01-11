/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_gl.c,v 1.1 2006-01-11 07:06:38 synap Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "lv_gl.h"

static VisGLCallbacks callbacks;

/**
 * @defgroup VisGL VisGL
 * @{
 */

int visual_gl_set_callback_attribute_set (VisGLSetAttributeFunc attribute_set)
{
	callbacks.attribute_set = attribute_set;

	return VISUAL_OK;
}

int visual_gl_set_callback_attribute_get (VisGLGetAttributeFunc attribute_get)
{
	callbacks.attribute_get = attribute_get;

	return VISUAL_OK;
}


int visual_gl_set_attribute (VisGLAttribute attribute, int value)
{
	if (callbacks.attribute_set == NULL)
		return -VISUAL_ERROR_GL_FUNCTION_NOT_SUPPORTED;

	callbacks.attribute_set (attribute, value);

	return VISUAL_OK;
}

int visual_gl_get_attribute (VisGLAttribute attribute, int *value)
{
	if (callbacks.attribute_get == NULL)
		return -VISUAL_ERROR_GL_FUNCTION_NOT_SUPPORTED;

	callbacks.attribute_get (attribute, value);

	return VISUAL_OK;
}


void *visual_gl_get_proc_address (char *procname)
{
	return NULL;
}

/**
 * @}
 */

