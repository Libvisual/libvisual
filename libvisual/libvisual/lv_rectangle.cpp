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
#include "lv_rectangle.h"
#include "lv_common.h"
#include "lv_math.h"

namespace LV {

  bool Rect::intersects (Rect const& r) const
  {
      if (x > (r.x + r.width - 1))
          return false;

      if (y > (r.y + r.height - 1))
          return false;

      if (r.x > (x + width - 1))
          return false;

      if (r.y > (y + height - 1))
          return false;

      return true;
  }

  bool Rect::contains (Rect const& r) const
  {
      if (r.x < x)
          return false;

      if (r.y < y)
          return false;

      if ((r.x + r.width) > (x + width))
          return false;

      if ((r.y + r.height) > (y + height))
          return false;

      return true;
  }

  Rect Rect::clip (Rect const& r) const
  {
      // Return an empty rectangle
      if (!intersects (r)) {
          return Rect ();
      }

      auto result = r;

      // Left, Upper boundries
      if (r.x < x) {
          result.width = r.width - (x - r.x);
          result.x = x;
      }

      if (r.y < y) {
          result.height = r.height - (y - r.y);
          result.y = y;
      }

      // Right, Lower boundries
      if (result.x + result.width > width)
          result.width = width - result.x;

      if (result.y + result.height > height)
          result.height = height - result.y;

      return result;
  }

  void Rect::denormalize_points (float const* fxlist, float const* fylist, int32_t *xlist, int32_t *ylist, unsigned int size) const
  {
      visual_return_if_fail (fxlist != nullptr);
      visual_return_if_fail (fylist != nullptr);
      visual_return_if_fail (xlist  != nullptr);
      visual_return_if_fail (ylist  != nullptr);
      visual_return_if_fail (size > 0);

      visual_math_simd_denorm_floats_to_int32s (xlist, fxlist, width, size);
      visual_math_simd_denorm_floats_to_int32s (ylist, fylist, height, size);
  }

  void Rect::denormalize_points_neg (float const* fxlist, float const* fylist, int32_t *xlist, int32_t *ylist, unsigned int size) const
  {
      visual_return_if_fail (fxlist != nullptr);
      visual_return_if_fail (fylist != nullptr);
      visual_return_if_fail (xlist  != nullptr);
      visual_return_if_fail (ylist  != nullptr);
      visual_return_if_fail (size > 0);

      visual_math_simd_denorm_neg_floats_to_int32s (xlist, fxlist, size, width);
      visual_math_simd_denorm_neg_floats_to_int32s (ylist, fylist, size, height);
  }

} // LV namespace
