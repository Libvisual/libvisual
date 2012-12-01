/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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

extern "C" {

  VisPalette* visual_palette_new (unsigned int ncolors)
  {
      return new LV::Palette (ncolors);
  }

  void visual_palette_free (VisPalette *self)
  {
      delete self;
  }

  VisPalette *visual_palette_clone (VisPalette *self)
  {
      return new LV::Palette (*self);
  }

  void visual_palette_copy (VisPalette *self, VisPalette *src)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (src  != nullptr);

      *self = *src;
  }

  void visual_palette_allocate_colors (VisPalette *self, int ncolors)
  {
      visual_return_if_fail (self != nullptr);

      self->allocate_colors (ncolors);
  }

  unsigned int visual_palette_get_size (VisPalette *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->size ();
  }

  VisColor *visual_palette_get_colors (VisPalette *self)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);
      visual_return_val_if_fail (self->size() > 0, nullptr);

      return self->colors.data ();
  }

  VisColor *visual_palette_get_color (VisPalette *self, int index)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);
      visual_return_val_if_fail (self->size() > 0, nullptr);
      visual_return_val_if_fail (index < 0, nullptr);

      return &self->colors[index];
  }

  void visual_palette_blend (VisPalette *self, VisPalette *src1, VisPalette *src2, float rate)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (src1 != nullptr);
      visual_return_if_fail (src2 != nullptr);

      self->blend (*src1, *src2, rate);
  }

  int visual_palette_find_color (VisPalette *self, VisColor *color)
  {
      visual_return_val_if_fail (self != nullptr, -1);

      return self->find_color (*color);
  }

} // C extern
