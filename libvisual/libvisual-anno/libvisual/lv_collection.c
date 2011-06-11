/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_collection.c,v 1.6 2006/01/22 13:23:37 synap Exp $
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

#include "lv_common.h"
#include "lv_collection.h"

static int collection_iter_dtor (VisObject *object);

static int collection_iter_dtor (VisObject *object)
{
	VisCollectionIter *iter = VISUAL_COLLECTIONITER (object);

	if (iter->collection != NULL)
		visual_object_unref (VISUAL_OBJECT (iter->collection));

	if (iter->context != NULL)
		visual_object_unref (VISUAL_OBJECT (iter->context));

	iter->collection = NULL;
	iter->context = NULL;

	return VISUAL_OK;
}


/**
 * @defgroup VisCollection VisCollection
 * @{
 */

int visual_collection_set_destroyer (VisCollection *collection, VisCollectionDestroyerFunc destroyer)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	collection->destroyer = destroyer;

	return VISUAL_OK;
}

VisCollectionDestroyerFunc visual_collection_get_destroyer (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, NULL);

	return collection->destroyer;
}

int visual_collection_set_destroy_func (VisCollection *collection, VisCollectionDestroyFunc destroyfunc)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	collection->destroyfunc = destroyfunc;

	return VISUAL_OK;
}

VisCollectionDestroyFunc visual_collection_get_destroy_func (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, NULL);

	return collection->destroyfunc;
}

int visual_collection_set_size_func (VisCollection *collection, VisCollectionSizeFunc sizefunc)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	collection->sizefunc = sizefunc;

	return VISUAL_OK;
}

VisCollectionSizeFunc visual_collection_get_size_func (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, NULL);

	return collection->sizefunc;
}

int visual_collection_set_iter_func (VisCollection *collection, VisCollectionIterFunc iterfunc)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	collection->iterfunc = iterfunc;

	return VISUAL_OK;
}

VisCollectionIterFunc visual_collection_get_iter_func (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, NULL);

	return collection->iterfunc;
}

int visual_collection_dtor (VisObject *object)
{
	VisCollection *collection = VISUAL_COLLECTION (object);

	collection->destroyfunc (collection);

	return VISUAL_OK;
}

int visual_collection_destroy (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	if (collection->destroyfunc != NULL)
		return collection->destroyfunc (collection);

	return VISUAL_OK;
}

int visual_collection_size (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, -VISUAL_ERROR_COLLECTION_NULL);

	if (collection->sizefunc != NULL)
		return collection->sizefunc (collection);

	return VISUAL_OK;
}

VisCollectionIter *visual_collection_get_iter (VisCollection *collection)
{
	visual_log_return_val_if_fail (collection != NULL, NULL);

	if (collection->iterfunc != NULL)
		return collection->iterfunc (collection);

	return NULL;
}

VisCollectionIter *visual_collection_iter_new (
		VisCollectionIterAssignFunc assignfunc, VisCollectionIterNextFunc nextfunc,
		VisCollectionIterHasMoreFunc hasmorefunc, VisCollectionIterGetDataFunc getdatafunc,
		VisCollection *collection, VisObject *context)
{
	VisCollectionIter *iter;

	iter = visual_mem_new0 (VisCollectionIter, 1);

	visual_collection_iter_init (iter, assignfunc, nextfunc, hasmorefunc, getdatafunc, collection, context);

	/* do the visobject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (iter), TRUE);
	visual_object_ref (VISUAL_OBJECT (iter));

	return iter;
}

int visual_collection_iter_init (VisCollectionIter *iter,
		VisCollectionIterAssignFunc assignfunc, VisCollectionIterNextFunc nextfunc,
		VisCollectionIterHasMoreFunc hasmorefunc, VisCollectionIterGetDataFunc getdatafunc,
		VisCollection *collection, VisObject *context)
{
	visual_log_return_val_if_fail (iter != NULL, -VISUAL_ERROR_COLLECTION_ITER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (iter));
	visual_object_set_dtor (VISUAL_OBJECT (iter), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (iter), FALSE);

	/* Set the VisCollectionIter data */
	iter->assignfunc = assignfunc;
	iter->nextfunc = nextfunc;
	iter->hasmorefunc = hasmorefunc;
	iter->getdatafunc = getdatafunc;
	iter->collection = collection;
	iter->context = context;

	if (iter->collection != NULL)
		visual_object_ref (VISUAL_OBJECT (iter->collection));

	return VISUAL_OK;
}

void visual_collection_iter_assign (VisCollectionIter *iter, int index)
{
	visual_log_return_if_fail (iter != NULL);

	if (iter->assignfunc != NULL)
		iter->assignfunc (iter, iter->collection, iter->context, index);
}


void visual_collection_iter_next (VisCollectionIter *iter)
{
	visual_log_return_if_fail (iter != NULL);

	if (iter->nextfunc != NULL)
		iter->nextfunc (iter, iter->collection, iter->context);
}

int visual_collection_iter_has_more (VisCollectionIter *iter)
{
	visual_log_return_val_if_fail (iter != NULL, -VISUAL_ERROR_COLLECTION_ITER_NULL);

	if (iter->hasmorefunc != NULL)
		return iter->hasmorefunc (iter, iter->collection, iter->context);

	return FALSE;
}

void *visual_collection_iter_get_data (VisCollectionIter *iter)
{
	visual_log_return_val_if_fail (iter != NULL, NULL);

	if (iter->getdatafunc != NULL)
		return iter->getdatafunc (iter, iter->collection, iter->context);

	return NULL;
}

/**
 * @}
 */

