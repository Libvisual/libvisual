#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv_display.h"

#include "GL/gl.h"

int main(int argc, char **argv)
{
	LvdDriver *drv;
	Lvd *v;
	VisEvent event;
	int quit_flag = 0;

	visual_init_path_add ("../drivers/glx/.libs");
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

	drv = lvdisplay_driver_create ("glx", "glx_new");
	if (drv == NULL){
		fprintf (stderr, "failed to load driver\n");
		exit (1);
	}

	v = lvdisplay_initialize(drv);
	if (v == NULL){
		fprintf (stderr, "failed to initialize vo\n");
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

	/* GL setup... */
	glViewport(0,0,320,240);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0, 1.0);

	/* main loop */
	while (!quit_flag){
		if (lvdisplay_poll_event(v, &event)){
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
				/* XXX */
				fprintf(stderr, "MOUSEBUTTONDOWN\n");
				break;
			case VISUAL_EVENT_MOUSEBUTTONUP:
				/* XXX */
				fprintf(stderr, "MOUSEBUTTONUP\n");
				break;

			case VISUAL_EVENT_VISIBILITY:
				if (event.visibility.is_visible){
					fprintf(stderr, "Visible again!\n");
				} else {
					fprintf(stderr, "Cool!.. I'm hidden!\n");
				}
				break;
			}
		}

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f( -0.2f, -0.2f);
		glVertex2f(  0.2f, -0.2f);
		glVertex2f( -0.5f,  0.5f);
		glVertex2f(  0.2f,  0.2f);
		glEnd();

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



