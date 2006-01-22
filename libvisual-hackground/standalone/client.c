#include <libvisual/libvisual.h>

#include "display.h"
#include "sdldriver.h"

int main (int argc, char **argv)
{
	SADisplay *display;
	VisVideo *video;

	VisInput *input;
	VisActor *actor;

	visual_init (&argc, &argv);

	display = display_new (sdl_driver_new ());

	display_create (display, VISUAL_VIDEO_DEPTH_32BIT, 320, 200);

	video = display_get_video (display);

	/* Libvisual stuff */
	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("gforce");

	visual_actor_realize (actor);
        visual_actor_set_video (actor, video);
	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	if (argc > 2)
		input = visual_input_new (argv[2]);
	else
		input = visual_input_new ("alsa");

	visual_input_realize (input);

	while (TRUE) {
		visual_input_run (input);

		display_lock (display);
		visual_actor_run (actor, input->audio);
		display_unlock (display);

		display_update_all (display);
	}

	return 0;
}
