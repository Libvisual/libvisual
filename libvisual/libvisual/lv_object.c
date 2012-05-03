/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_object.c,v 1.12 2006/01/22 13:23:37 synap Exp $
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

#include "config.h"
#include "lv_object.h"
#include "lv_common.h"

void visual_object_collection_destroyer (void *data)
{
	if (data)
		visual_object_unref (VISUAL_OBJECT (data));
}

VisObject *visual_object_new ()
{
	VisObject *object;

	object = visual_mem_new0 (VisObject, 1);
    visual_object_init (object, NULL);

	return object;
}

void visual_object_destroy (VisObject *object)
{
	visual_return_if_fail (object != NULL);

	if (object->dtor != NULL)
		object->dtor (object);
}

int visual_object_init (VisObject *object, VisObjectDtorFunc dtor)
{
	visual_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

    object->dtor = dtor;
    object->refcount = 1;

	return VISUAL_OK;
}

void visual_object_ref (VisObject *object)
{
	visual_return_if_fail (object != NULL);

	object->refcount++;
}

void visual_object_unref (VisObject *object)
{
	visual_return_if_fail (object != NULL);

	object->refcount--;

	if (object->refcount <= 0) {
		visual_object_destroy (object);
	}
}

void visual_object_set_private (VisObject *object, void *priv)
{
	visual_return_if_fail (object != NULL);

	/* mhm, this can lead to a memory leak. We must check here
	   for priv == NULL and return some -VISUAl_ERROR_NON_NULL
	   when it's not, or print some debug message at least. */
	object->priv = priv;
}

void *visual_object_get_private (VisObject *object)
{
	visual_return_val_if_fail (object != NULL, NULL);

	return object->priv;
}
