/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_object.c,v 1.13 2006-09-19 18:28:51 synap Exp $
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
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "lv_log.h"
#include "lv_error.h"
#include "lv_mem.h"
#include "lv_object.h"


/**
 * @defgroup VisObject VisObject
 * @{
 */

/**
 * This function is a global VisListDestroyerFunc handler that unrefs VisObjects.
 *
 * @param data Pointer to the VisObject that needs to be unrefed
 */
void visual_object_collection_destroyer (void *data)
{
	if (data == NULL)
		return;

	visual_object_unref (VISUAL_OBJECT (data));
}

/**
 * Creates a new VisObject structure of a certain size. This can be used for
 * easy allocation of simple VisObjects.
 *
 * @return A newly allocated VisObject.
 */
VisObject *visual_object_new_of_size (visual_size_t size)
{
	VisObject *object;

	object = visual_mem_malloc0 (size);

	visual_object_ref (object);

	return object;
}

/**
 * Creates a new VisObject structure.
 *
 * @return A newly allocated VisObject, or NULL on failure.
 */
VisObject *visual_object_new ()
{
	VisObject *object;

	object = visual_mem_new0 (VisObject, 1);

	object->allocated = TRUE;

	visual_object_ref (object);

	return object;
}

/**
 * Frees the VisObject. This does not destroy the object itself but only releases the memory it's using.
 *
 * @param object Pointer to a VisObject that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL, -VISUAL_ERROR_OBJECT_NOT_ALLOCATED or error values
 *	returned by visual_mem_free on failure.
 */
int visual_object_free (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);
	visual_log_return_val_if_fail (object->allocated == TRUE, -VISUAL_ERROR_OBJECT_NOT_ALLOCATED);

	return visual_mem_free (object);
}

/**
 * Destroys the VisObject. This does destruct the VisObject
 * by using the dtor function if it's set and also frees the memory
 * it's using. It's valid to pass non allocated VisObjects,
 * the function will recognize this by a flag that is set in the VisObject.
 *
 * @param object Pointer to a VisObject that needs to be destroyed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL or error values returned byvisual_object_free on failure.
 */
int visual_object_destroy (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	if (object->dtor != NULL) {
		if (object->dtor (object) == FALSE)
			return VISUAL_OK;
	}

	if (object->allocated == TRUE)
		return visual_object_free (object);

	return VISUAL_OK;
}

/**
 * Initializes a VisObject for usage. This also ups the refcount by one, so this function really is for initial object
 * creation.
 *
 * @param object Pointer to a VisObject that is initialized.
 * @param allocated Flag to indicate if the VisObject itself is an allocated piece of memory.
 * @param dtor The destructor function, that is used to destroy the VisObject when it loses all references or when it's
 *	being destroyed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_initialize (VisObject *object, int allocated, VisObjectDtorFunc dtor)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	visual_object_set_dtor (object, dtor);
	visual_object_set_allocated (object, allocated);

	visual_object_clear (object);

	visual_object_ref (object);

	return VISUAL_OK;
}

/**
 * Clears a VisObject. This basically means setting it's private to NULL and it's refcount to 0. This won't unref, or destroy
 * the object and this function is mostly used for object creation.
 *
 * @param object Pointer to a VisObject that is to be cleared.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_clear (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	visual_object_set_private (object, NULL);
	visual_object_set_refcount (object, 0);
	visual_object_set_ref (object, NULL);
	visual_object_set_unref (object, NULL);

	return VISUAL_OK;
}

/**
 * Sets the destructor function to a VisObject.
 *
 * @param object pointer to a VisObject to which the destructor function is set.
 * @param dtor The Destructor function, that is used to destroy the VisObject when it loses all references or when it's
 *	being destroyed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_dtor (VisObject *object, VisObjectDtorFunc dtor)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->dtor = dtor;

	return VISUAL_OK;
}

/**
 * Sets whether a VisObject is allocated or not. This is used when a VisObject is unreffed. If it's
 * allocated it will get freed, if not, only the dtor gets called to cleanup the inside of the VisObject.
 *
 * @param object pointer to a VisObject to which the destructor function is set.
 * @param allocated Boolean whether a VisObject is allocated or not.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_allocated (VisObject *object, int allocated)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->allocated = allocated;

	return VISUAL_OK;
}

/**
 * Sets the refcount to a certain number. Mostly used in VisObject initialization.
 *
 * @param object Pointer to a VisObject to which the refcount is set.
 * @param refcount The value for the VisObject it's refcount.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_refcount (VisObject *object, int refcount)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->refcount = refcount;

	return VISUAL_OK;
}

/**
 * Sets the ref increase callback.
 *
 * @param object Pointer to a VisObject to which the ref increase callback is set.
 * @param ref The reference increase callback function.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_ref (VisObject *object, VisObjectRefFunc ref)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->ref = ref;

	return VISUAL_OK;
}

int visual_object_set_unref (VisObject *object, VisObjectUnrefFunc unref)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->unref = unref;

	return VISUAL_OK;
}

/**
 * Increases the reference counter for a VisObject.
 *
 * @param object Pointer to a VisObject in which the reference count is increased.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_ref (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->refcount++;

	if (object->ref != NULL)
		return object->ref (object);

	return VISUAL_OK;
}

int visual_object_ref_many (VisObject *object, ...)
{
	VisObject *va_object;
	va_list ap;

//	if (object != NULL && object != VISUAL_OBJECT_LIST_END)
		visual_object_ref (object);

	va_start (ap, object);

	while ((va_object = va_arg (ap, VisObject *)) /* != VISUAL_OBJECT_LIST_END*/) {
		if (va_object != NULL)
			visual_object_ref (va_object);
	}

	va_end (ap);

	return VISUAL_OK;
}

/**
 * Decreases the reference counter for a VisObject. If the reference counter hits zero it will
 * destruct the object using visual_object_destroy.
 *
 * @see visual_object_destroy
 *
 * @param object Pointer to a VisObject in which the reference count is decreased.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL or error values returned by
 *	visual_object_destroy on failure.
 */
int visual_object_unref (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	object->refcount--;

	/* No reference left, start dtoring of this VisObject */
	if (object->refcount <= 0) {
		if (object->unref != NULL)
			object->unref (object);

		object->refcount = 0;

		return visual_object_destroy (object);
	}

	if (object->unref != NULL)
		return object->unref (object);

	return VISUAL_OK;
}

int visual_object_unref_many (VisObject *object, ...)
{
	VisObject *va_object;
	va_list ap;

//	if (object != NULL && object != VISUAL_OBJECT_LIST_END)
		visual_object_unref (object);

	va_start (ap, object);

	while ((va_object = va_arg (ap, VisObject *))/* != VISUAL_OBJECT_LIST_END*/) {
		if (va_object != NULL)
			visual_object_unref (va_object);
	}

	va_end (ap);

	return VISUAL_OK;
}

/**
 * Returns the number of references to the object.
 *
 * @param object Pointer to a VisObject of which the number of references is requested.
 *
 * @return The number of references to the object, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_refcount (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	return object->refcount;
}

/**
 * Sets the private data pointer to a VisObject.
 *
 * @param object Pointer to a VisObject to which the private data is set.
 * @param priv Pointer to the private data that is set to the VisObject.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_private (VisObject *object, void *priv)
{
	visual_log_return_val_if_fail (object != NULL, -VISUAL_ERROR_OBJECT_NULL);

	/* mhm, this can lead to a memory leak. We must check here
	   for priv == NULL and return some -VISUAl_ERROR_NON_NULL
	   when it's not, or print some debug message at least. */
	object->priv = priv;

	return VISUAL_OK;
}

/**
 * Retrieves the private data from a VisObject.
 *
 * @param object Pointer to a VisObject from which the private data is requested.
 *
 * @return Pointer to the private data or NULL.
 */
void *visual_object_get_private (VisObject *object)
{
	visual_log_return_val_if_fail (object != NULL, NULL);

	return object->priv;
}

/**
 * @}
 */

