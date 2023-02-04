#include "test.h"
#include <libvisual/libvisual.h>

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    auto video = LV::Video::create(640, 480, VISUAL_VIDEO_DEPTH_16BIT);

    LV_TEST_ASSERT (video->validate ());
    LV_TEST_ASSERT (video->has_same_content (video));

    LV::System::destroy ();
}
