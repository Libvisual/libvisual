/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_video_png.hpp"
#include "lv_common.h"
#include <iostream>
#include <bit>
#include <vector>
#include <csetjmp>
#include <png.h>
#include <zlib.h>

namespace LV {

  namespace {

    void handle_png_read (png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto  io_ptr {png_get_io_ptr (png_ptr)};
        auto& input  {*static_cast<std::istream*> (io_ptr)};

        if (!input.read (reinterpret_cast<char*> (data), length)) {
            std::longjmp (png_jmpbuf (png_ptr), -1);
        }
    }

    void handle_png_write (png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto  io_ptr {png_get_io_ptr (png_ptr)};
        auto& output {*static_cast<std::ostream*> (io_ptr)};

        if (!output.write(reinterpret_cast<char*>(data), length)) {
            std::longjmp (png_jmpbuf (png_ptr), -1);
        }
    }

    void handle_png_flush (png_structp png_ptr)
    {
        auto  io_ptr {png_get_io_ptr (png_ptr)};
        auto& output {*static_cast<std::ostream*> (io_ptr)};

        output.flush ();
    }

    void handle_png_load_warning (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_WARNING, "PNG load error: %s", message);
    }

    void handle_png_load_error (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_ERROR, "PNG load error: %s", message);
    }

    void handle_png_save_warning (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_WARNING, "PNG save error: %s", message);
    }

    void handle_png_save_error (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_ERROR, "PNG save error: %s", message);
    }

  } // anonymous

  VideoPtr bitmap_load_png (std::istream& input)
  {
      auto start_stream_pos = input.tellg ();

      // Check PNG signature.

      png_byte signature[8];
      if (!input.read (reinterpret_cast<char*> (signature), sizeof (signature))) {
          input.clear ();
          input.seekg (start_stream_pos);
          return nullptr;
      }

      auto is_png {!png_sig_cmp (signature, 0, sizeof (signature))};

      // Clean up test by rewinding to the beginning, like we have read nothing.
      if (!input.seekg (start_stream_pos)) {
          return nullptr;
      }

      if (!is_png) {
          return nullptr;
      }

      auto png_ptr {png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            handle_png_load_error,
                                            handle_png_load_warning)};
      if (!png_ptr) {
          return nullptr;
      }

      auto info_ptr {png_create_info_struct (png_ptr)};
      if (!info_ptr) {
          png_destroy_read_struct (&png_ptr, nullptr, nullptr);
          return nullptr;
      }

      auto end_info {png_create_info_struct (png_ptr)};
      if (!end_info) {
          png_destroy_read_struct (&png_ptr, &info_ptr, nullptr);
          return nullptr;
      }

      // Read PNG image data

      // Skip to the first chunk, which comes right after the signature.
      input.seekg (start_stream_pos + std::streampos {sizeof (signature)});

      uint8_t*  pixels = nullptr;
      uint8_t** pixel_row_ptrs = nullptr;

      if (setjmp (png_jmpbuf (png_ptr))) {
          // Some error happened during reading. Rewind to the beginning, like we have read nothing.
          input.clear ();
          input.seekg (start_stream_pos);

          png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);

          delete[] pixel_row_ptrs;
          visual_mem_free (pixels);

          return nullptr;
      }

      png_set_read_fn (png_ptr, &input, handle_png_read);

      png_set_sig_bytes (png_ptr, sizeof (signature));

      png_read_info (png_ptr, info_ptr);

      auto color_type {png_get_color_type (png_ptr, info_ptr)};
      auto bit_depth  {png_get_bit_depth (png_ptr, info_ptr)};

      if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
          png_set_expand_gray_1_2_4_to_8 (png_ptr);
      }

      if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
          png_set_tRNS_to_alpha (png_ptr);
      }

      if (bit_depth < 8) {
          png_set_packing (png_ptr);
      }

      if (bit_depth == 16) {
          png_set_strip_16 (png_ptr);
      }

      if (color_type == PNG_COLOR_TYPE_GRAY ||
          color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
          png_set_gray_to_rgb (png_ptr);
      }

      if constexpr (std::endian::native == std::endian::little) {
          png_set_bgr (png_ptr);
      }

      png_read_update_info (png_ptr, info_ptr);

      auto depth {VISUAL_VIDEO_DEPTH_NONE};

      switch (png_get_color_type (png_ptr, info_ptr)) {
          case PNG_COLOR_TYPE_PALETTE:
              depth = VISUAL_VIDEO_DEPTH_8BIT;
              break;
          case PNG_COLOR_TYPE_RGB:
              depth = VISUAL_VIDEO_DEPTH_24BIT;
              break;
          case PNG_COLOR_TYPE_RGB_ALPHA:
              depth = VISUAL_VIDEO_DEPTH_32BIT;
              break;
          default:
              std::longjmp (png_jmpbuf (png_ptr), -1);
      }

      auto width      {png_get_image_width (png_ptr, info_ptr)};
      auto height     {png_get_image_height (png_ptr, info_ptr)};
      auto row_stride {png_get_rowbytes (png_ptr, info_ptr)};

      // NOTE: We have to use visual_mem_malloc() here as LV::Video
      // will free the buffer with visual_mem_free()
      pixels = static_cast<uint8_t*> (visual_mem_malloc (row_stride * height));

      pixel_row_ptrs = new uint8_t*[height];

      for (unsigned int y {0}; y < height; y++) {
          pixel_row_ptrs[y] = pixels + y * row_stride;
      }

      png_read_image (png_ptr, pixel_row_ptrs);

      auto video = Video::wrap (pixels, true, width, height, depth);

      if (depth == VISUAL_VIDEO_DEPTH_8BIT) {
          png_colorp file_palette = nullptr;
          int file_color_count = 0;
          png_get_PLTE (png_ptr, info_ptr, &file_palette, &file_color_count);

          LV::Palette palette (256);
          int color_count = std::min (256, file_color_count);

          for (int i = 0; i < color_count; i++) {
              auto& color = palette.colors[i];
              color.r = file_palette[i].red;
              color.g = file_palette[i].green;
              color.b = file_palette[i].blue;
          }
          video->set_palette (std::move (palette));
      }

      png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);

      delete[] pixel_row_ptrs;

      return video;
  }

  bool bitmap_save_png(Video const& bitmap, std::ostream& output)
  {
      auto saved_stream_pos {output.tellp ()};

      auto png_ptr {png_create_write_struct (PNG_LIBPNG_VER_STRING,
                                             nullptr,
                                             handle_png_save_error,
                                             handle_png_save_warning)};
      if (!png_ptr) {
          visual_log (VISUAL_LOG_ERROR, "Failed to create PNG write struct.");
          return false;
      }

      auto info_ptr {png_create_info_struct (png_ptr)};
      if (!info_ptr) {
          visual_log (VISUAL_LOG_ERROR, "Failed to create PNG info struct.");
          png_destroy_write_struct (&png_ptr, nullptr);
          return false;
      }

      auto bitmap_width {bitmap.get_width ()};
      auto bitmap_height {bitmap.get_height ()};

      std::vector<png_byte*> pixel_row_ptrs;
      pixel_row_ptrs.reserve (bitmap_height);

      for (auto y {0}; y < bitmap_height; y++) {
          pixel_row_ptrs.push_back (static_cast<png_byte*> (bitmap.get_pixel_ptr (0, y)));
      }

      if (setjmp (png_jmpbuf (png_ptr))) {
          visual_log (VISUAL_LOG_ERROR, "Some error occurred.");
          output.seekp (saved_stream_pos);
          png_destroy_write_struct (&png_ptr, &info_ptr);
      }

      auto bits_per_channel {0};
      auto color_type {0};

      switch (bitmap.get_depth ()) {
          case VISUAL_VIDEO_DEPTH_8BIT: {
              bits_per_channel = 8;
              color_type = PNG_COLOR_TYPE_PALETTE;
              break;
          }
          case VISUAL_VIDEO_DEPTH_24BIT: {
              bits_per_channel = 8;
              color_type = PNG_COLOR_TYPE_RGB;
              break;
          }
          case VISUAL_VIDEO_DEPTH_32BIT: {
              bits_per_channel = 8;
              color_type = PNG_COLOR_TYPE_RGB_ALPHA;
              break;
          }
          default: {
              visual_log (VISUAL_LOG_ERROR, "Invalid depth or depth not supported.");
              return false;
          }
      }

      if constexpr (std::endian::native == std::endian::little) {
          png_set_bgr (png_ptr);
      }

      png_set_filter (png_ptr, 0, PNG_FILTER_NONE);

      png_set_compression_level (png_ptr, Z_BEST_COMPRESSION);

      png_set_IHDR (png_ptr, info_ptr, bitmap_width, bitmap_height,
                    bits_per_channel, color_type, PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

      if (color_type == PNG_COLOR_TYPE_PALETTE) {
          auto const& colors {bitmap.get_palette ().colors};

          std::vector<png_color> out_palette;
          out_palette.reserve (colors.size ());

          for (auto const& color : colors) {
              out_palette.emplace_back (color.r, color.g, color.b);
          }

          png_set_PLTE (png_ptr, info_ptr, out_palette.data (), out_palette.size ());
      }

      png_set_sRGB (png_ptr, info_ptr, PNG_sRGB_INTENT_PERCEPTUAL);

      png_set_write_fn (png_ptr, &output, handle_png_write, handle_png_flush);

      png_write_info (png_ptr, info_ptr);
      png_write_rows (png_ptr, pixel_row_ptrs.data (), bitmap_height);
      png_write_end (png_ptr, info_ptr);

      png_destroy_write_struct (&png_ptr, &info_ptr);

      return true;
  }

} // LV namespace
