/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
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
#include "lv_video_fill.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"

#pragma pack(1)

union pixel16_t {
    struct {
    #ifdef VISUAL_LITTLE_ENDIAN
        uint16_t b:5, g:6, r:5;
    #else
        uint16_t r:5, g:6, b:5;
    #endif
    } rgb;

    uint16_t value;
};

#pragma pack()

namespace LV {

  void VideoFill::fill_color_index8 (Video& video, Color const& color)
  {
      auto buf = static_cast<uint8_t*> (video.get_pixels ());

      int8_t col = ((color.r + color.g + color.b) / 3);

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set (buf, col, video.m_impl->width);

          buf += video.m_impl->pitch;
      }
  }

  void VideoFill::fill_color_rgb16 (Video& video, Color const& color)
  {
      auto buf = static_cast<uint8_t*> (video.get_pixels ());

      pixel16_t pixel;
      pixel.rgb.r = color.r >> 3;
      pixel.rgb.g = color.g >> 2;
      pixel.rgb.b = color.b >> 3;

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set16 (buf, pixel.value, video.m_impl->width);

          buf += video.m_impl->pitch;
      }
  }

  void VideoFill::fill_color_rgb24 (Video& video, Color const& color)
  {
      auto rbuf = static_cast<uint8_t*> (video.get_pixels ());

  #if VISUAL_LITTLE_ENDIAN == 1
      uint32_t cola = (color.b << 24) | (color.r << 16) | (color.g << 8) | color.b;
      uint32_t colb = (color.g << 24) | (color.b << 16) | (color.r << 8) | color.g;
      uint32_t colc = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.r;
  #else
      uint32_t cola = (color.b << 24) | (color.g << 16) | (color.r << 8) | color.b;
      uint32_t colb = (color.g << 24) | (color.r << 16) | (color.b << 8) | color.g;
      uint32_t colc = (color.r << 24) | (color.b << 16) | (color.g << 8) | color.r;
  #endif

      for (int y = 0; y < video.m_impl->height; y++) {
          auto buf = reinterpret_cast<uint32_t*> (rbuf);

          int x = video.m_impl->width;
          while (x >= 4) {
              *(buf++) = cola;
              *(buf++) = colb;
              *(buf++) = colc;
              x -= 4;
          }

          switch (x) {
              case 3: {
                  *buf++ = cola;
                  *buf++ = colb;
                  auto buf8 = reinterpret_cast<uint8_t*> (buf);
                  *buf8++ = color.r;
                  break;
              }
              case 2: {
                  *buf++ = cola;
                  auto buf8 = reinterpret_cast<uint8_t*> (buf);
                  *buf8++ = color.g;
                  *buf8++ = color.r;
                  break;
              }
              case 1: {
                  auto buf8 = reinterpret_cast<uint8_t*> (buf);
                  *buf8++ = color.b;
                  *buf8++ = color.g;
                  *buf8++ = color.r;
                  break;
              }
          }

          rbuf += video.m_impl->pitch;
      }
  }

  void VideoFill::fill_color_argb32 (Video& video, Color const& color)
  {
      auto buf = static_cast<uint8_t*> (video.get_pixels ());

      uint32_t col = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set32 (buf, col, video.m_impl->width);

          buf += video.m_impl->pitch;
      }
  }

} // LV namespace
