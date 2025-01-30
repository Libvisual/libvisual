#include "test.h"
#include <libvisual/libvisual.h>
#include <sstream>

void test_save_indexed8 ()
{
      auto png_image {LV::Video::create_from_file ("../images/additive-colors-indexed8.png")};
      LV_TEST_ASSERT (png_image);

      std::stringstream buffer;
      LV_TEST_ASSERT (png_image->save_to_stream (buffer, LV::ImageFormat::PNG));

      auto png_saved_image {LV::Video::create_from_stream (buffer)};
      LV_TEST_ASSERT (png_saved_image);
      LV_TEST_ASSERT (png_saved_image->has_same_content (png_image));
}

void test_save_rgb24 ()
{
      auto png_image {LV::Video::create_from_file ("../images/additive-colors-rgb24.png")};
      LV_TEST_ASSERT (png_image);

      std::stringstream buffer;
      LV_TEST_ASSERT (png_image->save_to_stream (buffer, LV::ImageFormat::PNG));

      auto png_saved_image {LV::Video::create_from_stream (buffer)};
      LV_TEST_ASSERT (png_saved_image);
      LV_TEST_ASSERT (png_saved_image->has_same_content (png_image));
}

void test_save_argb32 ()
{
      auto png_image {LV::Video::create_from_file ("../images/additive-colors-argb32.png")};
      LV_TEST_ASSERT (png_image);

      std::stringstream buffer;
      LV_TEST_ASSERT (png_image->save_to_stream (buffer, LV::ImageFormat::PNG));

      auto png_saved_image {LV::Video::create_from_stream (buffer)};
      LV_TEST_ASSERT (png_saved_image);
      LV_TEST_ASSERT (png_saved_image->has_same_content (png_image));
}

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    test_save_indexed8 ();
    test_save_rgb24 ();
    test_save_argb32 ();

    LV::System::destroy ();
}
