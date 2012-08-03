/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Chong Kai Xiong <kaixiong@codeleft.sg>
 *               2004-2006 Dennis Smit <ds@nerds-incorporated.org>
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

#ifndef _LV_PARAM_H
#define _LV_PARAM_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_event.h>

/**
 * @defgroup VisParam VisParam
 * @{
 */

#define VISUAL_PARAM(obj)       ((VisParam *) (obj))
#define VISUAL_PARAM_VALUE(obj) ((VisParamValue *) (obj))
#define VISUAL_PARAM_LIST(obj)  ((VisParamList *) (obj))

/**
 * Different types of parameters that can be used.
 */
typedef enum {
    VISUAL_PARAM_TYPE_NONE,
    VISUAL_PARAM_TYPE_INTEGER,
    VISUAL_PARAM_TYPE_FLOAT,
    VISUAL_PARAM_TYPE_DOUBLE,
    VISUAL_PARAM_TYPE_COLOR,
    VISUAL_PARAM_TYPE_STRING,
    VISUAL_PARAM_TYPE_PALETTE,
} VisParamType;

typedef struct _VisParam      VisParam;
typedef struct _VisParamValue VisParamValue;
typedef struct _VisParamList  VisParamList;

typedef struct _VisParamValidateClosure VisParamValidateClosure;
typedef struct _VisParamChangedClosure  VisParamChangedClosure;

typedef int  (*VisParamValidateFunc) (VisParamValue *value, VisParamValue *old_value, void *priv);
typedef void (*VisParamChangedFunc)  (VisParam *param, void *priv);
typedef void (*VisDestroyFunc)       (void *data);

struct _VisParamChangedClosure
{
    VisParamChangedFunc func;
    void *              data;
    VisDestroyFunc      destroy_func;
};

struct _VisParamValidateClosure
{
    VisParamValidateFunc func;
    void *               data;
    VisDestroyFunc       destroy_func;
};

struct _VisParamValue
{
    VisParamType type;

    union {
        int          integer;
        float        single_float;
        double       double_float;
        char *       string;
        VisColor *   color;
        VisPalette * palette;
    } value;
};

struct _VisParam
{
    VisObject               object;
    char *                  name;
    char *                  description;
    VisParamValue           value;
    VisParamValue           default_value;
    VisParamValidateClosure validate_handler;
    VisList *               changed_handlers;
    VisParamList*           parent;
};

/**
 * Parameter container, is the container for a set of parameters.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamList {
    VisObject       object;       /**< The VisObject data. */
    VisList *       entries;      /**< The list that contains all the parameters. */
    VisEventQueue * eventqueue;   /**< Pointer to an optional eventqueue to which events can be emitted
                                    *  on parameter changes. */
};

LV_BEGIN_DECLS

/**
 * Creates a new VisParamList structure.
 *
 * @return A newly allocated VisParamList structure.
 */
LV_API VisParamList *visual_param_list_new (void);

/**
 * Sets the eventqueue in the VisParamList, so events can be
 * emitted on param changes.
 *
 * @param list       A pointer to the VisParamList to which the VisEventQueue needs to be set.
 * @param eventqueue A Pointer to the VisEventQueue that is used for the events the VisParamList can emit.
 */
LV_API void visual_param_list_set_eventqueue (VisParamList *list, VisEventQueue *eventqueue);

/**
 * Get the pointer to the VisEventQueue the VisParamList is
 * emitting events to.
 *
 * @param list A pointer to the VisParamList from which the VisEventQueue is requested.
 *
 * @return Pointer to the VisEventQueue possibly NULL, NULL on failure.
 */
LV_API VisEventQueue *visual_param_list_get_eventqueue (VisParamList *list);

/**
 * Adds a VisParam to a VisParamList.
 *
 * @param list  A pointer to the VisParamList in which the VisParam is added.
 * @param param A pointer to the VisParam that is added to the VisParamList.
 *
 * @return TRUE on success, FALSE otherwise
 */
LV_API int visual_param_list_add (VisParamList *list, VisParam *entry);

/**
 * Adds a list of VisParam elements, the list is terminated by an
 * entry of type VISUAL_PARAM_TYPE_END.  All the elements are
 * reallocated, so this function can be used for static param lists.
 *
 * @param list A pointer to the VisParamList in which the VisParam elements are added.
 * @param params A pointer to the VisParam elements that are added to the VisParamList.
 *
 * @return Number of entries added
 */
LV_API unsigned int visual_param_list_add_many (VisParamList *list, VisParam **params, unsigned int nparams);

/**
 * Removes a VisParam from the VisParamList by giving the
 * name of the VisParam that needs to be removed.
 *
 * @param list A pointer to the VisParamList from which a VisParam needs to be removed.
 * @param name The name of the VisParam that needs to be removed from the VisParamList.
 *
 * @return TRUE on success, FALSE otherwise
 */
LV_API int visual_param_list_remove (VisParamList *list, const char *name);

/**
 * Retrieve a VisParam from a VisParamList by giving the
 * name of the VisParam that is requested.
 *
 * @param list A pointer to the VisParamList from which a VisParam is requested.
 * @param name The name of the VisParam that is requested from the VisParamList.
 *
 * @return Pointer to the VisParam, or NULL if not found
 */
LV_API VisParam *visual_param_list_get (VisParamList *list, const char *name);

/**
 * Creates a new parameter entry
 *
 * @param name          Name
 * @param type          Type
 * @param description   Description
 * @param default_value Default Value
 *
 * @return A newly allocated VisParam
 */
LV_API VisParam *visual_param_new (const char * name,
                                   const char * description,
                                   VisParamType type,
                                   void *       default_value);

/**
 * Adds a change notification callback.
 *
 * @param param    VisParam to add a change notification callback to.
 * @param callback The notification callback, which is called on changes in the VisParam.
 * @param priv     A private that can be used in the callback function.
 */
LV_API VisParamChangedClosure *visual_param_add_callback (VisParam *          param,
                                                          VisParamChangedFunc func,
                                                          void *              priv,
                                                          VisDestroyFunc      destroy_func);

/**
 * Removes a change notification callback from the list of callbacks.
 *
 * @param param   VisParam object to remove callback from.
 * @param closure The closure pointer that was given by the visual_param_add_callback().
 *
 * @return TRUE on successful removeal, FALSE otherwise
 */
LV_API int visual_param_remove_callback (VisParam *param, VisParamChangedClosure *closure);

/**
 * Notifies all the callbacks for the given VisParam parameter.
 *
 * @param param Pointer to the VisParam of which all the change notification
 *              callbacks need to be called.
 */
LV_API void visual_param_notify_callbacks (VisParam *param);

/**
 * Checks if a VisParam is of a given name.
 *
 * @param param Pointer to the VisParam of which we want to check the name.
 * @param name  The name we want to check against.
 *
 * @return TRUE if the VisParam is the one we requested, or FALSE if not.
 */
LV_API int visual_param_has_name (VisParam *param, const char *name);

/**
 * When called, emits an event in the VisParamList it's
 * VisEventQueue when the VisEventQueue is set.
 *
 * @param param Pointer to the VisParam that is changed.
 */
LV_API void visual_param_changed (VisParam *param);

LV_API const char * visual_param_get_name        (VisParam *param);
LV_API VisParamType visual_param_get_type        (VisParam *param);
LV_API const char * visual_param_get_description (VisParam *param);

/**
 * Copies the value of the src param into the param. Also sets the
 * param to the type of which the source param is.
 *
 * @param param Pointer to the VisParam to which a parameter is set.
 * @param src Pointer to the VisParam from which the value is retrieved.
 *
 * @return TRUE on success, FALSE otherwise
 */
LV_API int visual_param_set_value_from_param     (VisParam *param, VisParam *src);
LV_API int visual_param_set_value                (VisParam *param, VisParamValue *value);
LV_API int visual_param_set_value_string         (VisParam *param, const char *string);
LV_API int visual_param_set_value_integer        (VisParam *param, int integer);
LV_API int visual_param_set_value_float          (VisParam *param, float flt);
LV_API int visual_param_set_value_double         (VisParam *param, double dbl);
LV_API int visual_param_set_value_color          (VisParam *param, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
LV_API int visual_param_set_value_color_by_color (VisParam *param, const VisColor *color);
LV_API int visual_param_set_value_palette        (VisParam *param, const VisPalette *pal);

LV_API const char *visual_param_get_value_string  (VisParam *param);
LV_API int         visual_param_get_value_integer (VisParam *param);
LV_API float       visual_param_get_value_float   (VisParam *param);
LV_API double      visual_param_get_value_double  (VisParam *param);
LV_API VisColor *  visual_param_get_value_color   (VisParam *param);
LV_API VisPalette *visual_param_get_value_palette (VisParam *param);

LV_API VisParamValue *visual_param_value_new        (VisParamType type, void *value);
LV_API void           visual_param_value_copy       (VisParamValue *value, VisParamValue *src);
LV_API void           visual_param_value_set        (VisParamValue *value, void *new_value);
LV_API void           visual_param_value_free       (VisParamValue *value);
LV_API void           visual_param_value_free_value (VisParamValue *value);

LV_API int visual_param_value_compare (VisParamValue *lhs, VisParamValue *rhs);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_PARAM_H */
