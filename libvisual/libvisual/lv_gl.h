/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
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

#ifndef _LV_GL_H
#define _LV_GL_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

/**
 * @defgroup VisGL VisGL
 * @{
 */

#define VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(options, attr, val) \
    options.gl_attrs[attr].attribute = attr; \
    options.gl_attrs[attr].value     = val;

/**
 * OpenGL framebuffer attributes.
 *
 * @see VisGLAttrEntry
 */
typedef enum {
	VISUAL_GL_ATTRIBUTE_NONE = 0,          /**< Special flag to indicate no attributes. */
	VISUAL_GL_ATTRIBUTE_BUFFER_SIZE,       /**< Depth of the color buffer. */
	VISUAL_GL_ATTRIBUTE_LEVEL,             /**< Level in plane stacking. */
	VISUAL_GL_ATTRIBUTE_RGBA,              /**< True if RGBA mode. */
	VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER,      /**< Double buffering supported. */
	VISUAL_GL_ATTRIBUTE_STEREO,            /**< Stereo buffering supported. */
	VISUAL_GL_ATTRIBUTE_AUX_BUFFERS,       /**< Number of aux buffers. */
	VISUAL_GL_ATTRIBUTE_RED_SIZE,          /**< Number of red component bits. */
	VISUAL_GL_ATTRIBUTE_GREEN_SIZE,        /**< Number of green component bits. */
	VISUAL_GL_ATTRIBUTE_BLUE_SIZE,         /**< Number of blue component bits. */
	VISUAL_GL_ATTRIBUTE_ALPHA_SIZE,        /**< Number of alpha component bits. */
	VISUAL_GL_ATTRIBUTE_DEPTH_SIZE,        /**< Number of depth bits. */
	VISUAL_GL_ATTRIBUTE_STENCIL_SIZE,      /**< Number of stencil bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE,    /**< Number of red accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE,  /**< Number of green accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE,   /**< Number of blue accum bits. */
	VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE,  /**< Number of alpha accum bits. */
	VISUAL_GL_ATTRIBUTE_LAST
} VisGLAttr;

typedef struct _VisGLAttrEntry VisGLAttrEntry;

/**
 * OpenGL framebuffer attribute entry.
 *
 * Used to specify OpenGL framebuffer requirements.
 */
struct _VisGLAttrEntry {
	VisGLAttr attribute;         /**< Attribute tag */
	int       value;             /**< Attribute value */
};

/**
 * @}
 */

#endif /* _LV_GL_H */
