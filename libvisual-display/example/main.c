/* Libvisual-display example - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id: main.c,v 1.19 2005-02-14 22:05:14 vitalyvb Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lvdisplay/lv_display.h"

static char actorname[100] = "testactor";
static char backend[100] = "glx";

int main(int argc, char **argv)
{
	int r;
	LvdDriver *drv;
	LvdDriver *drv_fs;
	Lvd *v;
	VisEvent event;
	int quit_flag = 0;
	int is_fullscreen = 0;

	visual_init_path_add ("plugins/.libs");
	visual_init_path_add ("../drivers/null/.libs");
	visual_init_path_add ("../drivers/glx/.libs");
	visual_init_path_add ("../drivers/x11/.libs");

	visual_init_path_add (PLUGINSDIR "/display");

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

	drv = lvdisplay_driver_create (backend, "x11_new");

	if (drv == NULL){
		fprintf (stderr, "failed to load driver\n");
		exit (1);
	}

	r = lvdisplay_driver_realize(drv);
	if (r){
		fprintf (stderr, "lvdisplay_driver_realize failed\n");
		exit (1);
	}

	{
		int params[100];
		int pc = 0;

		params[pc++] = LVD_SET_DEPTH;
		params[pc++] = VISUAL_VIDEO_DEPTH_32BIT;
//		params[pc++] = VISUAL_VIDEO_DEPTH_GL; // whatever.... lvdisplay_realize sets it again

		params[pc++] = LVD_SET_WIDTH;
		params[pc++] = 400;

		params[pc++] = LVD_SET_HEIGHT;
		params[pc++] = 300;

		params[pc++] = LVD_SET_VISIBLE;
		params[pc++] = 1;

		lvdisplay_driver_set_opts(drv, params, pc);
	}

	drv_fs = lvdisplay_driver_create (backend, "x11_fullscreen");

	if (drv_fs == NULL){
		fprintf (stderr, "failed to load fs driver\n");
		exit (1);
	}

	r = lvdisplay_driver_realize(drv_fs);
	if (r){
		fprintf (stderr, "lvdisplay_driver_realize failed for fs\n");
		exit (1);
	}

	{
		LvdVideoMode *vms;
		int cnt, i;
		
		if (lvdisplay_get_videomodes(drv_fs, &vms, &cnt) == 0){
			fprintf (stderr, "Available video modes:\n");
			for (i=0;i<cnt;i++){
				const char *ds[] = {"", " Doublescan"};
				const char *il[] = {"", " Interlaced"};
				fprintf (stderr, "\t%3d:  %dx%d @ %d Hz%s%s\n", i, vms[i].width, vms[i].height,
					vms[i].vfreq,
					ds[(vms[i].flags & LVD_VIDEOMODE_DOUBLESCAN) ? 1:0],
					il[(vms[i].flags & LVD_VIDEOMODE_INTERLACED) ? 1:0]);
			}
			fprintf (stderr, "warning: if it crashes try to change here\n");
			lvdisplay_set_videomode(drv_fs, &vms[5]); // XXX FIXME or not :)
			visual_mem_free(vms);
		}
	
	}

	{
		int params[100];
		int pc = 0;

		params[pc++] = LVD_SET_DEPTH;
		params[pc++] = VISUAL_VIDEO_DEPTH_32BIT;

		/* lvdisplay_set_videomode or these ones */
/*
		params[pc++] = LVD_SET_WIDTH; params[pc++] = 320;
		params[pc++] = LVD_SET_HEIGHT; params[pc++] = 240;
*/

		/* DO NOT set it visible! */
		/*params[pc++] = LVD_SET_VISIBLE;
		  params[pc++] = 1;*/

		lvdisplay_driver_set_opts(drv_fs, params, pc);
	}

	v = lvdisplay_initialize();
	if (v == NULL){
		fprintf (stderr, "failed to initialize v\n");
		exit (1);
	}

	r = lvdisplay_set_driver(v, drv);
	if (r){
		fprintf (stderr, "failed to set driver\n");
		exit (1);
	}

	if (argc > 1)
		strcpy(actorname, argv[1]);


	VisBin *bin = lvdisplay_visual_get_bin(v);

	VisInput *input = visual_input_new("alsa");
	visual_object_ref (VISUAL_OBJECT (input));

	VisActor *actor = visual_actor_new(actorname);

	visual_bin_connect(bin, actor, input);

	if ((r=lvdisplay_realize(v)) != 0){
		fprintf (stderr, "failed to realize vo %d\n", r);
		exit (1);
	}

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
					case VKEY_q:
						quit_flag = 1;
						break;
					case VKEY_f:

						is_fullscreen = !is_fullscreen;

						lvdisplay_driver_set_visible(drv, !is_fullscreen);
						lvdisplay_driver_set_visible(drv_fs, is_fullscreen);

						r = lvdisplay_set_driver(v, is_fullscreen?drv_fs:drv);
						if (r){
							fprintf (stderr, "failed to set driver\n");
							exit (1);
						}

						if ((r=lvdisplay_realize(v)) != 0){
							fprintf (stderr, "failed to realize vo %d\n", r);
							exit (1);
						}

						break;
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
			default:
				break;
			}
		}

		lvdisplay_run(v);
	}

	/* cleanup...
	 */

	visual_object_unref(VISUAL_OBJECT(bin));

	visual_object_unref(VISUAL_OBJECT(v));

	visual_object_unref(VISUAL_OBJECT(drv));
	visual_object_unref(VISUAL_OBJECT(drv_fs));

	visual_quit ();

	return 0;
}


