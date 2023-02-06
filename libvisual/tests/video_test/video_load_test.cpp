#include "test.h"
#include <libvisual/libvisual.h>
#include <fstream>

namespace
{
  LV::VideoPtr load_raw_image (std::string const& path, int width, int height, VisVideoDepth depth)
  {
      auto image {LV::Video::create (width, height, depth)};

      std::size_t const content_bytes_per_row = image->get_width () * image->get_bpp ();

      {
          std::ifstream input {path, std::ios::binary};

          for (int y = 0; y < image->get_height (); y++) {
              auto pixel_row_ptr = static_cast<char*> (image->get_pixel_ptr (0, y));
              input.read (pixel_row_ptr, content_bytes_per_row);
          }
      }

#if VISUAL_LITTLE_ENDIAN == 1
      auto byteswapped_image {LV::Video::create (width, height, depth)};
      byteswapped_image->flip_pixel_bytes (image);

      return byteswapped_image;
#else
      return image;
#endif
  }

  void test_png_load ()
  {
      {
          auto png_image {LV::Video::create_from_file ("../images/additive-colors-rgb24.png")};
          LV_TEST_ASSERT (png_image);

          auto raw_image {load_raw_image ("../images/additive-colors-rgb24.raw",
                                          png_image->get_width (),
                                          png_image->get_height (),
                                          VISUAL_VIDEO_DEPTH_24BIT)};

          LV_TEST_ASSERT (png_image->has_same_content (raw_image));
      }

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
}

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    test_png_load ();

    LV::System::destroy ();
}
