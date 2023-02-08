/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
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
 * Allocates a block of memory.
 *
 * @param size Size in bytes
 *
 * @return Pointer to newly allocated memory block, or NULL on failure
 */
LV_NODISCARD LV_API void *visual_mem_malloc (visual_size_t size) LV_ATTR_MALLOC;

/**
 * Allocates a block of memory with its content zeroed.
 *
 * @param size Size in bytes
 *
 * @return Pointer to newly allocated memory block with its contents
 * zeroed, or NULL on failure
 */
LV_NODISCARD LV_API void *visual_mem_malloc0 (visual_size_t size) LV_ATTR_MALLOC;

/**
 * Reallocates memory, can be used to grow a buffer.
 *
 * @param ptr  Pointer to memory block to be reallocated
 * @param size Size in bytes
 *
 * @return pointer to the reallocated memory block, or NULL on failure
 */
LV_NODISCARD LV_API void *visual_mem_realloc (void *ptr, visual_size_t size) LV_ATTR_MALLOC;

/**
 * Frees a memory block allocated by visual_mem_malloc() and visual_mem_realloc().
 *
 * @param ptr Frees memory to which ptr points to
 *
 * @note visual_mem_free() accepts NULL pointers.
 */
LV_API void visual_mem_free (void *ptr);

/**
 * Allocates a memory block aligned to a given address boundary.
 *
 * @param size      Size in bytes
 * @param alignment Address alignment
 *
 * @return pointer to newly allocated memory block at the specified alignment
 *
 * @note Memory allocated by this function must be fred by visual_mem_free_aligned().
 */
LV_NODISCARD LV_API void *visual_mem_malloc_aligned (visual_size_t size, visual_size_t alignment);

/**
 * Frees a memory block allocated by visual_mem_alloc_aligned().
 *
 * @param ptr Pointer to memory block
 *
 * @note visual_mem_free_aligned() accepts NULL pointers.
 */
LV_API void visual_mem_free_aligned (void *ptr);

/* Optimal performance functions set by visual_mem_initialize(). */
extern LV_API VisMemCopyFunc visual_mem_copy;
extern LV_API VisMemCopyPitchFunc visual_mem_copy_pitch;

extern LV_API VisMemSet8Func  visual_mem_set;
extern LV_API VisMemSet16Func visual_mem_set16;
extern LV_API VisMemSet32Func visual_mem_set32;

/**
 * Convenience macro to allocate an array with visual_mem_malloc0().
 *
 * @param struct_type Type of struct
 * @param n_structs   Number of elements
 *
 * @return pointer to newly allocated array
 */
#define visual_mem_new0(struct_type, n_structs) \
    ((struct_type *) visual_mem_malloc0 (((visual_size_t) sizeof (struct_type)) * ((visual_size_t) (n_structs))))

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_MEM_H */
