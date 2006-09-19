/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_collection.h,v 1.8 2006-09-19 18:28:51 synap Exp $
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

#ifndef _LV_COLLECTION_H
#define _LV_COLLECTION_H

#include <libvisual/lv_common.h>

VISUAL_BEGIN_DECLS

#define VISUAL_COLLECTION(obj)				(VISUAL_CHECK_CAST ((obj), VisCollection))
#define VISUAL_COLLECTIONITERATOR(obj)			(VISUAL_CHECK_CAST ((obj), VisCollectionIterator))

typedef struct _VisCollection VisCollection;
typedef struct _VisCollectionIterator VisCollectionIterator;

/**
 * A VisCollection destroyer function needs this signature, these functions are used
 * to destroy data entries within collections.
 *
 * @arg data The data that was stored in a collection entry and thus can be destroyed.
 */
typedef void (*VisCollectionDestroyerFunc)(void *data);

/**
 */
typedef int (*VisCollectionDestroyFunc)(VisCollection *collection);

/**
 */
typedef int (*VisCollectionSizeFunc)(VisCollection *collection);

/**
 */
typedef int (*VisCollectionIteratorFunc)(VisCollectionIterator *iter, VisCollection *collection);

/**
 */
typedef void (*VisCollectionIteratorAssignFunc)(VisCollectionIterator *iter, VisCollection *collection, VisObject *context,
		int index);

/**
 */
typedef void (*VisCollectionIteratorNextFunc)(VisCollectionIterator *iter, VisCollection *collection, VisObject *context);

/**
 */
typedef int (*VisCollectionIteratorHasMoreFunc)(VisCollectionIterator *iter, VisCollection *collection, VisObject *context);

/**
 */
typedef void *(*VisCollectionIteratorGetDataFunc)(VisCollectionIterator *iter, VisCollection *collection,
		VisObject *context);

/**
 */
struct _VisCollection {
	VisObject				 object;	/**< The VisObject data. */

	VisCollectionDestroyerFunc		 destroyer;
	VisCollectionDestroyFunc		 destroyfunc;
	VisCollectionSizeFunc			 sizefunc;
	VisCollectionIteratorFunc		 iterfunc;
};

/**
 */
struct _VisCollectionIterator {
	VisObject				 object;

	VisCollectionIteratorAssignFunc		 assignfunc;
	VisCollectionIteratorNextFunc		 nextfunc;
	VisCollectionIteratorHasMoreFunc	 hasmorefunc;
	VisCollectionIteratorGetDataFunc	 getdatafunc;

	VisCollection				*collection;

	VisObject				*context;
};

/* prototypes */
int visual_collection_set_destroyer (VisCollection *collection, VisCollectionDestroyerFunc destroyer);
VisCollectionDestroyerFunc visual_collection_get_destroyer (VisCollection *collection);

int visual_collection_set_destroy_func (VisCollection *collection, VisCollectionDestroyFunc destroyfunc);
VisCollectionDestroyFunc visual_collection_get_destroy_func (VisCollection *collection);

int visual_collection_set_size_func (VisCollection *collection, VisCollectionSizeFunc sizefunc);
VisCollectionSizeFunc visual_collection_get_size_func (VisCollection *collection);

int visual_collection_set_iterator_func (VisCollection *collection, VisCollectionIteratorFunc iterfunc);
VisCollectionIteratorFunc visual_collection_get_iterator_func (VisCollection *collection);

int visual_collection_dtor (VisObject *object);

int visual_collection_destroy (VisCollection *collection);
int visual_collection_size (VisCollection *collection);

VisCollectionIterator *visual_collection_get_iterator_new (VisCollection *collection);
int visual_collection_get_iterator (VisCollectionIterator *iter, VisCollection *collection);


VisCollectionIterator *visual_collection_iterator_new (
		VisCollectionIteratorAssignFunc assignfunc, VisCollectionIteratorNextFunc nextfunc,
		VisCollectionIteratorHasMoreFunc hasmorefunc, VisCollectionIteratorGetDataFunc getdatafunc,
		VisCollection *collection, VisObject *context);
int visual_collection_iterator_init (VisCollectionIterator *iter,
		VisCollectionIteratorAssignFunc assignfunc, VisCollectionIteratorNextFunc nextfunc,
		VisCollectionIteratorHasMoreFunc hasmorefunc, VisCollectionIteratorGetDataFunc getdatafunc,
		VisCollection *collection, VisObject *context);

void visual_collection_iterator_assign (VisCollectionIterator *iter, int index);
void visual_collection_iterator_next (VisCollectionIterator *iter);
int visual_collection_iterator_has_more (VisCollectionIterator *iter);
void *visual_collection_iterator_get_data (VisCollectionIterator *iter);

VISUAL_END_DECLS

#endif /* _LV_COLLECTION_H */
