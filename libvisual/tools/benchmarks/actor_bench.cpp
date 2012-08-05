#include <libvisual/libvisual.h>
#include <iostream>
#include <cstdlib>

int main (int argc, char **argv)
{
    VisVideoDepth depth        = VISUAL_VIDEO_DEPTH_32BIT;
    bool          forced_depth = false;
    unsigned int  max_runs     = 500;
    std::string   actor_name   = "blursk";

    LV::System::init (argc, argv);

    if (argc > 1)
        actor_name = argv[1];

    if (argc > 2) {
        depth = visual_video_depth_enum_from_value (std::atoi (argv[2]));
        if (depth == VISUAL_VIDEO_DEPTH_NONE) {
            std::cerr << "Invalid video depth specified\n";
            return EXIT_FAILURE;
        }
    }

    auto actor = visual_actor_new (actor_name.c_str ());
    if (!actor) {
        std::cerr << "Cannot load actor '" << actor_name << "'\n";
        return EXIT_FAILURE;
    }

    visual_actor_realize (actor);

    if (!forced_depth) {
        auto supported_depths = visual_actor_get_supported_depth (actor);
        depth = visual_video_depth_get_highest (supported_depths);
    }

    auto dest = LV::Video::create (640, 400, depth);

    visual_actor_set_video (actor, dest.get ());
    visual_actor_video_negotiate (actor, depth, false, false);

    LV::Audio audio;

    for (unsigned int i = 0; i < max_runs; i++)
        visual_actor_run (actor, &audio);

    return EXIT_SUCCESS;
}
