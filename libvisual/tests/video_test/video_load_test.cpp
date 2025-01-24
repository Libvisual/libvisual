#include "test.h"
#include "video_test/common.hpp"
#include <libvisual/libvisual.h>

namespace
{

  void test_load_indexed8 ()
  {
      auto bmp_image {LV::Video::create_from_file ("../images/additive-colors-indexed8.bmp")};
      LV_TEST_ASSERT (bmp_image);

      auto png_image {LV::Video::create_from_file ("../images/additive-colors-indexed8.png")};
      LV_TEST_ASSERT (png_image);

      auto raw_image {load_raw_indexed_image ("../images/additive-colors-indexed8.raw",
                                              "../images/additive-colors-indexed8.raw.pal",
                                              png_image->get_width (),
                                              png_image->get_height ())};

      LV_TEST_ASSERT (bmp_image->has_same_content (raw_image));
      LV_TEST_ASSERT (png_image->has_same_content (raw_image));
  }

  void test_load_rgb24 ()
  {
      auto bmp_image {LV::Video::create_from_file ("../images/additive-colors-rgb24.bmp")};
      LV_TEST_ASSERT (bmp_image);

      auto png_image {LV::Video::create_from_file ("../images/additive-colors-rgb24.png")};
      LV_TEST_ASSERT (png_image);

      auto raw_image {load_raw_image ("../images/additive-colors-rgb24.raw",
                                      png_image->get_width (),
                                      png_image->get_height (),
                                      VISUAL_VIDEO_DEPTH_24BIT)};

      LV_TEST_ASSERT (bmp_image->has_same_content (raw_image));
      LV_TEST_ASSERT (png_image->has_same_content (raw_image));
  }

  void test_load_argb32 ()
  {
      auto png_image {LV::Video::create_from_file ("../images/additive-colors-argb32.png")};
      LV_TEST_ASSERT (png_image);

      auto raw_image {load_raw_image ("../images/additive-colors-argb32.raw",
                                      png_image->get_width (),
                                      png_image->get_height (),
                                      VISUAL_VIDEO_DEPTH_32BIT)};

      LV_TEST_ASSERT (png_image->has_same_content (raw_image));
  }
}

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    test_load_indexed8 ();
    test_load_rgb24 ();
    test_load_argb32 ();

    LV::System::destroy ();
}
