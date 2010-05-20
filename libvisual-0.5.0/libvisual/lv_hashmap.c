/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_hashmap.c,v 1.11 2006/02/17 22:00:17 synap Exp $
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
	int		 retrieved;
	int		 first;

	VisListEntry	*le;
};


static int hashmap_destroy (VisCollection *collection);
static int hashmap_chain_destroy (VisHashmap *hashmap, VisList *list);
static int hashmap_size (VisCollection *collection);
static VisCollectionIter *hashmap_iter (VisCollection *collection);

static void hashmap_iter_assign (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext, int index);
static int hashmap_iter_has_more (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext);
static void hashmap_iter_next (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext);
static void *hashmap_iter_get_data (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext);

static int integer_hash (uint32_t key);
static int string_hash (char *key);
static int get_hash (VisHashmap *hashmap, void *key, VisHashmapKeyType keytype);

static int create_table (VisHashmap *hashmap);


static int hashmap_destroy (VisCollection *collection)
{
	VisCollectionDestroyerFunc destroyer;
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);
	VisHashmapEntry *mentry;
	int i;

	for (i = 0; i < hashmap->size; i++)
		hashmap_chain_destroy (hashmap, &hashmap->table[i].list);

	if (hashmap->table != NULL)
		visual_mem_free (hashmap->table);

	hashmap->table = NULL;

	return VISUAL_OK;
}

/* We can't use collection dtor because we need to chain up (HashChainElem -> DataElem) */
static int hashmap_chain_destroy (VisHashmap *hashmap, VisList *list)
{
	VisCollectionDestroyerFunc destroyer;
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;

	destroyer = visual_collection_get_destroyer (VISUAL_COLLECTION (hashmap));

	if (destroyer == NULL) {
		while ((mentry = visual_list_next (list, &le)) != NULL)
			visual_list_destroy (list, &le);
	} else {
		while ((mentry = visual_list_next (list, &le)) != NULL) {
			destroyer (mentry->data);
			visual_list_destroy (list, &le);
		}
	}
}

static int hashmap_list_entry_destroyer (void *data)
{
	VisHashmapChainEntry *mentry = VISUAL_HASHMAPCHAINENTRY (data);

	if (mentry == NULL)
		return -1;

	if (mentry->keytype == VISUAL_HASHMAP_KEY_TYPE_STRING) {
		visual_mem_free (mentry->key.string);
	}

	return visual_mem_free (mentry);
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
	context->retrieved = FALSE;
	context->first = TRUE;

	iter = visual_collection_iter_new (hashmap_iter_assign, hashmap_iter_next, hashmap_iter_has_more,
			hashmap_iter_get_data, collection, VISUAL_OBJECT (context));

	return iter;
}

static void hashmap_iter_assign (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext, int index)
{
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);
	int i;

	if (index >= hashmap->tablesize)
		return;

	for (i = 0; i < index; i++) {
		hashmap_iter_next (iter, collection, itercontext);
	}
}

static int hashmap_iter_has_more (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext)
{
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);
	HashmapIterContext *context = HASHMAP_ITERCONTEXT (itercontext);

	if (context->index >= hashmap->tablesize)
		return FALSE;

	/* First entry */
	if (context->first) {
		context->first = FALSE;

		for (; context->index < hashmap->tablesize; context->index++) {
			if (visual_collection_size (VISUAL_COLLECTION (&hashmap->table[context->index].list)) > 0) {
				context->le = hashmap->table[context->index].list.head;
				context->retrieved = FALSE;

				return TRUE;
			}
		}
	}

	/* Check for next chain entry */
	if (context->le != NULL && context->le->next != NULL) {
		context->le = context->le->next;

		return TRUE;
	}

	/* No next in chain, next array entry */
	context->index++;
	for (; context->index < hashmap->tablesize; context->index++) {
		if (visual_collection_size (VISUAL_COLLECTION (&hashmap->table[context->index].list)) > 0) {
			context->le = hashmap->table[context->index].list.head;
			context->retrieved = FALSE;

			return TRUE;
		}
	}

	return FALSE;
}

static void hashmap_iter_next (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext)
{
	VisHashmap *hashmap = VISUAL_HASHMAP (collection);
	HashmapIterContext *context = HASHMAP_ITERCONTEXT (itercontext);

	if (context->retrieved == FALSE) {
		if (context->first == TRUE) {
			hashmap_iter_has_more (iter, collection, itercontext);

			context->first = FALSE;
		}

		context->retrieved = TRUE;

		return;
	}

	hashmap_iter_has_more (iter, collection, itercontext);
	context->retrieved = TRUE;

	return;
}

static void *hashmap_iter_get_data (VisCollectionIter *iter, VisCollection *collection, VisObject *itercontext)
{
	HashmapIterContext *context = HASHMAP_ITERCONTEXT (itercontext);
	VisHashmapChainEntry *mentry;

	mentry = context->le->data;

	return mentry->data;
}


/* Thomas Wang's 32 bit Mix Function: http://www.concentric.net/~Ttwang/tech/inthash.htm */
static int integer_hash (uint32_t key)
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);

	return key;
}

/* X31 HASH found in g_str_hash */
static int string_hash (char *key)
{
	char *p;
	int hash = 0;

	for (p = key; *p != '\0'; p++)
		hash = (hash << 5) - hash  + *p;

	return hash;
}

static int get_hash (VisHashmap *hashmap, void *key, VisHashmapKeyType keytype)
{
	if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER)
		return integer_hash (*((uint32_t *) key)) % hashmap->tablesize;
	else if (keytype = VISUAL_HASHMAP_KEY_TYPE_STRING)
		return string_hash ((char *) key) % hashmap->tablesize;

	return 0;
}

static int create_table (VisHashmap *hashmap)
{
	int i;

	hashmap->table = visual_mem_new0 (VisHashmapEntry, hashmap->tablesize);

	/* Initialize first entry */
	visual_list_init (&hashmap->table[0].list, hashmap_list_entry_destroyer);

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
	hashmap->tablesize = VISUAL_HASHMAP_START_SIZE;
	hashmap->size = 0;
	hashmap->table = NULL;

	return VISUAL_OK;
}

int visual_hashmap_put (VisHashmap *hashmap, void *key, VisHashmapKeyType keytype, void *data)
{
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	/* Create initial hashtable */
	if (hashmap->table == NULL)
		create_table (hashmap);

	hash = get_hash (hashmap, key, keytype);

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is already in the chain */
	if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER) {
		while ((mentry = visual_list_next (chain, &le)) != NULL) {
			if (mentry->keytype == keytype) {

				if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER &&
						mentry->key.integer != *((uint32_t *) key))
					continue;
				else if (keytype == VISUAL_HASHMAP_KEY_TYPE_STRING &&
						strcmp (mentry->key.string, (char *) key) != 0)
					continue;

				mentry->data = data;

				return VISUAL_OK;
			}
		}
	}

	/* Key not in chain, append at end */
	mentry = visual_mem_new0 (VisHashmapChainEntry, 1);

	mentry->keytype = keytype;

	if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER)
		mentry->key.integer = *((uint32_t *) key);
	else if (keytype == VISUAL_HASHMAP_KEY_TYPE_STRING)
		mentry->key.string = strdup ((char *) key);

	mentry->data = data;

	visual_list_add (chain, mentry);

	hashmap->size++;

	return VISUAL_OK;
}

int visual_hashmap_put_integer (VisHashmap *hashmap, uint32_t key, void *data)
{
	return visual_hashmap_put (hashmap, &key, VISUAL_HASHMAP_KEY_TYPE_INTEGER, data);
}

int visual_hashmap_put_string (VisHashmap *hashmap, char *key, void *data)
{
	return visual_hashmap_put (hashmap, key, VISUAL_HASHMAP_KEY_TYPE_STRING, data);
}

int visual_hashmap_remove (VisHashmap *hashmap, void *key, VisHashmapKeyType keytype, int destroy)
{
	VisCollectionDestroyerFunc destroyer;
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	if (hashmap->table == NULL)
		return -VISUAL_ERROR_HASHMAP_NOT_IN_MAP;

	hash = get_hash (hashmap, key, keytype);

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is in the chain */
	while ((mentry = visual_list_next (chain, &le)) != NULL) {
		if (mentry->keytype == keytype) {

			if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER &&
					mentry->key.integer != *((uint32_t *) key))
				continue;
			else if (keytype == VISUAL_HASHMAP_KEY_TYPE_STRING &&
					strcmp (mentry->key.string, (char *) key) != 0)
				continue;

			if (destroy != FALSE) {
				destroyer = visual_collection_get_destroyer (VISUAL_COLLECTION (hashmap));

				destroyer (mentry->data);
				visual_list_destroy (chain, &le);
			} else {
				visual_list_destroy (chain, &le);
			}

			hashmap->size--;

			return VISUAL_OK;
		}
	}

	return -VISUAL_ERROR_HASHMAP_NOT_IN_MAP;
}

int visual_hashmap_remove_integer (VisHashmap *hashmap, uint32_t key, int destroy)
{
	return visual_hashmap_remove (hashmap, &key, VISUAL_HASHMAP_KEY_TYPE_INTEGER, destroy);
}

int visual_hashmap_remove_string (VisHashmap *hashmap, char *key, int destroy)
{
	return visual_hashmap_remove (hashmap, key, VISUAL_HASHMAP_KEY_TYPE_STRING, destroy);
}

void *visual_hashmap_get (VisHashmap *hashmap, void *key, VisHashmapKeyType keytype)
{
	VisHashmapChainEntry *mentry;
	VisListEntry *le = NULL;
	VisList *chain;
	int hash;

	visual_log_return_val_if_fail (hashmap != NULL, NULL);

	/* Create initial hashtable */
	if (hashmap->table == NULL)
		return NULL;

	hash = get_hash (hashmap, key, keytype);

	chain = &hashmap->table[hash].list;

	/* Iterate list to check if the key is already in the chain */
	while ((mentry = visual_list_next (chain, &le)) != NULL) {
		if (mentry->keytype == keytype) {
			if (keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER &&
					mentry->key.integer != *((uint32_t *) key))
				continue;
			else if (keytype == VISUAL_HASHMAP_KEY_TYPE_STRING &&
					strcmp (mentry->key.string, (char *) key) != 0)
				continue;

			return mentry->data;
		}
	}

	return NULL;
}

void *visual_hashmap_get_integer (VisHashmap *hashmap, uint32_t key)
{
	return visual_hashmap_get (hashmap, &key, VISUAL_HASHMAP_KEY_TYPE_INTEGER);
}

void *visual_hashmap_get_string (VisHashmap *hashmap, char *key)
{
	return visual_hashmap_get (hashmap, key, VISUAL_HASHMAP_KEY_TYPE_STRING);
}

int visual_hashmap_set_table_size (VisHashmap *hashmap, int tablesize)
{
	int oldsize;

	visual_log_return_val_if_fail (hashmap != NULL, -VISUAL_ERROR_HASHMAP_NULL);

	/* Table was not empty, rehash */
	if (hashmap->table != NULL) {
		VisHashmap tempmap;

		visual_hashmap_init (&tempmap, NULL);

		tempmap.table = hashmap->table;
		tempmap.tablesize = hashmap->tablesize;
		tempmap.size = hashmap->size;

		VisCollectionIter *iter = visual_collection_get_iter (VISUAL_COLLECTION (hashmap));

		hashmap->tablesize = tablesize;
		create_table (hashmap);

		/* Rehash all entries */
		while (visual_collection_iter_has_more (iter) == TRUE) {
			VisHashmapChainEntry *mentry = visual_collection_iter_get_data (iter);

			if (mentry->keytype == VISUAL_HASHMAP_KEY_TYPE_INTEGER) {
				visual_hashmap_put_integer (hashmap, mentry->key.integer, mentry->data);

			} else if (mentry->keytype == VISUAL_HASHMAP_KEY_TYPE_STRING) {
				visual_hashmap_put_string (hashmap, mentry->key.string, mentry->data);

			}
		}

		/* Free old table */
		visual_object_unref (VISUAL_OBJECT (&tempmap));

	} else {
		hashmap->tablesize = tablesize;
		create_table (hashmap);
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

