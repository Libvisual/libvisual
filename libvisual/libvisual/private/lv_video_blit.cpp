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
#include "lv_video_blit.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"
#include "lv_cpu.h"

#pragma pack(1)

typedef struct {
#if VISUAL_LITTLE_ENDIAN == 1
    uint16_t b:5, g:6, r:5;
#else
    uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

namespace LV {

  void VideoBlit::blit_overlay_noalpha (Video* dest, Video* src)
  {
      auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
      auto srcbuf  = static_cast<uint8_t const*> (src->get_pixels ());

      /* src and dest are completely equal, do one big mem copy instead of a per line mem copy.
       * Also check if the pitch is equal to it's width * bpp, this is because of subregions. */
      if (dest->compare_attrs (src) && (src->m_impl->pitch == (src->m_impl->width * src->m_impl->bpp))) {
          visual_mem_copy (destbuf, srcbuf, dest->get_size ());
          return;
      }

      for (int y = 0; y < src->m_impl->height; y++) {
          visual_mem_copy (destbuf, srcbuf, src->m_impl->width * src->m_impl->bpp);

          destbuf += dest->m_impl->pitch;
          srcbuf += src->m_impl->pitch;
      }
  }

  void VideoBlit::blit_overlay_alphasrc (Video* dst, Video* src)
  {
      if (visual_cpu_has_mmx ()) {
          blit_overlay_alphasrc_mmx (dst, src);
          return;
      }

      auto dst_pixel_row_ptr = static_cast<uint8_t*> (dst->get_pixels ());
      auto src_pixel_row_ptr = static_cast<uint8_t const*> (src->get_pixels ());

      for (int y = 0; y < src->m_impl->height; y++) {
          auto dst_pixel = dst_pixel_row_ptr;
          auto src_pixel = src_pixel_row_ptr;

          for (int x = 0; x < src->m_impl->width; x++) {
              uint8_t const src_alpha = src_pixel[3];

              // NOTE: This is effectively
              //       "(src_alpha / 255) * src_pixel[i] + (1 - src_alpha / 255) * dst_pixel[i]"
              //       but with only a single multiplication, a single division by 256 rather than 255, for speed.
              dst_pixel[0] = (src_alpha * (src_pixel[0] - dst_pixel[0]) >> 8) + dst_pixel[0];
              dst_pixel[1] = (src_alpha * (src_pixel[1] - dst_pixel[1]) >> 8) + dst_pixel[1];
              dst_pixel[2] = (src_alpha * (src_pixel[2] - dst_pixel[2]) >> 8) + dst_pixel[2];

              src_pixel += 4;
              dst_pixel += 4;
          }

          dst_pixel_row_ptr += dst->m_impl->pitch;
          src_pixel_row_ptr += src->m_impl->pitch;
      }
  }

  void VideoBlit::blit_overlay_colorkey (Video* dest, Video* src)
  {
      unsigned int pixel_count = dest->m_impl->width * dest->m_impl->height;

      if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT) {
          auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
          auto srcbuf = static_cast<uint8_t const*> (src->get_pixels ());

          auto& palette = src->m_impl->palette;

          if (!palette.empty ()) {
              blit_overlay_noalpha (dest, src);
              return;
          }

          int index = palette.find_color (*src->m_impl->colorkey);

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (*srcbuf != index)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_16BIT) {
          auto destbuf = static_cast<uint16_t*> (dest->get_pixels ());
          auto srcbuf  = static_cast<uint16_t const*> (src->get_pixels ());

          uint16_t color = src->m_impl->colorkey->to_uint16 ();

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (color != *srcbuf)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_24BIT) {
          auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
          auto srcbuf  = static_cast<uint8_t const*> (src->get_pixels ());

          uint8_t r = src->m_impl->colorkey->r;
          uint8_t g = src->m_impl->colorkey->g;
          uint8_t b = src->m_impl->colorkey->b;

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
                  destbuf[0] = srcbuf[0];
                  destbuf[1] = srcbuf[1];
                  destbuf[2] = srcbuf[2];
              }

              destbuf += 3;
              srcbuf  += 3;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT) {
          auto destbuf = static_cast<uint32_t*> (dest->get_pixels ());
          auto srcbuf  = static_cast<uint32_t const*> (src->get_pixels ());

          uint32_t color = src->m_impl->colorkey->to_uint32 ();

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (color != *srcbuf)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }
      }
  }

  void VideoBlit::blit_overlay_surfacealpha (Video* dest, Video* src)
  {
      auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
      auto srcbuf  = static_cast<uint8_t const*> (src->get_pixels ());

      uint8_t alpha = src->m_impl->alpha;

      if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT) {

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  *destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

                  destbuf += dest->m_impl->bpp;
                  srcbuf += src->m_impl->bpp;
              }

              destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
              srcbuf  += src->m_impl->pitch  - (src->m_impl->width  * src->m_impl->bpp);
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_16BIT) {

          for (int y = 0; y < src->m_impl->height; y++) {
              auto destr = reinterpret_cast<rgb16_t*> (destbuf);
              auto srcr  = reinterpret_cast<rgb16_t const*> (srcbuf);

              for (int x = 0; x < src->m_impl->width; x++) {
                  destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
                  destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
                  destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;

                  destr++;
                  srcr++;
              }

              destbuf += dest->m_impl->pitch;
              srcbuf += src->m_impl->pitch;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_24BIT) {

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                  destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                  destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

                  destbuf += dest->m_impl->bpp;
                  srcbuf  += src->m_impl->bpp;
              }

              destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
              srcbuf  += src->m_impl->pitch  - (src->m_impl->width  * src->m_impl->bpp);
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT) {

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                  destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                  destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

                  destbuf += dest->m_impl->bpp;
                  srcbuf  += src->m_impl->bpp;
              }

              destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
              srcbuf  += src->m_impl->pitch  - (src->m_impl->width  * src->m_impl->bpp);
          }
      }
  }

  void VideoBlit::blit_overlay_surfacealphacolorkey (Video* dest, Video* src)
  {
      auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
      auto srcbuf = static_cast<uint8_t const*> (src->get_pixels ());

      uint8_t alpha = src->m_impl->alpha;

      if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT) {
          auto const& palette = src->m_impl->palette;

          if (!palette.empty ()) {
              blit_overlay_noalpha (dest, src);
              return;
          }

          int index = palette.find_color (*src->m_impl->colorkey);

          int dstep = dest->m_impl->pitch - dest->m_impl->width * dest->m_impl->bpp;
          int sstep = src->m_impl->pitch  - src->m_impl->width  * src->m_impl->bpp;

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  if (*srcbuf != index)
                      *destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

                  destbuf += dest->m_impl->bpp;
                  srcbuf += src->m_impl->bpp;
              }

              destbuf += dstep;
              srcbuf  += sstep;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_16BIT) {
          uint16_t color = src->m_impl->colorkey->to_uint16 ();

          for (int y = 0; y < src->m_impl->height; y++) {
              auto destr = reinterpret_cast<rgb16_t*> (destbuf);
              auto srcr  = reinterpret_cast<rgb16_t const*> (srcbuf);

                  for (int x = 0; x < src->m_impl->width; x++) {
                      if (color != *((uint16_t *) srcr)) {
                          destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
                          destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
                          destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;
                      }

                      destr++;
                      srcr++;
                  }

              destbuf += dest->m_impl->pitch;
              srcbuf  += src->m_impl->pitch;
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_24BIT) {
          uint8_t r = src->m_impl->colorkey->r;
          uint8_t g = src->m_impl->colorkey->g;
          uint8_t b = src->m_impl->colorkey->b;

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
                      destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                      destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                      destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
                  }

                  destbuf += dest->m_impl->bpp;
                  srcbuf += src->m_impl->bpp;
              }

              destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
              srcbuf += src->m_impl->pitch - (src->m_impl->width * src->m_impl->bpp);
          }

      } else if (dest->m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT) {
          uint32_t color = src->m_impl->colorkey->to_uint32 ();

          for (int y = 0; y < src->m_impl->height; y++) {
              for (int x = 0; x < src->m_impl->width; x++) {
                  if (color == *reinterpret_cast<uint32_t*> (destbuf)) {
                      destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                      destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                      destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
                  }

                  destbuf += dest->m_impl->bpp;
                  srcbuf += src->m_impl->bpp;
              }

              destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
              srcbuf += src->m_impl->pitch - (src->m_impl->width * src->m_impl->bpp);
          }
      }
  }

} // LV namespace
