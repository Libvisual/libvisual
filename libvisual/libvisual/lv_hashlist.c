/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_hashlist.c,v 1.4 2006/01/22 13:23:37 synap Exp $
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
#include "lv_hashlist.h"

static int hashlist_destroy (VisCollection *collection);
static int hashlist_size (VisCollection *collection);
static VisCollectionIter *hashlist_iter (VisCollection *collection);

static int hashlist_destroy (VisCollection *collection)
{
	VisHashlist *hashlist = VISUAL_HASHLIST (collection);
	VisListEntry *le = NULL;

	/* Destroy all entries in hashlist first */
	while (visual_list_next (hashlist->list, &le) != NULL) {
		VisListEntry *prev = le;
		VisListEntry *next = le;

		visual_list_prev (hashlist->list, &prev);
		visual_list_next (hashlist->list, &next);

		visual_hashlist_remove_list_entry (hashlist, le);

		if (next == NULL)
			break;

		le = prev;
	}

	/* Destroy the rest */
	if (hashlist->list != NULL)
		visual_object_unref (VISUAL_OBJECT (hashlist->list));

	if (hashlist->index != NULL)
		visual_object_unref (VISUAL_OBJECT (hashlist->index));

	hashlist->list = NULL;
	hashlist->index = NULL;

	return VISUAL_OK;
}

static int hashlist_size (VisCollection *collection)
{
	VisHashlist *hashlist = VISUAL_HASHLIST (collection);

	return visual_collection_size (VISUAL_COLLECTION (hashlist->list));
}

static VisCollectionIter *hashlist_iter (VisCollection *collection)
{
	VisHashlist *hashlist = VISUAL_HASHLIST (collection);

	return visual_collection_get_iter (VISUAL_COLLECTION (hashlist->list));
}


/**
 * @defgroup VisHashlist VisHashlist
 * @{
 */

/**
 * Creates a new VisHashlist.
 * 
 * @return A newly allocated VisHashlist.
 */

VisHashlist *visual_hashlist_new (VisCollectionDestroyerFunc destroyer, int size)
{
	VisHashlist *hashlist;

	hashlist = visual_mem_new0 (VisHashlist, 1);

	visual_hashlist_init (hashlist, destroyer, size);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (hashlist), TRUE);
	visual_object_ref (VISUAL_OBJECT (hashlist));

	return hashlist;
}

int visual_hashlist_init (VisHashlist *hashlist, VisCollectionDestroyerFunc destroyer, int size)
{
	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (hashlist));
	visual_object_set_dtor (VISUAL_OBJECT (hashlist), visual_collection_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (hashlist), FALSE);

	/* Set the VisCollection data */
	visual_collection_set_destroyer (VISUAL_COLLECTION (hashlist), destroyer);
	visual_collection_set_destroy_func (VISUAL_COLLECTION (hashlist), hashlist_destroy);
	visual_collection_set_size_func (VISUAL_COLLECTION (hashlist), hashlist_size);
	visual_collection_set_iter_func (VISUAL_COLLECTION (hashlist), hashlist_iter);

	/* Set the VisHashlist data */
	visual_hashlist_set_size (hashlist, size);

	hashlist->list = visual_list_new (NULL);

	hashlist->index = visual_hashmap_new (NULL); /* FIXME create in set_limits, rehash if not NULL */
	visual_hashmap_set_table_size (hashlist->index, size); /* <- also */

	return VISUAL_OK;
}

int visual_hashlist_clear (VisHashlist *hashlist)
{
	VisListEntry *le = NULL;

	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);

	/* Destroy all entries in hashlist first */
	while (visual_list_next (hashlist->list, &le) != NULL)
		visual_hashlist_remove_list_entry (hashlist, le);

	if (hashlist->index != NULL)
		visual_object_unref (VISUAL_OBJECT (hashlist->index));

	hashlist->index = visual_hashmap_new (NULL);
	visual_hashmap_set_table_size (hashlist->index, hashlist->size);

	return VISUAL_OK;
}

int visual_hashlist_put (VisHashlist *hashlist, char *key, void *data)
{
	VisHashlistEntry *hentry;
	VisListEntry *le;

	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);
	visual_log_return_val_if_fail (key != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);

	le = visual_hashmap_get_string (hashlist->index, key);

	if (le != NULL) {
		hentry = le->data;

		hentry->data = data;

	} else {
		hentry = visual_mem_new0 (VisHashlistEntry, 1);

		hentry->key = key;
		hentry->data = data;

		visual_list_add (hashlist->list, hentry);

		le = hashlist->list->tail;

		visual_hashmap_put_string (hashlist->index, key, le);
	}

	return VISUAL_OK;
}

int visual_hashlist_remove (VisHashlist *hashlist, char *key)
{
	VisListEntry *le;

	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);
	visual_log_return_val_if_fail (key != NULL, -VISUAL_ERROR_NULL);

	le = visual_hashmap_get_string (hashlist->index, key);

	if (le != NULL)
		visual_hashlist_remove_list_entry (hashlist, le);

	return VISUAL_OK;
}

int visual_hashlist_remove_list_entry (VisHashlist *hashlist, VisListEntry *le)
{
	VisCollectionDestroyerFunc destroyer;
	VisHashlistEntry *hentry;

	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);
	visual_log_return_val_if_fail (le != NULL, -VISUAL_ERROR_LIST_ENTRY_NULL);

	hentry = le->data;

	visual_hashmap_remove_string (hashlist->index, hentry->key, FALSE);

	destroyer = visual_collection_get_destroyer (VISUAL_COLLECTION (hashlist));

	if (destroyer != NULL)
		destroyer (hentry->data);

	visual_list_destroy (hashlist->list, &le);

	return VISUAL_OK;
}

void *visual_hashlist_get (VisHashlist *hashlist, char *key)
{
	VisHashlistEntry *hentry;
	VisListEntry *le;

	visual_log_return_val_if_fail (hashlist != NULL, NULL);
	visual_log_return_val_if_fail (key != NULL, NULL);

	le = visual_hashmap_get_string (hashlist->index, key);

	if (le == NULL)
		return NULL;

	hentry = le->data;

	return hentry->data;
}

int visual_hashlist_get_size (VisHashlist *hashlist)
{
	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);

	return visual_collection_size (VISUAL_COLLECTION (hashlist->list));
}

int visual_hashlist_set_size (VisHashlist *hashlist, int size)
{
	visual_log_return_val_if_fail (hashlist != NULL, -VISUAL_ERROR_HASHLIST_NULL);

	/* FIXME limit size change, rehash the index */

	hashlist->size = size;

	return VISUAL_OK;
}

VisList *visual_hashlist_get_list (VisHashlist *hashlist)
{
	visual_log_return_val_if_fail (hashlist != NULL, NULL);

	return hashlist->list;
}

/**
 * @}
 */

