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

namespace LV {

  void VideoBlit::blit_overlay_alphasrc_mmx (Video* dest, Video* src)
  {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
      auto destbuf = static_cast<uint8_t*> (dest->get_pixels ());
      auto srcbuf = static_cast<uint8_t const*> (src->get_pixels ());

      for (int i = 0; i < src->m_impl->height; i++) {
          for (int j = 0; j < src->m_impl->width; j++) {
              __asm __volatile
                  ("\n\t movd %[spix], %%mm0"
                   "\n\t movd %[dpix], %%mm1"
                   "\n\t movq %%mm0, %%mm2"
                   "\n\t movq %%mm0, %%mm3"
                   "\n\t psrlq $24, %%mm2"  /* The alpha */
                   "\n\t movq %%mm0, %%mm4"
                   "\n\t psrld $24, %%mm3"
                   "\n\t psrld $24, %%mm4"
                   "\n\t psllq $32, %%mm2"
                   "\n\t psllq $16, %%mm3"
                   "\n\t por %%mm4, %%mm2"
                   "\n\t punpcklbw %%mm6, %%mm0"    /* interleaving dest */
                   "\n\t por %%mm3, %%mm2"
                   "\n\t punpcklbw %%mm6, %%mm1"    /* interleaving source */
                   "\n\t psubsw %%mm1, %%mm0"   /* (src - dest) part */
                   "\n\t pmullw %%mm2, %%mm0"   /* alpha * (src - dest) */
                   "\n\t psrlw $8, %%mm0"       /* / 256 */
                   "\n\t paddb %%mm1, %%mm0"    /* + dest */
                   "\n\t packuswb %%mm0, %%mm0"
                   "\n\t movd %%mm0, %[dest]"
                   : [dest] "=m" (*destbuf)
                   : [dpix] "m" (*destbuf)
                   , [spix] "m" (*srcbuf));

              destbuf += 4;
              srcbuf += 4;
          }

          destbuf += dest->m_impl->pitch - (dest->m_impl->width * dest->m_impl->bpp);
          srcbuf += src->m_impl->pitch - (src->m_impl->width * src->m_impl->bpp);
      }
#endif /* !VISUAL_ARCH_X86 */
  }

} // LV namespace
