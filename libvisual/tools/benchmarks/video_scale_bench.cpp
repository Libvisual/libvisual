#include <libvisual/libvisual.h>
#include <iostream>
#include <cstdlib>

int main (int argc, char **argv)
{
    LV::System::init (argc, argv);

    unsigned int max_runs = 1000;

    VisVideoDepth depth = VISUAL_VIDEO_DEPTH_32BIT;
    VisVideoScaleMethod method = VISUAL_VIDEO_SCALE_BILINEAR;

	visual_init (&argc, &argv);

	auto dest = LV::Video::create (640, 400, depth);
	auto src  = LV::Video::create (320, 200, depth);

	for (unsigned int i = 0; i < max_runs; i++)
		dest->scale (src, method);

	return EXIT_SUCCESS;
}
