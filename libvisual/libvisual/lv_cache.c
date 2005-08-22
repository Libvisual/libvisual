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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_common.h"
#include "lv_cache.h"

static int cache_dtor (VisObject *object);

static int cache_dtor (VisObject *object)
{

	return VISUAL_OK;
}

/**
 * @defgroup VisCache VisCache
 * @{
 */

/**
 * Creates a new VisCache.
 * 
 * @return A newly allocated VisCache.
 */
VisCache *visual_cache_new (VisCollectionDestroyerFunc destroyer)
{
	VisCache *cache;

	cache = visual_mem_new0 (VisCache, 1);

	visual_cache_init (cache, destroyer);

	/* do the visobject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (cache), TRUE);
	visual_object_ref (VISUAL_OBJECT (cache));

	return cache;
}

int visual_cache_init (VisCache *cache, VisCollectionDestroyerFunc destroyer)
{
	visual_log_return_val_if_fail (cache != NULL, -VISUAL_ERROR_CACHE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (cache));
	visual_object_set_dtor (VISUAL_OBJECT (cache), cache_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (cache), FALSE);

	return VISUAL_OK;
}

int visual_cache_put (VisCache *cache, int key, void *data)
{
	visual_log_return_val_if_fail (cache != NULL, -VISUAL_ERROR_CACHE_NULL);

}

int visual_cache_remove (VisCache *cache, int key)
{
	visual_log_return_val_if_fail (cache != NULL, -VISUAL_ERROR_CACHE_NULL);

}

void *visual_cache_get (VisCache *cache, int key)
{
	visual_log_return_val_if_fail (cache != NULL, NULL);

}

int visual_cache_size (VisCache *cache)
{
	visual_log_return_val_if_fail (cache != NULL, -VISUAL_ERROR_CACHE_NULL);

	return visual_collection_size (VISUAL_COLLECTION (cache->hashmap));
}

/**
 * @}
 */

