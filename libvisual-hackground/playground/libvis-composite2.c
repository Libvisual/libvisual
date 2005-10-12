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
VisVideo *bmpvideo;
VisVideo *sbmpvideo;
VisVideo *rbmpvideo;
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
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
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

static int compfunc5 (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = 0;

	for (i = 0; i < src->height; i++) {

		for (j = 0; j < src->width; j++) {
			alpha = 0;

			if (((*destbuf + *(destbuf + 1) + *(destbuf + 2)) / 3) < 128)
				alpha = 255;

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

static int compfunc3 (VisVideo *dest, VisVideo *src)
{
	static float o = 0;
	static float p = 0;
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
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
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);

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
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
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

/* for the argv shizzle, this is all hacky */
int edge1 = 190;
int edge2 = 260;
int smoothing = 2;

static int fakechroma (VisVideo *dest, VisVideo *src)
{
	VisVideo *video;
	VisPalette *pal;
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha;

//	int edge1 = 190;
//	int edge2 = 260;
	float threshold = 0.50;
	float satthresh = 0.20;

	int table[360];

	float percentage = (edge2 - edge1) / 100.0;
	int noise = 15;
	int alphaadd = 255 / noise;

	/* FIXME won't work for the wrap at 360, think about this */
	/* Better edging */
	for (i = 0; i < 360; i++) {
		if (i > edge1 && i < edge2) {
			if (i * percentage < noise + edge1)
				table[i] = 128;
			if (i * percentage > (edge2 - noise))
				table[i] = 128;
			else
				table[i] = 0;
		} else {
			table[i] = 255;
		}
	}

//	video = visual_video_new_with_buffer (src->width, src->height, VISUAL_VIDEO_DEPTH_8BIT);

//	pal = visual_palette_new (256);
//	visual_video_set_palette (src, pal);
//	visual_video_set_palette (video, pal);

//	visual_video_blit_overlay (video, src, 0, 0, FALSE);
//	visual_video_blit_overlay (dest, video, 0, 0, FALSE);

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			float h, s, v;
			VisColor col;
			col.r = srcbuf[2];
			col.g = srcbuf[1];
			col.b = srcbuf[0];

			visual_color_to_hsv (&col, &h, &s, &v);

			if (v > threshold && s > satthresh)
				alpha = table[(int) h];
			else
				alpha = 255;

			*(destbuf + 3) = alpha;

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}


	int a;

	/* Very sub optimal */
	for (a = 0; a < smoothing; a++) {
	destbuf = visual_video_get_pixels (dest);
	destbuf += 3;
	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			int alphasum;

			alphasum =
				(destbuf[dest->pitch] +
				destbuf[-4] +
				destbuf[4] +
				destbuf[-dest->pitch]) / 4;

			if (alphasum > 250)
				alphasum = 0xff;
//			alphasum = 0xff;

			destbuf[0] = alphasum;

			destbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
	}}



	destbuf = visual_video_get_pixels (dest);
	srcbuf = visual_video_get_pixels (src);
	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			alpha = *(destbuf + 3);
			*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
			*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
			*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

//	visual_object_unref (VISUAL_OBJECT (video));
//	visual_object_unref (VISUAL_OBJECT (pal));

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
		actor = visual_actor_new ("gforce");

	visual_actor_realize (actor);

	video = visual_video_new ();

	int blahblahdepth = VISUAL_VIDEO_DEPTH_32BIT;
	visual_actor_set_video (actor, video);
	visual_video_set_depth (video, blahblahdepth);
	visual_video_set_dimension (video, width, height);
	visual_video_allocate_buffer (video);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

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

	if (argc > 2)
		bmpvideo = visual_bitmap_load_new_video (argv[2]);
	else
		bmpvideo = visual_bitmap_load_new_video ("boom.bmp");

	sbmpvideo = visual_video_scale_new (bmpvideo, width, height, VISUAL_VIDEO_SCALE_BILINEAR);
	visual_video_composite_set_type (sbmpvideo, VISUAL_VIDEO_COMPOSITE_TYPE_CUSTOM);
	visual_video_composite_set_function (sbmpvideo, fakechroma);

	rbmpvideo = visual_video_new ();
	visual_video_set_depth (rbmpvideo, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (rbmpvideo, sbmpvideo->width, sbmpvideo->height);
	visual_video_allocate_buffer (rbmpvideo);
	visual_video_blit_overlay (rbmpvideo, sbmpvideo, 0, 0, TRUE);

	if (argc > 3)
		edge1 = atoi (argv[3]);

	if (argc > 4)
		edge2 = atoi (argv[4]);

	if (argc > 5)
		smoothing = atoi (argv[5]);

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	while (1) {
		visual_input_run (input);
		visual_actor_run (actor, input->audio);
	
		visual_video_blit_overlay (sdlvid, video, 0, 0, FALSE);
		visual_video_blit_overlay (sdlvid, sbmpvideo, 0, 0, TRUE);

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

