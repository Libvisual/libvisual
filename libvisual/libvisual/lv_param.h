#ifndef _LV_PARAM_H
#define _LV_PARAM_H

#include <libvisual/lv_list.h>
#include <libvisual/lv_event.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	VISUAL_PARAM_TYPE_NULL,
	VISUAL_PARAM_TYPE_STRING,
	VISUAL_PARAM_TYPE_INTEGER,
	VISUAL_PARAM_TYPE_FLOAT
} VisParamType;

typedef enum {
	VISUAL_PARAM_FLAG_NONE		= 0,
	VISUAL_PARAM_FLAG_RONLY		= 1,
	VISUAL_PARAM_FLAG_RW		= 2
} VisParamFlags;

typedef struct _VisParamContainer VisParamContainer;
typedef struct _VisParamEntry VisParamEntry;

struct _VisParamContainer {
	VisList		 entries;
	VisEventQueue	*eventqueue;
};

struct _VisParamEntry {
	VisParamContainer	*parent;
	char			*name;
	VisParamType		 type;
	int			 flags;

	union {
		char	*string;
		int	*integer;
		float	*floating;
		/** @todo add a VisColor entry as well */
	} data;
};

/* prototypes */
VisParamContainer *visual_param_container_new ();
int visual_param_container_destroy (VisParamContainer *paramcontainer);

VisParamEntry *visual_param_entry_new ();
int visual_param_entry_free (VisParamEntry *param);
int visual_param_entry_set (VisParamEntry *param, char *name, VisParamType type, int flags, void *data);

int visual_param_container_add (VisParamContainer *paramcontainer, VisParamEntry *param);
int visual_param_container_add_new (VisParamContainer *paramcontainer, char *name, VisParamType type, int flags, void *data);
int visual_param_container_remove (VisParamContainer *paramcontainer, char *name);
VisParamEntry *visual_param_container_find (VisParamContainer *paramcontainer, char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PARAM_H */
