#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include <gtk/gtk.h>

#include <xmms/plugin.h>
#include <xmms/xmmsctrl.h>
#include <xmms/configfile.h>

#include <libvisual/libvisual.h>

#include <lv_xmms.h>

/* SDL variables */
SDL_Surface *screen = NULL;
SDL_Color sdlpal[256];
SDL_Thread *render_thread;
SDL_mutex *pcm_mutex;

/* Libvisual and visualisation variables */
VisVideo *video;
VisPalette *pal;

char *cur_lv_plugin = NULL;

VisBin *bin = NULL;

VisSongInfo *songinfo;

int lv_width = 320;
int lv_height = 200;

int gl_plug = 0;
int depth;

int plug_index = 0;

short xmmspcm[2][512];

/* Thread state variables */
int visual_running = 0;
int visual_stopped = 1;

/* About gui variables */
GtkWidget *about_window = NULL;

static void lv_xmms_init (void);
static void lv_xmms_cleanup (void);
static void lv_xmms_about (void);
static void lv_xmms_configure (void);
static void lv_xmms_disable (VisPlugin *);
static void lv_xmms_playback_start (void);
static void lv_xmms_playback_stop (void);
static void lv_xmms_render_pcm (gint16 data[2][512]);

static void gui_about_closed (GtkWidget *w, GdkEvent *e, gpointer data);
static void gui_about_destroy (GtkWidget *w, gpointer data);

static int sdl_init (void);
static int sdl_quit (void);
static void sdl_set_pal (void);
static void sdl_draw (SDL_Surface *screen);
static int sdl_create (int width, int height);
static int sdl_event_handle (void);

static int visual_upload_callback (VisInput *input, VisAudio *audio, void *private);
static int visual_resize (int width, int height);
static int visual_initialize (int width, int height);
static void *visual_render (void);

VisPlugin *get_vplugin_info (void);
	
VisPlugin lv_xmms_vp =
{
	NULL,
	NULL,
	0,
	"Libvisual xmms plugin " VERSION, /* description */
	2,
	0,
	lv_xmms_init,				/* init */
	lv_xmms_cleanup,			/* cleanup */
	lv_xmms_about,				/* about */
	lv_xmms_configure,			/* configure */
	lv_xmms_disable,			/* disable plugin */
	lv_xmms_playback_start,			/* playback start */
	lv_xmms_playback_stop,			/* playback stop */
	lv_xmms_render_pcm,			/* render pcm */
	NULL,					/* render freq */
};

VisPlugin *get_vplugin_info ()
{
	return &lv_xmms_vp;
}

static char *lv_xmms_get_songname ()
{
	return xmms_remote_get_playlist_title (lv_xmms_vp.xmms_session,
			xmms_remote_get_playlist_pos (lv_xmms_vp.xmms_session));
}

static int lv_xmms_prefs_load ()
{
	char *name;
	char *vstr;
	ConfigFile *f;
	
	name = g_strdup_printf ("%s%s", g_get_home_dir (), "/.xmms/config");
	if ((f = xmms_cfg_open_file (name)) == NULL)
		return -1;

	if (xmms_cfg_read_string (f, "libvisual_xmms", "version", &vstr)) {
		if (strcmp (vstr, VERSION) != 0) {
			xmms_cfg_free (f);

			return -1;
		}
	}

	xmms_cfg_read_string (f, "libvisual_xmms", "last_plugin", &cur_lv_plugin);
	xmms_cfg_read_int (f, "libvisual_xmms", "width", &lv_width);
	xmms_cfg_read_int (f, "libvisual_xmms", "height", &lv_height);
	
	xmms_cfg_free (f);

	g_free (name);

	return 0;
}

static int lv_xmms_prefs_save ()
{
	ConfigFile *f;

	if((f = xmms_cfg_open_default_file ()) == NULL)
		f = xmms_cfg_new ();

	xmms_cfg_write_string (f, "libvisual_xmms", "version", VERSION);
	xmms_cfg_write_string (f, "libvisual_xmms", "last_plugin", cur_lv_plugin);
	xmms_cfg_write_int (f, "libvisual_xmms", "width", lv_width);
	xmms_cfg_write_int (f, "libvisual_xmms", "height", lv_height);

	xmms_cfg_write_default_file (f);
	xmms_cfg_free (f);
}

static void lv_xmms_init ()
{
	visual_log (VISUAL_LOG_DEBUG, "calling sdl_init()");
	sdl_init ();
	visual_log (VISUAL_LOG_DEBUG, "calling SDL_CreateMutex()");
	pcm_mutex = SDL_CreateMutex ();
	visual_init (NULL, NULL);

	lv_xmms_prefs_load ();
	if (cur_lv_plugin == NULL)
		cur_lv_plugin = visual_actor_get_next_by_name (cur_lv_plugin);

	if (visual_actor_valid_by_name (cur_lv_plugin) != TRUE)
		cur_lv_plugin = visual_actor_get_next_by_name (NULL);
 
	visual_initialize (lv_width, lv_height);

	visual_log (VISUAL_LOG_DEBUG, "calling SDL_CreateThread()");
	render_thread = SDL_CreateThread ((void *) visual_render, NULL);
}

static void lv_xmms_cleanup ()
{
	int i = 0;

	visual_log (VISUAL_LOG_DEBUG, "entering...");
	visual_running = 0;
	usleep (100000);
	while (visual_stopped != 1) {
		usleep (100000);
		i++;
		if (i > 10)
			break;
	}
	SDL_KillThread (render_thread);
	render_thread = NULL;
	visual_stopped = 1;
	
	visual_log (VISUAL_LOG_DEBUG, "calling SDL_DestroyMutex()");
	SDL_DestroyMutex (pcm_mutex);
	pcm_mutex = NULL;

	visual_log (VISUAL_LOG_DEBUG, "calling lv_xmms_prefs_save()");
	lv_xmms_prefs_save ();

	visual_log (VISUAL_LOG_DEBUG, "destroying VisBin...");
	visual_bin_destroy (bin);
	bin = NULL;

	visual_log (VISUAL_LOG_DEBUG, "calling visual_quit()");
	visual_quit ();

	visual_log (VISUAL_LOG_DEBUG, "calling sdl_quit()");
	sdl_quit ();

	visual_log (VISUAL_LOG_DEBUG, "leaving...");
}

static void lv_xmms_about ()
{
	GtkWidget *vbox;
	GtkWidget *buttonbox;
	GtkWidget *close;
	GtkWidget *label;

	if (about_window != NULL)
		return;

	about_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (about_window), NULL);
	gtk_window_set_policy (GTK_WINDOW (about_window), FALSE, FALSE, FALSE);
	
	vbox = gtk_vbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (about_window), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 8);
	gtk_widget_show (vbox);

	label=gtk_label_new("\n\
Libvisual xmms plugin\nCopyright (C) 2004, Dennis Smit <ds@nerds-incorporated.org>\n\
The libvisual xmms plugin, more information about libvisual can be found at\n\
http://libvisual.sf.net\n\n");

	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 8);
	gtk_widget_show (label);

	buttonbox = gtk_hbutton_box_new ();
	gtk_box_pack_end (GTK_BOX (vbox), buttonbox, FALSE, FALSE,8);
	gtk_widget_show (buttonbox);

	close = gtk_button_new_with_label ("Close");
	GTK_WIDGET_SET_FLAGS (close, GTK_CAN_DEFAULT);
	gtk_window_set_default (GTK_WINDOW (about_window), close);
	gtk_hbutton_box_set_layout_default (GTK_BUTTONBOX_END);
	gtk_box_pack_end (GTK_BOX (buttonbox), close, FALSE, FALSE,8);
	gtk_widget_show (close);

	gtk_signal_connect (GTK_OBJECT (close), "clicked", GTK_SIGNAL_FUNC (gui_about_closed), NULL);
	gtk_signal_connect (GTK_OBJECT (about_window), "delete-event", GTK_SIGNAL_FUNC (gui_about_destroy), NULL);
	      
	gtk_widget_show (about_window);
}

static void lv_xmms_configure ()
{

}

static void lv_xmms_disable (VisPlugin* plugin)
{

}

static void lv_xmms_playback_start ()
{

}
static void lv_xmms_playback_stop ()
{

}

static void lv_xmms_render_pcm (gint16 data[2][512])
{
	int i;

	if (visual_running == 1) {
		SDL_mutexP (pcm_mutex);

		/* FIXME This can be copied on one operation */
		for (i = 0; i < 512; i++) {
			xmmspcm[0][i] = data[0][i];
			xmmspcm[1][i] = data[1][i];
		}

		SDL_mutexV (pcm_mutex);
	}
}

static void gui_about_closed (GtkWidget *w, GdkEvent *e, gpointer data)
{
	gui_about_destroy (about_window, data);
}
                                                                                                                                               
static void gui_about_destroy (GtkWidget *w, gpointer data)
{
	gtk_widget_destroy (w);
	about_window = NULL;
}

static int sdl_init ()
{
	gchar *error_msg;
	
	if (SDL_Init (SDL_INIT_VIDEO) < 0) {
		error_msg = g_strdup_printf ("%s: could not initialize SDL: %s",
						PACKAGE_NAME, SDL_GetError());
		visual_log (VISUAL_LOG_CRITICAL, error_msg);
		g_free (error_msg);
		return -1;
	}
	atexit (SDL_Quit);

	return 0;
}

static int sdl_quit ()
{
	visual_log (VISUAL_LOG_DEBUG, "calling SDL_FreeSurface()");
	if (screen != NULL)
		SDL_FreeSurface (screen);

	screen = NULL;

	/*
	 * FIXME this doesn't work!
	 * 
	visual_log (VISUAL_LOG_DEBUG, "sdl_quit: calling SDL_Quit()");
	SDL_Quit ();*/
	
	visual_log (VISUAL_LOG_DEBUG, "leaving...");
	return 0;
}

static void sdl_set_pal ()
{
	int i;

	if (pal == NULL)
		return;

	for (i = 0; i < 256; i ++) {
		sdlpal[i].r = pal->r[i];
		sdlpal[i].g = pal->g[i];
		sdlpal[i].b = pal->b[i];
	}

	SDL_SetColors (screen, sdlpal, 0, 256);
}

static void sdl_draw (SDL_Surface *screen)
{
	g_assert (screen != NULL);
	SDL_Flip (screen);
}

static int sdl_create (int width, int height)
{
	const SDL_VideoInfo *videoinfo;
	int videoflags;

	if (screen != NULL)
		SDL_FreeSurface (screen);

	printf ("OI OI SDL_CREATE video->bpp %d\n", video->bpp);
	printf ("GL PLUG at create :%d\n", gl_plug);
	if (gl_plug == 1) {
		videoinfo = SDL_GetVideoInfo ();

		if (videoinfo == 0) {
			visual_log (VISUAL_LOG_CRITICAL, "could not get video info");
			return -1;
		}

		videoflags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

		if (videoinfo->hw_available)
			videoflags |= SDL_HWSURFACE;
		else
			videoflags |= SDL_SWSURFACE;

		if (videoinfo->blit_hw)
			videoflags |= SDL_HWACCEL;

		SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

		screen = SDL_SetVideoMode (width, height, 16, videoflags);
	} else
		screen = SDL_SetVideoMode (width, height, video->bpp * 8, SDL_RESIZABLE);

	visual_video_set_buffer (video, screen->pixels);
	printf ("Pointer to the pixels: %p\n", screen->pixels);
	visual_video_set_pitch (video, screen->pitch);
	printf ("*********** **************** pitch: %d\n", video->pitch);

	return 0;
}

static int sdl_event_handle ()
{
	SDL_Event event;
	VisEventQueue *vevent;

	while (SDL_PollEvent (&event)) {
		vevent = visual_actor_get_eventqueue (visual_bin_get_actor (bin));
		
		switch (event.type) {
			case SDL_KEYUP:
				visual_event_queue_add_keyboard (vevent, event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_UP);
				break;
				
			case SDL_KEYDOWN:
				visual_event_queue_add_keyboard (vevent, event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_DOWN);
				
				switch (event.key.keysym.sym) {
					/* XMMS CONTROLS */
					case SDLK_z:
						xmms_remote_playlist_prev (lv_xmms_vp.xmms_session);
						break;

					case SDLK_x:
						xmms_remote_play (lv_xmms_vp.xmms_session);
						break;

					case SDLK_c:
						xmms_remote_pause (lv_xmms_vp.xmms_session);
						break;

					case SDLK_v:
						xmms_remote_stop (lv_xmms_vp.xmms_session);
						break;

					case SDLK_b:
						xmms_remote_playlist_next (lv_xmms_vp.xmms_session);
						break;

					/* PLUGIN CONTROLS */
					case SDLK_F11:
						SDL_WM_ToggleFullScreen (screen);

						if ((screen->flags & SDL_FULLSCREEN) > 0)
							SDL_ShowCursor (SDL_DISABLE);
						else
							SDL_ShowCursor (SDL_ENABLE);

						break;

					case SDLK_a:
						cur_lv_plugin = visual_actor_get_prev_by_name (cur_lv_plugin);

						if (cur_lv_plugin == NULL)
							cur_lv_plugin = visual_actor_get_prev_by_name (cur_lv_plugin);

						if (SDL_MUSTLOCK (screen) == SDL_TRUE)
							SDL_LockSurface (screen);

						visual_bin_set_morph_by_name (bin, "alphablend");
						visual_bin_switch_actor_by_name (bin, cur_lv_plugin);

						if (SDL_MUSTLOCK (screen) == SDL_TRUE)
							SDL_UnlockSurface (screen);

						break;

					case SDLK_s:
						cur_lv_plugin = visual_actor_get_next_by_name (cur_lv_plugin);

						if (cur_lv_plugin == NULL)
							cur_lv_plugin = visual_actor_get_next_by_name (cur_lv_plugin);

						if (SDL_MUSTLOCK (screen) == SDL_TRUE)
							SDL_LockSurface (screen);

						visual_bin_set_morph_by_name (bin, "alphablend");
						visual_bin_switch_actor_by_name (bin, cur_lv_plugin);

						if (SDL_MUSTLOCK (screen) == SDL_TRUE)
							SDL_UnlockSurface (screen);

						break;
				}
				break;

			case SDL_VIDEORESIZE:
				visual_resize (event.resize.w, event.resize.h);
				break;

			case SDL_MOUSEMOTION:
				visual_event_queue_add_mousemotion (vevent, event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_DOWN);
				break;

			case SDL_MOUSEBUTTONUP:
				visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_UP);
				break;

			case SDL_QUIT:
				/* FIXME refuses to work ! */
//				lv_xmms_vp.disable_plugin (&lv_xmms_vp);
				break;

			default:
				break;
		}
	}

	return 0;
}

static int visual_upload_callback (VisInput *input, VisAudio *audio, void *private)
{
	int i;

	for (i = 0; i < 512; i++) {
		audio->plugpcm[0][i] = xmmspcm[0][i];
		audio->plugpcm[1][i] = xmmspcm[1][i];
	}

	return 0;
}

static int visual_resize (int width, int height)
{
	visual_video_set_dimension (video, width, height);

	sdl_create (width, height);
	
	lv_width = width;
	lv_height = height;

	visual_bin_sync (bin, FALSE);
}

static int visual_initialize (int width, int height)
{
	VisInput *input;

	bin = visual_bin_new ();
	visual_bin_set_supported_depth (bin, VISUAL_VIDEO_DEPTH_ALL);
//	visual_bin_set_preferred_depth (bin, VISUAL_BIN_DEPTH_LOWEST);

	depth = VISUAL_VIDEO_DEPTH_32BIT;

	video = visual_video_new ();
	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);

	visual_bin_set_video (bin, video);
	visual_bin_connect_by_names (bin, cur_lv_plugin, NULL);

	if (visual_bin_get_depth (bin) == VISUAL_VIDEO_DEPTH_GL) {
		visual_video_set_depth (video, VISUAL_VIDEO_DEPTH_GL);
		gl_plug = 1;
	} else
		gl_plug = 0;

	printf ("GL PLUG: %d\n", gl_plug);
	sdl_create (width, height);
	
	/* Called so the flag is set to FALSE, seen we create the initial environment here */
	visual_bin_depth_changed (bin);
	
	input = visual_bin_get_input (bin);
	visual_input_set_callback (input, visual_upload_callback, NULL);
	
	visual_bin_switch_set_style (bin, VISUAL_SWITCH_STYLE_MORPH);
	visual_bin_switch_set_automatic (bin, TRUE);
	visual_bin_switch_set_steps (bin, 100);

	visual_bin_realize (bin);
	visual_bin_sync (bin, FALSE);

	return 0;
}

static void *visual_render ()
{
	visual_running = 1;
	visual_stopped = 0;
	
	while (visual_running == 1) {
		/* Update songinfo */
		songinfo = visual_actor_get_songinfo (visual_bin_get_actor (bin));
		visual_songinfo_set_type (songinfo, VISUAL_SONGINFO_TYPE_SIMPLE);

		visual_songinfo_set_simple_name (songinfo, lv_xmms_get_songname ());

		if ((SDL_GetAppState () & SDL_APPACTIVE) == FALSE) {
			usleep (100000);
		} else {
			/* On depth change */
			if (visual_bin_depth_changed (bin) == TRUE) {
				if (SDL_MUSTLOCK (screen) == SDL_TRUE)
					SDL_LockSurface (screen);

				visual_video_set_buffer (video, screen->pixels);
//				printf ("pixel buf %p\n", screen->pixels);
				if (visual_bin_get_depth (bin) == VISUAL_VIDEO_DEPTH_GL)
					gl_plug = 1;
				else
					gl_plug = 0;
			
//				printf ("HALLO HALLO %d %d %d\n", gl_plug, video->depth, video->bpp);
				sdl_create (lv_width, lv_height);
//				printf ("SDL_CREATE_PITCH %d\n", screen->pitch);
				visual_bin_sync (bin, TRUE);

				if (SDL_MUSTLOCK (screen) == SDL_TRUE)
					SDL_UnlockSurface (screen);
			}

			if (gl_plug == 1) {
				visual_bin_run (bin);

				SDL_GL_SwapBuffers ();
			} else {
				if (SDL_MUSTLOCK (screen) == SDL_TRUE)
					SDL_LockSurface (screen);

				visual_video_set_buffer (video, screen->pixels);
				visual_bin_run (bin);

				if (SDL_MUSTLOCK (screen) == SDL_TRUE)
					SDL_UnlockSurface (screen);

				pal = visual_bin_get_palette (bin);
				sdl_set_pal ();

				sdl_draw (screen);
			}
		}
		sdl_event_handle ();
	}

	visual_stopped = 1;
	return NULL;
}

