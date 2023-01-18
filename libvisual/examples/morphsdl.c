#include <unistd.h>
#include <stdlib.h>

#include <SDL.h>

#include "../libvisual/libvisual.h"


/* The code in this example is written a bit hacky, but the libvisual code is as it should
 * be used so you could take that and use that as an example */

static SDL_Surface *screen;
static SDL_Color colors[256];

/* The bin in which the visual pipe runs */
static VisBin *bin;

/* The video that the bin returns so sdl can draw it */
static VisVideo *video;

static int bppres = 1;

static int have_opengl;

/* Function prototypes */
void sdl_fullscreen_toggle (void);
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);

void sdl_size_request (int width, int height);

void sdl_init (int width, int height);
void sdl_create (int width, int height);

void sdl_palette (VisPalette *pal);

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

	visual_video_set_dimension (video, width, height);

	sdl_create (width, height);
	
	visual_video_set_pitch (video, screen->pitch);
	visual_video_set_buffer (video, screen->pixels);
	
	visual_bin_sync (bin, FALSE);
}

void sdl_init (int width, int height)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Unable to init SDL VIDEO: %s\n", SDL_GetError ());
		exit (1);
	}

	sdl_create (width, height);
}

void sdl_create (int width, int height)
{
	int video_flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWPALETTE | SDL_RESIZABLE;

//	if (have_opengl)
//		video_flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;
	
	screen = SDL_SetVideoMode (width, height, bppres * 8, video_flags);
	if (screen == NULL) {	
		fprintf (stderr, "Unable to get an SDL video screen: %s\n", SDL_GetError ());
		exit (1);
	}
}

void sdl_palette (VisPalette *pal)
{
	int i;

	if (pal == NULL)
		return;

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
	/* Palette for 8bit mode */
	VisPalette *palbin;

	int width = 320, height = 200;
	VisVideoDepth depthuse = VISUAL_VIDEO_DEPTH_32BIT;

	int freeze = 0;

	time_t begin = time (NULL), end;
	int frames = 0;

	/* Morph controls */
	float alpha_var = 0.5;
	float morph_speed = 0.01;
	int morphing = 0;
	int morph_to = 0;

	/* Used to list all the plugins */
	const char *name = NULL;

	SDL_Event event;
	
	visual_init (&argc, &argv);
	
	/* Check libvisual version */
	printf ("Libvisual version %s\n", visual_get_version ());

	/*
	 * In future we will conditionally set this
	 * 
#ifdef HAVE_OPENGL
	have_opengl = 1;
else
	have_opengl = 0;
#endif	
	*/
	have_opengl = 1;
	/* Show a list of plugins */
	if (argc < 4) {
		printf ("usage: %s actor1 actor2 morph [targetdepth]\n", argv[0]);

		printf ("\nValid actors:\n");
		do {
			name = visual_actor_get_next_by_name (name);
	
			if (name != NULL)
				printf ("%s\n", name);
		} while (name != NULL);

		printf ("\nValid morphs:\n");
		do {
			name = visual_morph_get_next_by_name (name);

			if (name != NULL)
				printf ("%s\n", name);
		} while (name != NULL);
		
		printf ("\n");
		
		exit (-1);
	}

	/* If there is a depth flag, use it */
	if (argc > 4)
		depthuse = visual_video_depth_enum_from_value (atoi (argv[4]));

	/* Check if the user given depth is sane */
	if (visual_video_depth_is_sane (depthuse) == 0) {
		printf ("The given actor and morph depth is insane\n");
		exit (-1);
	}

	/* Check the bytes per pixel */
	bppres = visual_video_bpp_from_depth (depthuse);

	/* Initialize SDL */
	sdl_init (width, height);

	video = visual_video_new ();
	
	/* Set the video props, do this in >THIS< order, first depth,
	 * then dimension, then pitch */
	visual_video_set_depth (video, depthuse);
	visual_video_set_dimension (video, width, height);
	visual_video_set_pitch (video, screen->pitch);

	bin = visual_bin_new ();

	/* When changing the actor the bin needs access to the video memory */
	if (SDL_MUSTLOCK (screen) == SDL_TRUE)
		SDL_LockSurface (screen);

	visual_video_set_buffer (video, screen->pixels);
	
	visual_bin_set_supported_depth (bin, VISUAL_VIDEO_DEPTH_ALL);
	
	visual_bin_set_video (bin, video);
	visual_bin_connect_by_names (bin, argv[1], "alsa");
	visual_bin_realize (bin);
	visual_bin_sync (bin, FALSE);

	visual_bin_set_morph_by_name (bin, argv[3]);
	visual_bin_switch_set_style (bin, VISUAL_SWITCH_STYLE_MORPH);
	visual_bin_switch_set_automatic (bin, FALSE);
	/* When automatic is set on TRUE this defines in how many
	 * frames the morph should take place */
	visual_bin_switch_set_steps (bin, 100);
	visual_bin_switch_actor_by_name (bin, argv[2]);

	if (SDL_MUSTLOCK (screen) == SDL_TRUE)
		SDL_UnlockSurface (screen);
	
	while (1) {
		if (freeze == 0) {
			if (SDL_MUSTLOCK (screen) == SDL_TRUE)
				SDL_LockSurface (screen);

			visual_video_set_buffer (video, screen->pixels);

			visual_bin_run (bin);
			
			palbin = visual_bin_get_palette (bin);
			sdl_palette (palbin);

			if (SDL_MUSTLOCK (screen) == SDL_TRUE)
				SDL_UnlockSurface (screen);

			SDL_Flip (screen);

			if (morphing == 1) {
				if (morph_to < alpha_var)
					alpha_var -= morph_speed;
				else
					alpha_var += morph_speed;

				if (alpha_var <= 0) {
					alpha_var = 0;
					morphing = 0;
				} else if (alpha_var >= 1) {
					alpha_var = 1;
					morphing = 0;
				}
			}

			visual_bin_switch_set_rate (bin, alpha_var);

			frames++;
		}
		usleep (5000);
		
		while (SDL_PollEvent (&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_F11:
							sdl_fullscreen_toggle ();
							break;

						case SDLK_f:
							freeze = 1 - freeze;
							break;


						case SDLK_z:
							alpha_var += 0.05;

							if (alpha_var > 1)
								alpha_var = 1.00;

							break;

						case SDLK_x:
							alpha_var -= 0.05;

							if (alpha_var < 0)
								alpha_var = 0.00;

							break;

						case SDLK_a:
							morph_speed += 0.01;
							break;

						case SDLK_s:
							morph_speed -= 0.01;
							break;

						case SDLK_v:
							morphing = 1;
							morph_to = 1 - morph_to;
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

	printf ("Drawn %d frames in %d seconds, average fps %d\n",
			(int)frames, (int)(end - begin),
			(end - begin) == 0 ? (int)frames : (int)(frames / (end - begin)));

	visual_video_free_buffer (video);

	visual_quit ();

	return 0;
}

