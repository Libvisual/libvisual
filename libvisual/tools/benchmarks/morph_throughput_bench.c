#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define DEPTH		VISUAL_VIDEO_DEPTH_32BIT
#define TIMES		500

int main (int argc, char **argv)
{
	float rate = 0.0;
	VisMorph *morph;
	VisAudio audio;
	VisVideo *dest, *src1, *src2;
	int i;

	visual_init (&argc, &argv);

	if (argc < 2)
		morph = visual_morph_new ("alphablend");
	else
		morph = visual_morph_new (argv[1]);

	visual_morph_realize (morph);

	dest = visual_video_new ();

	visual_video_set_depth (dest, DEPTH);
	visual_video_set_dimension (dest, 640, 400);
	visual_video_allocate_buffer (dest);

	src1 = visual_video_new ();
	src2 = visual_video_new ();

	visual_video_clone (src1, dest);
	visual_video_clone (src2, dest);

	visual_video_allocate_buffer (src1);
	visual_video_allocate_buffer (src2);

	visual_morph_set_video (morph, dest);
	for (i = 0; i < TIMES; i++) {
		visual_morph_set_rate (morph, rate);
		visual_morph_run (morph, &audio, src1, src2);

		rate += 0.1;

		if (rate > 1.0)
			rate = 0.0;
	}

	printf ("Morph throughput bench %d times depthBPP %d morph: %s\n", TIMES, dest->bpp,
			(visual_plugin_get_info (visual_morph_get_plugin (morph)))->plugname);

	return EXIT_SUCCESS;
}
