/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Jean-Christophe Hoelt <jeko@ios-software.com>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include <x86intrin.h>

namespace LV {

  void VideoBlit::blit_overlay_alphasrc_mmx (Video* dst, Video* src)
  {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
      auto dst_pixel_row_ptr = static_cast<uint8_t*> (dst->get_pixels ());
      auto src_pixel_row_ptr = static_cast<uint8_t const*> (src->get_pixels ());

      for (int y = 0; y < src->m_impl->height; y++) {
          auto dst_pixel = reinterpret_cast<uint32_t*> (dst_pixel_row_ptr);
          auto src_pixel = reinterpret_cast<uint32_t const*> (src_pixel_row_ptr);

          for (int x = 0; x < src->m_impl->width; x++) {
              // We work with 32-bit pixel values packed as 4 x 16-bit ints in MMX registers.
              // See the pure C implementation in blit_overlay_alphsrc() for the calculation involved.

              // Load source alpha as a 16-bit int.
              uint16_t const src_alpha = reinterpret_cast<uint8_t const*> (src_pixel)[3];

              // Load source and target pixel values into MMX registers, each channel zero-extended into 16 bits.
              auto src = _mm_cvtsi32_si64 (*src_pixel);
              auto dst = _mm_cvtsi32_si64 (*dst_pixel);
              src = _mm_unpacklo_pi8 (src, _mm_setzero_si64 ());
              dst = _mm_unpacklo_pi8 (dst, _mm_setzero_si64 ());

              // Load src_alpha and (255 - src_alpha) and broadcast them into a1 and a2.
              auto a1 = _mm_set1_pi16 (src_alpha);
              auto a2 = _mm_set1_pi16 (static_cast<uint16_t> (255) - src_alpha);

              // Interpolate between source and target.
              auto result = _mm_add_pi16 (_mm_mullo_pi16 (src, a1), _mm_mullo_pi16 (dst, a2));
              result = _mm_srli_pi16 (result, 8);

              // Unpack result but keep the target pixel alpha.
              // Is there a nicer way to do this?
              uint32_t int_result = _mm_cvtsi64_si32 (_mm_packs_pu16 (result, result));
              int_result = (int_result & 0x00'ff'ff'ff) | (*dst_pixel & 0xff'00'00'00);

              *dst_pixel = int_result;

              dst_pixel++;
              src_pixel++;
          }

          dst_pixel_row_ptr += dst->m_impl->pitch;
          src_pixel_row_ptr += src->m_impl->pitch;
      }

      // FIXME: Some sources said this is not needed for x64 as MMX registers are no longer
      // overlayed on FP ones.
      _mm_empty ();

#endif /* !VISUAL_ARCH_X86 */
  }

} // LV namespace
