#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xutil.h>

#include "lv_display.h"

#define XDPY (data->dpy)
#define XWIN (data->win)

typedef struct {
	LvdCompatDataX11 x11data;
	VisVideo *video;
	int is_visible;
} privdata;

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);

static int create(VisPluginData*, int **params, int *params_count, VisVideo *video);
static void *get_compat_data(VisPluginData *plugin);
static int get_events(VisPluginData *plugin, VisEventQueue *eventqueue);


static void finit_x(LvdCompatDataX11 *data);
static int init_x(LvdCompatDataX11 *data, int width, int height);
static int setup_window(LvdCompatDataX11 *data, int width, int height);




const VisPluginInfo *get_plugin_info (int *count)
{
	static const LvdFrontendDescription descr[] = {{
		.compat_count = 1,
		.compat_type = LVD_X11,

		.create = create,
		.get_compat_data = get_compat_data,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_TYPE,

		.plugname = "glx_new",
		.name = "glx_new",
		.author = "me",
		.version = "0.0",
		.about = "asd",
		.help = "call 911!",

		.init = plugin_init,
		.cleanup = plugin_cleanup,
		.events = get_events,

		.plugin = (void*)&descr[0],

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
	LvdCompatDataX11 *x11 = &priv->x11data;

	finit_x(x11);

	free(priv);

	return 0;
}

void *get_compat_data(VisPluginData *plugin)
{
    return &((privdata*)plugin->priv)->x11data;
}

int process_events(privdata *priv, LvdCompatDataX11 *data,
	VisEventQueue *eventqueue, VisVideo *video);

int get_events(VisPluginData *plugin, VisEventQueue *eventqueue)
{
	LvdCompatDataX11 *data;

	// XXX are checks necessary? this func called once per frame...
	data = &((privdata*)plugin->priv)->x11data;

	while (XEventsQueued(XDPY, QueuedAfterReading)){
		process_events(plugin->priv, data, eventqueue,
			((privdata*)plugin->priv)->video);
	}

	return 0;
}


int process_events(privdata *priv, LvdCompatDataX11 *data,
	VisEventQueue *eventqueue, VisVideo *video)
{
	XEvent ev;

	XNextEvent(XDPY, &ev);
	switch (ev.type){
		case KeyPress:{
			// XXX autorepeat is on. will work kinda wrong.
			KeySym ks;
			ks = XKeycodeToKeysym(XDPY, ev.xkey.keycode, 0);
			// XXX keys must be transformed
			if (ks < VKEY_LAST)
				visual_event_queue_add_keyboard (eventqueue, ks, ev.xkey.state, VISUAL_KEY_DOWN);
			break;
		}
		case KeyRelease:{
			KeySym ks;
			ks = XKeycodeToKeysym(XDPY, ev.xkey.keycode, 0);
			// XXX keys must be transformed
			if (ks < VKEY_LAST)
				visual_event_queue_add_keyboard (eventqueue, ks, ev.xkey.state, VISUAL_KEY_UP);
			break;
		}

		case ButtonRelease:
		case ButtonPress:
			visual_event_queue_add_mousebutton (eventqueue, ev.xbutton.button,
				(ev.type == ButtonPress) ? VISUAL_MOUSE_DOWN:VISUAL_MOUSE_UP,
				ev.xbutton.x, ev.xbutton.y);
			break;


		case MotionNotify:
			visual_event_queue_add_mousemotion (eventqueue, ev.xmotion.x, ev.xmotion.y);
			break;

		case MapNotify:
			if (priv->is_visible != 1){
				priv->is_visible = 1;
				visual_event_queue_add_visibility(eventqueue, 1);
			}
			break;
		case UnmapNotify:
			if (priv->is_visible != 0){
				priv->is_visible = 0;
				visual_event_queue_add_visibility(eventqueue, 0);
			}
			break;
		case VisibilityNotify:
			if (priv->is_visible != (ev.xvisibility.state!=2)){
				priv->is_visible = ev.xvisibility.state!=2;
				visual_event_queue_add_visibility(eventqueue, priv->is_visible );
			}
			break;

		case ConfigureNotify:{
			int vbuf = visual_video_have_allocated_buffer(video);

			if (vbuf)
				visual_video_free_buffer(video);

			visual_video_set_dimension(video, ev.xconfigure.width, ev.xconfigure.height);

			if (vbuf)
				visual_video_allocate_buffer(video);

			visual_event_queue_add_resize(eventqueue, video , ev.xconfigure.width, ev.xconfigure.height);
			break;
		}
		/* message sent by a window manager. i think ;) */
		case ClientMessage:
			if ((ev.xclient.format == 32) &&
				(ev.xclient.data.l[0] == data->wm_delete)){
				visual_event_queue_add_quit(eventqueue, 0);
			}
			break;
	}


	return 0;
}



int create(VisPluginData *plugin,
		int **params, int *params_count, VisVideo *video)
{
	privdata *priv = plugin->priv;
	int w = 320;
	int h = 240;

	/* FIXME will break on invalid *params */
	if (params && *params){
		int p = 0;
		while (p < *params_count){
			switch (*params[p]){
				case LVD_SET_WIDTH:
					w = *params[p+1];
					break;
				case LVD_SET_HEIGHT:
					h = *params[p+1];
					break;
				default:
					break;
			}
			p+=2;
		}
	
	}

	priv->video = video;
	visual_video_set_dimension(video, w, h);

	init_x(&priv->x11data, w, h);

	return 0;
}


int init_x(LvdCompatDataX11 *data, int width, int height)
{
	int screen;
	XSetWindowAttributes xattr;

	XDPY = XOpenDisplay (NULL);
	if (!XDPY){
		printf("Can not open DISPLAY\n");
		return 1;
	}

	screen = DefaultScreen (XDPY);

	memset(&xattr, 0, sizeof (XSetWindowAttributes));
	xattr.background_pixel = BlackPixel (XDPY, DefaultScreen (XDPY));
	xattr.border_pixel = 0;

	XWIN = XCreateWindow (XDPY, RootWindow (XDPY, screen), 0, 0,
		width, height,
		0, 0, InputOutput, (Visual*)CopyFromParent,
		CWBackPixel | CWBorderPixel, &xattr);

	if (!XWIN){
		printf("Can not create window\n");
		return 4;
	}

	return setup_window(data, width, height);
}

int setup_window(LvdCompatDataX11 *data, int width, int height)
{
	/* based on SDL X11 video */
	XWMHints *hints;

	hints = XAllocWMHints();
	if(hints) {
		hints->input = True;
		hints->flags = InputHint;
		XSetWMHints(XDPY, XWIN, hints);
		XFree(hints);
	}

	XSelectInput(XDPY, XWIN,
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		VisibilityChangeMask | StructureNotifyMask |
		ConfigureNotify
		/* XXX and others :) */);

	/* Set the class hints so we can get an icon (AfterStep) */
	{
		XClassHint *classhints;
		classhints = XAllocClassHint();
		if(classhints != NULL) {
			char *classname = "xxxxx"; /* XXX need to set this from app */
			classhints->res_name = classname;
			classhints->res_class = classname;
			XSetClassHint(XDPY, XWIN, classhints);
			XFree(classhints);
		}
	}

	/* Allow the window to be deleted by the window manager */
	data->wm_delete = XInternAtom(XDPY, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(XDPY, XWIN, &data->wm_delete, 1);

	return 0;
}



void finit_x(LvdCompatDataX11 *data)
{
	if (XDPY){
		if (XWIN)
			XDestroyWindow(XDPY, XWIN);
		XCloseDisplay(XDPY);
		XDPY = 0;
		XWIN = 0;
	}
}

