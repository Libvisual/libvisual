#include <unistd.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <libvisual/libvisual.h>

#include <gdk/gdk.h>

SDL_Surface *screen;
SDL_Color colors[256];
VisActor *actor;
VisInput *input;
VisVideo *video1;
VisVideo *video2;
VisVideo *video3;
VisVideo *target;

unsigned char *scrbuf;
int bpp;

void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf ();

/* Sdl stuff */
void sdl_size_request (int width, int height)
{
	SDL_FreeSurface (screen);

	sdl_create (width, height);

	free (scrbuf);
	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);
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
	
	screen = SDL_SetVideoMode (width, height, bpp * 8, 0);
}

void sdl_draw_buf ()
{
	unsigned char *str = (unsigned char *) screen->pixels;
	int i;
	
	memcpy (str, scrbuf, screen->pitch * screen->h);

	SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
}

/* Main stuff */
int main (int argc, char *argv[])
{
	int width = 600, height = 81;
	int i, j;
	int freeze = 0;
	int depthflag = 0;
	int alpha = 128;
	int depth = VISUAL_VIDEO_DEPTH_32BIT;
	GdkPixbuf *pixbuf;
	GdkPixbuf *scaled;

	bpp = visual_video_bpp_from_depth (depth);
	sdl_init (width, height);

	visual_init (&argc, &argv);

	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);

	printf ("oi: %d\n", screen->pitch);
	
	target = visual_video_new ();
	visual_video_set_depth (target, depth);
	visual_video_set_dimension (target, width, height);
	visual_video_set_pitch (target, screen->pitch);
	visual_video_set_buffer (target, scrbuf);

	video1 = visual_video_new ();
	visual_video_clone (video1, target);
	visual_video_allocate_buffer (video1);

	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else 
		actor = visual_actor_new ("G-Force");
	input = visual_input_new ("alsa");
	
	visual_actor_set_video (actor, video1);
	visual_actor_realize (actor);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	visual_input_realize (input);

	g_type_init ();

	pixbuf = gdk_pixbuf_new_from_file ("header_background.svg", NULL);
	scaled = gdk_pixbuf_scale_simple (pixbuf, width, height, GDK_INTERP_NEAREST);

	VisVideo bgr;
	memset (&bgr, 0, sizeof (VisVideo));

	/* Create a VisVideo from the pixbuf */
	visual_video_set_depth (&bgr,
			visual_video_depth_enum_from_value (gdk_pixbuf_get_n_channels (scaled) * 8));
	visual_video_set_dimension (&bgr, gdk_pixbuf_get_width (scaled), gdk_pixbuf_get_height (scaled));
	visual_video_set_pitch (&bgr, gdk_pixbuf_get_rowstride (scaled));
	visual_video_set_buffer (&bgr, gdk_pixbuf_get_pixels (scaled));

	video2 = visual_video_new ();
	visual_video_clone (video2, &bgr);
	visual_video_allocate_buffer (video2);

	/* Gdk uses a different color order than we do */
	visual_video_color_bgr_to_rgb (video2, &bgr);
	
	/* Create a VisVideo from the pixbuf, for the button */
	pixbuf = gdk_pixbuf_new_from_file ("button.png", NULL);

	visual_video_set_depth (&bgr,
			visual_video_depth_enum_from_value (gdk_pixbuf_get_n_channels (pixbuf) * 8));
	visual_video_set_dimension (&bgr, gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height (pixbuf));
	visual_video_set_pitch (&bgr, gdk_pixbuf_get_rowstride (pixbuf));
	visual_video_set_buffer (&bgr, gdk_pixbuf_get_pixels (pixbuf));

	video3 = visual_video_new ();
	visual_video_clone (video3, &bgr);
	visual_video_allocate_buffer (video3);

	/* Gdk uses a different color order than we do */
	visual_video_color_bgr_to_rgb (video3, &bgr);

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	SDL_Event event;

	while (1) {
		int blah;
		
		visual_input_run (input);
		visual_actor_run (actor, input->audio);

		visual_video_alpha_fill (video1, alpha);
// Use to set full transparancy on black background, doesn't look nice for most plugins) (does for the analyzer tho)
//		visual_video_alpha_color (video1, 0, 0, 0, 0);
	
		printf ("alpha %d\n", alpha);
		visual_video_blit_overlay (target, video2, 0, 0, FALSE);
		visual_video_blit_overlay (target, video1, 0, 0, TRUE);
		visual_video_blit_overlay (target, video3, target->width - video3->width - 5,
				target->height - video3->height - 5, FALSE);


		sdl_draw_buf ();
		
		usleep (5000);
		
		while (SDL_PollEvent (&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_q:
							alpha -= 8;
							if (alpha < 0)
								alpha = 0;

							break;

						case SDLK_a:
							alpha += 8;
							if (alpha > 255)
								alpha = 255;

							break;
						
						case SDLK_ESCAPE:
							goto out;
							break;
					}
					break;

				case SDL_VIDEORESIZE:
					sdl_size_request (event.resize.w, event.resize.h);
					break;

				case SDL_QUIT:
					goto out;
					break;
			}
		}
	}
out:
	SDL_Quit ();
}

