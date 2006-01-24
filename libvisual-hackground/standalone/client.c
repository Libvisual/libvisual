#include <libvisual/libvisual.h>

#include <stdlib.h>

#include "display.h"
#include "sdldriver.h"

int main (int argc, char **argv)
{
	SADisplay *display;
	VisVideo *video;

	VisInput *input;
	VisActor *actor;
	VisEventQueue *localqueue;

	int running = TRUE;
	int fullscreen = FALSE;

	int depth;

	visual_init (&argc, &argv);

	display = display_new (sdl_driver_new ());

	/* Libvisual stuff */
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("gforce");


	if (argc > 3) {
		depth = visual_video_depth_enum_from_value (atoi (argv[3]));
	} else
		depth = visual_video_depth_get_highest (visual_actor_get_supported_depth (actor));

	display_create (display, depth, 320, 200, TRUE);

	visual_actor_realize (actor);

	video = display_get_video (display);

        visual_actor_set_video (actor, video);
	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	if (argc > 2)
		input = visual_input_new (argv[2]);
	else
		input = visual_input_new ("alsa");

	visual_input_realize (input);

	localqueue = visual_event_queue_new ();

	while (running) {
		VisEventQueue *pluginqueue;
		VisEvent *ev;

		/* Handle all events */
		display_drain_events (display, localqueue);

		pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (actor));
		while (visual_event_queue_poll_by_reference (localqueue, &ev)) {

			if (ev->type != VISUAL_EVENT_RESIZE)
				visual_event_queue_add (pluginqueue, ev);

			switch (ev->type) {
				case VISUAL_EVENT_RESIZE:
					video = display_get_video (display);
					visual_actor_set_video (actor, video);

					visual_actor_video_negotiate (actor, depth, FALSE, FALSE);
					break;

				case VISUAL_EVENT_MOUSEMOTION:
					break;

				case VISUAL_EVENT_MOUSEBUTTONDOWN:
				case VISUAL_EVENT_MOUSEBUTTONUP:
					break;

				case VISUAL_EVENT_KEYDOWN:
					switch (ev->event.keyboard.keysym.sym) {
						case VKEY_ESCAPE:
							running = FALSE;
							break;

						case VKEY_TAB:
							fullscreen = !fullscreen;

							display_set_fullscreen (display, fullscreen, TRUE);

							/* Resync video */
							video = display_get_video (display);
							visual_actor_set_video (actor, video);

							visual_actor_video_negotiate (actor, depth, FALSE, FALSE);

							break;

						default:
							break;
					}

					break;

				case VISUAL_EVENT_KEYUP:
					break;

				default:
					break;
			}
		}

		/* Do a run cycle */
		visual_input_run (input);

		display_lock (display);
		visual_actor_run (actor, input->audio);
		display_unlock (display);

		display_update_all (display);

		display_fps_limit (display, 30);
	}

	/* Termination procedure */
	display_set_fullscreen (display, FALSE, TRUE);

	printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));

	return 0;
}
