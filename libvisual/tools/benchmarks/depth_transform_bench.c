#include <libvisual/libvisual.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMES	500

int main (int argc, char **argv)
{
	char *d1, *d2;
	VisVideo *dest, *src;
	VisVideoDepth depth1, depth2;
	int i;

	visual_init (&argc, &argv);

	if (argc > 2) {
		d1 = argv[1];
		d2 = argv[2];
	} else {
		d1 = visual_strdup("32");
		d2 = visual_strdup("16");
	}

	depth1 = visual_video_depth_enum_from_value(atoi(d1));
	depth2 = visual_video_depth_enum_from_value(atoi(d2));

	dest = visual_video_new ();
	visual_video_set_depth (dest, depth1);
	visual_video_set_dimension (dest, 640, 400);
	visual_video_set_palette (dest, visual_palette_new (256));
	visual_video_allocate_buffer (dest);

	src = visual_video_new ();
	visual_video_set_depth (src, depth2);
	visual_video_set_dimension (src, 640, 400);
	visual_video_set_palette (src, visual_palette_new (256));
	visual_video_allocate_buffer (src);

	for (i = 0; i < TIMES; i++)
		visual_video_depth_transform (dest, src);

	printf ("Depth transformed %d times from %s to %s\n", TIMES, d1, d2);

	return EXIT_SUCCESS;
}
