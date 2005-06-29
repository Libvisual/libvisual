#include <unistd.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <libvisual/libvisual.h>


SDL_Surface *screen;
SDL_Color colors[256];
unsigned char *scrbuf;
int bpp;

VisActor *actor;
VisActor *actor2;
VisActor *actor3;
VisVideo *video;
VisVideo *video2;
VisVideo *video3;
VisVideo *sdlvid;
VisVideo *scalevid;
VisPalette *pal;
VisInput *input;

static float bdd = 0;
static float bdd2 = 0;
static float add3 = 0;
static float add6 = 0;
static float add7 = 0;
static float add8 = 0;

void sdl_fullscreen_toggle ();
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);
void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf ();

static int compfunc (VisVideo *dest, VisVideo *src);

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

static int compfunc4 (VisVideo *dest, VisVideo *src)
{
	static float o = 0;
	int i, j;
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;
	uint8_t alpha = 128;

	float sin = 0;
	float add = 0;
	float add2 = 0;
	float add4 = 0;
	float add5 = 0;

	
	add += bdd;
	add2 += bdd2;

	bdd2 += cos (add8) / 2;
	add8 += 0.1;

	for (i = 0; i < src->height; i++) {

		add2 += (cos (add4) / ((cos (add7) * 12) + 15));
		add = bdd2;


		for (j = 0; j < src->width; j++) {

			alpha = ((cos(add) * (cos(add2) * 62)) + ((cos(add5) * 20) + (cos(add6) * 40) + 128));
			add += 0.06;
			add5 += 0.02;


			*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
			*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
			*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}
		add4 += 0.1;


		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	add3 += 0.01;
	add6 += 0.05;
	add7 += 0.07;

	return VISUAL_OK;
}

static int compfunc3 (VisVideo *dest, VisVideo *src)
{
	static float o = 0;
	static float p = 0;
	int i, j;
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		alpha = (1 + sin(o)) * 128;

		o += 0.0003;

		for (j = 0; j < src->width; j++) {

			*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
			*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
			*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int compfunc2 (VisVideo *dest, VisVideo *src)
{
	int i;
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;

	for (i = 0; i < src->height; i++) {
		if (i % 2)
			visual_mem_copy (destbuf, srcbuf, src->pitch);

		destbuf += dest->pitch;
		srcbuf += src->pitch;
	}

	return VISUAL_OK;
}

static int compfunc (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((0 * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((alpha * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((*destbuf * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

/* Main stuff */
int main (int argc, char *argv[])
{
	int width = 500, height = 300;
	int i, j;
	int freeze = 0;
	int depthflag = 0;
	int alpha = 128;
	int xoff = 0, yoff = 0;
	int sxsize = 300;
	int sysize = 300;
	int interpol = VISUAL_VIDEO_SCALE_NEAREST;

	bpp = 4;
	sdl_init (width, height);

	scrbuf = malloc (screen->pitch * screen->h);
	memset (scrbuf, 0, screen->pitch * screen->h);

	SDL_Event event;

	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
	visual_init (&argc, &argv);
	
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("G-Force");

	visual_actor_realize (actor);

	video = visual_video_new ();

	int blahblahdepth = VISUAL_VIDEO_DEPTH_32BIT;
	visual_actor_set_video (actor, video);
	visual_video_set_depth (video, blahblahdepth);
	visual_video_set_dimension (video, width, height);
	visual_video_allocate_buffer (video);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	/* Second actor */
	if (argc > 2)
		actor2 = visual_actor_new (argv[2]);
	else
		actor2 = visual_actor_new ("oinksie");

	visual_actor_realize (actor2);

	video2 = visual_video_new ();
	visual_actor_set_video (actor2, video2);
	visual_video_set_depth (video2, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (video2, width, height);
	visual_video_allocate_buffer (video2);

	visual_video_composite_set_type (video2, VISUAL_VIDEO_COMPOSITE_TYPE_CUSTOM);
	visual_video_composite_set_function (video2, compfunc3);

	visual_actor_video_negotiate (actor2, 0, FALSE, FALSE);

	/* Third actor */
	if (argc > 3)
		actor3 = visual_actor_new (argv[3]);
	else
		actor3 = visual_actor_new ("corona");

	visual_actor_realize (actor3);

	video3 = visual_video_new ();
	visual_actor_set_video (actor3, video3);
	visual_video_set_depth (video3, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (video3, width, height);
	visual_video_allocate_buffer (video3);

	visual_video_composite_set_type (video3, VISUAL_VIDEO_COMPOSITE_TYPE_CUSTOM);
	visual_video_composite_set_function (video3, compfunc);

	visual_actor_video_negotiate (actor3, 0, FALSE, FALSE);


	/* done. */
	scalevid = visual_video_new ();
	visual_video_set_depth (scalevid, blahblahdepth);
	visual_video_set_dimension (scalevid, sxsize, sysize);
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
		visual_actor_run (actor2, input->audio);
		visual_actor_run (actor3, input->audio);
	
		visual_video_blit_overlay (sdlvid, video, 0, 0, FALSE);

		visual_video_blit_overlay (sdlvid, video2, 0, 0, TRUE);
		
		visual_video_blit_overlay (sdlvid, video3, 0, 0, TRUE);

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

						case SDLK_i:

							if (interpol == VISUAL_VIDEO_SCALE_NEAREST)
								interpol = VISUAL_VIDEO_SCALE_BILINEAR;
							else
								interpol = VISUAL_VIDEO_SCALE_NEAREST;

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

