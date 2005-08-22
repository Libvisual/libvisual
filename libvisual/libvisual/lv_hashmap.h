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

#ifndef _LV_HASHMAP_H
#define _LV_HASHMAP_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_collection.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_HASHMAP_START_SIZE	512

#define VISUAL_HASHMAP(obj)				(VISUAL_CHECK_CAST ((obj), VisHashmap))
#define VISUAL_HASHMAPENTRY(obj)			(VISUAL_CHECK_CAST ((obj), VisHashmapEntry))
#define VISUAL_HASHMAPCHAINENTRY(obj)			(VISUAL_CHECK_CAST ((obj), VisHashmapChainEntry))

typedef struct _VisHashmap VisHashmap;
typedef struct _VisHashmapEntry VisHashmapEntry;
typedef struct _VisHashmapChainEntry VisHashmapChainEntry;

/**
 * Using the VisHashmap structure you can store a collection of data within a hashmap.
 */
struct _VisHashmap {
	VisCollection			 collection;	/**< The VisCollection data. */

	int				 tablesize;
	int				 size;

	VisHashmapEntry			*table;
};

/**
 */
struct _VisHashmapEntry {
	VisList		 list;
};

/**
 */
struct _VisHashmapChainEntry {
	uint32_t	 key;

	void		*data;
};

/* prototypes */
VisHashmap *visual_hashmap_new (VisCollectionDestroyerFunc destroyer);
int visual_hashmap_init (VisHashmap *hashmap, VisCollectionDestroyerFunc destroyer);

int visual_hashmap_put (VisHashmap *hashmap, int32_t key, void *data);
int visual_hashmap_remove (VisHashmap *hashmap, int32_t key, int destroy);

void *visual_hashmap_get (VisHashmap *hashmap, int32_t key);

int visual_hashmap_set_table_size (VisHashmap *hashmap, int tablesize);
int visual_hashmap_get_table_size (VisHashmap *hashmap);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_HASHMAP_H */
