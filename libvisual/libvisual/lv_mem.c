/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem.c,v 1.31 2006-09-19 18:28:51 synap Exp $
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

#include <config.h>

#include <string.h>
#include <stdlib.h>
#include <gettext.h>

#include "lv_mem.h"
#include "lv_common.h"
#include "lv_log.h"
#include "lv_error.h"
#include "lv_cpu.h"
#include "lv_bits.h"
#include "lv_hashmap.h"

#define ALLOC_PROFILE_DATA(obj)				(VISUAL_CHECK_CAST ((obj), AllocProfileData))
#define ALLOC_PROFILE_CALLENTRY(obj)			(VISUAL_CHECK_CAST ((obj), AllocProfileCallEntry))
#define ALLOC_PROFILE_SIZES(obj)			(VISUAL_CHECK_CAST ((obj), AllocProfileSizes))

typedef struct _AllocProfileData AllocProfileData;
typedef struct _AllocProfileCallEntry AllocProfileCallEntry;
typedef struct _AllocProfileSizes AllocProfileSizes;

#define ALLOC_PROFILE_HASHMAP_POINTERS_SIZE	65536
#define ALLOC_PROFILE_HASHMAP_SIZES_SIZE	65536

struct _AllocProfileData {
	VisObject	 object;

	VisHashmap	 allocated_pointers;
	VisHashmap	 allocated_sizes;

	int		 mem_allocated_total;
	int		 mem_allocated_peak;
	int		 mem_allocated;

	int		 mem_count_malloc;
	int		 mem_count_realloc;
	int		 mem_count_free;
};

struct _AllocProfileCallEntry {
	char		*file;
	int		 line;
	char		*funcname;
};

struct _AllocProfileSizes {
	VisHashmap	 callers;

	int		 count;
};

static void *alloc_standard_malloc (visual_size_t nbytes, const char *file, int line, const char *funcname);
static void *alloc_standard_malloc0 (visual_size_t nbytes, const char *file, int line, const char *funcname);
static void *alloc_standard_realloc (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname);
static int alloc_standard_free (void *ptr, const char *file, int line, const char *funcname);

static void *alloc_profile_malloc (visual_size_t nbytes, const char *file, int line, const char *funcname);
static void *alloc_profile_malloc0 (visual_size_t nbytes, const char *file, int line, const char *funcname);
static void *alloc_profile_realloc (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname);
static int alloc_profile_free (void *ptr, const char *file, int line, const char *funcname);

static VisMemAllocVTable __lv_alloc_vtable = {
	.malloc		= alloc_standard_malloc,
	.malloc0	= alloc_standard_malloc0,
	.realloc	= alloc_standard_realloc,
	.free		= alloc_standard_free,
	.priv		= NULL
};

/**
 * @defgroup VisMem VisMem
 * @{
 */

/**
 * Allocates @a nbytes of uninitialized memory.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 *
 * @return On success, a pointer to a new allocated memory block
 * of size @a nbytes, on failure, program is aborted.
 */
void *visual_mem_malloc_impl (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	return __lv_alloc_vtable.malloc (nbytes, file, line, funcname);
}

/**
 * Allocates @a nbytes of memory initialized to 0.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 *
 * @return On success, a pointer to a new allocated memory initialized
 * to 0 of size @a nbytes, on failure, program is aborted.
 */
void *visual_mem_malloc0_impl (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	return __lv_alloc_vtable.malloc0 (nbytes, file, line, funcname);
}

/**
 * Reallocates memory, can be used to grow a buffer.
 *
 * @param ptr Pointer that is to be reallocated.
 * @param nbytes The size of the new buffer.
 *
 * @return On success, a pointer to the new reallocated memory, on failure NULL.
 */
void *visual_mem_realloc_impl (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	return __lv_alloc_vtable.realloc (ptr, nbytes, file, line, funcname);
}

/**
 * Frees allocated memory.
 *
 * @param ptr Frees memory to which ptr points to.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MEM_NULL on failure.
 */
int visual_mem_free_impl (void *ptr, const char *file, int line, const char *funcname)
{
	return __lv_alloc_vtable.free (ptr, file, line, funcname);
}

VisMemAllocVTable *visual_mem_alloc_vtable_standard ()
{
	static VisMemAllocVTable alloc_standard_vtable = {
		.malloc		= alloc_standard_malloc,
		.malloc0	= alloc_standard_malloc0,
		.realloc	= alloc_standard_realloc,
		.free		= alloc_standard_free,
		.priv		= NULL
	};

	return &alloc_standard_vtable;
}

VisMemAllocVTable *visual_mem_alloc_vtable_profile ()
{
	static VisMemAllocVTable alloc_profile_vtable = {
		.malloc		= alloc_profile_malloc,
		.malloc0	= alloc_profile_malloc0,
		.realloc	= alloc_profile_realloc,
		.free		= alloc_profile_free,
		.priv		= NULL
	};

	alloc_profile_vtable.priv = VISUAL_OBJECT (visual_mem_new0 (AllocProfileData, 1));

	visual_hashmap_init (&ALLOC_PROFILE_DATA (alloc_profile_vtable.priv)->allocated_pointers, NULL);
	visual_hashmap_set_table_size (&ALLOC_PROFILE_DATA (alloc_profile_vtable.priv)->allocated_pointers,
			ALLOC_PROFILE_HASHMAP_POINTERS_SIZE);

	visual_hashmap_init (&ALLOC_PROFILE_DATA (alloc_profile_vtable.priv)->allocated_sizes, NULL);
	visual_hashmap_set_table_size (&ALLOC_PROFILE_DATA (alloc_profile_vtable.priv)->allocated_sizes,
			ALLOC_PROFILE_HASHMAP_SIZES_SIZE);

	return &alloc_profile_vtable;
}

int visual_mem_alloc_profile ()
{
	VisCollectionIterator iter;
	visual_log_return_val_if_fail (-VISUAL_ERROR_GENERAL, &__lv_alloc_vtable == visual_mem_alloc_vtable_profile ());

	/* FIXME: should probably be LOG_DEBUG.. */
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Total amount of memory allocated: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_total);
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Total amount of memory still allocated: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated);
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Peak amount of memory allocated: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_peak);
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Malloc calls: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_malloc);
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Realloc calls: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_realloc);
	visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Free calls: %d",
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_free);

	visual_collection_get_iterator (&iter, VISUAL_COLLECTION (
				&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_sizes));

	while (visual_collection_iterator_has_more (&iter)) {
		VisHashmapChainEntry *mentry = visual_collection_iterator_get_data (&iter);

		visual_log (VISUAL_LOG_INFO, "[ALLOC PROFILE] Allocated sizes %d\t::\t%d",
				mentry->key.integer, mentry->data);

		visual_collection_iterator_next (&iter);
	}

	visual_object_unref (VISUAL_OBJECT (&iter));

	return VISUAL_OK;
}

int visual_mem_alloc_install_vtable (VisMemAllocVTable *allocvtable)
{
	if (__lv_alloc_vtable.priv != NULL)
		visual_object_unref (__lv_alloc_vtable.priv);

	__lv_alloc_vtable = *allocvtable;

	return VISUAL_OK;
}

VisMemAllocVTable *visual_mem_alloc_installed_vtable (void)
{
	return &__lv_alloc_vtable;
}

static void *alloc_standard_malloc (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	void *buf;

	visual_log_return_val_if_fail (nbytes > 0, NULL);

	buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory"), nbytes);

		return NULL;
	}

	return buf;
}

static void *alloc_standard_malloc0 (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	void *buf;

	visual_log_return_val_if_fail (nbytes > 0, NULL);

	buf = visual_mem_malloc (nbytes);

	visual_mem_set (buf, 0, nbytes);

	return buf;
}

static void *alloc_standard_realloc (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	return realloc (ptr, nbytes);
}

static int alloc_standard_free (void *ptr, const char *file, int line, const char *funcname)
{
	/* FIXME remove eventually, we keep it for now for explicit debug */
	visual_log_return_val_if_fail (ptr != NULL, -VISUAL_ERROR_MEM_NULL);

	free (ptr);

	return VISUAL_OK;
}

static void *alloc_profile_malloc (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	void *buf;
	VisMemMallocFunc malloc_;
	int size_count;

	visual_log_return_val_if_fail (nbytes > 0, NULL);

	buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory"), nbytes);

		return NULL;
	}

	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_total += nbytes;
	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_malloc++;
	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated += nbytes;

	if (ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated >
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_peak) {

		ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_peak =
			ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated;
	}

	malloc_ = __lv_alloc_vtable.malloc;
	__lv_alloc_vtable.malloc = alloc_standard_malloc;

	visual_hashmap_put_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) buf, (void *) nbytes);

	size_count = (int) visual_hashmap_get_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_sizes,
			(uint32_t) nbytes);
	size_count++;

	visual_hashmap_put_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_sizes,
			(uint32_t) nbytes, (void *) size_count);

	__lv_alloc_vtable.malloc = malloc_;

	return buf;
}

static void *alloc_profile_malloc0 (visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	void *buf;

	visual_log_return_val_if_fail (nbytes > 0, NULL);

	buf = visual_mem_malloc (nbytes);

	visual_mem_set (buf, 0, nbytes);

	return buf;
}

static void *alloc_profile_realloc (void *ptr, visual_size_t nbytes, const char *file, int line, const char *funcname)
{
	int size;
	int size_count;
	void *buf = NULL;
	VisMemMallocFunc malloc_;

	buf = realloc (ptr, nbytes);

	size = (int) visual_hashmap_get_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) ptr);

	visual_hashmap_remove_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) ptr, FALSE);

	if (size != 0)
		ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated -= size;

	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated_total += nbytes;
	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_realloc++;
	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated += nbytes;

	malloc_ = __lv_alloc_vtable.malloc;
	__lv_alloc_vtable.malloc = alloc_standard_malloc;

	visual_hashmap_put_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) buf, (void *) nbytes);

	size_count = (int) visual_hashmap_get_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_sizes,
			(uint32_t) nbytes);
	size_count++;

	visual_hashmap_put_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) nbytes, (void *) size_count);

	__lv_alloc_vtable.malloc = malloc_;

	return buf;
}

static int alloc_profile_free (void *ptr, const char *file, int line, const char *funcname)
{
	int size;
	VisMemFreeFunc free_;

	/* FIXME remove eventually, we keep it for now for explicit debug */
	visual_log_return_val_if_fail (ptr != NULL, -VISUAL_ERROR_MEM_NULL);

	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_count_free++;

	// FIXME will bork on 64 bits.
	size = (int) visual_hashmap_get_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) ptr);

	ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->mem_allocated -= size;

	free (ptr);

	free_ = __lv_alloc_vtable.free;
	__lv_alloc_vtable.free = alloc_standard_free;

	visual_hashmap_remove_integer (&ALLOC_PROFILE_DATA (__lv_alloc_vtable.priv)->allocated_pointers,
			(uint32_t) ptr, FALSE);

	__lv_alloc_vtable.free = free_;

	return VISUAL_OK;
}

/**
 * @}
 */

