#include <libvisual/libvisual.h>
#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>

SDL_Surface *screen;
SDL_Color colors[256];
int32_t *srcbuf;
VisVideoDepth depth = VISUAL_VIDEO_DEPTH_32BIT;
VisVideoScaleMethod interpol = VISUAL_VIDEO_SCALE_NEAREST;

VisInput *input;
VisActor *actor;
LV::VideoPtr actvid;
LV::VideoPtr video;
LV::VideoPtr video32_actor;
LV::VideoPtr video32_image;
LV::VideoPtr sdlvid;
LV::VideoPtr scalevid;
VisPalette *pal;

int n_tile_cols = 5;
int n_tile_rows = 5;

void sdl_fullscreen_toggle ();
void sdl_fullscreen_xy (int *x, int *y);
int sdl_fullscreen_set (int mode);
void sdl_size_request (int width, int height);
void sdl_init (int width, int height);
void sdl_create (int width, int height);
void sdl_draw_buf ();
void do_alpha (LV::Video *vid, uint8_t rate);

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

	free (srcbuf);
	srcbuf = (int32_t*)malloc (screen->pitch * screen->h);
	memset (srcbuf, 0, screen->pitch * screen->h);
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
	screen = SDL_SetVideoMode (width, height, visual_video_bpp_from_depth(depth) * 8, 0);
}

void sdl_draw_buf ()
{
	unsigned char *str = (unsigned char *) screen->pixels;

	memcpy (str, srcbuf, screen->pitch * screen->h);

	memset (srcbuf, 0, screen->pitch * screen->h);
	SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
}

void do_checkers(LV::VideoPtr const& destptr, LV::VideoPtr const& src1, LV::VideoPtr const& src2)
{
    static LV::Timer timer;
    static int flip = -1;

    if(flip == -1)
    {
        flip = 0;
        timer.reset();
        timer.start();
    }
    if(timer.elapsed().to_msecs() > 2000)
    {
        flip = !flip;
        timer.reset();
        timer.start();
    }

    LV::Color black = LV::Color::black();
    destptr->fill_color(black);

    unsigned int tile_width  = src1->get_width()  / n_tile_cols;
    unsigned int tile_height = src1->get_height() / n_tile_rows;

    LV::VideoPtr srcptr = flip % 2 == 0 ? src2 : src1;

    LV::Rect area(0, 0, tile_width, tile_height);

    area = LV::Rect::clip(destptr->get_extents(), area);

    destptr->blit(area, srcptr, area, false);

    //return;
    for(unsigned int row = 0, y = 0;
        y < (unsigned int)src1->get_height();
        row++, y += tile_height)
    {
        for(unsigned int col = 0, x = 0;
            x < (unsigned int)src2->get_width();
            col++, x += tile_width)
        {
            LV::VideoPtr srcptr = (row + col + flip) & 1 ? src1 : src2;

            LV::Rect area(x, y, tile_width, tile_height);

            area = LV::Rect::clip(destptr->get_extents(), area);

            destptr->blit(area, srcptr, area, false);
        }
    }
}

void do_alpha (LV::VideoPtr const& vid, uint8_t rate)
{
	int i;
	uint32_t *ptr = (uint32_t *)vid->get_pixels();
	union {
		uint32_t c32;
		uint8_t c8[4];
	} col;

	for (i = 0; i < vid->get_width() * vid->get_height(); i++) {
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
	int width = 512, height = 128;
	int alpha = 190;
	int xoff = 0, yoff = -90;
    int frames = 0;
	//VisTime start, end;


	sdl_init (width, height);

	SDL_Event event;

    visual_log_set_verbosity(VISUAL_LOG_DEBUG);
	visual_init (&argc, &argv);


	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("corona");

	visual_actor_realize (actor);

	VisVideo *tmpvid;

	if (argc > 2)
		tmpvid = visual_video_load_from_file (argv[2]);
	else
		tmpvid = visual_video_load_from_file ("images/bg.bmp");

    video = LV::Video::wrap(tmpvid->get_pixels(), false, tmpvid->get_width(), tmpvid->get_height(), tmpvid->get_depth());


    scalevid = LV::Video::create(screen->w, screen->h, video->get_depth());
    scalevid->scale(video, interpol);

    video32_image = LV::Video::create(screen->w, screen->h, depth);

    // Set this once.
    video32_image->convert_depth(scalevid);

    video32_actor = LV::Video::create(screen->w, screen->h, depth);


	srcbuf = (int32_t *)malloc (screen->pitch * screen->h);
	memset (srcbuf, 0, screen->pitch * screen->h);

    sdlvid = LV::Video::wrap(srcbuf, false, screen->w, screen->h, depth);

    actvid = LV::Video::create(screen->w, screen->h, visual_video_depth_get_highest(visual_actor_get_supported_depth (actor)));

	visual_actor_set_video (actor, actvid.get());

	visual_actor_video_negotiate (actor, VISUAL_VIDEO_DEPTH_NONE, FALSE, FALSE);

	input = visual_input_new ("debug");
	visual_input_realize (input);

	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    LV::Rect area(0, 0, screen->w, screen->h);
    video32_image->set_extents(area);
    video32_actor->set_extents(area);
    sdlvid->set_extents(area);

	//visual_time_get (&start);

	while (1) {
		visual_input_run (input);
		visual_actor_run (actor, input->audio);

        video32_actor->convert_depth(actvid);

        scalevid->scale(video, interpol);

        video32_image->convert_depth(scalevid);

        do_checkers(sdlvid, video32_image, video32_actor);

        //sdlvid->blit(video32_actor, 0, 0, true);

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
							//sysize -= 10;

							break;

						case SDLK_a:
							//sysize += 10;

							break;

						case SDLK_z:
							//sxsize -= 10;

							break;

						case SDLK_x:
							//sxsize += 10;

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

						default:
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
	//visual_time_get (&end);

/*
	VisTime diff;

	visual_time_difference (&diff, &start, &end);


	printf ("Ran: %d:%d, drawn %d frames\n",
			diff.tv_sec, diff.tv_usec, frames);
*/

	SDL_Quit ();

	return EXIT_SUCCESS;
}
