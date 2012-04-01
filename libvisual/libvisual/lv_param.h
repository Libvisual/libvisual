/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_param.h,v 1.32 2006/01/22 13:23:37 synap Exp $
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

VISUAL_BEGIN_DECLS

#define VISUAL_PARAMCONTAINER(obj)            (VISUAL_CHECK_CAST ((obj), VisParamContainer))
#define VISUAL_PARAMENTRY_CALLBACK(obj)       (VISUAL_CHECK_CAST ((obj), VisParamEntryCallback))
#define VISUAL_PARAMENTRY(obj)                (VISUAL_CHECK_CAST ((obj), VisParamEntry))

/* Use 0 for pointers instead of NULL because of C++ programs shocking on ((void *) 0) */
#define VISUAL_PARAM_LIST_ENTRY(name)                { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_NULL }
#define VISUAL_PARAM_LIST_ENTRY_STRING(name, string) { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_STRING , string, {0,   0  , 0  } }
#define VISUAL_PARAM_LIST_ENTRY_INTEGER(name, val)   { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_INTEGER, 0     , {val, 0  , 0  } }
#define VISUAL_PARAM_LIST_ENTRY_FLOAT(name, val)     { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_FLOAT  , 0     , {0  , val, 0  } }
#define VISUAL_PARAM_LIST_ENTRY_DOUBLE(name, val)    { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_DOUBLE , 0     , {0  , 0  , val} }
#define VISUAL_PARAM_LIST_ENTRY_COLOR(name, r, g, b) { {0}, 0, name, VISUAL_PARAM_ENTRY_TYPE_COLOR  , 0     , {0  , 0  , 0  }, { r, g, b, 255 } }
#define VISUAL_PARAM_LIST_END                        { {0}, 0, 0,    VISUAL_PARAM_ENTRY_TYPE_END }

#define VISUAL_PARAM_CALLBACK_ID_MAX    2147483647

/**
 * Different types of parameters that can be used.
 */
typedef enum {
    VISUAL_PARAM_ENTRY_TYPE_NULL,       /**< No parameter. */
    VISUAL_PARAM_ENTRY_TYPE_STRING,     /**< String parameter. */
    VISUAL_PARAM_ENTRY_TYPE_INTEGER,    /**< Integer parameter. */
    VISUAL_PARAM_ENTRY_TYPE_FLOAT,      /**< Floating point parameter. */
    VISUAL_PARAM_ENTRY_TYPE_DOUBLE,     /**< Double floating point parameter. */
    VISUAL_PARAM_ENTRY_TYPE_COLOR,      /**< VisColor parameter. */
    VISUAL_PARAM_ENTRY_TYPE_PALETTE,    /**< VisPalette parameter. */
    VISUAL_PARAM_ENTRY_TYPE_OBJECT,     /**< VisObject parameter. */
    VISUAL_PARAM_ENTRY_TYPE_COLLECTION, /**< VisCollection parameter. */
    VISUAL_PARAM_ENTRY_TYPE_END         /**< List end, and used as terminator for VisParamEntry lists. */
} VisParamEntryType;

typedef struct _VisParamContainer VisParamContainer;
typedef struct _VisParamEntryCallback VisParamEntryCallback;
typedef struct _VisParamEntry VisParamEntry;

/**
 * The param changed callback is used to be able to notify of changes within parameters. This should
 * not be used within the plugin itself, instead use the event queue there. This is so it's possible to
 * notify of changes outside the plugin. For example, this is needed by VisUI.
 *
 * @arg param Pointer to the param that has been changed, and to which the callback was set.
 * @arg priv Private argument, that can be set when adding the callback to the callback list.
 */
typedef void (*VisParamChangedCallbackFunc)(VisParamEntry *param, void *priv);


/**
 * Parameter container, is the container for a set of parameters.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamContainer {
    VisObject      object;       /**< The VisObject data. */
    VisList        entries;      /**< The list that contains all the parameters. */
    VisEventQueue *eventqueue;   /**< Pointer to an optional eventqueue to which events can be emitted
                      * on parameter changes. */
};

/**
 * A parameter callback entry, used for change notification callbacks.
 */
struct _VisParamEntryCallback {
    VisObject object;                        /**< The VisObject data. */
    int       id;                            /**< Callback ID. */
    VisParamChangedCallbackFunc callback;    /**< The param change callback function. */
};

/**
 * A parameter entry, used for plugin parameters and such.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamEntry {
    VisObject          object;     /**< The VisObject data. */
    VisParamContainer *parent;     /**< Parameter container in which the param entry is encapsulated. */
    char              *name;       /**< Parameter name. */
    VisParamEntryType  type;       /**< Parameter type. */

    char              *string;     /**< String data. */

    /* No union, we can't choose a member of the union using static initializers */
    struct {
        int            integer;    /**< Integer data. */
        float          floating;   /**< Floating point data. */
        double         doubleflt;  /**< Double floating point data. */
    } numeric;

    VisColor           color;      /**< VisColor data. */
    VisPalette        *pal;        /**< VisPalette data. */
    VisObject         *objdata;    /**< VisObject data for a VisObject parameter. */

    VisList            callbacks;  /**< The change notify callbacks. */

    VisCollection     *collection; /**< VisCollection data. */

    char              *annotation; /**< Parameter annotation. */

    /**< Fields for  configuring minimum and maximum thresholds. */

    struct {
        int    integer;
        float  floating;
        double doubleflt;
    } min; /**< ParamEntry's minimum threshold. */

    struct {
        int    integer;
        float  floating;
        double doubleflt;
    } max; /**< ParemEntry's maximum threshold. */

    /**< Deault values. */
    struct {
        int integer;
        float floating;
        double doubleflt;
    } defaultnum; /**< ParamEntry's default number value. */

    char *defaultstring;   /**< ParamEntry's default string value. */

    VisColor defaultcolor; /**< ParamEntry's default VisColor. */
};


/**
 * Creates a new VisParamContainer structure.
 *
 * @return A newly allocated VisParamContainer structure.
 */
VisParamContainer *visual_param_container_new (void);


/**
 * Sets the eventqueue in the VisParamContainer, so events can be
 * emitted on param changes.
 *
 * @param paramcontainer A pointer to the VisParamContainer to which the VisEventQueue needs to be set.
 * @param eventqueue A Pointer to the VisEventQueue that is used for the events the VisParamContainer can emit.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL on failure.
 */
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue);

/**
 * Get the pointer to the VisEventQueue the VisParamContainer is
 * emitting events to.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which the VisEventQueue is requested.
 *
 * @return Pointer to the VisEventQueue possibly NULL, NULL on failure.
 */
VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer);

/**
 * Adds a VisParamEntry to a VisParamContainer.
 *
 * @param paramcontainer A pointer to the VisParamContainer in which the VisParamEntry is added.
 * @param param A pointer to the VisParamEntry that is added to the VisParamContainer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL, -VISUAL_ERROR_PARAM_NULL or
 *    error values returned by visual_list_add () on failure.
 */
int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param);

/**
 * Adds a VisParamEntry to a VisParamContainer, storing default values
 * in the process.
 *
 * @param paramcontainer A pointer to the VisParamContainer in which the VisParamEntry is added.
 * @param param A pointer to the VisParamEntry that is added to the VisParamContainer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL, -VISUAL_ERROR_PARAM_NULL or
 *    error values returned by visual_list_add () on failure.
 */
int visual_param_container_add_with_defaults (VisParamContainer *paramcontainer, VisParamEntry *param);


/**
 * Adds a list of VisParamEntry elements, the list is terminated by an
 * entry of type VISUAL_PARAM_ENTRY_TYPE_END.  All the elements are
 * reallocated, so this function can be used for static param lists.
 *
 * @param paramcontainer A pointer to the VisParamContainer in which the VisParamEntry elements are added.
 * @param params A pointer to the VisParamEntry elements that are added to the VisParamContainer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL or -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_container_add_many (VisParamContainer *paramcontainer, VisParamEntry *params);

/**
 * Removes a VisParamEntry from the VisParamContainer by giving the
 * name of the VisParamEntry that needs to be removed.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry needs to be removed.
 * @param name The name of the VisParamEntry that needs to be removed from the VisParamContainer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL, -VISUAL_ERROR_NULL
 *    or -VISUAL_ERROR_PARAM_NOT_FOUND on failure.
 */
int visual_param_container_remove (VisParamContainer *paramcontainer, const char *name);


/**
 * Clones the source VisParamContainer into the destination
 * VisParamContainer. When an entry with a certain name already exists
 * in the destination container, it will be overwritten with a new
 * value.
 *
 * @param destcont A pointer to the VisParamContainer in which the VisParamEntry values are copied.
 * @param srccont A pointer to the VisParamContainer from which the VisParamEntry values are copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL, on failure.
 */
int visual_param_container_copy (VisParamContainer *destcont, VisParamContainer *srccont);


/**
 * Copies matching VisParamEntry elements from srccont into destcont,
 * matching on the name.
 *
 * @param destcont A pointer to the VisParamContainer in which the VisParamEntry values are copied.
 * @param srccont A pointer to the VisParamContainer from which the VisParamEntry values are copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_CONTAINER_NULL, on failure.
 */
int visual_param_container_copy_match (VisParamContainer *destcont, VisParamContainer *srccont);


/**
 * Retrieve a VisParamEntry from a VisParamContainer by giving the
 * name of the VisParamEntry that is requested.
 *
 * @param paramcontainer A pointer to the VisParamContainer from which a VisParamEntry is requested.
 * @param name The name of the VisParamEntry that is requested from the VisParamContainer.
 *
 * @return Pointer to the VisParamEntry, or NULL.
 */
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, const char *name);


/**
 * Creates a new VisParamEntry structure.
 *
 * @param name The name that is assigned to the VisParamEntry.
 *
 * @return A newly allocated VisParamEntry structure.
 */
VisParamEntry *visual_param_entry_new (const char *name);


/**
 * Adds a change notification callback, this shouldn't be used to get
 * notificated within a plugin, but is for things like VisUI.
 *
 * @param param Pointer to the VisParamEntry to which a change notification callback is added.
 * @param callback The notification callback, which is called on changes in the VisParamEntry.
 * @param priv A private that can be used in the callback function.
 *
 * return callback id in the form of a positive value on success,
 *     -VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_CALLBACK_NULL or
 *     -VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY on failure.
 */
int visual_param_entry_add_callback (VisParamEntry *param, VisParamChangedCallbackFunc callback, void *priv);

/**
 * Removes a change notification callback from the list of callbacks.
 *
 * @param param Pointer to the VisParamEntry from which a change notification callback is removed.
 * @param id The callback ID that was given by the visual_param_entry_add_callback method.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_remove_callback (VisParamEntry *param, int id);

/**
 * Notifies all the callbacks for the given VisParamEntry parameter.
 *
 * @param param Pointer to the VisParamEntry of which all the change notification
 *     callbacks need to be called.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_notify_callbacks (VisParamEntry *param);

/**
 * Checks if the VisParamEntry it's name is the given name.
 *
 * @param param Pointer to the VisParamEntry of which we want to check the name.
 * @param name The name we want to check against.
 *
 * @return TRUE if the VisParamEntry is the one we requested, or FALSE if not.
 */
int visual_param_entry_is (VisParamEntry *param, const char *name);

/**
 * Compares two parameters with each other, When they are the same,
 * TRUE is returned, if not FALSE.  Keep in mind that FALSE is always
 * returned for VISUAL_PARAM_ENTRY_TYPE_PALETTE and
 * VISUAL_PARAM_ENTRY_TYPE_OBJECT.
 *
 * @param src1 Pointer to the first VisParamEntry for comparison.
 * @param src2 Pointer to the second VisParamEntry for comparison.
 *
 * @return TRUE if the same, FALSE if not the same,
 *    -VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_INVALID_TYPE or -VISUAL_ERROR_IMPOSSIBLE on failure.
 */
int visual_param_entry_compare (VisParamEntry *src1, VisParamEntry *src2);

/**
 * When called, emits an event in the VisParamContainer it's
 * VisEventQueue when the VisEventQueue is set.
 *
 * @param param Pointer to the VisParamEntry that is changed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_changed (VisParamEntry *param);

/**
 * Retrieves the VisParamEntryType from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the VisParamEntryType is requested.
 *
 * @return The VisParamEntryType on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
VisParamEntryType visual_param_entry_get_type (VisParamEntry *param);

/**
 * Copies the value of the src param into the param. Also sets the
 * param to the type of which the source param is.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param src Pointer to the VisParamEntry from which the value is retrieved.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL, -VISUAL_ERROR_PARAM_INVALID_TYPE on failure.
 */
int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src);

/**
 * Set the name for a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry to which the name is set.
 * @param name The name that is set to the VisParamEntry.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_name (VisParamEntry *param, const char *name);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_STRING and
 * assigns the string given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param string The string for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_string (VisParamEntry *param, const char *string);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_INTEGER and
 * assigns the integer given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param integer The integer value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_integer (VisParamEntry *param, int integer);


/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_FLOAT and assigns
 * the float given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param floating The float value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_float (VisParamEntry *param, float floating);


/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_DOUBLE and
 * assigns the double given as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param doubleflt The double value for this parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLOR and assigns
 * the rgb values given as arguments to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param r The red value for this color parameter.
 * @param g The green value for this color parameter.
 * @param b The blue value for this color parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLOR and assigns
 * the rgb values from the given VisColor as argument to it.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param color Pointer to the VisColor from which the rgb values are copied into the parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_color_by_color (VisParamEntry *param, VisColor *color);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_PALETTE and
 * assigns a VisPalette to the VisParamEntry.  This function does not
 * check if there is a difference between the prior set palette and
 * the new one, and always emits the changed event. so watch out with
 * usage.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param pal Pointer to the VisPalette from which the palette data is retrieved for the VisParamEntry.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_palette (VisParamEntry *param, VisPalette *pal);

/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_OBJECT and
 * assigns a VisObject to the VisParamEntry.  With a VisObject
 * VisParamEntry, the VisObject is referenced, not cloned.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param object Pointer to the VisObject that is linked to the VisParamEntry.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_object (VisParamEntry *param, VisObject *object);


/**
 * Sets the VisParamEntry to VISUAL_PARAM_ENTRY_TYPE_COLLECTION and
 * assigns a VisCollection to the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param object Pointer to the VisObject that is linked to the VisParamEntry.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_set_collection (VisParamEntry *param, VisCollection *collection);


/**
 * Sets the annotation field of the VisParamEntry to the provided null
 * terminated string.
 *
 * @param param Pointer to the VisParamEntry to which a parameter is set.
 * @param anno A null terminated string which to assign to the VisParamEntry's annotation field.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PARAM_NULL or -VISUAL_ERROR_PARAM_ANNO_NULL on failure.
 */
int visual_param_entry_set_annotation (VisParamEntry *param, char *anno);

/**
 * Get the name of the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the name is requested.
 *
 * @return The name of the VisParamEntry or NULL.
 */
char *visual_param_entry_get_name (VisParamEntry *param);

/**
 * Get the string parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the string parameter is requested.
 *
 * @return The string parameter from the VisParamEntry or NULL.
 */
char *visual_param_entry_get_string (VisParamEntry *param);

/**
 * Get the integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the integer parameter is requested.
 *
 * @return The integer parameter from the VisParamEntry.
 */
int visual_param_entry_get_integer (VisParamEntry *param);

/**
 * Get the float parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the float parameter is requested.
 *
 * @return The float parameter from the VisParamEntry.
 */
float visual_param_entry_get_float (VisParamEntry *param);

/**
 * Get the double parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the double parameter is requested.
 *
 * @return The double parameter from the VisParamEntry.
 */
double visual_param_entry_get_double (VisParamEntry *param);


/**
 * Get the color parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the color parameter is requested.
 *
 * @return Pointer to the VisColor parameter from the VisParamEntry. It's adviced to
 *    use the VisColor that is returned as read only seen changing it directly won't emit events and
 *    can cause synchronous problems between the plugin and the parameter system. Instead use the
 *    visual_param_entry_set_color* methods to change the parameter value.
 */
VisColor *visual_param_entry_get_color (VisParamEntry *param);


/**
 * Get the palette parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the palette parameter is requested.
 *
 * @return Pointer to the VisPalette parameter from the VisParamEntry. The returned VisPalette
 *    should be exclusively used as read only.
 */
VisPalette *visual_param_entry_get_palette (VisParamEntry *param);

/**
 * Get the object parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the object parameter is requested.
 *
 * @return Pointer to the VisObject parameter from the VisParamEntry.
 */
VisObject *visual_param_entry_get_object (VisParamEntry *param);


/**
 * Get the collection parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the collection parameter is requested.
 *
 * @return Pointer to the VisCollection parameter from the VisParamEntry.
 */
VisCollection *visual_param_entry_get_collection (VisParamEntry *param);

/**
 * Get the annotation parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the annotation parameter is requested.
 *
 * @return Pointer to the annotation string parameter from the VisParamEntry.
 */
char *visual_param_entry_get_annotation (VisParamEntry *param);


/**
 * Set the minimum integer parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_min_set_integer (VisParamEntry *param, int integer);

/**
 * Set the minimum float parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_min_set_float (VisParamEntry *param, float floating);

/**
 * Set the minimum double parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_min_set_double (VisParamEntry *param, double doubleflt);

/**
 * Get the minimum integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the minimum integer parameter from the VisParamEntry.
 */
int visual_param_entry_min_get_integer (VisParamEntry *param);

/**
 * Get the minimum float parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the minimum float parameter from the VisParamEntry.
 */
float visual_param_entry_min_get_float(VisParamEntry *param);

/**
 * Get the minimum double parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the minimum double parameter from the VisParamEntry.
 */
double visual_param_entry_min_get_double (VisParamEntry *param);

/**
 * Set the maxium integer parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_max_set_integer (VisParamEntry *param, int integer);

/**
 * Set the maxium floating point parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_max_set_float (VisParamEntry *param, float floating);

/**
 * Set the maxium double precision parameter for VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_max_set_double (VisParamEntry *param, double doubleflt);

/**
 * Get the maximum integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the maximum integer parameter from the VisParamEntry.
 */
int visual_param_entry_max_get_integer (VisParamEntry *param);

/**
 * Get the maximum float parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the maximum floating point parameter from the VisParamEntry.
 */
float visual_param_entry_max_get_float(VisParamEntry *param);


/**
 * Get the maximum double precision parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the maxiumum double precision parameter from the VisParamEntry.
 */
double visual_param_entry_max_get_double (VisParamEntry *param);


/**
 * Set the default string parameter for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_default_set_string(VisParamEntry *param, char *str);

/**
 * Set the default integer parameter for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_default_set_integer(VisParamEntry *param, int integer);

/**
 * Set the default floating point parameter for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_default_set_float (VisParamEntry *param, float floating);

/**
 * Set the default floating point parameter for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_default_set_double (VisParamEntry *param, double doubleflt);


/**
 * Set the default color parameter for the VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return VISUAL_OK on sucess, -VISUAL_ERROR_PARAM_NULL on failure.
 */
int visual_param_entry_default_set_color (VisParamEntry *param, VisColor *color);

/**
 * Get the default string parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the default string parameter from the VisParamEntry.
 */
char * visual_param_entry_default_get_string (VisParamEntry *param);

/**
 * Get the default integer parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the default integer parameter from the VisParamEntry.
 */
int visual_param_entry_default_get_integer (VisParamEntry *param);

/**
 * Get the default floating point parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the default floating point parameter from the VisParamEntry.
 */
float visual_param_entry_default_get_float(VisParamEntry *param);

/**
 * Get the default double precision parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the default double precision parameter from the VisParamEntry.
 */
double visual_param_entry_default_get_double (VisParamEntry *param);

/**
 * Get the default VisColor parameter from a VisParamEntry.
 *
 * @param param Pointer to the VisParamEntry from which the parameter is requested.
 *
 * @return Pointer to the default VisColor parameter from the VisParamEntry.
 */
VisColor *visual_param_entry_default_get_color (VisParamEntry *param);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_PARAM_H */
