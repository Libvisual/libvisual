#include <libvisual/libvisual.h>

#include <stdlib.h>

#include "display.h"
#include "sdldriver.h"

int main (int argc, char **argv)
{
	SADisplay *display;
	VisVideo *video;
	VisVideo *overlay;

	VisInput *input;
	VisActor *actor;
	VisEventQueue *localqueue;
	VisVideoAttributeOptions *vidoptions;

	int running = TRUE;
	int fullscreen = FALSE;
	int visible = TRUE;

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

	vidoptions = visual_actor_get_video_attribute_options (actor);

	display_create (display, depth, vidoptions, 520, 520, TRUE);

	visual_actor_realize (actor);

	overlay = display_get_video (display);

	video = visual_video_new ();
	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, 320, 200);
	visual_video_set_pitch (video, overlay->pitch);
	visual_video_set_buffer (video, visual_video_get_pixels (overlay));

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
					overlay = display_get_video (display);
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
							overlay= display_get_video (display);

							break;

						default:
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

		if (visible == FALSE) {
			visual_input_run (input);

			visual_time_usleep (10000);

			continue;
		}

		/* Do a run cycle */
		visual_input_run (input);

		display_lock (display);
		visual_actor_run (actor, input->audio);

		overlay->pal = visual_actor_get_palette (actor);
		visual_video_blit_overlay (overlay, video, 0, 0, FALSE);

		display_unlock (display);

		display_update_all (display);

		display_fps_limit (display, 30);
	}

	/* Termination procedure */
	display_set_fullscreen (display, FALSE, TRUE);

	printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));

	return 0;
}
