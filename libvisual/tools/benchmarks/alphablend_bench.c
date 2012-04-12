#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	500

int main (int argc, char **argv)
{
	VisVideo *dest, *src;
	int i;

	visual_init (&argc, &argv);

	dest = visual_video_new ();
	visual_video_set_depth (dest, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (dest, 640, 400);
	visual_video_allocate_buffer (dest);

	src = visual_video_new ();
	visual_video_copy_attrs (src, dest);
	visual_video_allocate_buffer (src);

	for (i = 0; i < TIMES; i++)
		visual_video_blit_overlay (dest, src, 0, 0, TRUE);

	printf ("Blit overlay %d times\n", TIMES);

	return EXIT_SUCCESS;
}
