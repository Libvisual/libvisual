#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv_display.h"

#include "GL/gl.h"

#define NULL_OUTPUT 0

int main(int argc, char **argv)
{
	LvdDriver *drv;
	Lvd *v;
	VisEvent event;
	int quit_flag = 0;

#if NULL_OUTPUT
	visual_init_path_add ("../drivers/null/.libs");
#else
	visual_init_path_add ("../drivers/glx/.libs");
#endif
	visual_init (&argc, &argv);
	/* variants are:
	 *	glx, new
	 *	glx, xwindow
	 *	glx, gtk-widget
	 *	glx, fltk-widget
	 *	...
	 *	x11, new         x11 can't display gl plugs
	 *	x11, xwindow
	 *	...
	 *	win32, new       ??? directdraw? gl?
	 *	win32, window
	 *	...
	 *	sdl, new
	 *	sdl, surface
	 *	fb,	???          linux framebuffer
	 *
	 *	others?
	 */

#if NULL_OUTPUT
	drv = lvdisplay_driver_create ("null", "null");
#else
	drv = lvdisplay_driver_create ("glx", "glx_new");
#endif

	if (drv == NULL){
		fprintf (stderr, "failed to load driver\n");
		exit (1);
	}

	v = lvdisplay_initialize(drv);
	if (v == NULL){
		fprintf (stderr, "failed to initialize vo\n");
		exit (1);
	}

	VisBin *bin = lvdisplay_visual_get_bin(v);

	VisInput *input = visual_input_new("alsa");
	VisActor *actor = visual_actor_new(argc>1 ? argv[1] : "oinksie");

	visual_bin_connect(bin, actor, input);

	if (lvdisplay_realize(v)){
		fprintf (stderr, "failed to realize vo\n");
		exit (1);
	}


	/*  start visualization. plugin will run in a
	 *  separate thread. Alternative call can be
	 *  vo_run(v) - just process everything and
	 *  draw 1 frame
	 */

#if 0
	/* not implemented */
	vo_start(v);
	sleep(5);
	vo_stop(v);
#endif

	/* main loop */
	while (!quit_flag){
		while (lvdisplay_poll_event(v, &event)){
			fprintf(stderr, "got event! hehe... gonna handle it!\n");
			switch (event.type){
			case VISUAL_EVENT_QUIT:
				fprintf(stderr, "Quit?!? Oh NOoooo!!!...\n");
				quit_flag = 1;
				break;

			case VISUAL_EVENT_KEYDOWN:
				fprintf(stderr, "KEYDOWN: '%c'=%d, %x\n",
						event.keyboard.keysym.sym,
						event.keyboard.keysym.sym,
						event.keyboard.keysym.mod);
				break;
			case VISUAL_EVENT_KEYUP:
				fprintf(stderr, "KEYUP: '%c'=%d, %x\n",
						event.keyboard.keysym.sym,
						event.keyboard.keysym.sym,
						event.keyboard.keysym.mod);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				fprintf(stderr, "WOW! it's moving! %dx%d\n",
					event.mousemotion.x, event.mousemotion.y);
				break;
			case VISUAL_EVENT_MOUSEBUTTONDOWN:
				fprintf(stderr, "MOUSEBUTTONDOWN %d %dx%d\n",
					event.mousebutton.button,
					event.mousebutton.x, event.mousebutton.y);
				break;
			case VISUAL_EVENT_MOUSEBUTTONUP:
				fprintf(stderr, "MOUSEBUTTONUP %d %dx%d\n",
					event.mousebutton.button,
					event.mousebutton.x, event.mousebutton.y);
				break;

			case VISUAL_EVENT_VISIBILITY:
				if (event.visibility.is_visible){
					fprintf(stderr, "Visible again!\n");
				} else {
					fprintf(stderr, "Cool!.. I'm hidden!\n");
				}
				break;
			case VISUAL_EVENT_RESIZE:
				fprintf(stderr, "resized: %dx%d\n", event.resize.width, event.resize.height);
				visual_bin_sync(bin, FALSE);
				visual_actor_video_negotiate (actor, 0, FALSE, FALSE);
				break;
			}
		}

		usleep(5);

		lvdisplay_run(v);
	}

	/* cleanup...
	 */

	lvdisplay_finalize(v);
	lvdisplay_driver_delete(drv);

	visual_quit ();

	return 0;
}



