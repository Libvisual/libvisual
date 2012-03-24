/* When using the sdl driver and Xgl, sdl will show a nice empty window. Be sure to set the:
 * XLIB_SKIP_ARGB_VISUALS environment variable to 1
 */

#include <libvisual/libvisual.h>

#include <stdlib.h>

#include "display.h"
#include "sdldriver.h"
#include "x11driver.h"
#include "glxdriver.h"

int main (int argc, char **argv)
{
	SADisplay *display;
	VisVideo *video;

    VisBin *bin;
	VisActor *actor;
    VisInput *input;
    VisMorph *morph;
	VisEventQueue *localqueue;
    VisParamContainer *params;
    VisParamEntry *param;
	VisVideoAttributeOptions *vidoptions;

	int running = TRUE;
	int fullscreen = FALSE;
	int visible = TRUE;
    int depthflag;
	int depth;

	visual_init (&argc, &argv);

    visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);


    bin = visual_bin_new();

    actor = visual_actor_new("lv_scope");
<<<<<<< HEAD
    input = visual_input_new("alsa");
=======
    input = visual_input_new("debug");
>>>>>>> 2679f3550822841d3b481655ab072c8abaa53188
    visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_MORPH);

    depthflag = visual_actor_get_supported_depth(actor);
    if(depthflag == VISUAL_VIDEO_DEPTH_GL)
        visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
    else
    {
        depth = visual_video_depth_get_highest(depthflag);
        if((bin->depthflag & depth) > 0)
            visual_bin_set_depth(bin, depth);
        else
            visual_bin_set_depth(bin, visual_video_depth_get_highest_nogl(bin->depthflag));
    }
    bin->depthforcedmain = bin->depth;

    //depth = visual_video_depth_get_highest_nogl(depthflag);
	vidoptions = visual_actor_get_video_attribute_options (actor);

	display = display_new (sdl_driver_new ());

	display_create (display, depth, vidoptions, 320, 200, TRUE);

	video = display_get_video (display);

    visual_bin_connect(bin, actor, input);
    visual_bin_set_video(bin, video);
    visual_bin_realize(bin);
    visual_bin_sync(bin, FALSE);
    visual_bin_depth_changed(bin);

/*
    params = visual_plugin_get_params(bin->input->plugin);

    param = visual_param_container_get(params, "songinfo");

    if(param != NULL)
        visual_param_entry_set_object(param, 
            VISUAL_OBJECT(visual_actor_get_songinfo(bin->actor)));
*/

    //params = visual_plugin_get_params(actor->plugin);

    //string = visual_string_new_with_value("filename");
    //param = visual_param_container_get(params, string);
    //visual_param_entry_set_string(param, "/home/starlon/Projects/libvisual-svn/trunk/libvisual-avs/testpresets/ring10.avs");

	localqueue = visual_event_queue_new ();


	while (running) {
		VisEventQueue *pluginqueue;
		VisEvent *ev;

		/* Handle all events */
		display_drain_events (display, localqueue);

		pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (bin->actor));
		while (visual_event_queue_poll_by_reference (localqueue, &ev)) {

			if (ev->type != VISUAL_EVENT_RESIZE)
				visual_event_queue_add (pluginqueue, ev);

			switch (ev->type) {
                case VISUAL_EVENT_PARAM:
                    break;
				case VISUAL_EVENT_RESIZE:
/*
					video = display_get_video (display);
					visual_bin_set_video (bin, video);
                    visual_bin_depth_changed(bin);
*/
					break;

				case VISUAL_EVENT_MOUSEMOTION:
					break;

				case VISUAL_EVENT_MOUSEBUTTONDOWN:
                    actor = visual_bin_get_actor(bin);

                    const char *morph_name = visual_morph_get_next_by_name(0);

                    const char *actor_name = visual_actor_get_next_by_name(actor->plugin->info->name);
                    if(!actor_name)
                        actor_name = visual_actor_get_next_by_name(0);

                    visual_bin_set_morph_by_name(bin, (char *)morph_name);
                    visual_bin_switch_actor_by_name(bin, (char *)actor_name);
                    actor = visual_bin_get_actor(bin);
                    depthflag = visual_actor_get_supported_depth(actor);
                    if(depthflag == VISUAL_VIDEO_DEPTH_GL)
                        visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
                    else
                    {
                        depth = visual_video_depth_get_highest(depthflag);
                        if((bin->depthflag & depth) > 0)
                            visual_bin_set_depth(bin, depth);
                        else
                            visual_bin_set_depth(bin, visual_video_depth_get_highest_nogl(bin->depthflag));
                    }
                    bin->depthforcedmain = bin->depth;

					break;

				case VISUAL_EVENT_MOUSEBUTTONUP:
					break;

				case VISUAL_EVENT_KEYDOWN:
					switch (ev->event.keyboard.keysym.sym) {
						case VKEY_ESCAPE:
							running = FALSE;
							break;

						case VKEY_TAB:
#if 0
							fullscreen = !fullscreen;

							display_set_fullscreen (display, fullscreen, TRUE);

							/* Resync video */
							video = display_get_video (display);
							visual_actor_set_video (actor, video);

							visual_actor_video_negotiate (actor, depth, FALSE, FALSE);
#endif
							break;

						default:
							printf ("key: %c\n", ev->event.keyboard.keysym.sym);
							break;
					}

					break;

				case VISUAL_EVENT_KEYUP:
					break;

				case VISUAL_EVENT_QUIT:
					running = FALSE;
					break;

				case VISUAL_EVENT_VISIBILITY:
					visible = ev->event.visibility.is_visible;
					break;

				default:
					break;
			}
		}

        if(visual_bin_depth_changed(bin)) 
        {
/*
            int w = video->width;
            int h = video->width;
        	vidoptions = visual_actor_get_video_attribute_options (bin->actor);
        	display_create (display, depth, vidoptions, w, h, TRUE);
            visual_object_unref(VISUAL_OBJECT(video));
			video = display_get_video (display);
			visual_bin_set_video (bin, video);
*/
            visual_bin_sync(bin, TRUE);
        }
		/* Do a run cycle */

        if(!visible)
            continue;

		display_lock (display);
		visual_bin_run (bin);
		display_unlock (display);

		display_update_all (display);

		display_fps_limit (display, 30);
	}

	/* Termination procedure */
	display_set_fullscreen (display, FALSE, TRUE);
	display_close (display);

        
	visual_quit ();
    
	printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));

	return 0;
}
