#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES		500
#define DEPTH		VISUAL_VIDEO_DEPTH_32BIT

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
		visual_video_zoom_double (dest, src);

	printf ("Scale bench overlay %d times, depth %d\n", TIMES,
			DEPTH);
}

