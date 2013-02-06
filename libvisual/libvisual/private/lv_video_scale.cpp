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
#include "lv_cpu.h"

#pragma pack(1)

// NOTE: Although the following struct members are named 'r', 'g' and 'b', the scale functions are very much pixel
// format agnostic. They only assume the number of components.

struct color16_t {
#if VISUAL_LITTLE_ENDIAN == 1
    uint16_t r:5, g:6, b:5;
#else
    uint16_t b:5, g:6, r:5;
#endif
};

struct color24_t {
    uint8_t b, g, r;
};

#pragma pack()

namespace LV {

  void VideoTransform::scale_nearest_color8 (Video& dst, Video const& src)
  {
      uint32_t du = dst.m_impl->width  > 1 ? ((src.m_impl->width  - 1) << 16) / (dst.m_impl->width  - 1) : 0;
      uint32_t dv = dst.m_impl->height > 1 ? ((src.m_impl->height - 1) << 16) / (dst.m_impl->height - 1) : 0;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          auto src_pixel_row = static_cast<uint8_t const*> (src.m_impl->pixel_rows[v >> 16]);

          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + dst.m_impl->width;

          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel++ = src_pixel_row[u >> 16];
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_nearest_color16 (Video& dst, Video const& src)
  {
      uint32_t du = dst.m_impl->width  > 1 ? ((src.m_impl->width  - 1) << 16) / (dst.m_impl->width  - 1) : 0;
      uint32_t dv = dst.m_impl->height > 1 ? ((src.m_impl->height - 1) << 16) / (dst.m_impl->height - 1) : 0;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          auto src_pixel_row = static_cast<uint16_t const*> (src.m_impl->pixel_rows[v >> 16]);

          auto dst_pixel     = reinterpret_cast<uint16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint16_t*> (dst_pixel_row) + dst.m_impl->width;

          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel++ = src_pixel_row[u >> 16];
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_nearest_color24 (Video& dst, Video const& src)
  {
      uint32_t du = dst.m_impl->width  > 1 ? ((src.m_impl->width  - 1) << 16) / (dst.m_impl->width  - 1) : 0;
      uint32_t dv = dst.m_impl->height > 1 ? ((src.m_impl->height - 1) << 16) / (dst.m_impl->height - 1) : 0;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          auto src_pixel_row = static_cast<color24_t const*> (src.m_impl->pixel_rows[v >> 16]);

          auto dst_pixel     = reinterpret_cast<color24_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<color24_t*> (dst_pixel_row) + dst.m_impl->width;

          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel++ = src_pixel_row[u >> 16];
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_nearest_color32 (Video& dst, Video const& src)
  {
      uint32_t du = dst.m_impl->width  > 1 ? ((src.m_impl->width  - 1) << 16) / (dst.m_impl->width  - 1) : 0;
      uint32_t dv = dst.m_impl->height > 1 ? ((src.m_impl->height - 1) << 16) / (dst.m_impl->height - 1) : 0;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          auto src_pixel_row = static_cast<uint32_t const*> (src.m_impl->pixel_rows[v >> 16]);

          if (v >> 16 >= (unsigned int) src.m_impl->height)
              v -= 0x10000;

          auto dst_pixel     = reinterpret_cast<uint32_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint32_t*> (dst_pixel_row) + dst.m_impl->width;

          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              *dst_pixel++ = src_pixel_row[u >> 16];
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_bilinear_color8 (Video& dst, Video const& src)
  {
      uint32_t du = ((src.m_impl->width  - 1) << 16) / dst.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dst.m_impl->height;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          auto src_pixel_rowu = static_cast<uint8_t const*> (src.m_impl->pixel_rows[v >> 16]);
          auto src_pixel_rowl = static_cast<uint8_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          auto dst_pixel     = dst_pixel_row;
          auto dst_pixel_end = dst_pixel_row + dst.m_impl->width;

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU  = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              uint8_t cul = src_pixel_rowu[u >> 16];
              uint8_t cll = src_pixel_rowl[u >> 16];
              uint8_t cur = src_pixel_rowu[(u >> 16) + 1];
              uint8_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul;
              b0 += ll * cll;
              b0 += ur * cur;
              b0 += lr * clr;

              *dst_pixel++ = b0 >> 16;
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_bilinear_color16 (Video& dst, Video const& src)
  {
      uint32_t du = ((src.m_impl->width - 1)  << 16) / dst.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dst.m_impl->height;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          auto src_pixel_rowu = static_cast<color16_t const*> (src.m_impl->pixel_rows[v >> 16]);
          auto src_pixel_rowl = static_cast<color16_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          auto dst_pixel     = reinterpret_cast<color16_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<color16_t*> (dst_pixel_row) + dst.m_impl->width;

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              color16_t cul = src_pixel_rowu[u >> 16];
              color16_t cll = src_pixel_rowl[u >> 16];
              color16_t cur = src_pixel_rowu[(u >> 16) + 1];
              color16_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.r;
              uint32_t b1 = ul * cul.g;
              uint32_t b2 = ul * cul.b;

              b0 += ll * cll.r;
              b1 += ll * cll.g;
              b2 += ll * cll.b;

              b0 += ur * cur.r;
              b1 += ur * cur.g;
              b2 += ur * cur.b;

              b0 += lr * clr.r;
              b1 += lr * clr.g;
              b2 += lr * clr.b;

              color16_t b;
              b.r = b0 >> 16;
              b.g = b1 >> 16;
              b.b = b2 >> 16;

              *dst_pixel++ = b;
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_bilinear_color24 (Video& dst, Video const& src)
  {
      uint32_t du = ((src.m_impl->width  - 1) << 16) / dst.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dst.m_impl->height;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          auto src_pixel_rowu = static_cast<color24_t const*> (src.m_impl->pixel_rows[v >> 16]);
          auto src_pixel_rowl = static_cast<color24_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          auto dst_pixel     = reinterpret_cast<color24_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<color24_t*> (dst_pixel_row) + dst.m_impl->width;

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU  = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              color24_t cul = src_pixel_rowu[u >> 16];
              color24_t cll = src_pixel_rowl[u >> 16];
              color24_t cur = src_pixel_rowu[(u >> 16) + 1];
              color24_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.r;
              uint32_t b1 = ul * cul.g;
              uint32_t b2 = ul * cul.b;

              b0 += ll * cll.r;
              b1 += ll * cll.g;
              b2 += ll * cll.b;

              b0 += ur * cur.r;
              b1 += ur * cur.g;
              b2 += ur * cur.b;

              b0 += lr * clr.r;
              b1 += lr * clr.g;
              b2 += lr * clr.b;

              color24_t b;
              b.r = b0 >> 16;
              b.g = b1 >> 16;
              b.b = b2 >> 16;

              *dst_pixel++ = b;
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

  void VideoTransform::scale_bilinear_color32 (Video& dst, Video const& src)
  {
      if (visual_cpu_has_mmx ()) {
          scale_bilinear_color32_mmx (dst, src);
          return;
      }

      uint32_t du = ((src.m_impl->width  - 1) << 16) / dst.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dst.m_impl->height;

      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ()) + dst.m_impl->pitch * dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          auto src_pixel_rowu = static_cast<uint32_t const*> (src.m_impl->pixel_rows[v >> 16]);
          auto src_pixel_rowl = static_cast<uint32_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          auto dst_pixel     = reinterpret_cast<uint32_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint32_t*> (dst_pixel_row) + dst.m_impl->width;

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              union {
                  uint8_t  c8[4];
                  uint32_t c32;
              } cul, cll, cur, clr, b;

              cul.c32 = src_pixel_rowu[u >> 16];
              cll.c32 = src_pixel_rowl[u >> 16];
              cur.c32 = src_pixel_rowu[(u >> 16) + 1];
              clr.c32 = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.c8[0];
              uint32_t b1 = ul * cul.c8[1];
              uint32_t b2 = ul * cul.c8[2];
              uint32_t b3 = ul * cul.c8[3];

              b0 += ll * cll.c8[0];
              b1 += ll * cll.c8[1];
              b2 += ll * cll.c8[2];
              b3 += ll * cll.c8[3];

              b0 += ur * cur.c8[0];
              b1 += ur * cur.c8[1];
              b2 += ur * cur.c8[2];
              b3 += ur * cur.c8[3];

              b0 += lr * clr.c8[0];
              b1 += lr * clr.c8[1];
              b2 += lr * clr.c8[2];
              b3 += lr * clr.c8[3];

              b.c8[0] = b0 >> 16;
              b.c8[1] = b1 >> 16;
              b.c8[2] = b2 >> 16;
              b.c8[3] = b3 >> 16;

              *dst_pixel++ = b.c32;
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }
  }

} // LV namespace
