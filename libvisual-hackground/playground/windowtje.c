#include <unistd.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include <string.h>

SDL_Surface *screen;
SDL_Color colors[256];
int bpp;

void sdl_init (int width, int height);
void sdl_create (int width, int height);

void line (int *buf, int color, int x0, int y0, int x1, int y1);

	/* Sdl stuff */
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

/* JA HELE VIEZE */
void line (int *buf, int color, int x0, int y0, int x1, int y1)
{
	register int dy = y1 - y0;
	register int dx = x1 - x0;
	register int stepx, stepy, stepy_;
	register int fraction;
	register int bp;
	register int x;
	register int y;
	int pbpp = screen->pitch / bpp;

	if (x0 < 0 || x0 > pbpp  - 1 || x1 < 0 || x1 > pbpp - 1||
			y0 < 0 || y0 > screen->h - 1 || y1 < 0 || y1 > screen->h - 1)
	{
		if (x0 < 0)
			x0 = 0;
		else if (x0 > pbpp - 1)
			x0 = pbpp - 1;

		if (x1 < 0)
			x1 = 0;
		else if (x1 > pbpp - 1)
			x1 = pbpp - 1;

		if (y0 < 0)
			y0 = 0;
		else if (y0 > screen->h - 1);
		y0 = screen->h - 1;

		if (y1 < 0)
			y1 = 0;
		else if (y1 > screen->h - 1);
		y1 = screen->h - 1;

		return; /* FIXME */
	}

	if (dy < 0)
	{
		dy = -dy;
		stepy_ = -pbpp;
		stepy = -1;
	}
	else
	{
		stepy = 1;
		stepy_ = pbpp;
	}

	if (dx < 0)
	{
		dx = -dx;
		stepx = -1;
	}
	else
	{
		stepx = 1;
	}

	dy <<= 1;
	dx <<= 1;

	bp = x0 + y0 * (pbpp);

	*(buf + bp) = color;

	x = x0;
	y = y0;
	if (dx > dy)
	{
		fraction = dy - (dx >> 1);
		while (x0 != x1)
		{
			if (fraction >= 0)
			{
				bp += stepy_;
				fraction -= dx;
			}
			x0 += stepx;
			bp += stepx;
			fraction += dy;

			buf[bp] = color;
		}
	}
	else
	{
		fraction = dx - (dy >> 1);
		while (y0 != y1)
		{
			if (fraction >= 0)
			{
				bp += stepx;
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			bp += stepy_;
			fraction += dx;

			buf[bp] = color;
		}
	}

}



/* Main stuff */
int main (int argc, char *argv[])
{
	unsigned char *str;

	bpp = 4;
	sdl_init (640, 400);

	str = (unsigned char *) screen->pixels;

	SDL_Event event;

	while (1) {
		memset (str, 0xff, 1000);

		line ((int *) str, 0xff00ff, 10, 10, 50, 50);
		
		SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
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

