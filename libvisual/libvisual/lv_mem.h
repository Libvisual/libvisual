/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem.h,v 1.20 2006/01/22 13:23:37 synap Exp $
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

/**
 * @defgroup VisMem VisMem
 * @{
 */

/**
 * The visual_mem_copy function needs this signature.
 *
 * @arg dest Pointer to the dest buffer.
 * @arg src Pointer to the source buffer.
 * @arg n The number of bytes to be copied.
 *
 * @return Pointer to the dest buffer.
 */
typedef void *(*VisMemCopyFunc)(void *dest, const void *src, visual_size_t n);

/**
 * The visual_mem_copy_pitch function needs this signature. This function supports a negative pitch.
 *
 * @arg dest Pointer to the dest buffer.
 * @arg src Pointer to the source buffer.
 * @arg pitch1 The number of bytes a row in the dest buffer.
 * @arg pitch2 The number of bytes a row in the src buffer.
 * @arg width The copy width of each row.
 * @arg rows The number of rows.
 *
 * @return Pointer to the dest buffer.
 */
typedef void *(*VisMemCopyPitchFunc)(void *dest, const void *src, int pitch1, int pitch2, int width, int rows);

/**
 * The visual_mem_set function needs this signature.
 *
 * @arg dest Pointer to the dest buffer.
 * @arg c Value that is used as the set value.
 * @arg n The number of bytes to be set.
 *
 * @return Pointer to the dest buffer.
 */
typedef void *(*VisMemSet8Func)(void *dest, int c, visual_size_t n);

/**
 * The visual_mem_set16 function needs this signature.
 *
 * @arg dest Pointer to the dest buffer.
 * @arg c Value that is used as the set value.
 * @arg n The number of words (16bits) to be set.
 *
 * @return Pointer to the dest buffer.
 */
typedef void *(*VisMemSet16Func)(void *dest, int c, visual_size_t n);

/**
 * The visual_mem_set32 function needs this signature.
 *
 * @arg dest Pointer to the dest buffer.
 * @arg c Value that is used as the set value.
 * @arg n The number of integers (32bits) to be set.
 *
 * @return Pointer to the dest buffer.
 */
typedef void *(*VisMemSet32Func)(void *dest, int c, visual_size_t n);

LV_BEGIN_DECLS

/**
 * Initialize the memory functions. This is used to set the function
 * pointers to the right optimized version.  It's legal to call
 * visual_mem_initialize more than once in the same context if it's
 * needed to reset the optimal function pointers. This function bases
 * it's choices upon the VisCPU system.
 *
 * Be aware that visual_mem_initialize() should be called to set the
 * most optimize mem_copy() and mem_set() functions is called. Be sure
 * that visual_cpu_initialize() is called before this however if
 * possible the best solution is to just call visual_init() which will
 * call all the libvisual initialize functions.
 *
 * return VISUAL_OK on succes.
 */
LV_API int visual_mem_initialize (void);

/**
 * Allocates @a nbytes of uninitialized memory.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 *
 * @return On success, a pointer to a new allocated memory block
 * of size @a nbytes, on failure, program is aborted.
 */
LV_API void *visual_mem_malloc (visual_size_t nbytes) LV_ATTR_MALLOC;

/**
 * Allocates @a nbytes of memory initialized to 0.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 *
 * @return On success, a pointer to a new allocated memory initialized
 * to 0 of size @a nbytes, on failure, program is aborted.
 */
LV_API void *visual_mem_malloc0 (visual_size_t nbytes) LV_ATTR_MALLOC;

/**
 * Reallocates memory, can be used to grow a buffer.
 *
 * @param ptr Pointer that is to be reallocated.
 * @param nbytes The size of the new buffer.
 *
 * @return On success, a pointer to the new reallocated memory, on failure NULL.
 */
LV_API void *visual_mem_realloc (void *ptr, visual_size_t nbytes) LV_ATTR_MALLOC;

/**
 * Frees allocated memory.
 *
 * @param ptr Frees memory to which ptr points to.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MEM_NULL on failure.
 */
LV_API int visual_mem_free (void *ptr);


LV_API void *visual_mem_malloc_aligned (visual_size_t size, visual_size_t alignment);

LV_API void visual_mem_free_aligned (void* ptr);

/* Optimal performance functions set by visual_mem_initialize(). */
extern LV_API VisMemCopyFunc visual_mem_copy;
extern LV_API VisMemCopyPitchFunc visual_mem_copy_pitch;

extern LV_API VisMemSet8Func visual_mem_set;
extern LV_API VisMemSet16Func visual_mem_set16;
extern LV_API VisMemSet32Func visual_mem_set32;

/**
 * Convenient macro to request @a n_structs structures of type @a struct_type
 * initialized to 0.
 */
#define visual_mem_new0(struct_type, n_structs)           \
    ((struct_type *) visual_mem_malloc0 (((visual_size_t) sizeof (struct_type)) * ((visual_size_t) (n_structs))))

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_MEM_H */
