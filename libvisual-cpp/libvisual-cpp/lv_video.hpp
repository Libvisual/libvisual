// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_video.hpp,v 1.5 2006-09-12 04:00:20 descender Exp $
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
#include <libvisual-cpp/lv_object.hpp>
#include <libvisual-cpp/lv_function.hpp>
#include <libvisual-cpp/lv_rectangle.hpp>
#include <libvisual-cpp/lv_palette.hpp>
#include <libvisual-cpp/lv_flags_wrapper.hpp>

namespace Lv
{
  class Video
      : public Object
  {
  public:

      enum Depth
      {
          DEPTH_ERROR   = VISUAL_VIDEO_DEPTH_ERROR,
          DEPTH_NONE    = VISUAL_VIDEO_DEPTH_NONE,
          DEPTH_8BIT    = VISUAL_VIDEO_DEPTH_8BIT,
          DEPTH_16BIT   = VISUAL_VIDEO_DEPTH_16BIT,
          DEPTH_24BIT   = VISUAL_VIDEO_DEPTH_24BIT,
          DEPTH_32BIT   = VISUAL_VIDEO_DEPTH_32BIT,
          DEPTH_GL      = VISUAL_VIDEO_DEPTH_GL,
          DEPTH_ENDLIST = VISUAL_VIDEO_DEPTH_ENDLIST,
          DEPTH_ALL     = VISUAL_VIDEO_DEPTH_ALL
      };

      enum ScaleMethod
      {
          SCALE_NEAREST  = VISUAL_VIDEO_SCALE_NEAREST,
          SCALE_BILINEAR = VISUAL_VIDEO_SCALE_BILINEAR
      };

      enum Rotation
      {
          ROTATE_NONE = VISUAL_VIDEO_ROTATE_NONE,
          ROTATE_90   = VISUAL_VIDEO_ROTATE_90,
          ROTATE_180  = VISUAL_VIDEO_ROTATE_180,
          ROTATE_270  = VISUAL_VIDEO_ROTATE_270
      };

      enum MirrorOrient
      {
          MIRROR_NONE = VISUAL_VIDEO_MIRROR_NONE,
          MIRROR_X    = VISUAL_VIDEO_MIRROR_X,
          MIRROR_Y    = VISUAL_VIDEO_MIRROR_Y
      };

      typedef VisVideoCompositeType CompositeType;

      Video ()
          : Object (vis_video_to_object (visual_video_new ()))
      {}

      Video (int width, int height, Depth depth)
          : Object (vis_video_to_object
                (visual_video_new_with_buffer (width, height, VisVideoDepth (depth))))
      {}

      Video (const Video& other)
          : Object (vis_video_to_object (visual_video_new ()))
      {
          visual_video_clone (&vis_video (),
                              const_cast<VisVideo *> (&other.vis_video ()));
      }

      Video (const Video& source, MirrorOrient orient)
          : Object (vis_video_to_object
                (visual_video_mirror_new (const_cast<VisVideo *> (&source.vis_video ()),
                                          VisVideoMirrorOrient (orient))))
      {}

      Video (const Video& source, Rotation rotation)
          : Object (vis_video_to_object
                (visual_video_rotate_new (const_cast<VisVideo *> (&source.vis_video ()),
                                          VisVideoRotateDegrees (rotation))))
      {}

      Video (const Video& source, ScaleMethod scale_method, float zoom_factor)
          : Object (vis_video_to_object
                (visual_video_zoom_new (const_cast<VisVideo *> (&source.vis_video ()),
                                        VisVideoScaleMethod (scale_method),
                                        zoom_factor)))
      {}

      inline void set_attributes (int width, int height, int pitch, Depth depth)
      {
          visual_video_set_attributes (&vis_video (), width, height, pitch, VisVideoDepth (depth));
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
          visual_video_set_depth (&vis_video (), VisVideoDepth (depth));
      }

      inline void set_buffer (void *buffer)
      {
          visual_video_set_buffer (&vis_video (), buffer);
      }

      // inline Buffer *get_buffer ()
      // {}

      inline void set_palette (const Palette& palette)
      {
          visual_video_set_palette (&vis_video (), const_cast<VisPalette *> (&palette.vis_palette ()));
      }

      const void *get_pixels () const
      {
          return visual_video_get_pixels (const_cast<VisVideo *> (&vis_video ()));
      }

      void *get_pixels ()
      {
          return visual_video_get_pixels (&vis_video ());
      }

      int get_size () const
      {
          return visual_video_get_size (const_cast<VisVideo *> (&vis_video ()));
      }

      void get_boundary (Rectangle& rect) const
      {
          visual_video_get_boundary (const_cast<VisVideo *> (&vis_video ()), &rect.vis_rect ());
      }

      inline bool has_allocated_buffer () const
      {
          return visual_video_have_allocated_buffer (const_cast<VisVideo *> (&vis_video ()));
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

      inline void rotate (const Video& source, Rotation rotation)
      {
          // FIXME: visual_video_rotate() may return VISUAL_ERROR_VIDEO_INVALID_ROTATE
          visual_video_rotate (&vis_video (), const_cast<VisVideo *> (&source.vis_video ()),
                               VisVideoRotateDegrees (rotation));
      }

      inline void scale (const Video& source, ScaleMethod scale_method)
      {
          // FIXME: visual_video_rotate() may return VISUAL_ERROR_VIDEO_INVALID_SCALE_METHOD, etc.
          visual_video_scale (&vis_video (), const_cast<VisVideo *> (&source.vis_video ()),
                              VisVideoScaleMethod (scale_method));
      }

      inline void depth_transform (const Video &source)
      {
          // FIXME: visual_video_depth_transform() may return VISUAL_ERROR_PALETTE_NULL, etc.
          visual_video_depth_transform (&vis_video (), const_cast<VisVideo *> (&source.vis_video ()));
      }

      inline const VisVideo& vis_video () const
      {
          return *reinterpret_cast<const VisVideo *> (&vis_object ());
      }

      inline VisVideo& vis_video ()
      {
          return *reinterpret_cast<VisVideo *> (&vis_object ());
      }

  private:

      static VisObject *vis_video_to_object (VisVideo *video)
      {
          return reinterpret_cast<VisObject *> (video);
      }
  };

  LV_FLAGS_WRAP (Video::Depth)

} // namespace Lv

#endif // #ifndef LV_VIDEO_HPP
