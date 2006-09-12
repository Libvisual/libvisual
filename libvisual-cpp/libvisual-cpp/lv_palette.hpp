// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2006 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_palette.hpp,v 1.1 2006-09-12 02:36:00 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LV_PALETTE_HPP
#define LV_PALETTE_HPP

#include <libvisual/lv_video.h>
#include <libvisual-cpp/lv_object.hpp>
#include <libvisual-cpp/lv_color.hpp>

namespace Lv
{
  class Palette
      : public Object
  {
  public:

      typedef unsigned int size_type;

      Palette (int n_colors)
          : Object (vis_palette_to_object (visual_palette_new (n_colors)))
      {}

      inline void copy (const Palette& other)
      {
          visual_palette_copy (&vis_palette (), const_cast<VisPalette *> (&other.vis_palette ()));
      }

      inline void allocate_colors (size_type n_colors)
      {
          visual_palette_allocate_colors (&vis_palette (), n_colors);
      }

      inline void free_colors ()
      {
          visual_palette_free_colors (&vis_palette ());
      }

      inline size_type find_color (const Color& color) const
      {
          return size_type (visual_palette_find_color (const_cast<VisPalette *> (&vis_palette ()),
                                                       const_cast<VisColor *> (&color.vis_color ())));
      }

      inline const VisPalette& vis_palette () const
      {
          return *reinterpret_cast<const VisPalette *> (&vis_object ());
      }

      inline VisPalette& vis_palette ()
      {
          return *reinterpret_cast<VisPalette *> (&vis_object ());
      }

  private:

      static inline VisObject *vis_palette_to_object (VisPalette *palette)
      {
          return reinterpret_cast<VisObject *> (palette);
      }
  };

} // namespace Lv

#endif // #ifndef LV_PALETTE_HPP
