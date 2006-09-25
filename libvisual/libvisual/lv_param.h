/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_param.h,v 1.35 2006-09-25 20:42:53 synap Exp $
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

#include <libvisual/lv_common.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_hashmap.h>
#include <libvisual/lv_string.h>

VISUAL_BEGIN_DECLS

#define VISUAL_PARAMCONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), VisParamContainer))
#define VISUAL_PARAMENTRY_CALLBACK(obj)			(VISUAL_CHECK_CAST ((obj), VisParamEntryCallback))
#define VISUAL_PARAMENTRY(obj)				(VISUAL_CHECK_CAST ((obj), VisParamEntry))

/* Use 0 for pointers instead of NULL because of C++ programs shocking on ((void *) 0) */
#define VISUAL_PARAM_LIST_ENTRY(name)				{ name, VISUAL_PARAM_ENTRY_TYPE_NULL }
#define VISUAL_PARAM_LIST_ENTRY_STRING(name, string)		{ name, VISUAL_PARAM_ENTRY_TYPE_STRING, string, 0 }
#define VISUAL_PARAM_LIST_ENTRY_INTEGER(name, val, lim)		{ name, VISUAL_PARAM_ENTRY_TYPE_INTEGER, 0, val, lim }
#define VISUAL_PARAM_LIST_ENTRY_FLOAT(name, val, lim)		{ name, VISUAL_PARAM_ENTRY_TYPE_FLOAT, 0, val, lim }
#define VISUAL_PARAM_LIST_ENTRY_DOUBLE(name, val, lim)		{ name, VISUAL_PARAM_ENTRY_TYPE_DOUBLE, 0, val, lim }
#define VISUAL_PARAM_LIST_ENTRY_COLOR(name, r, g, b)		{ name, VISUAL_PARAM_ENTRY_TYPE_COLOR, 0, 0, {{}, r, g, b, 0}}
#define VISUAL_PARAM_LIST_END					{ 0, VISUAL_PARAM_ENTRY_TYPE_END }

#define VISUAL_PARAM_LIMIT_NONE					{ VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL, 0, 0, 0, 0, 0, 0 }
#define VISUAL_PARAM_LIMIT_BOOLEAN				{ VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL, 0, 0, 0, 0, 0, 0 }
#define VISUAL_PARAM_LIMIT_INTEGER(min, max)			{ VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER, min, 0, 0, max, 0, 0 }
#define VISUAL_PARAM_LIMIT_FLOAT(min, max)			{ VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT, 0, min, 0, 0, max, 0 }
#define VISUAL_PARAM_LIMIT_DOUBLE(min, max)			{ VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE, 0, 0, min, 0, 0, max }

/* FIXME: put all limit in defines lv_types or lv_defines or something */
#define VISUAL_PARAM_CALLBACK_ID_MAX	2147483647

/**
 * Different types of parameters that can be used.
 */
typedef enum {
	VISUAL_PARAM_ENTRY_TYPE_NULL,		/**< No parameter. */
	VISUAL_PARAM_ENTRY_TYPE_STRING,		/**< String parameter. */
	VISUAL_PARAM_ENTRY_TYPE_INTEGER,	/**< Integer parameter. */
	VISUAL_PARAM_ENTRY_TYPE_FLOAT,		/**< Floating point parameter. */
	VISUAL_PARAM_ENTRY_TYPE_DOUBLE,		/**< Double floating point parameter. */
	VISUAL_PARAM_ENTRY_TYPE_COLOR,		/**< VisColor parameter. */
	VISUAL_PARAM_ENTRY_TYPE_PALETTE,	/**< VisPalette parameter. */
	VISUAL_PARAM_ENTRY_TYPE_OBJECT,		/**< VisObject parameter. */
	VISUAL_PARAM_ENTRY_TYPE_END		/**< List end, and used as terminator for VisParamEntry lists. */
} VisParamEntryType;

typedef enum {
	VISUAL_PARAM_ENTRY_LIMIT_TYPE_NULL,
	VISUAL_PARAM_ENTRY_LIMIT_TYPE_INTEGER,
	VISUAL_PARAM_ENTRY_LIMIT_TYPE_FLOAT,
	VISUAL_PARAM_ENTRY_LIMIT_TYPE_DOUBLE,
	VISUAL_PARAM_ENTRY_LIMIT_TYPE_END
} VisParamEntryLimitType;

typedef struct _VisParamContainer VisParamContainer;
typedef struct _VisParamEntryCallback VisParamEntryCallback;
typedef struct _VisParamEntryLimitProxy VisParamEntryLimitProxy;
typedef struct _VisParamEntryLimit VisParamEntryLimit;
typedef struct _VisParamEntryProxy VisParamEntryProxy;
typedef struct _VisParamEntry VisParamEntry;

#define _LV_VIS_PARAM_ENTRY_FORWARD_DECL	1
#include <libvisual/lv_event.h>

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
	VisObject	 object;	/**< The VisObject data. */
	VisHashmap	 entries;	/**< The list that contains all the parameters. */
	VisEventQueue	*eventqueue;	/**< Pointer to an optional eventqueue to which events can be emitted
					  * on parameter changes. */
};

/**
 * A parameter callback entry, used for change notification callbacks.
 */
struct _VisParamEntryCallback {
	VisObject			 object;	/**< The VisObject data. */
	int				 id;		/**< Callback ID. */
	VisParamChangedCallbackFunc	 callback;	/**< The param change callback function. */
};

/**
 *
 */
struct _VisParamEntryLimitProxy {
	VisParamEntryLimitType	type;

	struct {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} min;

	struct {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} max;
};

/**
 *
 */
struct _VisParamEntryLimit {
	VisParamEntryLimitType	type;

	union {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} min;

	union {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} max;
};

/**
 * A proxy type for param entries so they can be defined statically more easily and converted
 * as real entries in a param container.
 */
struct _VisParamEntryProxy {
	const char		*name;

	VisParamEntryType	 type;

	char			*string;
	double			 value;

	VisColor		 color;

	VisParamEntryLimitProxy	 limit;
};

/**
 * A parameter entry, used for plugin parameters and such.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamEntry {
	VisObject		 object;	/**< The VisObject data. */
	VisParamContainer	*parent;	/**< Parameter container in which the param entry is encapsulated. */
	VisString		*name;		/**< Parameter name. */
	VisParamEntryType	 type;		/**< Parameter type. */

	VisParamEntryLimit	 limit;		/**< Parameter limits. */

	char			*string;	/**< String data. */

	union {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} numeric;

	VisColor		 color;		/**< VisColor data. */
	VisPalette		 pal;		/**< VisPalette data. */
	VisObject		*objdata;	/**< VisObject data for a VisObject parameter. */

	VisList			 callbacks;	/**< The change notify callbacks. */
};


/* prototypes */
VisParamContainer *visual_param_container_new (void);
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue);
VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer);

int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param);
int visual_param_container_add_many (VisParamContainer *paramcontainer, VisParamEntry *params);
int visual_param_container_add_many_proxy (VisParamContainer *paramcontainer, VisParamEntryProxy *proxies);
int visual_param_container_remove (VisParamContainer *paramcontainer, VisString *name);
int visual_param_container_copy (VisParamContainer *destcont, VisParamContainer *srccont);
int visual_param_container_copy_match (VisParamContainer *destcont, VisParamContainer *srccont);
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, VisString *name);

VisParamEntry *visual_param_entry_new (VisString *name);
int visual_param_entry_add_callback (VisParamEntry *param, VisParamChangedCallbackFunc callback, void *priv);
int visual_param_entry_remove_callback (VisParamEntry *param, int id);
int visual_param_entry_notify_callbacks (VisParamEntry *param);
int visual_param_entry_is (VisParamEntry *param, VisString *name);
int visual_param_entry_compare (VisParamEntry *src1, VisParamEntry *src2);
int visual_param_entry_changed (VisParamEntry *param);
VisParamEntryType visual_param_entry_get_type (VisParamEntry *param);

int visual_param_entry_set_from_proxy_param (VisParamEntry *param, VisParamEntryProxy *proxy);
int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src);
int visual_param_entry_set_name (VisParamEntry *param, VisString *name);
int visual_param_entry_set_string (VisParamEntry *param, char *string);
int visual_param_entry_set_integer (VisParamEntry *param, int integer);
int visual_param_entry_set_float (VisParamEntry *param, float floating);
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt);
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b);
int visual_param_entry_set_color_by_color (VisParamEntry *param, VisColor *color);
int visual_param_entry_set_palette (VisParamEntry *param, VisPalette *pal);
int visual_param_entry_set_object (VisParamEntry *param, VisObject *object);

VisString *visual_param_entry_get_name (VisParamEntry *param);
char *visual_param_entry_get_string (VisParamEntry *param);
int visual_param_entry_get_integer (VisParamEntry *param);
float visual_param_entry_get_float (VisParamEntry *param);
double visual_param_entry_get_double (VisParamEntry *param);
VisColor *visual_param_entry_get_color (VisParamEntry *param);
VisPalette *visual_param_entry_get_palette (VisParamEntry *param);
VisObject *visual_param_entry_get_object (VisParamEntry *param);

int visual_param_entry_limit_set_from_limit_proxy (VisParamEntry *param, VisParamEntryLimitProxy *limit);
int visual_param_entry_limit_set_from_limit (VisParamEntry *param, VisParamEntryLimit *limit);
int visual_param_entry_limit_set_integer (VisParamEntry *param, int min, int max);
int visual_param_entry_limit_set_float (VisParamEntry *param, float min, float max);
int visual_param_entry_limit_set_double (VisParamEntry *param, double min, double max);
int visual_param_entry_limit_nearest_integer (VisParamEntry *param, int *integer);
int visual_param_entry_limit_nearest_float (VisParamEntry *param, float *floating);
int visual_param_entry_limit_nearest_double (VisParamEntry *param, double *doubleflt);
double visual_param_entry_limit_get_minimum (VisParamEntryLimit *limit);
double visual_param_entry_limit_get_maximum (VisParamEntryLimit *limit);
double visual_param_entry_limit_proxy_get_minimum (VisParamEntryLimitProxy *limit);
double visual_param_entry_limit_proxy_get_maximum (VisParamEntryLimitProxy *limit);


VISUAL_END_DECLS

#endif /* _LV_PARAM_H */
