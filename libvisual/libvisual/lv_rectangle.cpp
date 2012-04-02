/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_rectangle.c,v 1.10 2006/02/13 20:54:08 synap Exp $
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

  Rect Rect::clip (Rect const& bounds, Rect const& r)
  {
      // Return an empty rectangle
      if (!bounds.intersects (r)) {
          return Rect ();
      }

      Rect result (r);

      // Left, Upper boundries
      if (r.x < bounds.x) {
          result.width = r.width - (bounds.x - r.x);
          result.x = bounds.x;
      }

      if (r.y < bounds.y) {
          result.height = r.height - (bounds.y - r.y);
          result.y = bounds.y;
      }

      // Right, Lower boundries
      if (result.x + result.width > bounds.width)
          result.width = bounds.width - result.x;

      if (result.y + result.height > bounds.height)
          result.height = bounds.height - result.y;

      return result;
  }

  void Rect::denormalize_points (float const* fxlist, float const* fylist, int32_t *xlist, int32_t *ylist, unsigned int size) const
  {
      visual_return_if_fail (fxlist != NULL);
      visual_return_if_fail (fylist != NULL);
      visual_return_if_fail (xlist  != NULL);
      visual_return_if_fail (ylist  != NULL);
      visual_return_if_fail (size > 0);

      visual_math_vectorized_floats_to_int32s_multiply (xlist, fxlist, size, width);
      visual_math_vectorized_floats_to_int32s_multiply (ylist, fylist, size, height);
  }

  void Rect::denormalize_points_neg (float const* fxlist, float const* fylist, int32_t *xlist, int32_t *ylist, unsigned int size) const
  {
      visual_return_if_fail (fxlist != NULL);
      visual_return_if_fail (fylist != NULL);
      visual_return_if_fail (xlist  != NULL);
      visual_return_if_fail (ylist  != NULL);
      visual_return_if_fail (size > 0);

      visual_math_vectorized_floats_to_int32s_multiply_denormalise (xlist, fxlist, size, width);
          visual_math_vectorized_floats_to_int32s_multiply_denormalise (ylist, fylist, size, height);
  }

} // LV namespace
