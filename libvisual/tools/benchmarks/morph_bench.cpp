#include <libvisual/libvisual.h>
#include <iostream>
#include <cstdlib>

int main (int argc, char **argv)
{
    std::string   morph_name = "alphablend";
    VisVideoDepth depth      = VISUAL_VIDEO_DEPTH_32BIT;
    unsigned int  max_runs   = 500;

    LV::System::init (argc, argv);

    if (argc > 1)
        morph_name = argv[1];

    auto morph = visual_morph_new (morph_name.c_str ());
    if (!morph) {
        std::cerr << "Cannot load morph '" << morph_name << "'\n";
        return EXIT_FAILURE;
    }

    visual_morph_realize (morph);

    auto dest = LV::Video::create (640, 400, depth);
    auto src1 = LV::Video::create (640, 400, depth);
    auto src2 = LV::Video::create (640, 400, depth);

    visual_morph_set_video (morph, dest.get ());

    LV::Audio audio;

    float rate = 0.0;
    for (unsigned int i = 0; i < max_runs; i++) {
        visual_morph_set_rate (morph, rate);
        visual_morph_run (morph, &audio, src1.get (), src2.get ());

        rate += 0.1;

        if (rate > 1.0)
            rate = 0.0;
    }

    return EXIT_SUCCESS;
}
