#include <libvisual/libvisual.h>
#include <cstdlib>

int main (int argc, char **argv)
{
    LV::System::init (argc, argv);

    unsigned int max_runs = 1000;

    VisVideoDepth depth = VISUAL_VIDEO_DEPTH_32BIT;

    auto dest = LV::Video::create (640, 400, depth);
    auto src  = LV::Video::create (640, 400, depth);

    for (unsigned int i = 0; i < max_runs; i++)
        dest->blit (src, 0, 0, true);

    return EXIT_SUCCESS;
}
