#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xutil.h>

#define HAVE_XF86VMODE 1

#ifdef HAVE_XF86VMODE
#	include <X11/extensions/xf86vmode.h>
#else
#	error
#endif

#include "lvdisplay/lv_display.h"

#define XDPY (data->dpy)
#define XWIN (data->win)

typedef struct {
	LvdCompatDataX11 x11data;
	VisVideo *video;
	int is_visible;
	int is_fullscreen;
	int dimensions_set;
	int storedmode_ok;
	Cursor blank_cursor;
	XF86VidModeModeInfo storedmode;
} privdata;

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);

static int create(VisPluginData*, VisVideo *video);
static void *get_compat_data(VisPluginData *plugin);
static int set_param(VisPluginData *plugin, int param, int *value, int count);
static int get_events(VisPluginData *plugin, VisEventQueue *eventqueue);


static void finit_x(privdata *priv);
static int init_x(privdata *priv);

static int fullscreen_start(privdata *priv);
static int fullscreen_stop(privdata *priv);


const VisPluginInfo *get_plugin_info (int *count)
{
	static const LvdFrontendDescription descr[] = {{
		.compat_count = 1,
		.compat_type = LVD_X11,

		.create = create,
		.get_compat_data = get_compat_data,
		.set_param = set_param,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_FRONTEND,

		.plugname = "x11_fullscreen",
		.name = "x11_fullscreen",
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

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	return 0;
}

int plugin_cleanup (VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	LvdCompatDataX11 *x11 = &priv->x11data;

	if (priv->is_fullscreen)
		fullscreen_stop(priv);

	finit_x(priv);

	free(priv);

	return 0;
}

void *get_compat_data(VisPluginData *plugin)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	return &(priv->x11data);
}

int process_events(privdata *priv, LvdCompatDataX11 *data,
	VisEventQueue *eventqueue, VisVideo *video);

int get_events(VisPluginData *plugin, VisEventQueue *eventqueue)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	LvdCompatDataX11 *data;

	// XXX are checks necessary? this func called once per frame...
	data = &priv->x11data;

	while (XEventsQueued(XDPY, QueuedAfterReading)){
		process_events(priv, data, eventqueue, priv->video);
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
			int w = ev.xconfigure.width;
			int h = ev.xconfigure.height;

			if ((w != video->width) || (h != video->height)){
				int vbuf = visual_video_have_allocated_buffer(video);

				if (vbuf)
					visual_video_free_buffer(video);

				visual_video_set_dimension(video, w, h);

				if (vbuf){
					int pitch = w*video->bpp;
					if (pitch&3)
						pitch = (pitch|3) + 1;
					visual_video_set_pitch(video, pitch);
					visual_video_allocate_buffer(video);
				}

				visual_event_queue_add_resize(eventqueue, video, w, h);
			}

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



int create(VisPluginData *plugin, VisVideo *video)
{
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	priv->video = video;

	init_x(priv);

	return 0;
}



int set_param(VisPluginData *plugin, int param, int *value, int count)
{
	int res;
	privdata *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	switch (param) {
		case LVD_SET_DEPTH:{
			visual_video_set_depth(priv->video, *value);

			if (priv->dimensions_set == 3){
				int pitch = priv->video->width * priv->video->bpp;
				if (pitch&3)
					pitch = (pitch|3) + 1;
				visual_video_set_pitch(priv->video, pitch);
			}

			return 1;
		}
		case LVD_SET_WIDTH:{
			int w = *value;
			int pitch;
			res = visual_video_set_dimension(priv->video,
				w, priv->video->height);

			if (priv->video->depth){
				pitch = w * priv->video->bpp;
				if (pitch&3)
					pitch = (pitch|3) + 1;
				visual_video_set_pitch(priv->video, pitch);
			} else {
				visual_log(VISUAL_LOG_WARNING, "Setting dimensions while depth is not set.\n");
			}
			priv->dimensions_set |= 1;

			if (priv->dimensions_set == 3)
				XResizeWindow(priv->x11data.dpy, priv->x11data.win,
					priv->video->width, priv->video->height);

			return 1;
		}
		case LVD_SET_HEIGHT:
			res = visual_video_set_dimension(priv->video,
				priv->video->width, *value);
			priv->dimensions_set |= 2;

			if (priv->dimensions_set == 3)
				XResizeWindow(priv->x11data.dpy, priv->x11data.win,
					priv->video->width, priv->video->height);

			return 1;
		case LVD_SET_VISIBLE:
			if (*value){
				fullscreen_start(priv);
			} else {
				fullscreen_stop(priv);
			}
    	    XFlush(priv->x11data.dpy);
			return 1;
	}

	return 0;
}

static int init_cursor(privdata *priv)
{
	static XColor black = {  0,  0,  0,  0 };
	static XColor white = { 0xffff, 0xffff, 0xffff, 0xffff };

	LvdCompatDataX11 *data = &priv->x11data;

	XImage *piximage;
	Pixmap pix;
	GC gc;
	XGCValues gc_vals;
	char *cdata;

	cdata = malloc(1);
	cdata[0] = 0;

	piximage = XCreateImage(XDPY, None, 1, XYBitmap, 0, cdata, 1, 1, 8, 8);
	if (piximage == NULL){
		return 1;
	}

	pix = XCreatePixmap(XDPY, XWIN, 1, 1, 1);
	if (pix == None){
		return 1;
	}

	gc_vals.foreground = ~0;
	gc_vals.background = 0;
	gc_vals.plane_mask = AllPlanes;
	gc_vals.function = GXcopy;
	gc = XCreateGC(XDPY, pix,
			GCFunction|GCForeground|GCBackground|GCPlaneMask, &gc_vals);
	if (gc == NULL){
		return 1;
	}

	XPutImage(XDPY, pix, gc, piximage, 0, 0, 0, 0, 1, 1);

	XFreeGC(XDPY, gc);
	XDestroyImage(piximage);

	priv->blank_cursor = XCreatePixmapCursor(XDPY, pix, pix,
		&black, &white, 0, 0);

	return 0;
}

int init_x(privdata *priv)
{
	LvdCompatDataX11 *data = &priv->x11data;

	int dummy1, dummy2, dummy3;
	int vmmaj, vmmin;
	int screen;
	XSetWindowAttributes xattr;

	XDPY = XOpenDisplay (NULL);
	if (!XDPY){
		printf("Can not open DISPLAY\n");
		return 1;
	}

#ifdef HAVE_XF86VMODE
	if (XQueryExtension(XDPY, "XFree86-VidModeExtension",
			&dummy1, &dummy2, &dummy3) &&
		XF86VidModeQueryExtension(XDPY, &dummy1, &dummy2) &&
		XF86VidModeQueryVersion(XDPY, &vmmaj, &vmmin)){

		visual_log(VISUAL_LOG_DEBUG, "xvidmode version: %d.%d\n", vmmaj, vmmin);
	} else {
		XCloseDisplay(XDPY);
		XDPY = None;
		return 2;
	}
#endif

	screen = DefaultScreen (XDPY);

	memset(&xattr, 0, sizeof (XSetWindowAttributes));
	xattr.override_redirect = True;
	xattr.background_pixel = BlackPixel (XDPY, DefaultScreen (XDPY));
	xattr.border_pixel = 0;

	XWIN = XCreateWindow (XDPY, RootWindow (XDPY, screen), 0, 0,
		320, 240,
		0, 0, InputOutput, (Visual*)CopyFromParent,
		CWOverrideRedirect | CWBackPixel | CWBorderPixel, &xattr);

	if (!XWIN){
		visual_log(VISUAL_LOG_ERROR, "Can not create window\n");
		return 4;
	}

	if (init_cursor(priv)){
		visual_log(VISUAL_LOG_WARNING, "Can not create blank cursor\n");
		return 5;
	}

	return 0;
}




void finit_x(privdata *priv)
{
	LvdCompatDataX11 *data = &priv->x11data;

	if (XDPY){
		if (priv->blank_cursor){
			XFreeCursor(XDPY, priv->blank_cursor);
		}
		if (XWIN)
			XDestroyWindow(XDPY, XWIN);
		XCloseDisplay(XDPY);

		XDPY = 0;
		XWIN = 0;
		priv->blank_cursor = None;
	}
}


static int switch_video_mode(privdata *priv)
{
	LvdCompatDataX11 *data = &priv->x11data;
	int res;
	int evbase, errbase;
	int vmaj, vmin;

	int prefer_interlaced = 0;
	int prefer_dblscan = 1; // XXX make configurable. Assumes lowres mode.
							// for higher res you need a _really_ good monitor
							// since hfreq demand doubles!
							// Makes no sense for hires modes.

	int cnt, i;
	XF86VidModeModeInfo **modes;

	priv->storedmode_ok = XF86VidModeGetModeLine(XDPY,
		DefaultScreen(XDPY),
		&priv->storedmode.dotclock,
			(XF86VidModeModeLine*)(((char*)&priv->storedmode) +
			sizeof(priv->storedmode.dotclock)));

	if (!priv->storedmode_ok)
		return 1;


	if (XF86VidModeGetAllModeLines(XDPY, DefaultScreen(XDPY),
		&cnt, &modes) && (cnt > 0)){
		int mw, mh;
		const int ok_modes_max = 32;
		int ok_modes[32];
		int ok_modes_cnt = 0;
		int bestmode;
		double bestvfreq;

		mw = modes[0]->hdisplay;
		mh = modes[0]->vdisplay;
		ok_modes[0] = 0;
		ok_modes_cnt = 1;

		/* find modes that fit the best */
		for (i=1;i<cnt;i++){
			if ((modes[i]->hdisplay >= priv->video->width) &&
				(modes[i]->vdisplay >= priv->video->height) &&
				(modes[i]->hdisplay <= mw) &&
				(modes[i]->vdisplay <= mh)){

				if ((modes[i]->hdisplay < mw) ||
					(modes[i]->vdisplay < mh)){

					ok_modes_cnt = 0;
					mw = modes[i]->hdisplay;
					mh = modes[i]->vdisplay;
				}

				if (ok_modes_cnt < ok_modes_max){
					ok_modes[ok_modes_cnt++] = i;
				}
			}
		}

		/* choose preferred mode with highest vfreq */

		if (mw > 640){
			prefer_dblscan = 0; // XXX make configurable.
		}

repeat_modesel:

		bestvfreq = 0.0;
		bestmode = -1;
		for (i=0;i<ok_modes_cnt;i++){
			int dblscan = (modes[ok_modes[i]]->flags & 0x20);
			int interlace = (modes[ok_modes[i]]->flags & 0x10);
			double vfreq;

			if (prefer_dblscan) {
				if (!dblscan)
					continue;
			} else
			if (prefer_interlaced && !interlace)
				continue;

			if (dblscan)
				vfreq = 500.0;  /* Doublescan */
			else
			if (interlace)
				vfreq = 2000.0; /* Interlaced */
			else    
				vfreq = 1000.0;

			vfreq *= (modes[ok_modes[i]]->dotclock);
			vfreq /= (modes[ok_modes[i]]->htotal*modes[ok_modes[i]]->vtotal);

			if ((bestmode < 0) || (bestvfreq < vfreq)){
				bestvfreq = vfreq;
				bestmode = i;
			}

		}

		if (prefer_dblscan && (bestmode == -1)){
			prefer_dblscan = 0;
			goto repeat_modesel;
		}

		if (prefer_interlaced && (bestmode == -1)){
			prefer_interlaced = 0;
			goto repeat_modesel;
		}

		visual_log(VISUAL_LOG_DEBUG, "X: %d %d %.1f\n",modes[ok_modes[bestmode]]->hdisplay,
			modes[ok_modes[bestmode]]->vdisplay, bestvfreq);

		res = XF86VidModeSwitchToMode(XDPY, DefaultScreen(XDPY), modes[ok_modes[bestmode]]);

		XFree(modes);

		if (!res)
			return 1;

		XWarpPointer(XDPY, None, XWIN, 0,0, 0,0, 0,0);
		XFlush(XDPY);
		XWarpPointer(XDPY, None, XWIN, 0,0, 0,0,
			priv->video->width/2, priv->video->height/2);

	}

	return 0;
}


int fullscreen_start(privdata *priv)
{
	LvdCompatDataX11 *data = &priv->x11data;
	int res;

	/* map window */

	XMapRaised(XDPY, XWIN);
	XSync(XDPY, False);

	/* Tell KDE to keep the fullscreen window on top */
	{
		XEvent ev;
		long mask;

		memset(&ev, 0, sizeof(ev));
		ev.xclient.type = ClientMessage;
		ev.xclient.window = DefaultRootWindow(XDPY);
		ev.xclient.message_type = XInternAtom(XDPY,
							"KWM_KEEP_ON_TOP", False);
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = XWIN;
		ev.xclient.data.l[1] = CurrentTime;
		mask = SubstructureRedirectMask;
		XSendEvent(XDPY, DefaultRootWindow(XDPY), False, mask, &ev);
	}

	/* grab input */
	while (1){
		res = XGrabPointer(XDPY, XWIN, True, 0, 
			GrabModeAsync, GrabModeAsync,
			XWIN, None, CurrentTime);
		if (res == GrabSuccess)
			break;
		/* TODO usleep() here */
		// XXX limit tries
	}

	XGrabKeyboard(XDPY, XWIN, True,
			GrabModeAsync, GrabModeAsync,
			CurrentTime);

	XSync(XDPY, False);


	if (switch_video_mode(priv)){
		fullscreen_stop(priv);
		return 1;
	}

	{
		XSetWindowAttributes xattr;
		xattr.cursor = priv->blank_cursor;
		XChangeWindowAttributes(XDPY, XWIN, CWCursor, &xattr);
		XSync(XDPY, False);
	}

	priv->is_fullscreen = 1;

	return 0;
}

int fullscreen_stop(privdata *priv)
{
	LvdCompatDataX11 *data = &priv->x11data;


	if (priv->storedmode_ok){
		XF86VidModeSwitchToMode(XDPY, DefaultScreen(XDPY), &priv->storedmode);
		if (priv->storedmode.privsize > 0){
			XFree(priv->storedmode.private);
		}
		memset(&priv->storedmode, 0, sizeof(XF86VidModeModeInfo));
		priv->storedmode_ok = 0;
	}


	XUngrabPointer(XDPY, CurrentTime);
	XUngrabKeyboard(XDPY, CurrentTime);
	XSync(XDPY, False);

	XUnmapWindow(XDPY, XWIN);
	XSync(XDPY, True);

	priv->is_fullscreen = 0;

	return 0;
}
