#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>

#define TIMES	500

int main (int argc, char **argv)
{
	VisVideo *dest, *src;
	VisVideoDepth depth = VISUAL_VIDEO_DEPTH_32BIT;
	int alpha = FALSE;
	int i;
	
	visual_init (&argc, &argv);

	if (argc > 1)
		depth = visual_video_depth_enum_from_value (atoi (argv[1]));

	if (argc > 2)
		alpha = atoi (argv[2]);

	dest =  visual_video_new();
	visual_video_set_depth (dest, depth);
	visual_video_set_dimension (dest, 1024, 768);
	visual_video_allocate_buffer (dest);

	src = visual_video_new ();
	visual_video_set_depth (src, depth);
	visual_video_set_dimension (src, 512, 512);
	visual_video_allocate_buffer (src);

	for (i = 0; i < TIMES; i++)
		visual_video_blit_overlay (dest, src, rand () % 1024, rand () % 768, alpha);

	printf ("Blit overlay %d times.\n", TIMES);
}

