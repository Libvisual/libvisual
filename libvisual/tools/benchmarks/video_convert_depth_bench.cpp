#include <libvisual/libvisual.h>
#include <iostream>
#include <cstdlib>

int main (int argc, char **argv)
{
    LV::System::init (argc, argv);

    unsigned int max_runs = 1000;

    VisVideoDepth depth1 = VISUAL_VIDEO_DEPTH_32BIT;
    VisVideoDepth depth2 = VISUAL_VIDEO_DEPTH_16BIT;

    if (argc > 2) {
        depth1 = visual_video_depth_enum_from_value (std::atoi (argv[1]));
        depth2 = visual_video_depth_enum_from_value (std::atoi (argv[2]));
    }

    if (depth1 == VISUAL_VIDEO_DEPTH_NONE || depth2 == VISUAL_VIDEO_DEPTH_NONE)
    {
        std::cerr << "Invalid bit depths specified!\n";
        return EXIT_FAILURE;
    }

    if (depth1 == depth2) {
        std::cerr << "Bit depths are the same, Nothing to test!\n";
        return EXIT_FAILURE;
    }

    auto dest = LV::Video::create (640, 400, depth1);
    if (depth1 == VISUAL_VIDEO_DEPTH_8BIT)
        dest->set_palette (LV::Palette (256));

    auto src = LV::Video::create (640, 400, depth2);
    if (depth2 == VISUAL_VIDEO_DEPTH_8BIT)
        src->set_palette (LV::Palette (256));

    for (unsigned int i = 0; i < max_runs; i++)
        dest->convert_depth (src);

    return EXIT_SUCCESS;
}
