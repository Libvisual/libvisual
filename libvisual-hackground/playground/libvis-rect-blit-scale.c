#include <unistd.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <libvisual/libvisual.h>


SDL_Surface *screen;
SDL_Color colors[256];
unsigned char *scrbuf;
int bpp;

VisActor *actor;
VisVideo *video;
VisVideo *sdlvid;
VisVideo *scalevid;
VisPalette *pal;
VisInput *input;

void sdl_fullscreen_toggle ();
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);
void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf ();

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
}

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
	int width = 1000, height = 600;
	int i, j;
	int freeze = 0;
	int depthflag = 0;
	int alpha = 128;
	int xoff = 0, yoff = 0;
	int sxsize = 50;
	int sysize = 50;
	int dismode = 0;

	VisRectangle drect;
	VisRectangle srect;

	bpp = 4;
	sdl_init (width, height);

	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);

	SDL_Event event;

	visual_init (&argc, &argv);
	
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("G-Force");

	visual_actor_realize (actor);

	video = visual_video_new ();
	
	visual_actor_set_video (actor, video);
	visual_video_set_depth (video, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (video, 256, 256);
	visual_video_allocate_buffer (video);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	scalevid = visual_video_new ();
	visual_video_allocate_buffer (scalevid);

	sdlvid = visual_video_new ();
	visual_video_set_depth (sdlvid, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (sdlvid, screen->w, screen->h);
	visual_video_set_pitch (sdlvid, screen->pitch);
	visual_video_set_buffer (sdlvid, scrbuf);
	
	input = visual_input_new ("alsa");
	visual_input_realize (input);
        
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	while (1) {
		visual_input_run (input);
		visual_actor_run (actor, input->audio);

		memset (visual_video_get_pixels (sdlvid), 0, visual_video_get_size (video) * video->bpp);
		
		/* place on screen */
		visual_video_blit_overlay (sdlvid, video, 0, 0, FALSE);

		visual_video_set_palette (scalevid, visual_actor_get_palette (actor));	

		if (dismode == 0) {
			visual_rectangle_set (&drect, xoff, yoff, sxsize, sysize);
			visual_rectangle_set (&srect, 0, 0, 50, 50);
		} else {
			visual_rectangle_set (&drect, 256, 0, sxsize, sysize);
			visual_rectangle_set (&srect, xoff, yoff, 50, 50);
		}

		visual_video_blit_overlay_rectangle_scale (sdlvid, &drect, video, &srect, FALSE, VISUAL_VIDEO_SCALE_BILINEAR);
		
//		visual_rectangle_set (&rect, xoff, yoff, sxsize, sysize);
//		visual_video_region_sub (scalevid, video, &rect);

		visual_video_blit_overlay (sdlvid, scalevid, 256, 0, FALSE);

		sdl_draw_buf ();
		
		usleep (5000);
		
		while (SDL_PollEvent (&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_F11:
							sdl_fullscreen_toggle ();
							break;
						case SDLK_UP:
							yoff -= 10;

							break;

						case SDLK_DOWN:
							yoff += 10;

							break;

						case SDLK_LEFT:
							xoff -= 10;

							break;

						case SDLK_RIGHT:
							xoff += 10;

							break;

						case SDLK_q:
							sysize -= 10;

							break;

						case SDLK_a:
							sysize += 10;

							break;

						case SDLK_z:
							sxsize -= 10;

							break;

						case SDLK_x:
							sxsize += 10;

							break;

						case SDLK_m:
							dismode = ~dismode;

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
	visual_quit ();
}

