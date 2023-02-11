#include "test.h"
#include "random.hpp"
#include <libvisual/libvisual.h>
#include <cassert>

namespace
{
  LV::VideoPtr clone_video (LV::VideoPtr const& source)
  {
      auto clone {LV::Video::create (source->get_width (), source->get_height (), source->get_depth ())};

      assert (clone->get_pitch () == source->get_pitch ());
      std::size_t buffer_size = static_cast<std::size_t> (clone->get_pitch () * clone->get_width ());

      visual_mem_copy (clone->get_pixels (), source->get_pixels (), buffer_size);

      return clone;
  }

  void test_blit_overlay_alphasrc ()
  {
      // Check that blit_overlay_alphasrc results are within +/- 1 of exact computation for each colour channel. The
      // errors largely arise from the use of 256 instead of 255 as divisor for performance reasons.

      int const test_width  = 31;
      int const test_height = 31;

      auto source = LV::Tests::create_random_video (test_width, test_height, VISUAL_VIDEO_DEPTH_32BIT);
      source->set_compose_type (VISUAL_VIDEO_COMPOSE_TYPE_SRC);

      auto target = LV::Tests::create_random_video (test_width, test_height, VISUAL_VIDEO_DEPTH_32BIT);

      auto actual {clone_video (target)};
      actual->blit (source, 0, 0, true);

      for (int y = 0; y < test_height; y++) {
          auto source_pixel = static_cast<uint8_t const*> (source->get_pixel_ptr (0, y));
          auto target_pixel = static_cast<uint8_t const*> (target->get_pixel_ptr (0, y));
          auto actual_pixel = static_cast<uint8_t const*> (actual->get_pixel_ptr (0, y));

          for (int x = 0; x < test_width; x++) {
              LV_TEST_ASSERT (actual_pixel[3] == target_pixel[3]);

              float source_alpha = static_cast<float> (source_pixel[3]) / 255.0f;
              uint8_t b = source_alpha * source_pixel[0] + (1.0f - source_alpha) * target_pixel[0];
              uint8_t g = source_alpha * source_pixel[1] + (1.0f - source_alpha) * target_pixel[1];
              uint8_t r = source_alpha * source_pixel[2] + (1.0f - source_alpha) * target_pixel[2];

              LV_TEST_ASSERT (std::abs (static_cast<int16_t> (actual_pixel[0]) - static_cast<int16_t> (b)) <= 1);
              LV_TEST_ASSERT (std::abs (static_cast<int16_t> (actual_pixel[1]) - static_cast<int16_t> (g)) <= 1);
              LV_TEST_ASSERT (std::abs (static_cast<int16_t> (actual_pixel[2]) - static_cast<int16_t> (r)) <= 1);

              source_pixel += 4;
              target_pixel += 4;
              actual_pixel += 4;
          }
      }
  }
} // anonymous namespace

int main(int argc, char *argv[])
{
    LV::System::init (argc, argv);
    test_blit_overlay_alphasrc ();
    LV::System::destroy ();
}
