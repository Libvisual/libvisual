/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#ifndef _LV_CACHE_H
#define _LV_CACHE_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_hashmap.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_CACHE(obj)				(VISUAL_CHECK_CAST ((obj), VisCache))
#define VISUAL_CACHEENTRY(obj)				(VISUAL_CHECK_CAST ((obj), VisCacheEntry))

typedef struct _VisCache VisCache;
typedef struct _VisCacheEntry VisCacheEntry;

/**
 * Using the VisCache structure
 */
struct _VisCache {
	VisObject	 object;

	int		 tablesize;
	int		 size;

	VisHashmap	*hashmap;
};

/**
 */
struct _VisCacheEntry {
	VisObject	 object;

	VisTimer	 timer;

	void		*data;
};

/* prototypes */
VisCache *visual_cache_new (VisCollectionDestroyerFunc destroyer);
int visual_cache_init (VisCache *cache, VisCollectionDestroyerFunc destroyer);

int visual_cache_put (VisCache *cache, int key, void *data);
int visual_cache_remove (VisCache *cache, int key);

void *visual_cache_get (VisCache *cache, int key);

int visual_cache_size (VisCache *cache);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_CACHE_H */
