#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define FORCED_DEPTH	TRUE
#define DEPTH		VISUAL_VIDEO_DEPTH_32BIT
#define TIMES		500

int main (int argc, char **argv)
{
	VisActor *actor;
	VisAudio audio;
	VisVideo *dest;
	int i;

	visual_init (&argc, &argv);

	if (argc < 2)
		actor = visual_actor_new ("oinksie");
	else
		actor = visual_actor_new (argv[1]);

	visual_actor_realize (actor);

	dest = visual_video_new ();

#ifdef FORCED_DEPTH
	visual_video_set_depth (dest, DEPTH);
#else
	visual_video_set_depth (dest, VISUAL_PLUGIN_ACTOR (visual_actor_get_plugin (actor)->info->plugin)->depth);
#endif

	if (argc > 2)
		visual_video_set_depth (dest, visual_video_depth_enum_from_value (atoi (argv[2])));

	visual_video_set_dimension (dest, 640, 400);
	visual_video_allocate_buffer (dest);

	visual_actor_set_video (actor, dest);
	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	for (i = 0; i < TIMES; i++)
		visual_actor_run (actor, &audio);

	printf ("Actor throughput bench %d times depthBPP %d actor: %s\n", TIMES, dest->bpp,
			(visual_plugin_get_info (visual_actor_get_plugin (actor)))->plugname);

	return EXIT_SUCCESS;
}
