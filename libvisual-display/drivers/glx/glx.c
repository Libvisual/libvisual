#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv_display.h"

#include <GL/gl.h>
#include <GL/glx.h>

typedef struct {
	GLXContext glxctx;
} glx_context;

typedef struct {
	Display *dpy;
	Window win;

	int glx13;
	void *fbconf;
	void **fbconfs;
	GLXDrawable glxw;

	glx_context *active_ctx;
} privdata;

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);


static int init (privdata *priv, int *params, int params_cnt);
static int init_glx13 (privdata *priv, int *params, int params_cnt);


static int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
	int *params, int params_count, VisVideo *video);
static LvdDContext *context_create(VisPluginData *plugin);
static void context_delete(VisPluginData *plugin, LvdDContext*);
static void context_activate(VisPluginData *plugin, LvdDContext*);
static void draw(VisPluginData *plugin);


const VisPluginInfo *get_plugin_info (int *count)
{
	static const LvdBackendDescription classdes[] = {{
		.compat_type = LVD_X11,

		.setup = (void*)setup,

		.context_create = context_create,
		.context_delete = context_delete,
		.context_activate = context_activate,

		.draw = draw,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_CLASS,

		.plugname = "glx",
		.name = "glx",
		.author = "me",
		.version = "0.0",
		.about = "asd",
		.help = "call 911!",

		.init = plugin_init,
		.cleanup = plugin_cleanup,

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

	plugin->priv = priv;


	return 0;
}

int plugin_cleanup (VisPluginData *plugin)
{
	privdata *priv = plugin->priv;

	if (priv->glxw)
		glXDestroyWindow(priv->dpy, priv->glxw);

	if (priv->fbconfs)
		XFree(priv->fbconfs);

	free(priv);

	return 0;
}

/*=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=*/

#define XGLXW (priv->glxw)
#define XDPY (priv->dpy)
#define XWIN (priv->win)

int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
		int *params, int params_count, VisVideo *video)
{
	privdata *priv = plugin->priv;

	if ((data->dpy == None) || (data->win == None))
		return -1;

	XDPY = data->dpy;
	XWIN = data->win;

	visual_video_set_depth(video, VISUAL_VIDEO_DEPTH_GL);

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

	XMapWindow(XDPY, XWIN);
	XFlush(XDPY);

	return 0;
}


int init_glx13(privdata *priv, int *params, int params_cnt)
{
	int fbconfsc;

	int i = 0;
	int attr[64];


	i=0;
	attr[i++] = GLX_RED_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_GREEN_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_BLUE_SIZE;	attr[i++] = 5;
	attr[i++] = GLX_DOUBLEBUFFER; attr[i++] = True;

	attr[i++] = GLX_DRAWABLE_TYPE;
	attr[i++] = GLX_WINDOW_BIT;

//if supported... FIXME
//	attr[i++] = GLX_X_VISUAL_TYPE;
//	attr[i++] = GLX_DIRECT_COLOR;

	attr[i++] = None;


	priv->fbconfs = (void*)glXChooseFBConfig(XDPY, 0, attr, &fbconfsc);

	if ((fbconfsc <= 0) || !priv->fbconfs){
		printf("Can not get FB configs\n"
				"Please check your OpenGL driver installation\n");
		return 1;
	}

	priv->fbconf = priv->fbconfs[0];

	XGLXW = glXCreateWindow(XDPY, priv->fbconf, XWIN, NULL);

	if (!XGLXW){
		printf("Can not create GLX window\n");
		return 1;
	}


	return 0;
}

LvdDContext *context_create(VisPluginData *plugin)
{
	glx_context *c;
	privdata *priv = plugin->priv;

	c = malloc(sizeof(glx_context));
	if (c == NULL)
		return NULL;
	memset(c, 0, sizeof(glx_context));

	c->glxctx = glXCreateNewContext(XDPY, priv->fbconf,
		GLX_RGBA_TYPE, NULL, True);

	if (!c->glxctx){
		free(c);
		return NULL;
	}

	return (LvdDContext*)c;
}

void context_delete(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = plugin->priv;
	glx_context *c = (glx_context*)ctx;
	assert(c);

	glXDestroyContext(XDPY, c->glxctx);

	free(c);
}

void context_activate(VisPluginData *plugin, LvdDContext *ctx)
{
	privdata *priv = plugin->priv;
	glx_context *c = (glx_context*)ctx;
	assert(c);

	priv->active_ctx = c;

	glXMakeContextCurrent(XDPY, XGLXW, XGLXW, c->glxctx);
}

void draw(VisPluginData *plugin)
{
	privdata *priv = plugin->priv;
	glXSwapBuffers(XDPY, XGLXW);
}
