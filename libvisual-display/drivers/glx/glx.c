/* Libvisual-display GLX backend - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id: glx.c,v 1.15 2005-02-15 15:43:46 vitalyvb Exp $
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

#include "lv_display.h"

#include <GL/gl.h>
#include <GL/glx.h>

enum {
	CONTEXT_TYPE_WIN = 0,
	CONTEXT_TYPE_PBUFFER,
};

typedef struct {
	int type;
	GLXDrawable drawable;
	GLXContext glxctx;
	VisVideo *video;
} glx_context;

typedef struct {
	Display *dpy;
	Window win;

	int glx13;
	void *fbconf;
	void **fbconfs;
	void *fbconf_pb;
	void **fbconfs_pb;
	GLXDrawable glxw;

	glx_context *active_ctx;
} privdata;

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);
static int hndevents(VisPluginData *plugin, VisEventQueue *eventqueue);


static int init (privdata *priv, int *params, int params_cnt);
static int init_glx13 (privdata *priv, int *params, int params_cnt);

static int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
	int *params, int params_count);
static int get_supported_depths(VisPluginData *plugin);
static LvdDContext *context_create(VisPluginData *plugin, VisVideo *video, int *params);
static void context_delete(VisPluginData *plugin, LvdDContext*);
static void context_activate(VisPluginData *plugin, LvdDContext*);
static void context_deactivate(VisPluginData *plugin, LvdDContext *ctx);
static LvdDContext *context_get_active(VisPluginData *plugin);
static void draw(VisPluginData *plugin);
static VisVideo *get_active_ctx_video(VisPluginData *plugin);


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
		.context_get_active = context_get_active,
		.get_active_ctx_video = get_active_ctx_video,

		.draw = draw,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_BACKEND,

		.plugname = "glx",
		.name = "glx",
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

	glXMakeContextCurrent(priv->dpy, None, None, NULL);

	if (priv->glxw)
		glXDestroyWindow(priv->dpy, priv->glxw);

	if (priv->fbconfs)
		XFree(priv->fbconfs);

	if (priv->fbconfs_pb)
		XFree(priv->fbconfs_pb);

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
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if ((data->dpy == None) || (data->win == None))
		return -1;

	XDPY = data->dpy;
	XWIN = data->win;

	return init (priv, params, params_count);
}

int init(privdata *priv, int *params, int params_cnt)
{
	int dummy1, dummy2, dummy3;
	int vmaj, vmin;
	int glx_errorBase, glx_eventBase;
	int res;

	if (!XQueryExtension(XDPY, "GLX", &dummy1, &dummy2, &dummy3)){
		printf("GLX extension is not available\n");
		return 1;
	}

	if (!glXQueryExtension(XDPY, &glx_errorBase, &glx_eventBase)){
		printf("GLX is not available: glXQueryExtension failed\n");
		return 2;
	}

	if (!glXQueryVersion(XDPY, &vmaj, &vmin)){
		printf("GLX is not available: glXQueryVersion failed\n");
		return 2;
	}

	if ((vmaj < 1) || ((vmaj == 1) && (vmin < 3))){
		if ((vmaj < 1) || ((vmaj == 1) && (vmin < 1))){
			printf("GLX version is too old. Sorry!\n");
			return 2;
		} else
			printf("Using GLX verson 1.2\n");
		priv->glx13 = 0;
	} else
		priv->glx13 = 1;


	if (priv->glx13){
		res = init_glx13(priv, params, params_cnt);
	} else {
		return 1;
	}

	if (res){
		/* cleanup? */
		return res;
	}

	return 0;
}


int get_supported_depths(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	return
		VISUAL_VIDEO_DEPTH_16BIT |
		VISUAL_VIDEO_DEPTH_24BIT |
		VISUAL_VIDEO_DEPTH_32BIT |
		VISUAL_VIDEO_DEPTH_GL;
}

int init_glx13(privdata *priv, int *params, int params_cnt)
{
	int fbconfsc;

	int i = 0;
	int attr[64];


	i=0;
	// XXX TODO get these defaults from frontend and/or environ
	attr[i++] = GLX_RED_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_GREEN_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_BLUE_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_DOUBLEBUFFER; attr[i++] = True;
	attr[i++] = GLX_DRAWABLE_TYPE; attr[i++] = GLX_WINDOW_BIT;
//if supported... FIXME
//	attr[i++] = GLX_X_VISUAL_TYPE; attr[i++] = GLX_DIRECT_COLOR;
	attr[i++] = None;


	priv->fbconfs = (void*)glXChooseFBConfig(XDPY, 0, attr, &fbconfsc);

	if ((fbconfsc <= 0) || !priv->fbconfs){
		printf("Can not get FB configs\n"
				"Please check your OpenGL driver installation\n");
		priv->fbconfs = NULL;
		return 1;
	}

	priv->fbconf = priv->fbconfs[0];

	XGLXW = glXCreateWindow(XDPY, priv->fbconf, XWIN, NULL);

	if (!XGLXW){
		printf("Can not create GLX window\n");
		return 1;
	}

	// prefetch configs for pbuffer

	i=0;
	// XXX TODO get these defaults from frontend and/or environ
	attr[i++] = GLX_RED_SIZE;	attr[i++] = 8;
	attr[i++] = GLX_GREEN_SIZE;	attr[i++] = 8;
	attr[i++] = GLX_BLUE_SIZE;	attr[i++] = 8;
	attr[i++] = GLX_DOUBLEBUFFER; attr[i++] = False;
	attr[i++] = GLX_DRAWABLE_TYPE;	attr[i++] = GLX_PBUFFER_BIT;
	attr[i++] = None;

	priv->fbconfs_pb = (void*)glXChooseFBConfig(XDPY, 0, attr, &fbconfsc);

	if ((fbconfsc <= 0) || !priv->fbconfs_pb){
		printf("Can not get FB configs for pbuffer\n");
		if (priv->fbconfs_pb)
			XFree(priv->fbconfs_pb);
		priv->fbconfs_pb = NULL;
	} else
		priv->fbconf_pb = priv->fbconfs_pb[0];

	return 0;
}

int hndevents(VisPluginData *plugin, VisEventQueue *eventqueue)
{
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

	return 0;
}


static GLXContext parentc = None;

LvdDContext *context_create(VisPluginData *plugin, VisVideo *video, int *params)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	glx_context *c;
	int p;

	int w = 128, h = 128;
	int onscrn = 1;
	int done = 0;

	if (params != NULL){
		p=0;
		while (!done){
			switch (params[p++]){
			case LVD_SET_DONE:
				done = 1;
				break;
			case LVD_SET_WIDTH:
				w = params[p++];
				break;
			case LVD_SET_HEIGHT:
				h = params[p++];
				break;
			case LVD_SET_RENDERTARGET:
				onscrn = (params[p++] == LVD_SET_ONSCREEN);
				break;
			default:
				// XXX FIXME warn
				done = 1;
				break;
			}
		}
	}

	// XXX check max texture sizes

	/* check if width and height are powers of 2 */
	if (!onscrn & ((w&(w-1)) || (h&(h-1)))){
		
		return NULL;
	}


	c = visual_mem_new0(glx_context, 1);
	if (c == NULL)
		return NULL;

	c->video = visual_video_new();
	visual_video_clone(c->video, video);

	if (onscrn) {
		c->type = CONTEXT_TYPE_WIN;
		c->drawable = XGLXW;
		c->glxctx = glXCreateNewContext(XDPY, priv->fbconf,
			GLX_RGBA_TYPE, NULL, True);

		if (!c->glxctx){
			visual_mem_free(c);
			return NULL;
		}
		parentc = c->glxctx;
	} else {
		int attr[100];
		int i;
		GLXDrawable drw;

		c->type = CONTEXT_TYPE_PBUFFER;

		i=0;
		attr[i++] = GLX_PBUFFER_WIDTH; attr[i++] = w;
		attr[i++] = GLX_PBUFFER_HEIGHT; attr[i++] = h;
		attr[i++] = GLX_PRESERVED_CONTENTS; attr[i++] = True;
		attr[i++] = None;

		drw = glXCreatePbuffer(XDPY, priv->fbconf_pb, attr);
		if (!drw){
			printf("Can not create GLX pbuffer\n");
			return NULL;
		}

		c->drawable = drw;
		c->glxctx = glXCreateNewContext(XDPY, priv->fbconf_pb,
			GLX_RGBA_TYPE, parentc, True);

		glXQueryDrawable(XDPY, drw, GLX_WIDTH, &w);
		glXQueryDrawable(XDPY, drw, GLX_HEIGHT, &h);
		printf("created GLX pbuffer %d x %d\n", w, h);
	}

	if (video->depth != VISUAL_VIDEO_DEPTH_GL)
		visual_video_allocate_buffer(c->video);


	return (LvdDContext*)c;
}

void context_delete(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	glx_context *c = (glx_context*)ctx;
	assert(c);

	if (c == priv->active_ctx){
		priv->active_ctx = NULL;
	}

	glXMakeContextCurrent(XDPY, None, None, NULL);

	glXDestroyContext(XDPY, c->glxctx);
	c->glxctx = NULL;

	if (c->type == CONTEXT_TYPE_PBUFFER)
		glXDestroyPbuffer(XDPY, c->drawable);
	c->drawable = 0;

	if (c->video){
		if (c->video->pixels)
			visual_video_free_buffer(c->video);

		visual_object_unref(VISUAL_OBJECT(c->video));
		c->video = NULL;
	}

	visual_mem_free(c);
}

VisVideo *get_active_ctx_video(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if (priv->active_ctx){
		return priv->active_ctx->video;
	}
	return NULL;
}

void context_deactivate(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	glx_context *c = (glx_context*)ctx;
	assert(c);

	priv->active_ctx = NULL;

	glXMakeContextCurrent(XDPY, None, None, NULL);
}


void context_activate(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	glx_context *c = (glx_context*)ctx;
	assert(c);

	priv->active_ctx = c;
	glXMakeContextCurrent(XDPY, c->drawable, c->drawable, c->glxctx);
}

LvdDContext *context_get_active(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	return (LvdDContext *)priv->active_ctx;
}

void draw(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	if ((priv->active_ctx->video->depth != VISUAL_VIDEO_DEPTH_GL) &&
		(priv->active_ctx->video->pixels)){

		int format, type;

		switch (priv->active_ctx->video->depth){
			case VISUAL_VIDEO_DEPTH_16BIT:
				format = GL_RGB;
				type = GL_UNSIGNED_SHORT_5_6_5;
				break;
			case VISUAL_VIDEO_DEPTH_24BIT:
				format = GL_RGB;
				type = GL_UNSIGNED_BYTE;
				break;
			case VISUAL_VIDEO_DEPTH_32BIT:
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				break;
			default:
				return;
		}

		glMatrixMode(GL_PROJECTION);
		glPixelZoom(1.0f, -1.0f);
		glRasterPos2f(-1.0f, 1.0f);
		glDrawPixels(priv->active_ctx->video->width, priv->active_ctx->video->height,
			format, type, priv->active_ctx->video->pixels);
	}

	glXSwapBuffers(XDPY, XGLXW);
}
