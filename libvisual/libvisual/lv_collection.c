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

#include "lv_common.h"
#include "lv_collection.h"

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


void *visual_collection_iter_next (VisCollectionIter *iter)
{

}

int visual_collection_iter_has_more (VisCollectionIter *iter)
{

}

/**
 * @}
 */

