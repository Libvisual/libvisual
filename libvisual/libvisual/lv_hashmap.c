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
#include "lv_hashmap.h"

#define HASHMAP_ITERCONTEXT(obj)                           (VISUAL_CHECK_CAST ((obj), HashmapIterContext))


typedef struct _HashmapIterContext HashmapIterContext;

struct _HashmapIterContext {
	VisObject	*object;

	int		 index;
	VisListEntry	*le;
};


static int hashmap_destroy (VisCollection *collection);
static int hashmap_size (VisCollection *collection);
static VisCollectionIter *hashmap_iter (VisCollection *collection);

static int hashmap_iter_has_more (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext);
static void *hashmap_iter_next (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext);

static int integer_hash (int key);
static int string_hash (char *key);

static int create_table (VisHashmap *hashmap);


static int hashmap_destroy (VisCollection *collection)
{
	VisCollectionDestroyerFunc destroyer;
	VisHashmap *hashmap = VISUAL_HASHMAP (hashmap);
	VisHashmapEntry *mentry;
	int i;

	for (i = 0; i < hashmap->size; i++)
		visual_collection_destroy (VISUAL_COLLECTION (&hashmap->table[i].list));

	if (hashmap->table != NULL)
		visual_mem_free (hashmap->table);

	hashmap->table = NULL;

	return VISUAL_OK;
}

static int hashmap_size (VisCollection *collection)
{
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);

	return hashmap->size;
}

static VisCollectionIter *hashmap_iter (VisCollection *collection)
{
	VisCollectionIter *iter;
	HashmapIterContext *context;
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);

	context = visual_mem_new0 (HashmapIterContext, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (context), TRUE, NULL);
	context->index = 0;
	context->le = NULL;

	iter = visual_collection_iter_new (hashmap_iter_next, hashmap_iter_has_more, collection, VISUAL_OBJECT (context));

	return iter;
}

static int hashmap_iter_has_more (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext)
{

}

static void *hashmap_iter_next (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext)
{
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);
	HashmapIterContext *context = HASHMAP_ITERCONTEXT (itercontext);
	int i;

	if (context->index >= hashmap->size)
		return NULL;

	/* FIXME initial start case doesn't work */
	if (context->le->next != NULL)
		return visual_list_next (&hashmap->table[i].list, &context->le);

	/* Find the next valid chain */
	for (i = context->index; i < hashmap->size; i++) {
		VisHashmapEntry *mentry;

		mentry = &hashmap->table[i];



	}

	return NULL;
}

/* Thomas Wang's 32 bit Mix Function: http://www.concentric.net/~Ttwang/tech/inthash.htm */
static int integer_hash (int key)
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);

	return key;
}

/* X31 HASH found in g_str_hash, not used at the moment, but we keep it for later */
static int string_hash (char *key)
{
	char *p;
	int hash = 0;

	for (p = key; *p != '\0'; p += 1)
		hash = (hash << 5) - hash  + *p;

	return hash;
}

static int create_table (VisHashmap *hashmap)
{
	int i;

	hashmap->table = visual_mem_new0 (VisHashmapEntry, hashmap->tablesize);

	/* Initialize first entry */
	visual_list_init (&hashmap->table[0].list,
			visual_collection_get_destroyer (VISUAL_COLLECTION (hashmap)));

	/* Copy the entries to increase speed */
	for (i = 1; i < hashmap->tablesize; i *= 2) {
		int n = (i + i) > hashmap->tablesize ? hashmap->tablesize - i : i;

		visual_mem_copy (&hashmap->table[i], &hashmap->table[0], sizeof (VisHashmapEntry) * n);
	}

	return VISUAL_OK;
}


/**
 * @defgroup VisHashmap VisHashmap
 * @{
 */

/**
 * Creates a new VisHashmap.
 * 
 * @return A newly allocated VisHashmap.
 */
VisHashmap *visual_hashmap_new (VisCollectionDestroyerFunc destroyer)
{
	VisHashmap *hashmap;

	hashmap = visual_mem_new0 (VisHashmap, 1);

	visual_hashmap_init (hashmap, destroyer);

	/* do the visobject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (hashmap), TRUE);
	visual_object_ref (VISUAL_OBJECT (hashmap));

	return hashmap;
}

int visual_hashmap_init (VisHashmap *hashmap, VisCollectionDestroyerFunc destroyer)
{
	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (hashmap));
	visual_object_set_dtor (VISUAL_OBJECT (hashmap), visual_collection_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (hashmap), FALSE);

	/* Set the VisCollection data */
	visual_collection_set_destroyer (VISUAL_COLLECTION (hashmap), destroyer);
	visual_collection_set_destroy_func (VISUAL_COLLECTION (hashmap), hashmap_destroy);
	visual_collection_set_size_func (VISUAL_COLLECTION (hashmap), hashmap_size);
	visual_collection_set_iter_func (VISUAL_COLLECTION (hashmap), hashmap_iter);

	/* Set the VisHashmap data */
	hashmap->tablesize = 0;
	hashmap->size = 0;
	hashmap->table = NULL;

	return VISUAL_OK;
}

int visual_hashmap_put (VisHashmap *hashmap, int32_t key, void *data)
{
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	/* Create initial hashtable */
	if (hashmap->table == NULL)
		create_table (hashmap);

	hash = integer_hash (key) % hashmap->tablesize;

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is already in the chain */
	while ((mentry = visual_list_next (chain, &le)) != NULL) {
		if (mentry->key == key) {
			mentry->data = data;

			return VISUAL_OK;
		}
	}

	/* Key not in chain, append at end */
	mentry = visual_mem_new0 (VisHashmapChainEntry, 1);

	mentry->key = key;
	mentry->data = data;

	visual_list_add (chain, mentry);

	hashmap->size++;

	return VISUAL_OK;
}

int visual_hashmap_remove (VisHashmap *hashmap, int32_t key, int destroy)
{
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	/* Create initial hashtable */
	if (hashmap->table == NULL)
		return -VISUAL_ERROR_HASHMAP_NOT_IN_MAP;

	hash = integer_hash (key) % hashmap->tablesize;

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is already in the chain */
	while ((mentry = visual_list_next (chain, &le)) != NULL) {
		if (mentry->key == key) {
			if (destroy != FALSE)
				visual_list_destroy (chain, &le);
			else
				visual_list_delete (chain, &le);

			hashmap->size--;

			return VISUAL_OK;
		}
	}

	return -VISUAL_ERROR_HASHMAP_NOT_IN_MAP;
}

void *visual_hashmap_get (VisHashmap *hashmap, int32_t key)
{
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, NULL);

	/* Create initial hashtable */
	if (hashmap->table == NULL)
		return NULL;

	hash = integer_hash (key) % hashmap->tablesize;

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is already in the chain */
	while ((mentry = visual_list_next (chain, &le)) != NULL) {
		if (mentry->key == key)
			return mentry;
	}

	return NULL;
}

int visual_hashmap_set_table_size (VisHashmap *hashmap, int tablesize)
{
	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	hashmap->tablesize = tablesize;

	/* Table was not empty, rehash */
	if (hashmap->table != NULL) {

	}

	return VISUAL_OK;
}

int visual_hashmap_get_table_size (VisHashmap *hashmap)
{
	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	return hashmap->tablesize;
}

/**
 * @}
 */

