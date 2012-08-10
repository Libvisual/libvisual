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
#include "lv_palette.h"
#include "lv_common.h"

namespace LV {

  Palette::Palette (unsigned int ncolors)
      : colors (ncolors)
  {
      // empty
  }

  Palette::~Palette ()
  {
      // empty
  }

  void Palette::allocate_colors (unsigned int ncolors)
  {
      colors.resize (ncolors);
  }

  void Palette::blend (Palette const& src1, Palette const& src2, float rate)
  {
      visual_return_if_fail (src1.size () != src2.size ());
      visual_return_if_fail (size ()      != src1.size ());

      for (unsigned int i = 0; i < colors.size (); i++) {
          colors[i].r = src1.colors[i].r + ((src2.colors[i].r - src1.colors[i].r) * rate);
          colors[i].g = src1.colors[i].g + ((src2.colors[i].g - src1.colors[i].g) * rate);
          colors[i].b = src1.colors[i].b + ((src2.colors[i].b - src1.colors[i].b) * rate);
      }
  }

  VisColor Palette::color_cycle (float rate)
  {
      int irate = int (rate);

      float rdiff = rate - irate;

      // If rate is exactly an item, return that item
      if (rdiff == 0) {
          return colors[irate];
      }

      irate %= colors.size ();

      uint8_t alpha = rdiff * 255;

      auto const tmp1 = &colors[irate];
      Color const* tmp2;

      if (irate == int (size() - 1))
          tmp2 = &colors[0];
      else
          tmp2 = &colors[irate + 1];

      return Color ((alpha * (tmp1->r - tmp2->r) >> 8) + tmp2->r,
                    (alpha * (tmp1->g - tmp2->g) >> 8) + tmp2->g,
                    (alpha * (tmp1->b - tmp2->b) >> 8) + tmp2->b);
  }

  int Palette::find_color (Color const& color) const
  {
      for (unsigned int i = 0; i < colors.size(); i++) {
          if (colors[i] == color)
              return i;
      }

      return -1;
  }

} // LV namespace


