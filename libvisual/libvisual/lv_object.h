#ifndef _LV_OBJECT_H
#define _LV_OBJECT_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_OBJECT(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisObject))

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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_OBJECT_DTOR_FAILED on failure.
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

void visual_object_list_destroyer (void *data);

VisObject *visual_object_new (void);
int visual_object_free (VisObject *object);
int visual_object_destroy (VisObject *object);

int visual_object_ref (VisObject *object);
int visual_object_unref (VisObject *object);

int visual_object_set_private (VisObject *object, void *priv);
void *visual_object_get_private (VisObject *object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_OBJECT_H */
