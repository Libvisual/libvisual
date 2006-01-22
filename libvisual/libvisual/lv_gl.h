/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_gl.h,v 1.2 2006-01-22 13:23:37 synap Exp $
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

VISUAL_BEGIN_DECLS

typedef enum {
	VISUAL_GL_ATTRIBUTE_NONE = 0
} VisGLAttribute;


typedef struct _VisGLCallbacks VisGLCallbacks;


typedef int (*VisGLSetAttributeFunc)(VisGLAttribute attribute, int value);
typedef int (*VisGLGetAttributeFunc)(VisGLAttribute attribute, int *value);


struct _VisGLCallbacks {
	VisGLSetAttributeFunc	attribute_set;
	VisGLGetAttributeFunc	attribute_get;
};

/* prototypes */
int visual_gl_set_callback_attribute_set (VisGLSetAttributeFunc attribute_set);
int visual_gl_set_callback_attribute_get (VisGLGetAttributeFunc attribute_get);

int visual_gl_set_attribute (VisGLAttribute attribute, int value);
int visual_gl_get_attribute (VisGLAttribute attribute, int *value);

void *visual_gl_get_proc_address (char *procname);

VISUAL_END_DECLS

#endif /* _LV_GL_H */
