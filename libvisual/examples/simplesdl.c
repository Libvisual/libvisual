#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "../libvisual/libvisual.h"

/* The code in this example is written a bit hacky, but the libvisual code is as it should
 * be used so you could take that and use that as an example */

SDL_Surface *screen;
SDL_Color colors[256];
unsigned char *scrbuf;

/* The visual plugin */
VisActor *actor;
/* The video context, contains information about, depth, bpp, width, height, etc etc */
VisVideo *video;
/* Palette data structure for 8 bits mode */
VisPalette *pal = NULL;

/* The input plugin */
VisInput *input;

/* The container */
VisBin *bin;

int bpp = 1;
int gl_plug = 0;

void sdl_fullscreen_toggle (void);
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);
void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf (void);
void sdl_set_pal (void);
	
/* Fullscreen stuff */
void sdl_fullscreen_toggle ()
{
	static int fullscreen = 0;

	fullscreen = 1 - fullscreen;

	sdl_fullscreen_set (fullscreen);
}

void sdl_fullscreen_xy (int *x, int *y)
{
	SDL_Rect **modelist;
	int i;
	int width = screen->w;
	int height = screen->h;

	modelist = SDL_ListModes (NULL, SDL_FULLSCREEN);

	if (modelist == NULL) {
		*x = width;
		*y = height;
	}

	/* Is the window bigger than the highest available resolution ? */
	if (modelist[0]->w <= width || modelist[0]->h <= height) {
		*x = modelist[0]->w;
		*y = modelist[0]->h;

		return;
	}

	for(i = 0; modelist[i]; ++i) {
		if (modelist[i]->w >= width && modelist[i]->h >= height) {
			*x = modelist[i]->w;
			*y = modelist[i]->h;
		}
	}
}

int sdl_fullscreen_set (int mode)
{
	static int oldw;
	static int oldh;
	int screen_fullscreen = 0;
	int tempx;
	int tempy;

	switch (mode) {
		case 0:
			if ((screen->flags & SDL_FULLSCREEN) != 0) {
				SDL_WM_ToggleFullScreen (screen);
				SDL_ShowCursor (SDL_ENABLE);

				sdl_size_request (oldw, oldh);
				screen_fullscreen = 0;

				return 0;
			}
			break;

		case 1:
			if ((screen->flags & SDL_FULLSCREEN) == 0) {
				if (screen_fullscreen == 0)
				{
					oldw = screen->w;
					oldh = screen->h;
				}

				if (screen_fullscreen == 1)
					SDL_WM_ToggleFullScreen (screen);

				sdl_fullscreen_xy (&tempx, &tempy);

				sdl_size_request (tempx, tempy);

				if (SDL_WM_ToggleFullScreen (screen)) {
					SDL_ShowCursor (SDL_DISABLE);
					screen_fullscreen = 1;

					return 0;
				} else {
					sdl_size_request (oldw, oldh);

					screen_fullscreen = 0;

					return -1;
				}
			}
			break;

		default:
			break;
	}

	return 0;
}

/* Sdl stuff */
void sdl_size_request (int width, int height)
{
	SDL_FreeSurface (screen);

	sdl_create (width, height);

	visual_video_set_dimension (video, width, height);
	/* Only needed if the framebuffer pitch is
	 * differs from the width, (this happens
	 * in SDL */
	visual_video_set_pitch (video, screen->pitch);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);
	
	if (gl_plug == 0) {
		free (scrbuf);
		scrbuf = malloc (visual_video_get_size (video));
		memset (scrbuf, 0, visual_video_get_size (video));

		visual_video_set_buffer (video, scrbuf);
	}		
}

void sdl_init (int width, int height)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Unable to init SDL VIDEO: %s\n", SDL_GetError ());
		exit (0);
	}

	sdl_create (width, height);
}

void sdl_create (int width, int height)
{
	const SDL_VideoInfo *videoinfo;
	int videoflags;
		
	if (gl_plug == 1) {
		videoinfo = SDL_GetVideoInfo ();

		if (videoinfo == 0) {
			printf ("Couldn't get video info\n");
			exit (-1);
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
		screen = SDL_SetVideoMode (width, height, bpp * 8, SDL_RESIZABLE);
}

void sdl_draw_buf ()
{
	unsigned char *str = (unsigned char *) screen->pixels;
	
	memcpy (str, scrbuf, visual_video_get_size (video));

	SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
}

void sdl_set_pal ()
{
	int i;

	for (i = 0; i < 256; i ++) {
		colors[i].r = pal->colors[i].r;
		colors[i].g = pal->colors[i].g;
		colors[i].b = pal->colors[i].b;
	}
	
	SDL_SetColors (screen, colors, 0, 256);
}

/* Main stuff */
int main (int argc, char *argv[])
{
	int width = 480, height = 360;
	int depth = VISUAL_VIDEO_DEPTH_8BIT;
	int freeze = 0;
	
	int depthflag = 0;
	
	time_t begin = time (NULL), end;
	int frames = 0;
	char *input_name = NULL;
	
	
	if (visual_init (&argc, &argv) < 0)
		visual_log (VISUAL_LOG_ERROR, "Could not initialize Libvisual");
	
	/* Check libvisual version */
	visual_log (VISUAL_LOG_INFO, "Libvisual version %s", visual_get_version ());

	/* Make a new actor from actlist, with pluginname */
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("oinksie");

	if (actor->plugin == NULL) {
		visual_log (VISUAL_LOG_ERROR, "Couldn't create actor plugin");
		return -1;
	}
	
	depthflag = visual_actor_get_supported_depth (actor);
	
	if (argc > 2) {
		/* Get an enum value from the depth argument */
		depth = visual_video_depth_enum_from_value (atoi (argv[2]));
				
		/* Check if the depth is supported */
		if (visual_video_depth_is_supported (depthflag, depth) < 1) {
			visual_log (VISUAL_LOG_INFO, "Plugin doesn't support this depth, but we'll set up an transformation enviroment.");
			visual_log (VISUAL_LOG_INFO, "However showing you a nice list of supported depths anyway");

			/* Show a list of supported depths */
			int i, j;
			i = VISUAL_VIDEO_DEPTH_NONE;

			if (visual_video_depth_is_supported (depthflag, i) == 1)
				visual_log (VISUAL_LOG_INFO, "Support visual video context NONE");

			do {
				j = i;
				
				i = visual_video_depth_get_next (depthflag, j);
				if (i == j)
					break;
				
				if (i == VISUAL_VIDEO_DEPTH_GL) {
				    visual_log (VISUAL_LOG_INFO, "Support visual depth GL");
				} else {
				    visual_log (VISUAL_LOG_INFO, "Support visual depth %d",
						    visual_video_depth_value_from_enum (i));
				}

			} while (i < VISUAL_VIDEO_DEPTH_GL);

			depth = i;
		}
	
	} else {
		/* Get the highest supported depth */
		depth = visual_video_depth_get_highest (depthflag);
	}

	/* When the plugin is an GL plugin */
	if (depth == VISUAL_VIDEO_DEPTH_GL)
		gl_plug = 1;

	sdl_init (width, height);

	/* We need to init sdl first when we use GL, and then realize
	 * and set dimensions, this is because the init and dimension
	 * code within the gl plugin contains GL code. */
	visual_actor_realize (actor);

	/* Create a new video context */
	video = visual_video_new ();

	/* Link the video context to the actor */
	visual_actor_set_video (actor, video);
	
	/* Set depth, 16BIT, 24BIT, 32BIT, GL do also exist */
	visual_video_set_depth (video, depth);
	
	/* Set dimension */
	visual_video_set_dimension (video, width, height);
	
	/* Negotiate with video, if needed, the actor will set up and enviroment
	 * for depth transformation and it does all the size negotation stuff */
	if (visual_actor_video_negotiate (actor, 0, FALSE, FALSE) == -1)
		visual_log (VISUAL_LOG_ERROR, "Couldn't negotiate the actor with the video");

	if (gl_plug == 0) {
		/* Retrieve the bpp from the depth so we can use that for buffer
		 * allocating and SDL init */
		bpp = visual_video_bpp_from_depth (depth);

		/* Now we know the size, allocate the buffer */
		scrbuf = malloc (visual_video_get_size (video));
		memset (scrbuf, 0, visual_video_get_size (video));

		/* Link the buffer to the video context */
		visual_video_set_buffer (video, scrbuf);
	}
	
	if (argc >= 4)
	        input_name = argv[3];
	else
	        input_name = "alsa";

	input = visual_input_new (input_name);
	visual_log_return_val_if_fail(input != NULL, -1 );
	
	if (input == NULL) {
		visual_log(VISUAL_LOG_ERROR, 
				"Plugin \"%s\" doesn't exist or could not be loaded.\n",
				input_name);
		return -1;
	}

	/* Create a new bin, a bin is a container for easy
	 * management of an working input, actor, render, output pipeline */
	bin = visual_bin_new ();

	/* Add the actor, input, video to the bin */
	visual_bin_connect (bin, actor, input);
	visual_bin_realize (bin);

	SDL_Event event;

	/* This kinda sucks, that we have to size twice, but this is needed for the GL plugins, we need
	 * to init sdl gl stuff before initting the plugin, because the plugin sets GL opts */
	sdl_create (video->width, video->height);
	
	while (1) {
		if (freeze == 0) {
			/* Run the bin, this will retrieve sound
			 * analyse the sound, push it towards the
			 * actor plugin, render and return */
			visual_bin_run (bin);

			if (gl_plug == 1) {
				SDL_GL_SwapBuffers ();
			} else {
				/* Retrieve palette information */
				pal = visual_bin_get_palette (bin);

				if (pal != NULL)
					sdl_set_pal ();

				sdl_draw_buf ();
			}

			frames++;
		}
		usleep (5000);
		
		while (SDL_PollEvent (&event)) {
			VisEventQueue *vevent;

			vevent = visual_plugin_get_eventqueue (visual_actor_get_plugin (visual_bin_get_actor (bin)));
			
			switch (event.type) {
				case SDL_KEYUP:
					visual_event_queue_add_keyboard (vevent, event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_UP);
					break;
				
				case SDL_KEYDOWN:
					visual_event_queue_add_keyboard (vevent, event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_DOWN);
					switch (event.key.keysym.sym) {
						case SDLK_F11:
							sdl_fullscreen_toggle ();
							break;

						case SDLK_f:
							freeze = 1 - freeze;
							break;
							
						case SDLK_ESCAPE:
							goto out;
							break;
						default: /* to avoid warnings */
							break;
					}
					break;

				case SDL_VIDEORESIZE:
					sdl_size_request (event.resize.w, event.resize.h);
					break;

				case SDL_QUIT:
					goto out;
					break;
				default: /* to avoid warnings */
					break;
			}
		}
	}
out:
	SDL_Quit ();

	end = time (NULL);

	visual_log (VISUAL_LOG_INFO, "Drawn %d frames in %d seconds, average fps %d",
			(int)frames, (int)(end - begin),
			(end - begin) == 0 ? (int)frames : (int)(frames / (end - begin)));

	visual_video_free_buffer (video);

	visual_quit ();

	return 0;
}

