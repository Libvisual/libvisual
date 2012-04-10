/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_color.c,v 1.18.2.1 2006/03/04 12:32:47 descender Exp $
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
#include "lv_color.h"
#include "lv_common.h"

struct rgb16_t {
#if VISUAL_LITTLE_ENDIAN == 1
  uint16_t b:5, g:6, r:5;
#else
  uint16_t r:5, g:6, b:5;
#endif
};

namespace LV {

  void Color::set_hsv (float h, float s, float v)
  {
      int i;
      float f, w, q, t, r = 0, g = 0, b = 0;

      if (s == 0.0)
          s = 0.000001;

      if (h == 360.0)
          h = 0.0;

      h = h / 60.0;
      i = int (h);
      f = h - i;
      w = v * (1.0 - s);
      q = v * (1.0 - (s * f));
      t = v * (1.0 - (s * (1.0 - f)));

      switch (i) {
          case 0: r = v; g = t; b = w; break;
          case 1: r = q; g = v; b = w; break;
          case 2: r = w; g = v; b = t; break;
          case 3: r = w; g = q; b = v; break;
          case 4: r = t; g = w; b = v; break;
          case 5: r = v; g = w; b = q; break;

          default:
              break;
      }

      set (r * 255, g * 255, b * 255);
  }

  void Color::get_hsv (float& h, float& s, float& v) const
  {
      float rf = r / 255.0;
      float gf = g / 255.0;
      float bf = b / 255.0;

      float max = rf;

      if (gf > max)
          max = gf;

      if (bf > max)
          max = bf;

      float min = rf;

      if (gf < min)
          min = gf;

      if (bf < min)
          min = bf;

      v = max;

      if (max != 0.0)
          s = (max - min) / max;
      else
          s = 0.0;

      if (s == 0.0) {
          h = 0.0;
      } else {
          float delta = max - min;

          if (rf == max)
              h = (gf - bf) / delta;
          else if (gf == max)
              h = 2.0 + (bf - rf) / delta;
          else if (b == max)
              h = 4.0 + (rf - gf) / delta;

          h *= 60.0;

          if (h < 0.0)
              h += 360;
      }
  }

  void Color::set_from_uint32 (uint32_t rgba)
  {
      uint8_t const* color = reinterpret_cast<uint8_t*> (&rgba);

#if VISUAL_LITTLE_ENDIAN == 1
      b = color[0];
      g = color[1];
      r = color[2];
      a = color[3];
#else
      a = color[0];
      r = color[1];
      g = color[2];
      b = color[3];
#endif // VISUAL_LITTLE_ENDIAN
  }

  void Color::set_from_uint16 (uint16_t rgb)
  {
      rgb16_t const& color = *(reinterpret_cast<rgb16_t*> (&rgb));

      r = color.r << 3;
      g = color.g << 2;
      b = color.b << 3;
  }

  uint32_t Color::to_uint32 () const
  {
      return ( (a << 24) | (r << 16) | (g << 8) | b );
  }

  uint16_t Color::to_uint16 () const
  {
      rgb16_t color;

      color.r = r >> 3;
      color.g = g >> 2;
      color.b = b >> 3;

      return *reinterpret_cast<uint16_t*> (&color);
  }

} // LV namespace
