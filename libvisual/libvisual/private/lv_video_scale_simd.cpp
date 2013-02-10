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
#include "lv_video_transform.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"

namespace LV {

  void VideoTransform::scale_bilinear_color32_mmx (Video& dst, Video const& src)
  {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
      auto dst_pixel_row     = static_cast<uint8_t*> (dst.get_pixels ());
      auto dst_pixel_row_end = static_cast<uint8_t*> (dst.get_pixels ());

      uint32_t du = ((src.m_impl->width  - 1) << 16) / dst.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dst.m_impl->height;

      uint32_t v = 0;

      while (dst_pixel_row != dst_pixel_row_end) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          auto dst_pixel     = reinterpret_cast<uint32_t*> (dst_pixel_row);
          auto dst_pixel_end = reinterpret_cast<uint32_t*> (dst_pixel_row) + dst.m_impl->width;

          auto src_pixel_rowu = static_cast<uint32_t const*> (src.m_impl->pixel_rows[v >> 16]);
          auto src_pixel_rowl = static_cast<uint32_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = ((v & 0xffff) >> 12) | 0x100000;
          uint32_t u = 0;

          while (dst_pixel != dst_pixel_end) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU  = ((u & 0xffff) >> 12) | 0x100000;

              __asm__ __volatile__
                  ("\n\t pxor %%mm7, %%mm7"
                   /* Prefetching does not show improvement on my Duron (maybe due to its small cache?) */
                   /*"\n\t prefetch  64%[pixel_l]" / * only work on 3dnow!/SSE cpu */
                   /*"\n\t prefetchw 64%[output]"  / * only work on 3dnow!/SSE cpu */

                   /* Computing coefs values (Thread #1 and #2) => ends on #C
                    *
                    * notice 0x10 = 1.0 (fixed point 28.4 - like fracU and fracV)
                    *
                    * coef[0] = (0x10 - fracU) * (0x10 - fracV); * UL=0 *
                    * coef[1] = (0x10 - fracU) * fracV;          * LL=1 *
                    * coef[2] = fracU * (0x10 - fracV);          * UR=2 *
                    * coef[3] = fracU * fracV;                   * LR=3 *
                    */

                   /*
                    * Unpacking colors (Thread #3 and #4)
                    */
                   /*
                    * Multiplying colors by coefs (Threads #5 and #6)
                    */
                   /*
                    * Adding colors together. (Thread #7)
                    */

                   "#1\n\t movd %[fracu], %%mm4"   /* mm4 = [ 0 | 0 | 0x10 | fracU ] */
                   "#2\n\t movd %[fracv], %%mm6"   /* mm6 = [ 0 | 0 | 0x10 | fracV ] */

                   "#1\n\t punpcklwd %%mm4, %%mm4" /* mm4 = [ 0x10 | 0x10 | fracU | fracU ] */
                   "#2\n\t movq      %%mm6, %%mm3"

                   "#1\n\t pxor      %%mm5, %%mm5"
                   "#2\n\t punpckldq %%mm6, %%mm6" /* mm6 = [ 0x10 | fracv | 0x10 | fracV ] */
                   "#3\n\t movq %[pixel_u], %%mm0" /* mm0 = [ col[0] | col[2] ] */

                   "#1\n\t punpckldq %%mm4, %%mm5" /* mm5 = [ fracU | fracU | 0 | 0 ] */
                   "#2\n\t punpcklwd %%mm7, %%mm3" /* mm3 = [ 0    | 0x10  | 0    | fracV ] */
                   "#3\n\t movq      %%mm0, %%mm2"

                   "#1\n\t psubusw   %%mm5, %%mm4" /* mm4 = [ 0x10-fracU | 0x10-fracU | fracU | fracU ] */
                   "#2\n\t punpckldq %%mm3, %%mm3" /* mm3 = [ 0    | fracV | 0    | fracV ] */
                   "#4\n\t movq %[pixel_l], %%mm1" /* mm1 = [ col[1] | col[3] ] */

                   "#2\n\t pslld     $16,   %%mm3" /* mm3 = [ fracV | 0 | fracV | 0 ] */
                   "#3\n\t punpcklbw %%mm7, %%mm0" /* mm0 = [ col[0] unpacked ] */

                   "#2\n\t psubusw   %%mm3, %%mm6" /* mm6 = [ 0x10-fracV | fracV | 0x10-fracV | fracV ] */
                   "#4\n\t movq      %%mm1, %%mm3"

                   "#C\n\t pmullw    %%mm6, %%mm4" /* mm4 = [ coef[0]|coef[1]|coef[2]|coef[3] ] */
                   "#5\n\t movq      %%mm4, %%mm5"

                   "#4\n\t punpcklbw %%mm7, %%mm1" /* mm1 = [ col[1] unpacked ] */
                   "#6\n\t punpckhwd %%mm4, %%mm4" /* mm4 = [ coef[1]|coef[1]|coef[0]|coef[0] ] */

                   "#3\n\t punpckhbw %%mm7, %%mm2" /* mm2 = [ col[2] unpacked ] */
                   "#5\n\t punpcklwd %%mm5, %%mm5" /* mm5 = [ coef[2]|coef[2]|coef[3]|coef[3] ] */

                   "#4\n\t punpckhbw %%mm7, %%mm3" /* mm3 = [ col[3] unpacked ] */
                   "#5\n\t movq      %%mm5, %%mm6"

                   "#6\n\t movq      %%mm4, %%mm7"
                   "#5\n\t punpcklwd %%mm6, %%mm6" /* mm6 = [ coef[3]|coef[3]|coef[3]|coef[3] ] */

                   "#6\n\t punpcklwd %%mm7, %%mm7" /* mm6 = [ coef[1]|coef[1]|coef[1]|coef[1] ] */
                   "#5\n\t pmullw    %%mm6, %%mm3" /* mm3 = [ coef[3] * col[3] unpacked ] */

                   "#5\n\t punpckhwd %%mm5, %%mm5" /* mm5 = [ coef[2]|coef[2]|coef[2]|coef[2] ] */
                   "#6\n\t pmullw    %%mm7, %%mm1" /* mm1 = [ coef[1] * col[1] unpacked ] */

                   "#5\n\t pmullw    %%mm5, %%mm2" /* mm2 = [ coef[2] * col[2] unpacked ] */
                   "#6\n\t punpckhwd %%mm4, %%mm4" /* mm4 = [ coef[0]|coef[0]|coef[0]|coef[0] ] */

                   "#6\n\t pmullw    %%mm4, %%mm0" /* mm0 = [ coef[0] * col[0] unpacked ] */
                   "#7\n\t paddw     %%mm2, %%mm3"
                   "#7\n\t paddw     %%mm1, %%mm0"

                   "#7\n\t paddw     %%mm3, %%mm0"
                   "#7\n\t psrlw     $8,    %%mm0"

                   /* Unpacking the resulting pixel */
                   "\n\t packuswb  %%mm7, %%mm0"
                   "\n\t movd    %%mm0, %[output]"

                   : [output]  "=m"(*dst_pixel)
                   : [pixel_u] "m"(src_pixel_rowu[u>>16])
                     , [pixel_l] "m"(src_pixel_rowl[u>>16])
                     , [fracu]   "m"(fracU)
                     , [fracv]   "m"(fracV));

              dst_pixel++;
              u += du;
          }

          dst_pixel_row += dst.m_impl->pitch;
          v += dv;
      }

      __asm__ __volatile__ ("\n\t emms");
#endif /* !VISUAL_ARCH_X86 */
  }

} // LV namespace
