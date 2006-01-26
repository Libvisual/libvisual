/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_gl.h,v 1.4 2006-01-26 15:13:37 synap Exp $
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

#ifndef _LV_GL_H
#define _LV_GL_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_hashmap.h>

VISUAL_BEGIN_DECLS

/**
 * Enumerate with GL attributes.
 */
typedef enum {
	VISUAL_GL_ATTRIBUTE_NONE = 0,		/**< No attribute. */
	VISUAL_GL_ATTRIBUTE_BUFFER_SIZE,	/**< Depth of the color buffer. */
	VISUAL_GL_ATTRIBUTE_LEVEL,		/**< Level in plane stacking. */
	VISUAL_GL_ATTRIBUTE_RGBA,		/**< True if RGBA mode. */
	VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER,	/**< Double buffering supported. */
	VISUAL_GL_ATTRIBUTE_STEREO,		/**< Stereo buffering supported. */
	VISUAL_GL_ATTRIBUTE_AUX_BUFFERS,	/**< Number of aux buffers. */
	VISUAL_GL_ATTRIBUTE_RED_SIZE,		/**< Number of red component bits. */
	VISUAL_GL_ATTRIBUTE_GREEN_SIZE,		/**< Number of green component bits. */
	VISUAL_GL_ATTRIBUTE_BLUE_SIZE,		/**< Number of blue component bits. */
	VISUAL_GL_ATTRIBUTE_ALPHA_SIZE,		/**< Number of alpha component bits. */
	VISUAL_GL_ATTRIBUTE_DEPTH_SIZE,		/**< Number of depth bits. */
	VISUAL_GL_ATTRIBUTE_STENCIL_SIZE,	/**< Number of stencil bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE,	/**< Number of red accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE,	/**< Number of green accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE,	/**< Number of blue accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE,	/**< Number of alpha accum bits. */
	VISUAL_GL_ATTRIBUTE_LAST
} VisGLAttribute;


typedef struct _VisGLCallbacks VisGLCallbacks;
typedef struct _VisGLAttributeEnviron VisGLAttributeEnviron;

typedef int (*VisGLSetAttributeFunc)(VisGLAttribute attribute, int value);
typedef int (*VisGLGetAttributeFunc)(VisGLAttribute attribute, int *value);


struct _VisGLCallbacks {
	VisGLSetAttributeFunc	attribute_set;
	VisGLGetAttributeFunc	attribute_get;
};

struct _VisGLAttributeEnviron {
	VisObject	 object;

	VisHashmap	 map;
};

/* prototypes */
int visual_gl_set_callback_attribute_set (VisGLSetAttributeFunc attribute_set);
int visual_gl_set_callback_attribute_get (VisGLGetAttributeFunc attribute_get);

int visual_gl_set_attribute (VisGLAttribute attribute, int value);
int visual_gl_get_attribute (VisGLAttribute attribute, int *value);

void *visual_gl_get_proc_address (char *procname);

VisGLCallbacks *visual_gl_get_callbacks (void);

VisGLAttributeEnviron *visual_gl_attribute_environ_new (void);
int visual_gl_attribute_environ_put (VisGLAttributeEnviron *glenviron, VisGLAttribute attribute, int value);
int visual_gl_attribute_environ_remove (VisGLAttributeEnviron *glenviron, VisGLAttribute attribute);

VISUAL_END_DECLS

#endif /* _LV_GL_H */
