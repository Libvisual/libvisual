#ifndef _LV_PARAM_H
#define _LV_PARAM_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_event.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_PARAMCONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), 0, VisParamContainer))
#define VISUAL_PARAMENTRY_CALLBACK(obj)			(VISUAL_CHECK_CAST ((obj), 0, VisParamEntryCallback))
#define VISUAL_PARAMENTRY(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisParamEntry))

#define VISUAL_PARAM_LIST_ENTRY_STRING(name, string)	{ {}, NULL, name, VISUAL_PARAM_TYPE_STRING, string, {0, 0, 0}}
#define VISUAL_PARAM_LIST_ENTRY_INTEGER(name, val)	{ {}, NULL, name, VISUAL_PARAM_TYPE_INTEGER, NULL, {val, 0, 0}}
#define VISUAL_PARAM_LIST_ENTRY_FLOAT(name, val)	{ {}, NULL, name, VISUAL_PARAM_TYPE_FLOAT, NULL, {0, val, 0}}
#define VISUAL_PARAM_LIST_ENTRY_DOUBLE(name, val)	{ {}, NULL, name, VISUAL_PARAM_TYPE_DOUBLE, NULL, {0, 0, val}}
#define VISUAL_PARAM_LIST_ENTRY_COLOR(name, r, g, b)	{ {}, NULL, name, VISUAL_PARAM_TYPE_COLOR, NULL, {0, 0, 0}, {{}, r, g, b, 0}}
#define VISUAL_PARAM_LIST_END				{ {}, NULL, NULL, VISUAL_PARAM_TYPE_END }

#define VISUAL_PARAM_CALLBACK_ID_MAX	2147483647

/**
 * Different types of parameters that can be used.
 */
typedef enum {
	VISUAL_PARAM_TYPE_NULL,		/**< No parameter. */
	VISUAL_PARAM_TYPE_STRING,	/**< String parameter. */
	VISUAL_PARAM_TYPE_INTEGER,	/**< Integer parameter. */
	VISUAL_PARAM_TYPE_FLOAT,	/**< Floating point parameter. */
	VISUAL_PARAM_TYPE_DOUBLE,	/**< Double floating point parameter. */
	VISUAL_PARAM_TYPE_COLOR,	/**< VisColor parameter. */
	VISUAL_PARAM_TYPE_END		/**< List end, and used as terminator for VisParamEntry lists. */
} VisParamType;

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
typedef void (*VisParamChangedCallbackFunc)(const VisParamEntry *param, void *priv);


/**
 * Parameter container, is the container for a set of parameters.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamContainer {
	VisObject	 object;	/**< The VisObject data. */
	VisList		 entries;	/**< The list that contains all the parameters. */
	VisEventQueue	*eventqueue;	/**< Pointer to an optional eventqueue to which events can be emitted
					  * on parameter changes. */
};

/**
 * A parameter callback entry, used for change notification callbacks.
 */
struct _VisParamEntryCallback {
	VisObject		 	 object;	/**< The VisObject data. */
	int				 id;		/**< Callback ID. */
	VisParamChangedCallbackFunc	 callback;	/**< The param change callback function. */
};

/**
 * A parameter entry, used for plugin parameters and such.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamEntry {
	VisObject		 object;	/**< The VisObject data. */
	VisParamContainer	*parent;	/**< Parameter container in which the param entry is encapsulated. */
	char			*name;		/**< Parameter name. */
	VisParamType		 type;		/**< Parameter type. */

	char			*string;	/**< String data. */

	/* No union, we can't choose a member of the union using static initializers */
	struct {
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
	} numeric;

	VisColor		 color;		/**< VisColor data. */
	VisList			 callbacks;	/**< The change notify callbacks. */
};

/* prototypes */
VisParamContainer *visual_param_container_new (void);
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue);
VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer);

int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param);
int visual_param_container_add_many (VisParamContainer *paramcontainer, VisParamEntry *params);
int visual_param_container_remove (VisParamContainer *paramcontainer, const char *name);
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, const char *name);

VisParamEntry *visual_param_entry_new (char *name);
int visual_param_entry_add_callback (VisParamEntry *param, VisParamChangedCallbackFunc callback, void *priv);
int visual_param_entry_remove_callback (VisParamEntry *param, int id);
int visual_param_entry_notify_callbacks (VisParamEntry *param);
int visual_param_entry_is (VisParamEntry *param, const char *name);
int visual_param_entry_compare (VisParamEntry *src1, VisParamEntry *src2);
int visual_param_entry_changed (VisParamEntry *param);

int visual_param_entry_set_from_param (VisParamEntry *param, VisParamEntry *src);
int visual_param_entry_set_name (VisParamEntry *param, char *name);
int visual_param_entry_set_string (VisParamEntry *param, char *string);
int visual_param_entry_set_integer (VisParamEntry *param, int integer);
int visual_param_entry_set_float (VisParamEntry *param, float floating);
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt);
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b);
int visual_param_entry_set_color_by_color (VisParamEntry *param, const VisColor *color);

char *visual_param_entry_get_name (VisParamEntry *param);
char *visual_param_entry_get_string (VisParamEntry *param);
int visual_param_entry_get_integer (VisParamEntry *param);
float visual_param_entry_get_float (VisParamEntry *param);
double visual_param_entry_get_double (VisParamEntry *param);
VisColor *visual_param_entry_get_color (VisParamEntry *param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PARAM_H */
