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

#ifndef _LV_PALETTE_H
#define _LV_PALETTE_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_color.h>

/**
 * @defgroup VisPalette VisPalette
 * @{
 */

#ifdef __cplusplus

#include <vector>

namespace LV {

  //! Colour palette class for indexed colour modes
  //!
  //! @note This is currently only used for 8-bit images
  //!
  struct LV_API Palette
  {
      using const_iterator = std::vector<Color>::const_iterator;
      using iterator = std::vector<Color>::iterator;

      std::vector<Color> colors;

      /**
       * Creates a new Palette object
       */
      Palette () = default;

      explicit Palette (unsigned int ncolors);

      Palette (Palette const&) = default;

      Palette (Palette&& palette)
          : colors (std::move (palette.colors))
      {}

      ~Palette ();

      Palette& operator= (Palette const& rhs) = default;

      Palette& operator= (Palette&& rhs)
      {
          colors.swap (rhs.colors);
          return *this;
      }

      constexpr friend bool operator== (Palette const& lhs, Palette const& rhs)
      {
          return lhs.colors == rhs.colors;
      }

      constexpr friend bool operator!= (Palette const& lhs, Palette const& rhs)
      {
          return !(lhs == rhs);
      }

      constexpr const_iterator cbegin () const noexcept
      {
          return colors.cbegin ();
      }

      constexpr const_iterator cend () const noexcept
      {
          return colors.cend ();
      }

      constexpr const_iterator begin () const noexcept
      {
          return colors.begin ();
      }

      constexpr const_iterator end () const noexcept
      {
          return colors.end ();
      }

      constexpr iterator begin () noexcept
      {
          return colors.begin ();
      }

      constexpr iterator end () noexcept
      {
          return colors.end ();
      }

      bool empty () const
      {
          return colors.empty ();
      }

      unsigned int size () const
      {
          return colors.size ();
      }

      /**
       * This function is capable of morphing from one palette to another.
       *
       * @param src1 Pointer to a VisPalette that acts as the first source for the morph.
       * @param src2 Pointer to a VisPalette that acts as the second source for the morph.
       * @param rate Value that sets the rate of the morph, which is valid between 0 and 1.
       */
      void blend (Palette const& src1, Palette const& src2, float rate);


      /**
       * Allocate an amount of colors for a VisPalette.
       *
       * @param size Number of colors to allocate
       */
      void allocate_colors (unsigned int size);

      /**
       * Can be used to cycle through the colors of a VisPalette and
       * blend between elements. The rate is from 0.0 to number of
       * VisColors in the VisPalette. The VisColor is newly allocated
       * so you have to unref it. The last VisColor in the VisPalette
       * is morphed with the first.
       *
       * @param rate Selection of the VisColor from the VisPalette, goes from 0.0 to number of VisColors in the VisPalette
       *    and morphs between colors if needed.
       *
       * @return A new Color, possibly a morph between two Colors
       */
      Color color_cycle (float rate);

      int find_color (Color const& color) const;
  };

} // LV namespace

#endif // __cplusplus

#ifdef __cplusplus
typedef struct LV::Palette VisPalette;
#else
typedef struct _VisPalette VisPalette;
struct _VisPalette;
#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisPalette *visual_palette_new (unsigned int ncolors);

LV_NODISCARD LV_API VisPalette *visual_palette_clone (VisPalette *self);

LV_API void visual_palette_free (VisPalette *palette);

LV_API void visual_palette_copy (VisPalette *dest, VisPalette *src);

LV_API VisColor *visual_palette_get_colors (VisPalette *pal);

LV_API VisColor *visual_palette_get_color (VisPalette *pal, int index);

LV_API unsigned int visual_palette_get_size (VisPalette *pal);

LV_API void visual_palette_blend (VisPalette *dest, VisPalette *src1, VisPalette *src2, float rate);

LV_API int visual_palette_find_color (VisPalette *pal, VisColor *color);


LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_PALETTE_H */
