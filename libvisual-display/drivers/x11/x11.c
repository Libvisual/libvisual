/* Libvisual-display X11 backend - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id:
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lvdisplay/lv_display.h"

#include <X11/Xutil.h>

typedef struct {
	VisVideo *video;
	int img_width, img_height, img_depth;
	char *img_data;
	XImage *img;
} x11_context;

typedef struct {
	Display *dpy;
	Window win;
	GC wgc;
	XWindowAttributes win_att;

	x11_context *active_ctx;
} privdata;

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);
static int hndevents(VisPluginData *plugin, VisEventQueue *eventqueue);


static int init (privdata *priv, int *params, int params_cnt);

static int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
	int *params, int params_count);
static int get_supported_depths(VisPluginData *plugin);
static LvdDContext *context_create(VisPluginData *plugin, VisVideo *video, int*);
static void context_delete(VisPluginData *plugin, LvdDContext*);
static void context_activate(VisPluginData *plugin, LvdDContext*);
static void context_deactivate(VisPluginData *plugin, LvdDContext *ctx);
static void draw(VisPluginData *plugin);


const VisPluginInfo *get_plugin_info (int *count)
{
	static const LvdBackendDescription classdes[] = {{
		.compat_type = LVD_X11,

		.setup = (void*)setup,
		.get_supported_depths = get_supported_depths,

		.context_create = context_create,
		.context_delete = context_delete,
		.context_activate = context_activate,
		.context_deactivate = context_deactivate,

		.draw = draw,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_BACKEND,

		.plugname = "x11",
		.name = "x11",
		.author = "me",
		.version = "0.0",
		.about = "asd",
		.help = "call 911!",

		.init = plugin_init,
		.cleanup = plugin_cleanup,
		.events = hndevents,

		.plugin = (void*)&classdes[0],

	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}


int plugin_init (VisPluginData *plugin)
{
	privdata *priv;

	priv = visual_mem_new0 (privdata, 1);

	if (priv == NULL){
		return -1;
	}

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);


	return 0;
}

int plugin_cleanup (VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if (priv->wgc){
		XFreeGC(priv->dpy, priv->wgc);
		priv->wgc = NULL;
	}

	visual_mem_free(priv);

	return 0;
}

/*=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=*/

#define XGLXW (priv->glxw)
#define XDPY (priv->dpy)
#define XWIN (priv->win)

int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
		int *params, int params_count)
{
	XGCValues gc_vals;
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if ((data->dpy == None) || (data->win == None))
		return -1;

	XDPY = data->dpy;
	XWIN = data->win;

	XGetWindowAttributes(XDPY, XWIN, &priv->win_att);

	gc_vals.foreground = ~0;
	gc_vals.background = 0;
	gc_vals.plane_mask = AllPlanes;
	gc_vals.function = GXcopy;
	gc_vals.graphics_exposures = 0;

	priv->wgc = XCreateGC(XDPY, XWIN,
		GCFunction|GCForeground|GCBackground|GCPlaneMask|
		GCGraphicsExposures, &gc_vals);

	if (priv->wgc == NULL){
		visual_log(VISUAL_LOG_ERROR, "can not create window gc\n");
		return -1;
	}

	return init (priv, params, params_count);
}


int init(privdata *priv, int *params, int params_cnt)
{
	return 0;
}


int get_supported_depths(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	// XXX do we need palletted images support?

	/*if (priv->win_att.depth == 8){
		
	}*/

	return VISUAL_VIDEO_DEPTH_32BIT;
}


int hndevents(VisPluginData *plugin, VisEventQueue *eventqueue)
{
#if 0
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if (priv->active_ctx == NULL){
		return 0;
	}

	if (eventqueue->resizenew &&
		(priv->active_ctx->video->depth != VISUAL_VIDEO_DEPTH_GL)){
		glViewport(0, 0,
			eventqueue->lastresize.resize.width,
			eventqueue->lastresize.resize.height);
	}
#endif
	return 0;
}


LvdDContext *context_create(VisPluginData *plugin, VisVideo *video, int *params)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	x11_context *c;

	c = visual_mem_new0(x11_context, 1);
	if (c == NULL)
		return NULL;

	c->video = video;

	return (LvdDContext*)c;
}


void context_delete(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	x11_context *c = (x11_context*)ctx;
	assert(c);

	if (c == priv->active_ctx){
		priv->active_ctx = NULL;
	}

	if (c->img){
		// NOTE: this will also destroy c->img_data
		XDestroyImage(c->img);
		c->img = NULL;
		c->img_data = NULL;
	}

	visual_mem_free(c);
}


void context_deactivate(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	x11_context *c = (x11_context*)ctx;
	assert(c);

	priv->active_ctx = NULL;
}


void context_activate(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	x11_context *c = (x11_context*)ctx;
	assert(c);

	priv->active_ctx = c;
}


LvdDContext *context_get_active(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	return (LvdDContext *)priv->active_ctx;
}


static int check_image_dims(privdata *priv, x11_context *c)
{
	if ((c->img_depth != c->video->depth) ||
		(c->img_width != c->video->width) ||
		(c->img_height != c->video->height) ||
		(c->img_data == NULL)){

		c->img_depth = c->video->depth;
		c->img_width = c->video->width;
		c->img_height = c->video->height;

		c->img_data = malloc(c->video->height * c->video->pitch);
		if (c->img_data == NULL){
			visual_log(VISUAL_LOG_ERROR, "can not alloc fb2\n");
			return -1;
		}

		// XXX check if c->img_depth and priv->win_att.depth match
		c->img = XCreateImage(XDPY, priv->win_att.visual, priv->win_att.depth,
			ZPixmap, 0, c->img_data, c->video->width, c->video->height, 32,
			/*c->video->pitch*/ 0);


		if (c->img == NULL){ /* not necessary... if error occurs, X will raise error */
			free(c->img_data);
			c->img_data = NULL;
			visual_log(VISUAL_LOG_ERROR, "can not create image\n");
			return -1;
		}

	}

	return 0;
}


static void copy_vbuff(x11_context *c)
{
	memcpy(c->img_data, c->video->pixels, c->video->height*c->video->pitch);
}


void draw(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	x11_context *c = priv->active_ctx;

	if (check_image_dims(priv, c))
		return;

	copy_vbuff(c);

	XPutImage(XDPY, XWIN, priv->wgc, c->img, 0, 0, 0, 0,
		c->video->width, c->video->height);

	XFlush(XDPY);
}
