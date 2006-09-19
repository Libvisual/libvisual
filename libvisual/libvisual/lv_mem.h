/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem.h,v 1.21 2006-09-19 18:28:51 synap Exp $
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

#ifndef _LV_MEM_H
#define _LV_MEM_H

#include <libvisual/lvconfig.h>

#include <libvisual/lv_defines.h>
#include <libvisual/lv_object.h>
#include <libvisual/lv_mem_ops.h>

VISUAL_BEGIN_DECLS

/**
 * @ingroup VisMem
 *
 * Convenient macro to request @a n_structs structures of type @a struct_type
 * initialized to 0.
 */
#define visual_mem_new0(struct_type, n_structs)           \
    ((struct_type *) visual_mem_malloc0 (((visual_size_t) sizeof (struct_type)) * ((visual_size_t) (n_structs))))

#define visual_mem_malloc(size)		\
	visual_mem_malloc_impl (size, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define visual_mem_malloc0(size)	\
	visual_mem_malloc0_impl (size, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define visual_mem_realloc(ptr, size)	\
	visual_mem_realloc_impl (ptr, size, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define visual_mem_free(ptr)		\
	visual_mem_free_impl (ptr, __FILE__, __LINE__, __PRETTY_FUNCTION__)


typedef struct _VisMemAllocVTable VisMemAllocVTable;

typedef void *(*VisMemMallocFunc)(visual_size_t nbytes, const char *file, int line, const char *funcname);
typedef void *(*VisMemMalloc0Func)(visual_size_t nbytes, const char *file, int line, const char *funcname);
typedef void *(*VisMemReallocFunc)(void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname);
typedef int (*VisMemFreeFunc)(void *ptr, const char *file, int line, const char *funcname);


struct _VisMemAllocVTable {
	VisMemMallocFunc	 malloc;
	VisMemMalloc0Func	 malloc0;
	VisMemReallocFunc	 realloc;
	VisMemFreeFunc		 free;

	VisObject		*priv;
};


/* prototypes */
void *visual_mem_malloc_impl (visual_size_t nbytes, const char *file, int line, const char *funcname) __malloc;
void *visual_mem_malloc0_impl (visual_size_t nbytes, const char *file, int line, const char *funcname) __malloc;
void *visual_mem_realloc_impl (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname) __malloc;
int visual_mem_free_impl (void *ptr, const char *file, int line, const char *funcname);

VisMemAllocVTable *visual_mem_alloc_vtable_standard (void);
VisMemAllocVTable *visual_mem_alloc_vtable_profile (void);

int visual_mem_alloc_profile (void);

int visual_mem_alloc_install_vtable (VisMemAllocVTable *allocvtable);
VisMemAllocVTable *visual_mem_alloc_installed_vtable (void);


VISUAL_END_DECLS

#endif /* _LV_MEM_H */
