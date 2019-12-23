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
#include <png.h>
#include <istream>
#include <csetjmp>

namespace LV {

  namespace {

    void handle_png_read (png_structp png_ptr, png_bytep data, png_size_t length)
    {
        auto  io_ptr = png_get_io_ptr (png_ptr);
        auto& input  = *static_cast<std::istream*> (io_ptr);

        if (!input.read (reinterpret_cast<char*> (data), length)) {
            std::longjmp (png_jmpbuf (png_ptr), -1);
        }
    }

    void handle_png_warning (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_WARNING, "PNG load error: %s", message);
    }

    void handle_png_error (png_structp png_ptr, char const* message)
    {
        (void)png_ptr;

        visual_log (VISUAL_LOG_ERROR, "PNG load error: %s", message);
    }

  } // anonymous

  VideoPtr bitmap_load_png (std::istream& input)
  {
      auto saved_stream_pos = input.tellg ();

      png_byte signature[8];
      input.read (reinterpret_cast<char*> (signature), sizeof (signature));

      bool is_png = !png_sig_cmp (signature, 0, sizeof (signature));

      if (!is_png) {
          return nullptr;
      }

      auto png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, nullptr, handle_png_error, handle_png_warning);
      if (!png_ptr) {
          return nullptr;
      }

      auto info_ptr = png_create_info_struct (png_ptr);
      if (!info_ptr) {
          png_destroy_read_struct (&png_ptr, nullptr, nullptr);
          return nullptr;
      }

      auto end_info = png_create_info_struct (png_ptr);
      if (!end_info) {
          png_destroy_read_struct (&png_ptr, &info_ptr, nullptr);
          return nullptr;
      }

      uint8_t*  pixels = nullptr;
      uint8_t** pixel_row_ptrs = nullptr;

      if (setjmp (png_jmpbuf (png_ptr))) {
          input.seekg (saved_stream_pos);

          png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);

          delete []pixel_row_ptrs;
          visual_mem_free (pixels);

          return nullptr;
      }

      png_set_read_fn (png_ptr, &input, handle_png_read);

      png_set_sig_bytes (png_ptr, sizeof (signature));

      png_read_info (png_ptr, info_ptr);

      auto color_type = png_get_color_type (png_ptr, info_ptr);
      auto bit_depth  = png_get_bit_depth (png_ptr, info_ptr);

      if (color_type == PNG_COLOR_TYPE_PALETTE) {
          png_set_palette_to_rgb (png_ptr);
      }

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

#if VISUAL_LITTLE_ENDIAN
      png_set_bgr (png_ptr);
#endif

      png_read_update_info (png_ptr, info_ptr);

      VisVideoDepth depth = VISUAL_VIDEO_DEPTH_NONE;

      switch (png_get_color_type (png_ptr, info_ptr)) {
          case PNG_COLOR_TYPE_RGB:
              depth = VISUAL_VIDEO_DEPTH_24BIT;
              break;
          case PNG_COLOR_TYPE_RGB_ALPHA:
              depth = VISUAL_VIDEO_DEPTH_32BIT;
              break;
          default:
              std::longjmp (png_jmpbuf (png_ptr), -1);
      }

      auto width      = png_get_image_width (png_ptr, info_ptr);
      auto height     = png_get_image_height (png_ptr, info_ptr);
      auto row_stride = png_get_rowbytes (png_ptr, info_ptr);

      // NOTE: We have to use visual_mem_malloc() here as LV::Video
      // will free the buffer with visual_mem_free()
      pixels = static_cast<uint8_t*> (visual_mem_malloc (row_stride * height));

      pixel_row_ptrs = new uint8_t*[height];

      for (unsigned int y = 0; y < height; y++) {
          pixel_row_ptrs[y] = pixels + y * row_stride;
      }

      png_read_image (png_ptr, pixel_row_ptrs);

      png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);

      delete []pixel_row_ptrs;

      return Video::wrap (pixels, true, width, height, depth);
  }

} // LV namespace
