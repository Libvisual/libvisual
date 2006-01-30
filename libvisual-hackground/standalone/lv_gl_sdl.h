/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_gl_sdl.h,v 1.1 2006-01-30 18:19:27 synap Exp $
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

#ifndef _LV_GL_SDL_H
#define _LV_GL_SDL_H

#include <SDL/SDL.h>
#include <libvisual/libvisual.h>

VISUAL_BEGIN_DECLS

static SDL_GLattr attribute_map[] = {
	[VISUAL_GL_ATTRIBUTE_NONE]		= -1,
	[VISUAL_GL_ATTRIBUTE_BUFFER_SIZE]	= SDL_GL_BUFFER_SIZE,
	[VISUAL_GL_ATTRIBUTE_LEVEL]		= -1,
	[VISUAL_GL_ATTRIBUTE_RGBA]		= -1,
	[VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER]	= SDL_GL_DOUBLEBUFFER,
	[VISUAL_GL_ATTRIBUTE_STEREO]		= SDL_GL_STEREO,
	[VISUAL_GL_ATTRIBUTE_AUX_BUFFERS]	= -1,
	[VISUAL_GL_ATTRIBUTE_RED_SIZE]		= SDL_GL_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_GREEN_SIZE]	= SDL_GL_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_BLUE_SIZE]		= SDL_GL_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ALPHA_SIZE]	= SDL_GL_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_DEPTH_SIZE]	= SDL_GL_DEPTH_SIZE,
	[VISUAL_GL_ATTRIBUTE_STENCIL_SIZE]	= SDL_GL_STENCIL_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE]	= SDL_GL_ACCUM_RED_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE]	= SDL_GL_ACCUM_GREEN_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE]	= SDL_GL_ACCUM_BLUE_SIZE,
	[VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE]	= SDL_GL_ACCUM_ALPHA_SIZE,
	[VISUAL_GL_ATTRIBUTE_LAST]		= -1
};

static int native_gl_attribute_set (VisGLAttribute attribute, int value)
{
	SDL_GLattr sdl_attribute = attribute_map[attribute];

	if (sdl_attribute < 0)
		return -1;

	return SDL_GL_SetAttribute (sdl_attribute, value);
}

static int native_gl_attribute_get (VisGLAttribute attribute, int *value)
{
	SDL_GLattr sdl_attribute = attribute_map[attribute];

	if (sdl_attribute < 0)
		return -1;

	return SDL_GL_GetAttribute (sdl_attribute, value);
}

VISUAL_END_DECLS

#endif /* _LV_GL_SDL_H */
