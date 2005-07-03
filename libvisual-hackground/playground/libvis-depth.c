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
VisVideo *palvid;
VisPalette *pal;
VisInput *input;

void sdl_fullscreen_toggle ();
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);
void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf ();
void draw_rgb_bars (VisVideo *video);

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

void draw_rgb_bars (VisVideo *video)
{
	int x, y;
	
	if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t *buf = visual_video_get_pixels (video);
		uint32_t col;
		
		for (y = 0; y < 150; y++) {

			if (y < 50)
				col = 0xff0000;
			else if (y < 100)
				col = 0x00ff00;
			else if (y < 150)
				col = 0x0000ff;

			for (x = 0; x < 100; x++) {
				buf[(y * video->pitch / 4) + x] = col;

			}
		}
	} else if (video->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t *buf = visual_video_get_pixels (video);
		uint32_t col;
		
		for (y = 0; y < 150; y++) {

			if (y < 50)
				col = 0xff0000;
			else if (y < 100)
				col = 0x00ff00;
			else if (y < 150)
				col = 0x0000ff;

			for (x = 0; x < 100 * 3; x += 3) {
				buf[(y * video->pitch) + x] = col & 0xff;
				buf[(y * video->pitch) + x + 1] = (col >> 8) & 0xff;
				buf[(y * video->pitch) + x + 2] = col >> 16;

			}
		}
	} else if (video->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t *buf = visual_video_get_pixels (video);
		uint32_t col;
		
		for (y = 0; y < 150; y++) {

			if (y < 50)
				col = 0xff0000;
			else if (y < 100)
				col = 0x00ff00;
			else if (y < 150)
				col = 0x0000ff;

			for (x = 0; x < 100; x++) {
				buf[(y * video->pitch / 2) + x] = (col & 0xff >> 3) | ((((col >> 8) & 0xff) >> 2) << 5) |
					(((col >> 16) >> 3) << 12);

			}
		}		
	} else if (video->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		uint8_t *buf = visual_video_get_pixels (video);
		uint32_t col;
		
		for (y = 0; y < 150; y++) {

			if (y < 50)
				col = 1;
			else if (y < 100)
				col = 2;
			else if (y < 150)
				col = 3;

			for (x = 0; x < 100; x++) {
				buf[(y * video->pitch) + x] = col;;

			}
		}

		video->pal->colors[1].r = 255;
		video->pal->colors[1].g = 0;
		video->pal->colors[1].b = 0;

		video->pal->colors[2].r = 0;
		video->pal->colors[2].g = 255;
		video->pal->colors[2].b = 0;

		video->pal->colors[3].r = 0;
		video->pal->colors[3].g = 0;
		video->pal->colors[3].b = 255;
	
	}


}

void draw_pal_debug (VisVideo *video, VisPalette *pal)
{
	VisPalette *pal2;
	int i, j;
	uint8_t *buf = (uint8_t *) visual_video_get_pixels (video);

	if (video->pal == NULL)
		video->pal = visual_palette_new (256);
	
	pal2 = video->pal;

	memset (buf, 0, video->size);

	visual_palette_copy (pal2, pal);

	pal2->colors[255].r = 255;
	pal2->colors[255].g = 255;
	pal2->colors[255].b = 255;

	pal2->colors[0].r = 0;
	pal2->colors[0].g = 0;
	pal2->colors[0].b = 0;

	for (i = 0; i < 256; i++) {
		for (j = 0; j < 20; j++) {
			buf[(video->pitch * j) + i] = i;
		}

		buf[(video->pitch * ((pal->colors[i].r / 4) + 20)) + i] = 255;
		buf[(video->pitch * ((pal->colors[i].g / 4) + 20 + 64)) + i] = 255;
		buf[(video->pitch * ((pal->colors[i].b / 4) + 20 + 128)) + i] = 255;
	}
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
	int interpol = VISUAL_VIDEO_SCALE_NEAREST;
	VisVideoDepth ds = VISUAL_VIDEO_DEPTH_32BIT, dt = VISUAL_VIDEO_DEPTH_8BIT;

	bpp = 4;
	sdl_init (width, height);

	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);

	SDL_Event event;

	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
	visual_init (&argc, &argv);
	
	if (argc > 2) {
		ds = visual_video_depth_enum_from_value(atoi(argv[1]));
		dt = visual_video_depth_enum_from_value(atoi(argv[2]));

	}

	actor = visual_actor_new ("oinksie");

	visual_actor_realize (actor);

	video = visual_video_new ();

	visual_actor_set_video (actor, video);
	visual_video_set_depth (video, ds);
	visual_video_set_dimension (video, width, height);
	visual_video_allocate_buffer (video);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	scalevid = visual_video_new ();
	visual_video_set_depth (scalevid, dt);
	visual_video_set_dimension (scalevid, video->width, video->height);
	visual_video_allocate_buffer (scalevid);

	palvid = visual_video_new ();
	visual_video_set_depth (palvid, VISUAL_VIDEO_DEPTH_8BIT);
	visual_video_set_dimension (palvid, 256, 400);
	visual_video_allocate_buffer (palvid);

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
	
		/* place on screen */
//		visual_video_blit_overlay (sdlvid, video, 0, 0, FALSE);
	
		visual_video_set_palette (scalevid, visual_actor_get_palette (actor));	

		draw_rgb_bars (video);
		
		visual_video_blit_overlay (scalevid, video, 0, 0, FALSE);

		visual_video_blit_overlay (sdlvid, scalevid, xoff, yoff, FALSE);

		draw_pal_debug (palvid, video->pal);

		visual_video_blit_overlay (sdlvid, palvid, sdlvid->width - 256, 0, FALSE);

		sdl_draw_buf ();
		
		usleep (5000);
		
		while (SDL_PollEvent (&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
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

