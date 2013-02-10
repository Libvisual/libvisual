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
#include "lv_video_transform.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"

namespace LV {

  void VideoTransform::rotate_90 (Video& dst, Video const& src)
  {
      visual_return_if_fail (dst.m_impl->width == src.m_impl->height);
      visual_return_if_fail (dst.m_impl->height == src.m_impl->width);

      auto tsbuf = static_cast<uint8_t*> (src.m_impl->pixel_rows[src.m_impl->height-1]);
      auto sbuf  = tsbuf;

      for (int y = 0; y < dst.m_impl->height; y++) {
          auto dbuf = static_cast<uint8_t*> (dst.m_impl->pixel_rows[y]);

          for (int x = 0; x < dst.m_impl->width; x++) {
              for (int i = 0; i < dst.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf -= src.m_impl->pitch;
          }

          tsbuf += src.m_impl->bpp;
          sbuf = tsbuf;
      }
  }

  void VideoTransform::rotate_180 (Video& dst, Video const& src)
  {
      int h1 = src.m_impl->height - 1;
      int w1 = (src.m_impl->width - 1) * src.m_impl->bpp;

      visual_return_if_fail (dst.m_impl->width  == src.m_impl->width);
      visual_return_if_fail (dst.m_impl->height == src.m_impl->height);

      for (int y = 0; y < dst.m_impl->height; y++) {
          auto dbuf = static_cast<uint8_t*> (dst.m_impl->pixel_rows[y]);
          auto sbuf = static_cast<uint8_t const*> (src.m_impl->pixel_rows[h1 - y]) + w1;

          for (int x = 0; x < dst.m_impl->width; x++) {
              for (int i = 0; i < src.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf -= src.m_impl->bpp;
          }
      }
  }

  void VideoTransform::rotate_270 (Video& dst, Video const& src)
  {
      auto tsbuf = static_cast<uint8_t*> (src.get_pixels ()) + src.m_impl->pitch - src.m_impl->bpp;
      auto sbuf = tsbuf;

      visual_return_if_fail (dst.m_impl->width == src.m_impl->height);
      visual_return_if_fail (dst.m_impl->height == src.m_impl->width);

      for (int y = 0; y < dst.m_impl->height; y++) {
          auto dbuf = static_cast<uint8_t*> (dst.m_impl->pixel_rows[y]);

          for (int x = 0; x < dst.m_impl->width; x++) {
              for (int i = 0; i < dst.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf += src.m_impl->pitch;
          }

          tsbuf -= src.m_impl->bpp;
          sbuf = tsbuf;
      }
  }

  // Mirror functions

  void VideoTransform::mirror_x (Video& dst, Video const& src)
  {
      const int step2 = dst.m_impl->bpp << 1;
      const int w1b = (dst.m_impl->width - 1) * dst.m_impl->bpp;

      for (int y = 0; y < dst.m_impl->height; y++) {
          auto sbuf = static_cast<uint8_t*> (src.m_impl->pixel_rows[y]) + w1b;
          auto dbuf = static_cast<uint8_t*> (dst.m_impl->pixel_rows[y]);

          for (int x = 0; x < dst.m_impl->width; x++) {

              for (int i = 0; i < dst.m_impl->bpp; i++)
                  *(dbuf++) = *(sbuf++);

              sbuf -= step2;
          }
      }
  }

  void VideoTransform::mirror_y (Video& dst, Video const& src)
  {
      int y;

      for (y = 0; y < dst.m_impl->height; y++) {
          visual_mem_copy (dst.m_impl->pixel_rows[y],
                           src.m_impl->pixel_rows[dst.m_impl->height - 1 - y],
                           dst.m_impl->width * dst.m_impl->bpp);
      }
  }

} // LV namespace
