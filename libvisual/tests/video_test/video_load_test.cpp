#include "test.h"
#include <libvisual/libvisual.h>
#include <bit>
#include <filesystem>
#include <fstream>
#include <optional>

namespace
{
  namespace fs = std::filesystem;

  LV::VideoPtr load_raw_image (fs::path const& path, int width, int height, VisVideoDepth depth)
  {
      auto image {LV::Video::create (width, height, depth)};

      std::size_t const content_bytes_per_row = image->get_width () * image->get_bpp ();

      {
          std::ifstream input {path, std::ios::binary};
          if (!input) {
              return nullptr;
          }

          for (int y = 0; y < image->get_height (); y++) {
              auto pixel_row_ptr = static_cast<char*> (image->get_pixel_ptr (0, y));
              if (!input.read (pixel_row_ptr, content_bytes_per_row)) {
                  return nullptr;
              }
          }
      }

      if constexpr (std::endian::native == std::endian::little) {
          auto byteswapped_image {LV::Video::create (width, height, depth)};
          byteswapped_image->flip_pixel_bytes (image);
          return byteswapped_image;
      } else {
          return image;
      }
  }

  std::optional<LV::Palette> load_raw_palette (fs::path const& path)
  {
      std::vector<std::uint8_t> palette_buffer (256 * 3);
      {
          std::ifstream input {path, std::ios::binary};
          if (!input.read (reinterpret_cast<char*> (palette_buffer.data ()), palette_buffer.size ())) {
              return std::nullopt;
          }
      }

      LV::Palette palette;
      palette.colors.reserve (256);
      for (unsigned int i = 0; i < 256; i++) {
          palette.colors.emplace_back (palette_buffer[i*3], palette_buffer[i*3+1], palette_buffer[i*3+2]);
      }

      return palette;
  }

  LV::VideoPtr load_raw_indexed_image (fs::path const& image_path,
                                       fs::path const& palette_path,
                                       int width,
                                       int height)
  {
      auto image {LV::Video::create (width, height, VISUAL_VIDEO_DEPTH_8BIT)};

      std::size_t const content_bytes_per_row = image->get_width () * image->get_bpp ();

      {
          std::ifstream input {image_path, std::ios::binary};
          if (!input) {
              return nullptr;
          }

          for (int y = 0; y < image->get_height (); y++) {
              auto pixel_row_ptr = static_cast<char*> (image->get_pixel_ptr (0, y));
              if (!input.read (pixel_row_ptr, content_bytes_per_row)) {
                  return nullptr;
              }
          }
      }

      auto palette {load_raw_palette (palette_path)};
      if (!palette.has_value ()) {
          return nullptr;
      }
      image->set_palette (palette.value ());

      return image;
  }

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
