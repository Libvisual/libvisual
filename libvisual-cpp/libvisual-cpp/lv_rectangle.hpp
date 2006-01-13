// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_rectangle.hpp,v 1.2 2006-01-13 07:44:44 descender Exp $
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

#ifndef LVCPP_RECTANGLE_HPP
#define LVCPP_RECTANGLE_HPP

#include <libvisual/lv_rectangle.h>
#include <libvisual-cpp/lv_object.hpp>

namespace Lv
{

  class Rectangle
      : public Object
  {
  public:

      Rectangle (int x = 0, int y = 0, int width = 0, int height = 0)
          : Object (vis_rect_to_object (visual_rectangle_new (x, y, width, height)))
      {}

      Rectangle (const Rectangle& other)
          : Object (vis_rect_to_object (visual_rectangle_new (0, 0, 0, 0)))
      {
          visual_rectangle_copy (&vis_rect (),
                                 const_cast<VisRectangle *> (&other.vis_rect ()));
      }

      inline const Rectangle& operator = (const Rectangle& other)
      {
          visual_rectangle_copy (&vis_rect (),
                                 const_cast<VisRectangle *> (&other.vis_rect ()));
          return *this;
      }

      // We're forced to provide the following functions because x, y,
      // width and height are public fields and 'rect.vis_rect ().width = x'
      // is uglier than 'rect.width () = x'.

      inline int& x () { return vis_rect ().x; }
      inline int& y () { return vis_rect ().y; }

      inline int& width  () { return vis_rect ().width; }
      inline int& height () { return vis_rect ().height; }

      inline int get_x () const { return vis_rect ().x; }
      inline int get_y () const { return vis_rect ().y; }

      inline int get_width  () const { return vis_rect ().width; }
      inline int get_height () const { return vis_rect ().height; }

      inline void set (int x, int y, int width, int height)
      {
          visual_rectangle_set (&vis_rect (), x, y, width, height);
      }

      inline bool is_empty ()
      {
          return visual_rectangle_is_empty (&vis_rect ());
      }

      inline bool contains (int x, int y)
      {
          return visual_rectangle_position_within (&vis_rect (), x, y);
      }

      inline bool contains (const Rectangle& other)
      {
          return visual_rectangle_within (&vis_rect (),
                                          const_cast<VisRectangle *> (&other.vis_rect ()));
      }

      inline bool intersects (const Rectangle& other)
      {
          return visual_rectangle_within_partially (&vis_rect (),
                                                    const_cast<VisRectangle *> (&other.vis_rect ()));
      }

      inline int clip (const Rectangle& boundary,
                       const Rectangle& rect)
      {
          return visual_rectangle_clip (&vis_rect (),
                                        const_cast<VisRectangle *> (&boundary.vis_rect ()),
                                        const_cast<VisRectangle *> (&rect.vis_rect ()));
      }

      inline void normalise ()
      {
          visual_rectangle_normalise (&vis_rect ());
      }

      inline void normalise_to (const Rectangle& target)
      {
          visual_rectangle_normalise_to (&vis_rect (),
                                         const_cast<VisRectangle *> (&target.vis_rect ()));
      }

      inline void denormalise_value (float fx, float fy, int& x, int& y)
      {
          visual_rectangle_denormalise_values (&vis_rect (), fx, fy,
                                               &x, &y);
      }

      inline void denormalise_values (const float *fx_list,
                                      const float *fy_list,
                                      int *x_list,
                                      int *y_list,
                                      int size)
      {
          visual_rectangle_denormalise_many_values (&vis_rect (),
                                                    const_cast<float *> (fx_list),
                                                    const_cast<float *> (fy_list),
                                                    x_list,
                                                    y_list,
                                                    size);
      }

      inline void denormalise_value_neg (float fx, float fy, int& x, int& y)
      {
          visual_rectangle_denormalise_values_neg (&vis_rect (), fx, fy, &x, &y);
      }

      inline void denormalise_values_neg (const float *fx_list,
                                          const float *fy_list,
                                          int *x_list,
                                          int *y_list,
                                          int size)
      {
          visual_rectangle_denormalise_many_values_neg (&vis_rect (),
                                                        const_cast<float *> (fx_list),
                                                        const_cast<float *> (fy_list),
                                                        x_list,
                                                        y_list,
                                                        size);
      }

      inline const VisRectangle &vis_rect () const
      {
          return *reinterpret_cast<const VisRectangle *> (&vis_object ());
      }

      inline VisRectangle &vis_rect ()
      {
          return *reinterpret_cast<VisRectangle *> (&vis_object ());
      }

  private:

      static VisObject *vis_rect_to_object (VisRectangle *rect)
      {
          return reinterpret_cast<VisObject *> (rect);
      }
  };

} // namespace Lv

#endif // #ifndef LVCPP_RECTANGLE_HPP
