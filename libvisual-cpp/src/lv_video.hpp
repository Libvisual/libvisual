// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_video.hpp,v 1.3 2006-01-13 06:51:54 descender Exp $
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

#ifndef LV_VIDEO_HPP
#define LV_VIDEO_HPP

#include <libvisual/lv_video.h>
#include <lv_object.hpp>
#include <lv_function.hpp>
#include <lv_rectangle.hpp>

namespace Lv
{
  class Video
      : public Object
  {
  public:

      typedef VisVideoDepth         Depth;
      typedef VisVideoScaleMethod   ScaleMethod;
      typedef VisVideoRotateDegrees Rotation;
      typedef VisVideoMirrorOrient  MirrorOrient;
      typedef VisVideoCompositeType CompositeType;

      Video ()
          : Object (vis_video_to_object (visual_video_new ()))
      {}

      Video (int width, int height, Depth depth)
          : Object (vis_video_to_object (visual_video_new_with_buffer (width, height, depth)))
      {}

      Video (const Video& other)
          : Object (vis_video_to_object (visual_video_new ()))
      {
          visual_video_clone (&vis_video (),
                              const_cast<VisVideo *> (&other.vis_video ()));
      }

      Video (const Video& source, MirrorOrient orient)
          : Object (vis_video_to_object
                (visual_video_mirror_new (const_cast<VisVideo *> (&source.vis_video ()), orient)))
      {}

      inline void set_attributes (int width, int height, int pitch, Depth depth)
      {
          visual_video_set_attributes (&vis_video (), width, height, pitch, depth);
      }

      inline void set_dimension (int width, int height)
      {
          visual_video_set_dimension (&vis_video (), width, height);
      }

      inline void set_pitch (int pitch)
      {
          visual_video_set_pitch (&vis_video (), pitch);
      }

      inline void set_depth (Depth depth)
      {
          visual_video_set_depth (&vis_video (), depth);
      }

      inline void set_buffer (void *buffer)
      {
          visual_video_set_buffer (&vis_video (), buffer);
      }

      // inline Buffer *get_buffer ()
      // {}

      // inline void set_palette (Palette *palette)
      // {}

      const void *get_pixels () const
      {
          return visual_video_get_pixels (const_cast<VisVideo *> (&vis_video ()));
      }

      void *get_pixels ()
      {
          return visual_video_get_pixels (&vis_video ());
      }

      int get_size ()
      {
          return visual_video_get_size (&vis_video ());
      }

      void get_boundary (Rectangle& rect)
      {
          visual_video_get_boundary (&vis_video (), &rect.vis_rect ());
      }

      inline bool has_allocated_buffer ()
      {
          return visual_video_have_allocated_buffer (&vis_video ());
      }

      inline bool compare (const Video& other) const
      {
          return visual_video_compare (const_cast<VisVideo *> (&vis_video ()),
                                       const_cast<VisVideo *> (&other.vis_video ()));
      }

      inline bool compare_ignore_pitch (const Video& other) const
      {
          return visual_video_compare_ignore_pitch (const_cast<VisVideo *> (&vis_video ()),
                                                    const_cast<VisVideo *> (&other.vis_video ()));
      }

      friend bool operator == (const Video& lhs, const Video& rhs)
      {
          return lhs.compare (rhs);
      }

      friend bool operator != (const Video& lhs, const Video& rhs)
      {
          return !(lhs == rhs);
      }

      inline const VisVideo &vis_video () const
      {
          return *reinterpret_cast<const VisVideo *> (&vis_object ());
      }

      inline VisVideo &vis_video ()
      {
          return *reinterpret_cast<VisVideo *> (&vis_object ());
      }

  private:

      static VisObject *vis_video_to_object (VisVideo *video)
      {
          return reinterpret_cast<VisObject *> (video);
      }
  };

} // namespace Lv

#endif // #ifndef LV_VIDEO_HPP
