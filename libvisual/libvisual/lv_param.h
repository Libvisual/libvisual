#ifndef _LV_PARAM_H
#define _LV_PARAM_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_color.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_event.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
} VisParamType;

typedef struct _VisParamContainer VisParamContainer;
typedef struct _VisParamEntry VisParamEntry;

/**
 * Parameter container, is the container for a set of parameters.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamContainer {
	VisList		 entries;	/**< The list that contains all the parameters. */
	VisEventQueue	*eventqueue;	/**< Pointer to an optional eventqueue to which events can be emitted
					  * on parameter changes. */
};

/**
 * A parameter entry, used for plugin parameters and such.
 *
 * All members should never be accessed directly, instead methods should be used.
 */
struct _VisParamEntry {
	VisParamContainer	*parent;	/**< Parameter container in which the param entry is encapsulated. */
	char			*name;		/**< Parameter name. */
	VisParamType		 type;		/**< Parameter type. */

	union {
		char		*string;		/**< String data. */
		int		 integer;		/**< Integer data. */
		float		 floating;		/**< Floating point data. */
		double		 doubleflt;		/**< Double floating point data. */
		VisColor	 color;			/**< VisColor data. */
	} data;
};

/* prototypes */
VisParamContainer *visual_param_container_new (void);
int visual_param_container_destroy (VisParamContainer *paramcontainer);
int visual_param_container_set_eventqueue (VisParamContainer *paramcontainer, VisEventQueue *eventqueue);
VisEventQueue *visual_param_container_get_eventqueue (VisParamContainer *paramcontainer);

int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param);
int visual_param_container_remove (VisParamContainer *paramcontainer, char *name);
VisParamEntry *visual_param_container_get (VisParamContainer *paramcontainer, char *name);

VisParamEntry *visual_param_entry_new (char *name);
int visual_param_entry_free (VisParamEntry *param);
int visual_param_entry_is (VisParamEntry *param, const char *name);
int visual_param_entry_changed (VisParamEntry *param);

int visual_param_entry_set_name (VisParamEntry *param, char *name);
int visual_param_entry_set_string (VisParamEntry *param, char *string);
int visual_param_entry_set_integer (VisParamEntry *param, int integer);
int visual_param_entry_set_float (VisParamEntry *param, float floating);
int visual_param_entry_set_double (VisParamEntry *param, double doubleflt);
int visual_param_entry_set_color (VisParamEntry *param, uint8_t r, uint8_t g, uint8_t b);
int visual_param_entry_set_color_by_color (VisParamEntry *param, VisColor *color);

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
