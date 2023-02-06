/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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
#include "lv_video_convert.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"
#include <algorithm>
#include <array>

#pragma pack(1)

struct rgb16_t {
#if VISUAL_LITTLE_ENDIAN == 1
    uint16_t r:5, g:6, b:5;
#else
    uint16_t b:5, g:6, r:5;
#endif
};

#pragma pack()

#if VISUAL_LITTLE_ENDIAN == 1
  #define ARGB(a,r,g,b) ((a)<<24 | (r)<<16 | (g) << 8 | (b))
  #define RGB(r,g,b)    ((r)<<16 | (g)<<8  | (b))
#else
  #define ARGB(a,r,g,b) ((b)<<24 | (g)<<16 | (r) << 8 | (a))
  #define RGB(r,g,b)    ((b)<<16 | (g)<<8  | (r))
#endif

namespace LV {

  void VideoConvert::convert_get_smallest (Video& dst, Video const& src, int& width, int& height)
  {
      width  = std::min (dst.m_impl->width,  src.m_impl->width);
      height = std::min (dst.m_impl->height, src.m_impl->height);
  }

  void VideoConvert::index8_to_rgb16 (Video& dst, Video const& src)
  {
      std::array<rgb16_t, 256> colors;

      auto const& src_colors = src.m_impl->palette.colors;

      for(int i = 0; i < 256; i++) {
          colors[i].r = src_colors[i].r >> 3;
          colors[i].g = src_colors[i].g >> 2;
          colors[i].b = src_colors[i].b >> 3;
      }

      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = reinterpret_cast<rgb16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<rgb16_t*> (dst_pixel_row) + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel = colors[*src_pixel];
              dst_pixel++;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::index8_to_rgb24 (Video& dst, Video const& src)
  {
      auto const& src_colors = src.m_impl->palette.colors;

      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width * 3;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_colors[*src_pixel].b;
              dst_pixel[1] = src_colors[*src_pixel].g;
              dst_pixel[2] = src_colors[*src_pixel].r;

              dst_pixel += 3;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::index8_to_argb32 (Video& dst, Video const& src)
  {
      std::array<uint32_t, 256> colors;

      auto const& src_colors = src.m_impl->palette.colors;

      for (int i = 0; i < 256; ++i) {
          colors[i] = ARGB (255, src_colors[i].r, src_colors[i].g, src_colors[i].b);
      }

      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = reinterpret_cast<uint32_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint32_t*> (dst_pixel_row) + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel = colors[*src_pixel];
              dst_pixel++;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb16_to_index8 (Video& dst, Video const& src)
  {
      auto& dst_colors = dst.m_impl->palette.colors;

      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width;
          auto src_pixel     = reinterpret_cast<rgb16_t const*> (src_pixel_row);

          while (dst_pixel != dst_pixel_end) {
              uint8_t r = src_pixel->r << 3;
              uint8_t g = src_pixel->g << 2;
              uint8_t b = src_pixel->b << 3;
              uint8_t i = (r + g + b) / 3;

              dst_colors[i].r = r;
              dst_colors[i].g = g;
              dst_colors[i].b = b;

              *dst_pixel = i;

              dst_pixel++;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb16_to_rgb24 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width * 3;
          auto src_pixel     = reinterpret_cast<rgb16_t const*> (src_pixel_row);

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel->b << 3;
              dst_pixel[1] = src_pixel->g << 2;
              dst_pixel[2] = src_pixel->r << 3;

              dst_pixel += 3;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb16_to_argb32 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width * 4;
          auto src_pixel     = reinterpret_cast<rgb16_t const*> (src_pixel_row);

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel->b << 3;
              dst_pixel[1] = src_pixel->g << 2;
              dst_pixel[2] = src_pixel->r << 3;
              dst_pixel[3] = 255;

              dst_pixel += 4;
              src_pixel++;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb24_to_index8 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto& dst_colors = dst.m_impl->palette.colors;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              uint8_t b = src_pixel[0];
              uint8_t g = src_pixel[1];
              uint8_t r = src_pixel[2];
              uint8_t i = (b + g + r) / 3;

              dst_colors[i].r = r;
              dst_colors[i].g = g;
              dst_colors[i].b = b;

              *dst_pixel = i;

              dst_pixel++;
              src_pixel += 3;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb24_to_rgb16 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = reinterpret_cast<rgb16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<rgb16_t*> (dst_pixel_row) + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel->b = src_pixel[0] >> 3;
              dst_pixel->g = src_pixel[1] >> 2;
              dst_pixel->r = src_pixel[2] >> 3;

              dst_pixel++;
              src_pixel += 3;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::rgb24_to_argb32 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width * 4;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel[0];
              dst_pixel[1] = src_pixel[1];
              dst_pixel[2] = src_pixel[2];
              dst_pixel[3] = 255;

              dst_pixel += 4;
              src_pixel += 3;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::argb32_to_index8 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto& dst_colors = dst.m_impl->palette.colors;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              uint8_t b = src_pixel[0];
              uint8_t g = src_pixel[1];
              uint8_t r = src_pixel[2];
              uint8_t i = (r + g + b) / 3;

              dst_colors[i].r = r;
              dst_colors[i].g = g;
              dst_colors[i].b = b;

              *dst_pixel = i;

              dst_pixel++;
              src_pixel += 4;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::argb32_to_rgb16 (Video& dst, Video const& src)
  {
      int width, height;
      convert_get_smallest (dst, src, width, height);

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + height * dst.m_impl->pitch;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = reinterpret_cast<rgb16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<rgb16_t*> (dst_pixel_row) + width;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel->b = src_pixel[0] >> 3;
              dst_pixel->g = src_pixel[1] >> 2;
              dst_pixel->r = src_pixel[2] >> 3;

              dst_pixel++;
              src_pixel += 4;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::argb32_to_rgb24 (Video& dst, Video const& src)
  {
      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ());
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      int width, height;
      convert_get_smallest (dst, src, width, height);

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + dst.m_impl->width * 3;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel[0];
              dst_pixel[1] = src_pixel[1];
              dst_pixel[2] = src_pixel[2];

              dst_pixel += 3;
              src_pixel += 4;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::flip_pixel_bytes_color16 (Video& dst, Video const& src)
  {
      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = reinterpret_cast<uint16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint16_t*> (dst_pixel_row) + dst.m_impl->width;
          auto src_pixel     = reinterpret_cast<uint16_t const*> (src_pixel_row);

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel = (*src_pixel >> 8) | (*src_pixel << 8);

              dst_pixel++;
              src_pixel++;
          }

          dst_pixel += dst.m_impl->pitch;
          src_pixel += src.m_impl->pitch;
      }
  }

  void VideoConvert::flip_pixel_bytes_color24 (Video& dst, Video const& src)
  {
      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + dst.m_impl->width * 3;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel[2];
              dst_pixel[1] = src_pixel[1];
              dst_pixel[2] = src_pixel[0];

              dst_pixel += 3;
              src_pixel += 3;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

  void VideoConvert::flip_pixel_bytes_color32 (Video& dst, Video const& src)
  {
      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;
      auto src_pixel_row     = static_cast<uint8_t const*> (src.get_pixels ());

      while (dst_pixel_row != dst_pixel_row_end) {
          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + dst.m_impl->width * 4;
          auto src_pixel     = src_pixel_row;

          while (dst_pixel != dst_pixel_end) {
              dst_pixel[0] = src_pixel[3];
              dst_pixel[1] = src_pixel[2];
              dst_pixel[2] = src_pixel[1];
              dst_pixel[3] = src_pixel[0];

              dst_pixel += 4;
              src_pixel += 4;
          }

          dst_pixel_row += dst.m_impl->pitch;
          src_pixel_row += src.m_impl->pitch;
      }
  }

} // LV namespace
