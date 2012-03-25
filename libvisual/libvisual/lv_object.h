/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_object.h,v 1.14 2006/01/23 21:06:24 synap Exp $
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

#ifndef _LV_OBJECT_H
#define _LV_OBJECT_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

/**
 * @defgroup VisObject VisObject
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_OBJECT(obj)				(VISUAL_CHECK_CAST ((obj), VisObject))

typedef struct _VisObject VisObject;

/**
 * The function defination for an object destructor. This can be assigned to any VisObject
 * and is mostly used for internal usage or by support libraries. Keep in mind that this should not free
 * the VisObject itself. This is done in the visual_object_destroy function itself.
 *
 * The destructor function should be safe to enter more than once, the object always contains the object
 * however make sure that freed members are set to NULL and that it's checked.
 *
 * @arg object The VisObject that is passed to the destructor.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_DTOR_FAILED on failure.
 */
typedef int (*VisObjectDtorFunc)(VisObject *object);

/**
 * The VisObject structure contains all the VisObject housekeeping data like refcounting and a pointer
 * to the VisObjectDtorFunc. Also it's possible to set private data on a VisObject.
 *
 * Nearly all libvisual structures inherent from a VisObject.
 */
struct _VisObject {
	int			 allocated;	/**< Set to TRUE if this object is allocated and should be freed completely.
						  * if set to FALSE, it will run the VisObjectDtorFunc but won't free the VisObject
						  * itself when refcount reaches 0. */
	int			 refcount;	/**< Contains the number of references to this object. */
	VisObjectDtorFunc	 dtor;		/**< Pointer to the object destruction function. */

	void			*priv;		/**< Private which can be used by application or plugin developers
						 * depending on the sub class object. */
};

/**
 * This function is a global VisListDestroyerFunc handler that unrefs VisObjects.
 *
 * @param data Pointer to the VisObject that needs to be unrefed
 *
 * @return VISUAL_OK on success, or error failures by visual_object_unref() on failure.
 */
int visual_object_collection_destroyer (void *data);


/**
 * Creates a new VisObject structure.
 *
 * @return A newly allocated VisObject, or NULL on failure.
 */
VisObject *visual_object_new (void);

/**
 * Frees the VisObject. This does not destroy the object itself but only releases the memory it's using.
 *
 * @param object Pointer to a VisObject that needs to be freed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL, -VISUAL_ERROR_OBJECT_NOT_ALLOCATED or error values
 *	returned by visual_mem_free on failure.
 */
int visual_object_free (VisObject *object);

/**
 * Destroys the VisObject. This does destruct the VisObject
 * by using the dtor function if it's set and also frees the memory
 * it's using. It's valid to pass non allocated VisObjects,
 * the function will recognize this by a flag that is set in the VisObject.
 *
 * @param object Pointer to a VisObject that needs to be destroyed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL or error values returned byvisual_object_free on failure.
 */
int visual_object_destroy (VisObject *object);

/**
 * Initializes a VisObject for usage. This also ups the refcount by
 * one, so this function really is for initial object creation.
 *
 * @param object Pointer to a VisObject that is initialized.
 * @param allocated Flag to indicate if the VisObject itself is an allocated piece of memory.
 * @param dtor The destructor function, that is used to destroy the VisObject when it loses all references or when it's
 *	being destroyed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_initialize (VisObject *object, int allocated, VisObjectDtorFunc dtor);

/**
 * Clears a VisObject. This basically means setting it's private to
 * NULL and it's refcount to 0. This won't unref, or destroy the
 * object and this function is mostly used for object creation.
 *
 * @param object Pointer to a VisObject that is to be cleared.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_clear (VisObject *object);

/**
 * Sets the destructor function to a VisObject.
 *
 * @param object pointer to a VisObject to which the destructor function is set.
 * @param dtor The Destructor function, that is used to destroy the VisObject when it loses all references or when it's
 *	being destroyed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_dtor (VisObject *object, VisObjectDtorFunc dtor);

/**
 * Sets whether a VisObject is allocated or not. This is used when a
 * VisObject is unreffed. If it's allocated it will get freed, if not,
 * only the dtor gets called to cleanup the inside of the VisObject.
 *
 * @param object pointer to a VisObject to which the destructor function is set.
 * @param allocated Boolean whether a VisObject is allocated or not.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_allocated (VisObject *object, int allocated);

/**
 * Sets the refcount to a certain number. Mostly used in VisObject initialization.
 *
 * @param object Pointer to a VisObject to which the refcount is set.
 * @param refcount The value for the VisObject it's refcount.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_refcount (VisObject *object, int refcount);

/**
 * Increases the reference counter for a VisObject.
 *
 * @param object Pointer to a VisObject in which the reference count is increased.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_ref (VisObject *object);

/**
 * Decreases the reference counter for a VisObject. If the reference counter hits zero it will
 * destruct the object using visual_object_destroy.
 *
 * @see visual_object_destroy
 *
 * @param object Pointer to a VisObject in which the reference count is decreased.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL or error values returned by
 *	visual_object_destroy on failure.
 */
int visual_object_unref (VisObject *object);

/**
 * Sets the private data pointer to a VisObject.
 *
 * @param object Pointer to a VisObject to which the private data is set.
 * @param priv Pointer to the private data that is set to the VisObject.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_OBJECT_NULL on failure.
 */
int visual_object_set_private (VisObject *object, void *priv);

/**
 * Retrieves the private data from a VisObject.
 *
 * @param object Pointer to a VisObject from which the private data is requested.
 *
 * @return Pointer to the private data or NULL.
 */
void *visual_object_get_private (VisObject *object);

/**
 * @ingroup VisObject
 *
 * Macro that will set the data after the VisObject data to 0.
 */
#define visual_object_clean(object, struct_type)	\
	visual_mem_set ((uint8_t *) (object) + sizeof (VisObject), 0, sizeof (struct_type) - sizeof (VisObject))

/**
 * @ingroup VisObject
 *
 * Macro that will copy the data after the VisObject data to another VisObject.
 */
#define visual_object_copy_data(dest, src, struct_type)			\
	visual_mem_copy ((uint8_t *) (dest) + sizeof (VisObject),	\
			(uint8_t *) (src) + sizeof (VisObject),		\
			sizeof (struct_type) - sizeof (VisObject))

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_OBJECT_H */
