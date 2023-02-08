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

#ifndef _LV_COLOR_H
#define _LV_COLOR_H

#include <libvisual/lv_types.h>

/**
 * @defgroup VisColor VisColor
 * @{
 */

#ifdef __cplusplus

namespace LV {

  struct LV_API Color
  {
      uint8_t r;    /**< Red channel   */
      uint8_t g;    /**< Green channel */
      uint8_t b;    /**< Blue channel  */
      uint8_t a;    /**< Alpha channel */

      /**
       * Creates a new Color object. All channels are set to 0.
       */
      Color ()
          : r (0), g (0), b (0), a (0)
      {}

      /**
       * Creates a new Color object with the given channel values.
       *
       * @param r red
       * @param g green
       * @param b blue
       * @param a alpha
       */
      Color (uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
          : r (r_), g(g_), b(b_), a(a_)
      {}

      /**
       * Equality operator for comparing two Color objects.
       *
       * @note RGB channels are compared.
       *
       * @param c1 a Color object
       * @param c2 another Color object
       *
       * @return true if colors are equal, false otherwise
       */
      friend bool operator== (Color const& c1, Color const& c2)
      {
          return ( c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
      }

      /**
       * Assigns a color from a set of RGBA values
       *
       * @param r The red value.
       * @param g The green value.
       * @param b The blue value.
       * @param a The alpha value.
       */
      void set (uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
      {
          r = r_; g = g_; b = b_; a = a_;
      }

      /**
       * Assigns a color given in HSV colorspace.
       *
       * @param h Hue value in [0, 360]
       * @param s Saturation value in [0, 1]
       * @param v Value value in [0, 1]
       */
      void set_hsv (float h, float s, float v);

      /**
       * Converts the current color to the HSV colorspace
       *
       * @param h Variable to store the Hue value in (values will be
       *          in [0, 360]
       *
       * @param s Variable to store the Saturation value in (values
       *          will be in [0, 1]
       *
       * @param v Variable to store the Value value in (values will be
       *          in [0, 1]
       */
      void get_hsv (float& h, float& s, float& v) const;

      void set_from_uint16 (uint16_t rgb);
      void set_from_uint32 (uint32_t rgba);

      uint32_t to_uint32 () const;
      uint16_t to_uint16 () const;

      static Color const& white()
      {
          static Color color(255, 255, 255);
          return color;
      }

      static Color const& black()
      {
          static Color color(0, 0, 0);
          return color;
      }
  };

} // LV namespace

#endif // __cplusplus

LV_BEGIN_DECLS

#ifdef __cplusplus
typedef ::LV::Color VisColor;
#else
typedef struct _VisColor VisColor;
struct _VisColor
{
    uint8_t r, g, b, a; // NOTE: this must be synced with LV::Color
};
#endif

LV_NODISCARD LV_API VisColor *visual_color_new   (void);
LV_NODISCARD LV_API VisColor *visual_color_clone (VisColor *color);

LV_API void visual_color_free (VisColor *color);

LV_API int visual_color_compare (VisColor *src1, VisColor *src2);

LV_API void visual_color_copy     (VisColor *dest, VisColor *src);
LV_API void visual_color_set      (VisColor *color, uint8_t r, uint8_t g, uint8_t b);
LV_API void visual_color_set_rgba (VisColor *color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
LV_API void visual_color_set_hsv  (VisColor *color, float h, float s, float v);
LV_API void visual_color_get_hsv  (VisColor *color, float *h, float *s, float *v);

LV_API void visual_color_set_from_uint32 (VisColor *color, uint32_t rgb);
LV_API void visual_color_set_from_uint16 (VisColor *color, uint16_t rgb);

LV_API uint32_t visual_color_to_uint32 (VisColor *color);
LV_API uint16_t visual_color_to_uint16 (VisColor *color);

LV_API VisColor *visual_color_black (void);
LV_API VisColor *visual_color_white (void);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_COLOR_H */
