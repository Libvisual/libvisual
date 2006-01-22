#include <libvisual/libvisual.h>

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

	visual_actor_realize (actor);

//	depth = visual_video_depth_get_highest (visual_actor_get_plugin (actor)->depth);
	depth = VISUAL_VIDEO_DEPTH_32BIT;

	display_create (display, depth, 320, 200, TRUE);

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
		VisEvent ev;

		/* Handle all events */
		display_drain_events (display, localqueue);

		pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (actor));
		while (visual_event_queue_poll (localqueue, &ev)) {
			// FIXME need some extra api in VisEvent (copy from event, a poll by reference)
//			visual_event_queue_add (pluginqueue, &ev);

			switch (ev.type) {
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
					switch (ev.event.keyboard.keysym.sym) {
						case VKEY_ESCAPE:
							running = FALSE;
							break;

						case VKEY_TAB:
							fullscreen = !fullscreen;

							display_set_fullscreen (display, fullscreen);
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
	}

	/* Termination procedure */
	display_set_fullscreen (display, FALSE);

	return 0;
}
