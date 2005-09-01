// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_color.hpp,v 1.1 2005-09-01 07:10:25 descender Exp $
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

#ifndef LVCPP_COLOR_HPP
#define LVCPP_COLOR_HPP

#include <libvisual/lv_color.h>
#include <lv_object.hpp>

namespace Lv
{
  class Color
      : public Object
  {
  public:

      Color ()
          : Object (vis_color_to_object (visual_color_new ()))
      {}

      Color (const Color& other)
      {
          visual_color_copy (&vis_color (),
                             const_cast<VisColor *> (&other.vis_color ()));
      }

      Color (uint8_t r, uint8_t g, uint8_t b)
          : Object (vis_color_to_object (visual_color_new ()))
      {
          set (r, g, b);
      }

      Color& operator = (const Color& other)
      {
          visual_color_copy (&vis_color (), 
                             const_cast<VisColor *> (&other.vis_color ()));
          return *this;
      }

      friend inline bool operator == (const Color& lhs, const Color& rhs)
      {
          return visual_color_compare (const_cast<VisColor *> (&lhs.vis_color ()),
                                       const_cast<VisColor *> (&rhs.vis_color ()));
      }

      friend inline bool operator != (const Color& lhs, const Color& rhs)
      {
          return !(lhs == rhs);
      }

      inline void set (uint8_t r, uint8_t g, uint8_t b)
      {
          visual_color_set (&vis_color (), r, g, b);
      }

      inline void get (uint8_t &r, uint8_t &g, uint8_t &b) const
      {
          VisColor color = vis_color ();
          r = color.r;
          g = color.g;
          b = color.b;
      }

      inline void to_hsv (float &h, float &s, float &v) const
      {
          visual_color_to_hsv (const_cast<VisColor *> (&vis_color ()),
                               &h, &s, &v);
      }

      inline void from_hsv (float h, float s, float v)
      {
          visual_color_from_hsv (&vis_color (), h, s, v);
      }

      inline const VisColor& vis_color () const
      {
          return reinterpret_cast<const VisColor&> (vis_object ());
      }

      inline VisColor& vis_color ()
      {
          return reinterpret_cast<VisColor&> (vis_object ());
      }

  private:
      
      static VisObject *vis_color_to_object (VisColor *color)
      {
          return reinterpret_cast<VisObject *> (color);
      }
  };

} // namespace Lv

#endif // #ifdef LVCPP_COLOR_HPP
