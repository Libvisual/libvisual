/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem_pool.h,v 1.1 2006-09-19 18:28:51 synap Exp $
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

#ifndef _LV_MEM_POOL_H
#define _LV_MEM_POOL_H

#include <libvisual/lvconfig.h>

#include <libvisual/lv_defines.h>
#include <libvisual/lv_thread.h>
#include <libvisual/lv_mem_utils.h>
#include <libvisual/lv_collection.h>

VISUAL_BEGIN_DECLS

#define VISUAL_MEM_POOL(obj)				(VISUAL_CHECK_CAST ((obj), VisMemPool))
#define VISUAL_MEM_POOL_SIMPLE(obj)			(VISUAL_CHECK_CAST ((obj), VisMemPoolSimple))
#define VISUAL_MEM_POOL_TLS(obj)			(VISUAL_CHECK_CAST ((obj), VisMemPoolTLS))


typedef struct _VisMemPool VisMemPool;
typedef struct _VisMemPoolSimple VisMemPoolSimple;
typedef struct _VisMemPoolTLS VisMemPoolTLS;

typedef int (*VisMemPoolCreateFunc)(VisMemPool *mpool);
typedef void *(*VisMemPoolAllocFunc)(VisMemPool *mpool);
typedef int (*VisMemPoolFreeFunc)(VisMemPool *mpool, void *ptr);


struct _VisMemPool {
	VisObject			 object;

	visual_size_t			 object_size;
	visual_size_t			 start_entries;

	int				 growable;

	VisCollectionDestroyerFunc	 destroyer;

	VisMemPoolCreateFunc		 createfunc;
	VisMemPoolAllocFunc		 allocfunc;
	VisMemPoolFreeFunc		 freefunc;
};

struct _VisMemPoolSimple {
	VisMemPool			 pool;

	VisMemChunk			*chunk;
	VisMemPointerStack		*free_stack;
};

struct _VisMemPoolTLS {
	VisMemPool			 pool;

	VisTLS				*tls;
};


/* prototypes */
VisMemPool *visual_mem_pool_simple_new (visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable);
int visual_mem_pool_simple_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable);

VisMemPool *visual_mem_pool_tls_new (visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable);
int visual_mem_pool_tls_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries,
		VisCollectionDestroyerFunc destroyer, int growable);

int visual_mem_pool_init (VisMemPool *mpool, visual_size_t object_size, visual_size_t start_entries, int growable,
		VisCollectionDestroyerFunc destroyer,
		VisMemPoolCreateFunc createfunc,
		VisMemPoolAllocFunc allocfunc,
		VisMemPoolFreeFunc freefunc);

int visual_mem_pool_set_object_size (VisMemPool *mpool, visual_size_t object_size);
int visual_mem_pool_set_start_entries (VisMemPool *mpool, visual_size_t start_entries);
int visual_mem_pool_set_growable (VisMemPool *mpool, int growable);
int visual_mem_pool_set_destroyer (VisMemPool *mpool, VisCollectionDestroyerFunc destroyer);

int visual_mem_pool_copy_attributes (VisMemPool *dest, VisMemPool *src);

visual_size_t visual_mem_pool_get_object_size (VisMemPool *mpool);
visual_size_t visual_mem_pool_get_start_entries (VisMemPool *mpool);
int visual_mem_pool_get_growable (VisMemPool *mpool);
VisCollectionDestroyerFunc visual_mem_pool_get_destroyer (VisMemPool *mpool);


int visual_mem_pool_create (VisMemPool *mpool);
void *visual_mem_pool_alloc (VisMemPool *mpool);
int visual_mem_pool_free (VisMemPool *mpool, void *ptr);


VISUAL_END_DECLS

#endif /* _LV_MEM_POOL_H */
