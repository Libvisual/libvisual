/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_gl_glx.h,v 1.1 2006-02-13 20:32:24 synap Exp $
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

#ifndef _LV_GL_GLX_H
#define _LV_GL_GLX_H

#include <GL/glx.h>

#include <libvisual/libvisual.h>

VISUAL_BEGIN_DECLS

static int __lv_glx_gl_attribute_map[] = {
	[VISUAL_GL_ATTRIBUTE_NONE]		= -1,
	[VISUAL_GL_ATTRIBUTE_BUFFER_SIZE]	= GLX_BUFFER_SIZE,
	[VISUAL_GL_ATTRIBUTE_LEVEL]		= GLX_LEVEL,
	[VISUAL_GL_ATTRIBUTE_RGBA]		= GLX_RGBA,
	[VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER]	= GLX_DOUBLEBUFFER,
	[VISUAL_GL_ATTRIBUTE_STEREO]		= GLX_STEREO,
	[VISUAL_GL_ATTRIBUTE_AUX_BUFFERS]	= GLX_AUX_BUFFERS,
	[VISUAL_GL_ATTRIBUTE_RED_SIZE]		= GLX_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_GREEN_SIZE]	= GLX_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_BLUE_SIZE]		= GLX_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ALPHA_SIZE]	= GLX_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_DEPTH_SIZE]	= GLX_DEPTH_SIZE,
	[VISUAL_GL_ATTRIBUTE_STENCIL_SIZE]	= GLX_STENCIL_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE]	= GLX_ACCUM_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE]	= GLX_ACCUM_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE]	= GLX_ACCUM_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE]	= GLX_ACCUM_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_LAST]		= -1
};

static int lv_glx_gl_attribute_set (VisGLAttribute attribute, int value)
{
	int glx_attribute = __lv_glx_gl_attribute_map[attribute];

	if (glx_attribute < 0)
		return -1;

	return 0;
}

static int lv_glx_gl_attribute_get (VisGLAttribute attribute, int *value)
{
	int glx_attribute = __lv_glx_gl_attribute_map[attribute];

	if (glx_attribute < 0)
		return -1;

	return 0;
}

VISUAL_END_DECLS

#endif /* _LV_GL_GLX_H */
