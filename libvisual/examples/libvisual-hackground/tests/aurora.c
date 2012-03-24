#include <unistd.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <libvisual/libvisual.h>


SDL_Surface *screen;
SDL_Color colors[256];
unsigned char *scrbuf;
int bpp;

VisActor *actor;
VisVideo *actvid;
VisVideo *video;
VisVideo *video32;
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
void do_alpha (VisVideo *vid, uint8_t rate);

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

	memset (scrbuf, 0, screen->pitch * screen->h);
	SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
}

void do_alpha (VisVideo *vid, uint8_t rate)
{
	int i;
	uint32_t *ptr = visual_video_get_pixels(vid); //vid->pixels;
	union {
		uint32_t c32;
		uint8_t c8[4];
	} col;

	for (i = 0; i < vid->width * vid->height; i++) {
		col.c32 = ptr[i];
	
		col.c8[3] = rate;
		
//		if (col.c8[0] > 140) {
//			col.c8[3] = rate - (200 - col.c8[0]);
//		}
		ptr[i] = col.c32;
	}
}

/* Main stuff */
int main (int argc, char *argv[])
{
	int width = 1000, height = 600;
	int i, j;
	int freeze = 0;
	int depthflag = 0;
	int alpha = 190;
	int xoff = 0, yoff = -90;
	int sxsize = 1000;
	int sysize = 700;
	int interpol = VISUAL_VIDEO_SCALE_NEAREST;
        int frames = 0;
	VisTime start, end;
		
	bpp = 4;
	sdl_init (width, height);

	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);

	SDL_Event event;

	visual_init (&argc, &argv);
	
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("corona");

	visual_actor_realize (actor);

	video = visual_video_new ();

	if (argc > 2)
		video = visual_bitmap_load_new_video (argv[2]);
	else
		video = visual_bitmap_load_new_video ("bg.bmp");

	actvid = visual_video_new ();
	visual_actor_set_video (actor, actvid);
	visual_video_set_depth (actvid, visual_video_depth_get_highest (visual_actor_get_supported_depth (actor)));
	visual_video_set_dimension (actvid, width, height);
	visual_video_allocate_buffer (actvid);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	video32 = visual_video_new ();
	visual_video_set_depth (video32, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (video32, video->width, video->height);
	visual_video_allocate_buffer (video32);
	
	scalevid = visual_video_new ();
	visual_video_set_depth (scalevid, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (scalevid, sxsize, sysize);
	visual_video_allocate_buffer (scalevid);

	sdlvid = visual_video_new ();
	visual_video_set_depth (sdlvid, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (sdlvid, screen->w, screen->h);
	visual_video_set_pitch (sdlvid, screen->pitch);
	visual_video_set_buffer (sdlvid, scrbuf);
	
	input = visual_input_new ("xmms2");
	visual_input_realize (input);

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
        
	visual_time_get (&start);
	
	while (1) {
		visual_input_run (input);
		visual_actor_run (actor, input->audio);
	
		/* place on screen */
//		visual_video_blit_overlay (sdlvid, video, 0, 0, FALSE);
	
		if (sxsize < 0)
			sxsize = 0;

		if (sysize < 0)
			sysize = 0;
		
		if (sxsize != scalevid->width || sysize != scalevid->height) {
			visual_video_set_dimension (scalevid, sxsize, sysize);
			visual_video_allocate_buffer (scalevid);
		}

		visual_video_depth_transform (video32, video);


//		visual_video_alpha_fill (sdlvid, 0);
		
//		visual_video_alpha_fill (video32, alpha);
//		visual_video_alpha_color (video32, 0, 0, 0, 255);
	

		do_alpha (video32, alpha);
		visual_video_scale (scalevid, video32, interpol);

		
		visual_video_blit_overlay (sdlvid, actvid, 0, 0, FALSE);
		visual_video_blit_overlay (sdlvid, scalevid, xoff, yoff, TRUE);

		sdl_draw_buf ();
		frames++;
		
		while (SDL_PollEvent (&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_F11:
							SDL_WM_ToggleFullScreen (screen);
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

						case SDLK_i:
							if (interpol == VISUAL_VIDEO_SCALE_NEAREST)
								interpol = VISUAL_VIDEO_SCALE_BILINEAR;
							else
								interpol = VISUAL_VIDEO_SCALE_NEAREST;
							
							break;

						case SDLK_o:
							alpha -= 8;
							if (alpha < 0)
								alpha = 0;

							break;

						case SDLK_p:
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
	visual_time_get (&end);
	
	VisTime diff;

	visual_time_difference (&diff, &start, &end);


	printf ("Ran: %d:%d, drawn %d frames\n",
			diff.tv_sec, diff.tv_usec, frames);

	SDL_Quit ();
}

