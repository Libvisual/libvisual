#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv_display.h"

#include "GL/gl.h"

#define NULL_OUTPUT 0

static char actorname[100] = "oinksie";
static char actorname2[100] = "lv_gltest";

int main(int argc, char **argv)
{
	LvdDriver *drv, *drv2;
	Lvd *v, *v2;
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
	drv2 = lvdisplay_driver_create ("glx", "glx_new");
#endif

	if ((drv == NULL) || (drv2 == NULL)){
		fprintf (stderr, "failed to load driver\n");
		exit (1);
	}

	v = lvdisplay_initialize(drv);
	v2 = lvdisplay_initialize(drv2);
	if ((v == NULL) || (v2 == NULL)){
		fprintf (stderr, "failed to initialize vo\n");
		exit (1);
	}

	if (argc > 1){
		strcpy(actorname, argv[1]);
		strcpy(actorname2, argv[1]);
	}
	if (argc > 2){
		strcpy(actorname2, argv[2]);
	}

	VisBin *bin = lvdisplay_visual_get_bin(v);
	VisBin *bin2 = lvdisplay_visual_get_bin(v2);

	VisInput *input = visual_input_new("alsa");
	visual_object_ref (VISUAL_OBJECT (input)); /* Might be nicer to increase ref when an object is added to a VisBin... */

	VisActor *actor = visual_actor_new(actorname);
	VisActor *actor2 = visual_actor_new(actorname2);

	visual_bin_connect(bin, actor, input);
	visual_bin_connect(bin2, actor2, input);

	if (lvdisplay_realize(v) || lvdisplay_realize(v2)){
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
			/*fprintf(stderr, "got event! hehe... gonna handle it!\n");*/
			switch (event.type){
			case VISUAL_EVENT_QUIT:
				/*fprintf(stderr, "Quit?!? Oh NOoooo!!!...\n");*/
				quit_flag = 1;
				break;

			case VISUAL_EVENT_KEYDOWN:{
				const char *p;
				int areload = 0;

				/*fprintf(stderr, "KEYDOWN: '%c'=%d, %x\n",
						event.keyboard.keysym.sym,
						event.keyboard.keysym.sym,
						event.keyboard.keysym.mod);*/

				switch (event.keyboard.keysym.sym){
					case VKEY_a:
						p = visual_actor_get_next_by_name(actorname);
						if (p){
							strcpy(actorname, p);
							areload = 1;
						}
						break;
					case VKEY_s:
						p = visual_actor_get_prev_by_name(actorname);
						if (p){
							strcpy(actorname, p);
							areload = 1;
						}
						break;
					default:
						break;
				}
				if (areload){
					visual_object_unref(VISUAL_OBJECT(actor));
					actor = visual_actor_new(actorname);
					visual_bin_connect(bin, actor, input);
					lvdisplay_realize(v);
				}

				break;
			}
			case VISUAL_EVENT_KEYUP:
				/*fprintf(stderr, "KEYUP: '%c'=%d, %x\n",
						event.keyboard.keysym.sym,
						event.keyboard.keysym.sym,
						event.keyboard.keysym.mod);*/
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				/*fprintf(stderr, "WOW! it's moving! %dx%d\n",
					event.mousemotion.x, event.mousemotion.y);*/
				break;
			case VISUAL_EVENT_MOUSEBUTTONDOWN:
				/*fprintf(stderr, "MOUSEBUTTONDOWN %d %dx%d\n",
					event.mousebutton.button,
					event.mousebutton.x, event.mousebutton.y);*/
				break;
			case VISUAL_EVENT_MOUSEBUTTONUP:
				/*fprintf(stderr, "MOUSEBUTTONUP %d %dx%d\n",
					event.mousebutton.button,
					event.mousebutton.x, event.mousebutton.y);*/
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

		while (lvdisplay_poll_event(v2, &event)){
			switch (event.type){
			case VISUAL_EVENT_KEYDOWN:{
				const char *p;
				int areload = 0;

				/*fprintf(stderr, "KEYDOWN: '%c'=%d, %x\n",
						event.keyboard.keysym.sym,
						event.keyboard.keysym.sym,
						event.keyboard.keysym.mod);*/

				switch (event.keyboard.keysym.sym){
					case VKEY_a:
						p = visual_actor_get_next_by_name(actorname2);
						if (p){
							strcpy(actorname2, p);
							areload = 1;
						}
						break;
					case VKEY_s:
						p = visual_actor_get_prev_by_name(actorname2);
						if (p){
							strcpy(actorname2, p);
							areload = 1;
						}
						break;
					default:
						break;
				}
				if (areload){
					visual_object_unref(VISUAL_OBJECT(actor));
					actor2 = visual_actor_new(actorname2);
					visual_bin_connect(bin2, actor2, input);
					lvdisplay_realize(v2);
				}

				break;
			}

			case VISUAL_EVENT_RESIZE:
				fprintf(stderr, "resized: %dx%d\n", event.resize.width, event.resize.height);
				visual_bin_sync(bin2, FALSE);
				visual_actor_video_negotiate (actor2, 0, FALSE, FALSE);
				break;
			}
		}

		usleep(5);

		lvdisplay_run(v);
		lvdisplay_run(v2);
	}

	/* cleanup...
	 */

	visual_object_unref(VISUAL_OBJECT(bin));
	visual_object_unref(VISUAL_OBJECT(bin2));

	visual_object_unref(VISUAL_OBJECT(v));
	visual_object_unref(VISUAL_OBJECT(v2));

	visual_object_unref(VISUAL_OBJECT(drv));
	visual_object_unref(VISUAL_OBJECT(drv2));

	visual_quit ();

	return 0;
}



