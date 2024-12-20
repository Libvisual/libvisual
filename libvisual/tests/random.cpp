#include "random.hpp"
#include <random>

namespace LV::Tests
{
  LV::VideoPtr create_random_video (int width, int height, VisVideoDepth depth)
  {
      std::random_device device {};
      std::uniform_int_distribution<uint8_t> distrib {0, 255};

      auto video {LV::Video::create (width, height, depth)};

      auto bytes_per_pixel = video->get_bpp ();
      auto pitch = video->get_pitch ();

      auto content_bytes_per_row = bytes_per_pixel * video->get_width ();

      auto pixel_row_ptr = static_cast<uint8_t *>(video->get_pixels ());

      for (int y = 0; y < video->get_height (); y++) {
          auto pixel = pixel_row_ptr;
          for (int c = 0; c < content_bytes_per_row; c++) {
              *pixel = distrib (device);
              pixel++;
          }

          pixel_row_ptr += pitch;
      }

      return video;
  }
} // LV::Tests namespace
