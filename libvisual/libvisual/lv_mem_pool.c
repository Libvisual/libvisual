/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem_pool.c,v 1.1 2006-09-19 18:28:51 synap Exp $
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

#include "lv_mem_pool.h"
#include "lv_common.h"
#include "lv_log.h"
#include "lv_error.h"

static int mempool_simple_dtor (VisObject *object);
static int mempool_tls_dtor (VisObject *object);

static int mempool_simple_create (VisMemPool *mpool);
static void *mempool_simple_alloc (VisMemPool *mpool);
static int mempool_simple_free (VisMemPool *mpool, void *ptr);
static int mempool_simple_resize (VisMemPool *mpool, visual_size_t entries);

static int mempool_tls_create (VisMemPool *mpool);
static void *mempool_tls_alloc (VisMemPool *mpool);
static int mempool_tls_free (VisMemPool *mpool, void *ptr);
static int mempool_tls_resize (VisMemPool *mpool, visual_size_t entries);


static int mempool_simple_dtor (VisObject *object)
{
	VisMemPoolSimple *mpool = VISUAL_MEM_POOL_SIMPLE (object);

	if (mpool->chunk != NULL)
		visual_mem_chunk_free_chain (mpool->chunk, TRUE);

	if (mpool->free_stack != NULL)
		visual_mem_pointerstack_free (mpool->free_stack);

	mpool->chunk = NULL;
	mpool->free_stack = NULL;

	return TRUE;
}

static int mempool_tls_dtor (VisObject *object)
{
	VisMemPoolTLS *mpool = VISUAL_MEM_POOL_TLS (object);

	if (mpool->tls != NULL)
		visual_thread_tls_delete_key (mpool->tls);

	mpool->tls = NULL;

	return TRUE;
}


static int mempool_simple_create (VisMemPool *mpool)
{
	VisMemPoolSimple *mpools = VISUAL_MEM_POOL_SIMPLE (mpool);

	mpools->chunk = visual_mem_chunk_new (NULL, mpool->object_size, mpool->start_entries);

	mpools->free_stack = visual_mem_pointerstack_new (mpool->start_entries);

	visual_mem_pointerstack_push_all (mpools->free_stack, mpool->object_size, mpool->start_entries, mpools->chunk->data);

	return VISUAL_OK;
}

static void *mempool_simple_alloc (VisMemPool *mpool)
{
	void *ptr;

	VisMemPoolSimple *mpools = VISUAL_MEM_POOL_SIMPLE (mpool);

	if ((ptr = visual_mem_pointerstack_pop (mpools->free_stack)) == NULL) {
		if (mpool->growable == TRUE) {
			VisMemChunk *mchunk = visual_mem_chunk_new (mpools->chunk, mpool->object_size, mpool->start_entries);

			visual_mem_pointerstack_resize (mpools->free_stack,
					visual_mem_pointerstack_size (mpools->free_stack) + mpool->start_entries);

			visual_mem_pointerstack_push_all (mpools->free_stack, mpool->object_size, mpool->start_entries, mchunk->data);
		}

		if ((ptr = visual_mem_pointerstack_pop (mpools->free_stack)) == NULL) {
			visual_log (VISUAL_LOG_ERROR, N_("Can't grow memory pool"));
		}
	}

	return ptr;
}

static int mempool_simple_free (VisMemPool *mpool, void *ptr)
{
	VisMemPoolSimple *mpools;

	visual_log_return_val_if_fail (ptr != NULL, -VISUAL_ERROR_NULL);

	mpools = VISUAL_MEM_POOL_SIMPLE (mpool);

	if (visual_mem_pointerstack_push (mpools->free_stack, ptr) != VISUAL_OK) {
		visual_log (VISUAL_LOG_ERROR, N_("Couldn't push pointer on memory pool freestack"));
	}

	if (mpool->destroyer != NULL)
		mpool->destroyer (ptr);

	return VISUAL_OK;
}

static int mempool_tls_create (VisMemPool *mpool)
{
	VisMemPoolTLS *mpooltls = VISUAL_MEM_POOL_TLS (mpool);

	mpooltls->tls = visual_thread_tls_create_key (visual_object_collection_destroyer);

	return VISUAL_OK;
}

static void *mempool_tls_alloc (VisMemPool *mpool)
{
	VisMemPoolTLS *mpooltls = VISUAL_MEM_POOL_TLS (mpool);
	VisMemPool *local;

	local = visual_thread_tls_get_data (mpooltls->tls);

	/* No local pool for this thread, create an register */
	if (local == NULL) {
		local = visual_mem_pool_simple_new (mpool->object_size,
				mpool->start_entries,
				mpool->destroyer,
				mpool->growable);

		visual_mem_pool_create (local);

		visual_thread_tls_set_data (mpooltls->tls, local);
	}

	visual_mem_pool_alloc (local);
}

static int mempool_tls_free (VisMemPool *mpool, void *ptr)
{
	VisMemPoolTLS *mpooltls = VISUAL_MEM_POOL_TLS (mpool);
	VisMemPool *local;

	local = visual_thread_tls_get_data (mpooltls->tls);

	/* We have a pointer in a thread for which no mempool was created */
	if (local == NULL) {
		visual_log (VISUAL_LOG_ERROR, N_("Thread doesn't have a TLS memory pool yet, pointer obtained in a different thread ?"));
	}

	visual_mem_pool_free (local, ptr);

	return VISUAL_OK;
}


/**
 * @defgroup VisMem VisMem
 * @{
 */

VisMemPool *visual_mem_pool_simple_new (visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable)
{
	VisMemPoolSimple *mpool;

	mpool = visual_mem_new0 (VisMemPoolSimple, 1);

	visual_mem_pool_simple_init (VISUAL_MEM_POOL (mpool), object_size, start_entries, destroyer, growable);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (mpool), TRUE);
	visual_object_ref (VISUAL_OBJECT (mpool));

	return VISUAL_MEM_POOL (mpool);
}

int visual_mem_pool_simple_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable)
{
	visual_mem_pool_init (mpool, object_size, start_entries, growable,
			destroyer,
			mempool_simple_create,
			mempool_simple_alloc,
			mempool_simple_free);

	visual_object_set_dtor (VISUAL_OBJECT (mpool), mempool_simple_dtor);

	return VISUAL_OK;
}

VisMemPool *visual_mem_pool_tls_new (visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable)
{
	VisMemPoolTLS *mpool;

	mpool = visual_mem_new0 (VisMemPoolTLS, 1);

	visual_mem_pool_tls_init (VISUAL_MEM_POOL (mpool), object_size, start_entries, destroyer, growable);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (mpool), TRUE);
	visual_object_ref (VISUAL_OBJECT (mpool));

	return VISUAL_MEM_POOL (mpool);
}

int visual_mem_pool_tls_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable)
{
	visual_mem_pool_init (mpool, object_size, start_entries, growable,
			destroyer,
			mempool_tls_create,
			mempool_tls_alloc,
			mempool_tls_free);

	visual_object_set_dtor (VISUAL_OBJECT (mpool), mempool_tls_dtor);

	return VISUAL_OK;
}

int visual_mem_pool_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries, int growable,
		VisCollectionDestroyerFunc destroyer,
		VisMemPoolCreateFunc createfunc,
		VisMemPoolAllocFunc allocfunc,
		VisMemPoolFreeFunc freefunc)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (mpool));
	visual_object_set_dtor (VISUAL_OBJECT (mpool), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (mpool), FALSE);

	/* Reset the VisMemPool data */
	visual_mem_pool_set_object_size (mpool, object_size);
	visual_mem_pool_set_start_entries (mpool, start_entries);
	visual_mem_pool_set_growable (mpool, growable);
	visual_mem_pool_set_destroyer (mpool, destroyer);

	mpool->createfunc = createfunc;
	mpool->allocfunc = allocfunc;
	mpool->freefunc = freefunc;

	return VISUAL_OK;
}

int visual_mem_pool_set_object_size (VisMemPool *mpool, visual_size_t object_size)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	mpool->object_size = object_size;

	return VISUAL_OK;
}

int visual_mem_pool_set_start_entries (VisMemPool *mpool, visual_size_t start_entries)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	mpool->start_entries = start_entries;

	return VISUAL_OK;
}

int visual_mem_pool_set_growable (VisMemPool *mpool, int growable)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	mpool->growable = growable;

	return VISUAL_OK;
}

int visual_mem_pool_set_destroyer (VisMemPool *mpool, VisCollectionDestroyerFunc destroyer)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	mpool->destroyer = destroyer;

	return VISUAL_OK;
}

int visual_mem_pool_copy_attributes (VisMemPool *dest, VisMemPool *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_MEM_POOL_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	dest->object_size = src->object_size;
	dest->start_entries = src->start_entries;
	dest->growable = src->growable;
	dest->destroyer = src->destroyer;

	return VISUAL_OK;
}

visual_size_t visual_mem_pool_get_object_size (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	return mpool->object_size;
}

visual_size_t visual_mem_pool_get_start_entries (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	return mpool->start_entries;
}

int visual_mem_pool_get_growable (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);

	return mpool->growable;
}

VisCollectionDestroyerFunc visual_mem_pool_get_destroyer (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, NULL);

	return mpool->destroyer;
}

int visual_mem_pool_create (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);
	visual_log_return_val_if_fail (mpool->createfunc != NULL, -VISUAL_ERROR_NULL);

	return mpool->createfunc (mpool);
}

void *visual_mem_pool_alloc (VisMemPool *mpool)
{
	visual_log_return_val_if_fail (mpool != NULL, NULL);
	visual_log_return_val_if_fail (mpool->allocfunc != NULL, NULL);

	return mpool->allocfunc (mpool);
}

int visual_mem_pool_free (VisMemPool *mpool, void *ptr)
{
	visual_log_return_val_if_fail (mpool != NULL, -VISUAL_ERROR_MEM_POOL_NULL);
	visual_log_return_val_if_fail (mpool->freefunc != NULL, -VISUAL_ERROR_NULL);

	return mpool->freefunc (mpool, ptr);
}

/**
 * @}
 */

