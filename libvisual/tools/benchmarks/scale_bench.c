#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES		500
#define DEPTH		VISUAL_VIDEO_DEPTH_32BIT
#define INTERPOL	VISUAL_VIDEO_SCALE_BILINEAR

int main (int argc, char **argv)
{
	VisVideo *dest, *src;
	int i;

	visual_init (&argc, &argv);

	dest = visual_video_new ();
	visual_video_set_depth (dest, DEPTH);
	visual_video_set_dimension (dest, 640, 400);
	visual_video_allocate_buffer (dest);

	src = visual_video_new ();
	visual_video_set_depth (src, DEPTH);
	visual_video_set_dimension (src, 320, 200);
	visual_video_allocate_buffer (src);

	for (i = 0; i < TIMES; i++)
		visual_video_scale (dest, src, INTERPOL);

	printf ("Scale bench overlay %d times, depth %d, interpol %d\n", TIMES,
			DEPTH, INTERPOL);

	return EXIT_SUCCESS;
}
