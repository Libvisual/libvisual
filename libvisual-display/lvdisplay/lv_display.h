#ifndef _LV_DISPLAY_H
#define _LV_DISPLAY_H

#include <libvisual/libvisual.h>

#define VISUAL_PLUGIN_TYPE_DISPLAY_CLASS 100000
#define VISUAL_PLUGIN_TYPE_DISPLAY_TYPE 100001

typedef enum {
	LVD_NONE = 0,
	LVD_X11,
} LvdCompatType;


#define SUPP_GLX 1

#if SUPP_GLX
#	include <X11/X.h>
#	include <X11/Xlib.h>
#	include <X11/keysym.h>

typedef struct {
	Display *dpy;
	Window win;
	Atom wm_delete;
} LvdCompatDataX11;

#endif

typedef enum {
	LVD_SET_DONE = 0,
	LVD_SET_WIDTH,
	LVD_SET_HEIGHT,
	LVD_SET_RENDERTARGET,
	LVD_SET_DEPTH,
} LvdSettings;

typedef struct _LvdDContext LvdDContext;

typedef struct _LvdBackendDescription  LvdBackendDescription;
typedef struct _LvdFrontendDescription LvdFrontendDescription;

typedef struct _LvdDriver LvdDriver;
typedef struct _Lvd Lvd;



struct _LvdDriver {
	int params_cnt;
	int *params;

	VisPluginData *pclass, *ptype;
	int prepared;

	void *compat_data;
};

struct _Lvd {
	LvdDriver         *drv;
	LvdDContext       *ctx;
};


struct _LvdBackendDescription {
	LvdCompatType     compat_type;

	int (*setup)(VisPluginData*,
		void *data, int *params, int params_count);

	LvdDContext *(*context_create)(VisPluginData*);
	void (*context_delete)(VisPluginData*, LvdDContext*);
	void (*context_activate)(VisPluginData*, LvdDContext*);

	void (*draw)(VisPluginData*);

};

struct _LvdFrontendDescription {
	int             compat_count;
	LvdCompatType	compat_type;

	int (*create)(VisPluginData*, int **params, int *params_count);
	void *(*get_compat_data)(VisPluginData *plugin);
};


LvdDriver *lvdisplay_driver_create(const char*, const char*);
void lvdisplay_driver_delete(LvdDriver*);
int lvdisplay_driver_set_opts(LvdDriver*, int *);

/* basic functions */
Lvd* lvdisplay_initialize(LvdDriver*);
void lvdisplay_finalize(Lvd*);
int lvdisplay_start(Lvd*);
int lvdisplay_stop(Lvd*);
int lvdisplay_run(Lvd*);
int lvdisplay_render_data(Lvd*, void *samples, int count);
VisEventQueue *lvdisplay_get_eventqueue(Lvd*);
int lvdisplay_poll_event(Lvd*, VisEvent*);

#endif /* _LV_DISPLAY_H */
