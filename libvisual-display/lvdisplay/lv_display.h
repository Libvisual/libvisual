/* Libvisual-display - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_display.h,v 1.12 2005-01-28 18:35:55 vitalyvb Exp $
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

#ifndef _LV_DISPLAY_H
#define _LV_DISPLAY_H

#include <libvisual/libvisual.h>

#define VISUAL_PLUGIN_TYPE_DISPLAY_BACKEND	"Libvisual:display:backend"
#define VISUAL_PLUGIN_TYPE_DISPLAY_FRONTEND	"Libvisual:display:frontend"

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
	LVD_SET_VISIBLE,
	
	LVD_PARAM_LAST,
} LvdSettings;

typedef struct _LvdDContext LvdDContext;

typedef struct _LvdBackendDescription  LvdBackendDescription;
typedef struct _LvdFrontendDescription LvdFrontendDescription;

typedef struct _LvdDriver LvdDriver;
typedef struct _Lvd Lvd;



struct _LvdDriver {
	VisObject	 object;	/**< The VisObject data. */

	VisPluginData *beplug, *feplug;
	LvdBackendDescription *be;
	LvdFrontendDescription *fe;

	VisVideo *video;
	int realized;

	void *compat_data;
};

struct _Lvd {
	VisObject	 object;	/**< The VisObject data. */

	LvdDriver *drv;
	VisPluginData *beplug, *feplug; /* copies of drv's */
	LvdBackendDescription *be;
	LvdFrontendDescription *fe;

	LvdDContext *ctx;

	VisBin *bin;
};


struct _LvdBackendDescription {
	VisObject object;
	LvdCompatType     compat_type;

	int (*setup)(VisPluginData*, void *data);

	int (*get_supported_depths)(VisPluginData *plugin);

	LvdDContext *(*context_create)(VisPluginData*, VisVideo*, int*);
	void (*context_delete)(VisPluginData*, LvdDContext*);
	void (*context_activate)(VisPluginData*, LvdDContext*);
	void (*context_deactivate)(VisPluginData*, LvdDContext*);
	LvdDContext *(*context_get_active)(VisPluginData*);

	void (*draw)(VisPluginData*);

};

struct _LvdFrontendDescription {
	VisObject object;
	int             compat_count;
	LvdCompatType	compat_type;

	int (*create)(VisPluginData*, VisVideo*);
	void *(*get_compat_data)(VisPluginData *plugin);
	int (*set_param)(VisPluginData *plugin, int param, int *value, int count);
};


LvdDriver *lvdisplay_driver_create(const char*, const char*);
int lvdisplay_driver_set_opts(LvdDriver *drv, int *params, int count);
int lvdisplay_driver_realize(LvdDriver *);
int lvdisplay_driver_set_visible(LvdDriver *, int);

/* basic functions */
Lvd* lvdisplay_initialize();
int lvdisplay_set_driver(Lvd*, LvdDriver*);
int lvdisplay_realize(Lvd*);
int lvdisplay_run(Lvd*);
int lvdisplay_render_data(Lvd*, void *samples, int count);
int lvdisplay_poll_event(Lvd*, VisEvent*);

/* obects returnet by these functions must be unref'ed when done */
VisEventQueue *lvdisplay_get_eventqueue(Lvd*);
VisBin *lvdisplay_visual_get_bin(Lvd *v);
VisVideo *lvdisplay_visual_get_video(Lvd *v);



/* plugin interface */

#define VISUAL_PLUGIN_ENVIRON_TYPE_LVD	"Libvisual:display:environ"

typedef struct _LvdPluginEnvironData LvdPluginEnvironData;

struct _LvdPluginEnvironData {
	VisObject	object;	/**< The VisObject data. */
	Lvd *lvd;
};

LvdDContext *lvdisplay_context_create(Lvd*);
LvdDContext *lvdisplay_context_create_special(Lvd*, int*);
int lvdisplay_context_delete(Lvd*, LvdDContext*);

int lvdisplay_context_push_activate(Lvd*, LvdDContext*);
int lvdisplay_context_pop(Lvd*);




#endif /* _LV_DISPLAY_H */
