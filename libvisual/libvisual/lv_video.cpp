/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *      Duilio J. Protti <dprotti@users.sourceforge.net>
 *      Chong Kai Xiong <kaixiong@codeleft.sg>
 *      Jean-Christophe Hoelt <jeko@ios-software.com>
 *      Jaak Randmets <jaak.ra@gmail.com>
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
#include "lv_video.h"
#include "lv_color.h"
#include "lv_common.h"
#include "lv_cpu.h"
#include "private/lv_video_private.hpp"
#include "private/lv_video_blit.hpp"
#include "private/lv_video_convert.hpp"
#include "private/lv_video_fill.hpp"
#include "private/lv_video_transform.hpp"

namespace LV {

  namespace {

    inline int is_valid_scale_method (VisVideoScaleMethod scale_method)
    {
        return scale_method == VISUAL_VIDEO_SCALE_NEAREST
            || scale_method == VISUAL_VIDEO_SCALE_BILINEAR;
    }

  } // anonymous namespace


  Video::Impl::Impl ()
      : width   (0)
      , height  (0)
      , depth   (VISUAL_VIDEO_DEPTH_NONE)
      , bpp     (0)
      , pitch   (0)
      , buffer  (Buffer::create ())
      , parent  (0)
  {}

  Video::Impl::~Impl ()
  {
      // empty
  }

  /* Precomputation functions */
  void Video::Impl::precompute_row_table ()
  {
      uint8_t* ptr = static_cast<uint8_t *> (buffer->get_data ());

      for (int y = 0; y < height; y++, ptr += pitch)
          pixel_rows[y] = ptr;
  }

  Video::Video ()
      : m_impl      (new Impl)
      , m_ref_count (1)
  {
      // empty
  }

  VideoPtr Video::create ()
  {
      return VideoPtr (new Video);
  }

  VideoPtr Video::create (int width, int height, VisVideoDepth depth)
  {
      VideoPtr self (new Video);

      self->set_depth (depth);
      self->set_dimension (width, height);
      self->allocate_buffer ();

      return self;
  }

  VideoPtr Video::create_sub (VideoConstPtr const& src, Rect const& area)
  {
      visual_return_val_if_fail (!area.empty (), 0);

      VideoPtr self (new Video);

      Rect const& vrect = src->get_extents ();

      if (!vrect.contains (area))
          return VideoPtr ();

      self->m_impl->extents = area;
      self->m_impl->parent  = src;

      self->set_attrs (area.width, area.height, src->m_impl->pitch, src->m_impl->depth);
      self->set_buffer (src->get_pixel_ptr (area.x, area.y));

      self->m_impl->compose_type = src->m_impl->compose_type;
      self->m_impl->compose_func = src->m_impl->compose_func;

      self->m_impl->colorkey = src->m_impl->colorkey;
      self->m_impl->alpha    = src->m_impl->alpha;

      self->set_palette (src->m_impl->palette);

      return self;
  }

  VideoPtr Video::create_sub (Rect const& drect, VideoConstPtr const& src, Rect const& srect)
  {
      Rect sbound = src->m_impl->extents;

      Rect rsrect = srect;
      rsrect.clip (sbound, srect);
      rsrect.clip (drect, rsrect);

      return create_sub (src, rsrect);
  }

  VideoPtr Video::wrap (void *data, bool owner, int width, int height, VisVideoDepth depth)
  {
      // FIXME: Support this
      if (owner) {
          visual_log (VISUAL_LOG_WARNING, "Memory leak: Ownership of wrapped pixel buffer is not supported at the moment");
      }

      VideoPtr self (new Video);

      self->set_depth (depth);
      self->set_dimension (width, height);
      self->set_buffer (data);

      return self;
  }

  VideoPtr Video::create_scale_depth (VideoConstPtr const& src,
                                      int                  width,
                                      int                  height,
                                      VisVideoDepth        depth,
                                      VisVideoScaleMethod  scale_method)
  {
      VideoPtr self = create (width, height, depth);
      self->scale_depth (src, scale_method);

      return self;
  }

  Video::~Video ()
  {
      // empty
  }

  void Video::ref () const
  {
      m_ref_count++;
  }

  void Video::unref () const
  {
      if (--m_ref_count == 0) {
          delete this;
      }
  }

  void Video::free_buffer ()
  {
      m_impl->pixel_rows.clear ();

      m_impl->buffer->set (NULL, 0);
  }

  bool Video::allocate_buffer ()
  {
      visual_return_val_if_fail (m_impl->buffer, false);
      visual_return_val_if_fail (m_impl->depth != VISUAL_VIDEO_DEPTH_GL, false);

      if (get_pixels ()) {
          if (m_impl->buffer->is_allocated ()) {
              free_buffer ();
          } else {
              visual_log (VISUAL_LOG_ERROR, "Not allocating buffer for Video with an external screen buffer");
              return false;
          }
      }

      if (get_size () == 0) {
          m_impl->buffer->set_data (NULL);
          return false;
      }

      m_impl->buffer->allocate (get_size ());

      m_impl->pixel_rows.resize (m_impl->height);
      m_impl->precompute_row_table ();

      return true;
  }

  bool Video::has_allocated_buffer () const
  {
      return m_impl->buffer->is_allocated ();
  }

  void Video::copy_attrs (VideoConstPtr const& src)
  {
      set_depth (src->m_impl->depth);
      set_dimension (src->m_impl->width, src->m_impl->height);
      set_pitch (src->m_impl->pitch);
  }

  bool Video::compare_attrs (VideoConstPtr const& src) const
  {
      if (!compare_attrs_ignore_pitch (src))
          return false;

      if (m_impl->pitch != src->m_impl->pitch)
          return false;

      return true;
  }

  bool Video::compare_attrs_ignore_pitch (VideoConstPtr const& src) const
  {
      if (m_impl->depth != src->m_impl->depth)
          return false;

      if (m_impl->width != src->m_impl->width)
          return false;

      if (m_impl->height != src->m_impl->height)
          return false;

      /* We made it to the end, the Videos are likewise in depth, pitch, dimensions */
      return true;
  }

  void Video::set_palette (Palette const& palette)
  {
      m_impl->palette = palette;
  }

  Palette const& Video::get_palette () const
  {
      return m_impl->palette;
  }

  Palette& Video::get_palette ()
  {
      return m_impl->palette;
  }

  void Video::set_buffer (void *ptr)
  {
      if (m_impl->buffer->is_allocated ()) {
          visual_log (VISUAL_LOG_ERROR,
                      "Trying to set a screen buffer on a Video object pointing to an allocated screen buffer");
          return;
      }

      m_impl->buffer->set_data (ptr);

      m_impl->pixel_rows.clear ();

      if (m_impl->buffer->get_data ()) {
          m_impl->pixel_rows.resize (m_impl->height);
          m_impl->precompute_row_table ();
      }
  }

  void Video::set_dimension (int width, int height)
  {
      m_impl->width  = width;
      m_impl->height = height;
      m_impl->pitch  = m_impl->width * m_impl->bpp;

      m_impl->buffer->set_size (m_impl->pitch * m_impl->height);
  }

  int Video::get_width () const
  {
      return m_impl->width;
  }

  int Video::get_height () const
  {
      return m_impl->height;
  }

  void Video::set_pitch (int pitch)
  {
      visual_return_if_fail (pitch > 0);

      if (m_impl->bpp <= 0)
          return;

      m_impl->pitch = pitch;
      m_impl->buffer->set_size (m_impl->pitch * m_impl->height);
  }

  int Video::get_pitch () const
  {
      return m_impl->pitch;
  }

  void Video::set_depth (VisVideoDepth depth)
  {
      m_impl->depth = depth;
      m_impl->bpp = visual_video_bpp_from_depth (m_impl->depth);
  }

  VisVideoDepth Video::get_depth () const
  {
      return m_impl->depth;
  }

  int Video::get_bpp () const
  {
      return m_impl->bpp;
  }

  void Video::set_attrs (int width, int height, int pitch, VisVideoDepth depth)
  {
      set_depth (depth);
      set_dimension (width, height);
      set_pitch (pitch);
  }

  std::size_t Video::get_size () const
  {
      return m_impl->pitch * m_impl->height;
  }

  BufferPtr Video::get_buffer () const
  {
      return m_impl->buffer;
  }

  Rect const& Video::get_extents () const
  {
      return m_impl->extents;
  }

  void* Video::get_pixels () const
  {
      return m_impl->buffer->get_data ();
  }

  void* Video::get_pixel_ptr (int x, int y) const
  {
      return static_cast<uint8_t*> (m_impl->pixel_rows[y]) + x * m_impl->bpp;
  }

  void Video::set_compose_type (VisVideoComposeType type)
  {
      m_impl->compose_type = type;
  }

  void Video::set_compose_colorkey (Color const& color)
  {
      m_impl->colorkey->set (0, 0, 0);
  }

  void Video::set_compose_surface (uint8_t alpha)
  {
      m_impl->alpha = alpha;
  }

  void Video::set_compose_function (VisVideoComposeFunc compose_func)
  {
      m_impl->compose_func = compose_func;
  }

  VisVideoComposeFunc Video::get_compose_function (VideoConstPtr const& src, bool alpha)
  {
      switch (src->m_impl->compose_type) {
          case VISUAL_VIDEO_COMPOSE_TYPE_NONE:
              return VideoBlit::blit_overlay_noalpha;

          case VISUAL_VIDEO_COMPOSE_TYPE_SRC:
              if (!alpha || src->m_impl->depth != VISUAL_VIDEO_DEPTH_32BIT)
                  return VideoBlit::blit_overlay_noalpha;
              else
                  return VideoBlit::blit_overlay_alphasrc;

          case VISUAL_VIDEO_COMPOSE_TYPE_COLORKEY:
              return VideoBlit::blit_overlay_colorkey;

          case VISUAL_VIDEO_COMPOSE_TYPE_SURFACE:
              return VideoBlit::blit_overlay_surfacealpha;

          case VISUAL_VIDEO_COMPOSE_TYPE_SURFACECOLORKEY:
              return VideoBlit::blit_overlay_surfacealphacolorkey;

          case VISUAL_VIDEO_COMPOSE_TYPE_CUSTOM:
              return src->m_impl->compose_func;

          default:
              return NULL;
      }
  }

  void Video::blit (Rect const&          drect,
                    VideoConstPtr const& src,
                    Rect const&          srect,
                    bool                 alpha)
  {
      VisVideoComposeFunc func = get_compose_function (src, alpha);

      compose (drect, src, srect, func);
  }

  void Video::compose (Rect const&          drect,
                       VideoConstPtr const& src,
                       Rect const&          srect,
                       VisVideoComposeFunc  compose_func)
  {
      Rect ndrect = drect;
      ndrect.normalize_to (srect);

      VideoPtr vsrc = create_sub (ndrect, src, srect);
      compose (vsrc, drect.x, drect.y, compose_func);
  }

  void Video::blit_scale (Rect const&          drect,
                          VideoConstPtr const& src,
                          Rect const&          srect,
                          bool                 alpha,
                          VisVideoScaleMethod  scale_method)
  {
      VisVideoComposeFunc func = get_compose_function (src, alpha);

      return compose_scale (drect, src, srect, scale_method, func);
  }

  void Video::compose_scale (Rect  const&         drect,
                             VideoConstPtr const& src,
                             Rect const&          srect,
                             VisVideoScaleMethod  scale_method,
                             VisVideoComposeFunc  compose_func)
  {
      Rect sbound = m_impl->extents;
      if (!sbound.intersects (drect))
          return;

      VideoPtr ssrc = create_sub (src, srect);

      VideoPtr svid = create();
      svid->set_attrs (drect.width,
                       drect.height,
                       src->m_impl->bpp * drect.width,
                       src->m_impl->depth);
      svid->allocate_buffer ();

      svid->scale (ssrc, scale_method);

      Rect frect = drect;
      frect.normalize ();

      compose (drect, svid, frect, compose_func);
  }

  void Video::blit (VideoConstPtr const& src, int x, int y, bool alpha)
  {
      VisVideoComposeFunc func = get_compose_function (src, alpha);

      compose (src, x, y, func);
  }

  void Video::compose (VideoConstPtr const& src, int x, int y, VisVideoComposeFunc compose_func)
  {
      visual_return_if_fail (compose_func != NULL);

      visual_return_if_fail (m_impl->depth != VISUAL_VIDEO_DEPTH_GL);
      visual_return_if_fail (src->m_impl->depth != VISUAL_VIDEO_DEPTH_GL);

      Rect drect = get_extents ();
      Rect srect = src->get_extents ();

      if (!drect.intersects (srect))
          return;

      VideoPtr transform;

      /* We're not the same depth, converting */
      if (m_impl->depth != src->m_impl->depth) {
          transform = create (src->m_impl->width, src->m_impl->height, m_impl->depth);
          transform->convert_depth (src);
      }

      /* Setting all the pointers right */
      VideoConstPtr srcp = transform ? VideoConstPtr (transform) : src;

      /* Negative offset fixture */
      if (x < 0) {
          srect.x -= x;
          srect.width += x;
          x = 0;
      }

      if (y < 0) {
          srect.y -= y;
          srect.height += y;
          y = 0;
      }

      /* Retrieve sub regions */
      Rect trect (x, y, srect.width, srect.height);

      VideoPtr dregion = create_sub (drect, LV::VideoPtr (this), trect);

      Rect redestrect = dregion->get_extents ();

      VideoPtr tempregion = create_sub (srcp, srect);
      VideoPtr sregion    = create_sub (drect, tempregion, redestrect);

      /* Call blitter */
      compose_func (dregion.get (), sregion.get ());
  }

  void Video::fill_alpha (uint8_t alpha)
  {
      visual_return_if_fail (m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT);

      uint8_t* vidbuf = static_cast<uint8_t *> (get_pixels ()) + 3;

      /* FIXME byte order sensitive */
      for (int y = 0; y < m_impl->height; y++) {
          for (int x = 0; x < m_impl->width; x++)
              *(vidbuf += m_impl->bpp) = alpha;

          vidbuf += m_impl->pitch - (m_impl->width * m_impl->bpp);
      }
  }

  void Video::fill_alpha (uint8_t alpha, Rect const& area)
  {
      visual_return_if_fail (m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT);

      VideoPtr rvid = create_sub (this, area);
      rvid->fill_alpha (alpha);
  }

  void Video::fill_color (Color const& color)
  {
      switch (m_impl->depth) {
          case VISUAL_VIDEO_DEPTH_8BIT:
              VideoFill::fill_color_index8 (*this, color);
              return;

          case VISUAL_VIDEO_DEPTH_16BIT:
              VideoFill::fill_color_rgb16 (*this, color);
              return;

          case VISUAL_VIDEO_DEPTH_24BIT:
              VideoFill::fill_color_rgb24 (*this, color);
              return;

          case VISUAL_VIDEO_DEPTH_32BIT:
              VideoFill::fill_color_argb32 (*this, color);
              return;

          default:
              return;
      }
  }

  void Video::fill_color (Color const& color, Rect const& area)
  {
      if (m_impl->extents.intersects (area))
          return;

      VideoPtr svid = create_sub (LV::VideoPtr (this), area);
      svid->fill_color (color);
  }


  void Video::flip_pixel_bytes (VideoConstPtr const& src)
  {
      visual_return_if_fail (compare_attrs (src));

      switch (m_impl->depth) {
          case VISUAL_VIDEO_DEPTH_16BIT:
              VideoConvert::flip_pixel_bytes_color16 (*this, *src);
              return;

          case VISUAL_VIDEO_DEPTH_24BIT:
              VideoConvert::flip_pixel_bytes_color24 (*this, *src);
              return;

          case VISUAL_VIDEO_DEPTH_32BIT:
              VideoConvert::flip_pixel_bytes_color32 (*this, *src);
              return;

          default:
              return;
      }
  }

  void Video::rotate (VideoConstPtr const& src, VisVideoRotateDegrees degrees)
  {
      switch (degrees) {
          case VISUAL_VIDEO_ROTATE_NONE:
              if (m_impl->width == src->m_impl->width && m_impl->height == src->m_impl->height)
                  blit (src, 0, 0, false);
              return;

          case VISUAL_VIDEO_ROTATE_90:
              VideoTransform::rotate_90 (*this, *src);
              return;

          case VISUAL_VIDEO_ROTATE_180:
              VideoTransform::rotate_180 (*this, *src);
              return;

          case VISUAL_VIDEO_ROTATE_270:
              VideoTransform::rotate_270 (*this, *src);
              return;

          default:
              return;
      }
  }

  void Video::mirror (VideoConstPtr const& src, VisVideoMirrorOrient orient)
  {
      visual_return_if_fail (src->m_impl->depth == m_impl->depth);

      switch (orient) {
          case VISUAL_VIDEO_MIRROR_NONE:
              blit (src, 0, 0, false);
              return;

          case VISUAL_VIDEO_MIRROR_X:
              VideoTransform::mirror_x (*this, *src);
              return;

          case VISUAL_VIDEO_MIRROR_Y:
              VideoTransform::mirror_y (*this, *src);
              return;

          default:
              return;
      }
  }

  void Video::convert_depth (VideoConstPtr const& src)
  {
      /* We blit overlay it instead of just visual_mem_copy because the pitch can still be different */
      if (m_impl->depth == src->m_impl->depth) {
          blit (src, 0, 0, false);
          return;
      }

      if (m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT || src->m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT) {
          visual_return_if_fail (src->m_impl->palette.size () == 256);
      }

      if (src->m_impl->depth == VISUAL_VIDEO_DEPTH_8BIT) {

          switch (m_impl->depth) {
              case VISUAL_VIDEO_DEPTH_16BIT:
                  VideoConvert::index8_to_rgb16 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_24BIT:
                  VideoConvert::index8_to_rgb24 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_32BIT:
                  VideoConvert::index8_to_argb32 (*this, *src);
                  return;

              default:
                  visual_log (VISUAL_LOG_ERROR, "Invalid depth conversion requested (%d -> %d)",
                              int (src->m_impl->depth), int (m_impl->depth));
                  return;
          }

      } else if (src->m_impl->depth == VISUAL_VIDEO_DEPTH_16BIT) {

          switch (m_impl->depth) {
              case VISUAL_VIDEO_DEPTH_8BIT:
                  VideoConvert::rgb16_to_index8 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_24BIT:
                  VideoConvert::rgb16_to_rgb24 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_32BIT:
                  VideoConvert::rgb16_to_argb32 (*this, *src);
                  return;

              default:
                  visual_log (VISUAL_LOG_ERROR, "Invalid depth conversion requested (%d -> %d)",
                              int (src->m_impl->depth), int (m_impl->depth));
                  return;
          }

      } else if (src->m_impl->depth == VISUAL_VIDEO_DEPTH_24BIT) {

          switch (m_impl->depth) {
              case VISUAL_VIDEO_DEPTH_8BIT:
                  VideoConvert::rgb24_to_index8 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_16BIT:
                  VideoConvert::rgb24_to_rgb16 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_32BIT:
                  VideoConvert::rgb24_to_argb32 (*this, *src);
                  return;

              default:
                  visual_log (VISUAL_LOG_ERROR, "Invalid depth conversion requested (%d -> %d)",
                              int (src->m_impl->depth), int (m_impl->depth));
                  return;
          }

      } else if (src->m_impl->depth == VISUAL_VIDEO_DEPTH_32BIT) {

          switch (m_impl->depth) {
              case VISUAL_VIDEO_DEPTH_8BIT:
                  VideoConvert::argb32_to_index8 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_16BIT:
                  VideoConvert::argb32_to_rgb16 (*this, *src);
                  return;

              case VISUAL_VIDEO_DEPTH_24BIT:
                  VideoConvert::argb32_to_rgb24 (*this, *src);
                  return;

              default:
                  visual_log (VISUAL_LOG_ERROR, "Invalid depth conversion requested (%d -> %d)",
                              int (src->m_impl->depth), int (m_impl->depth));
                  return;
          }
      }
  }

  void Video::scale (VideoConstPtr const& src, VisVideoScaleMethod method)
  {
      visual_return_if_fail (m_impl->depth == src->m_impl->depth);
      visual_return_if_fail (is_valid_scale_method (method));

      /* If the dest and source are equal in dimension and scale_method is nearest, do a
       * blit overlay */
      if (compare_attrs_ignore_pitch (src) && method == VISUAL_VIDEO_SCALE_NEAREST) {
          blit (src, 0, 0, false);
          return;
      }

      switch (m_impl->depth) {
          case VISUAL_VIDEO_DEPTH_8BIT:
              if (method == VISUAL_VIDEO_SCALE_NEAREST)
                  VideoTransform::scale_nearest_color8 (*this, *src);
              else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
                  VideoTransform::scale_bilinear_color8 (*this, *src);

              break;

          case VISUAL_VIDEO_DEPTH_16BIT:
              if (method == VISUAL_VIDEO_SCALE_NEAREST)
                  VideoTransform::scale_nearest_color16 (*this, *src);
              else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
                  VideoTransform::scale_bilinear_color16 (*this, *src);

              break;

          case VISUAL_VIDEO_DEPTH_24BIT:
              if (method == VISUAL_VIDEO_SCALE_NEAREST)
                  VideoTransform::scale_nearest_color24 (*this, *src);
              else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
                  VideoTransform::scale_bilinear_color24 (*this, *src);

              break;

          case VISUAL_VIDEO_DEPTH_32BIT:
              if (method == VISUAL_VIDEO_SCALE_NEAREST)
                  VideoTransform::scale_nearest_color32 (*this, *src);
              else if (method == VISUAL_VIDEO_SCALE_BILINEAR) {
                  VideoTransform::scale_bilinear_color32 (*this, *src);
              }

              break;

          default:
              visual_log (VISUAL_LOG_ERROR, "Invalid depth passed to the scaler");
              break;
      }
  }

  void Video::scale_depth (VideoConstPtr const& src, VisVideoScaleMethod scale_method)
  {
      if (m_impl->depth != src->m_impl->depth) {
          VideoPtr dtransform = create ();
          dtransform->set_attrs (m_impl->width, m_impl->height, m_impl->width * m_impl->bpp, m_impl->depth);
          dtransform->allocate_buffer ();
          dtransform->convert_depth (src);

          scale (dtransform, scale_method);
      } else {
          scale (src, scale_method);
      }
  }

} // LV namespace

/* VisVideoDepth functions */

int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth)
{
    if (visual_video_depth_is_sane (depth) == 0)
        return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

    if ((depth & depthflag) > 0)
        return TRUE;

    return FALSE;
}

VisVideoDepth visual_video_depth_get_next (int depthflag, VisVideoDepth depth)
{
    int i = depth;

    if (visual_video_depth_is_sane (depth) == 0)
        return VISUAL_VIDEO_DEPTH_ERROR;

    if (i == VISUAL_VIDEO_DEPTH_NONE) {
        i = VISUAL_VIDEO_DEPTH_8BIT;

        if ((i & depthflag) > 0)
            return VisVideoDepth (i);
    }

    while (i < VISUAL_VIDEO_DEPTH_GL) {
        i *= 2;

        if ((i & depthflag) > 0)
            return VisVideoDepth (i);
    }

    return depth;
}

VisVideoDepth visual_video_depth_get_prev (int depthflag, VisVideoDepth depth)
{
    int i = depth;

    if (visual_video_depth_is_sane (depth) == 0)
        return VISUAL_VIDEO_DEPTH_ERROR;

    if (i == VISUAL_VIDEO_DEPTH_NONE)
        return VISUAL_VIDEO_DEPTH_NONE;

    while (i > VISUAL_VIDEO_DEPTH_NONE) {
        i >>= 1;

        if ((i & depthflag) > 0)
            return VisVideoDepth (i);
    }

    return depth;
}

VisVideoDepth visual_video_depth_get_lowest (int depthflag)
{
    return visual_video_depth_get_next (depthflag, VISUAL_VIDEO_DEPTH_NONE);
}

VisVideoDepth visual_video_depth_get_highest (int depthflag)
{
    VisVideoDepth highest = VISUAL_VIDEO_DEPTH_NONE;
    VisVideoDepth i = VISUAL_VIDEO_DEPTH_NONE;
    int firstentry = TRUE;

    while (highest != i || firstentry) {
        highest = i;

        i = visual_video_depth_get_next (depthflag, i);

        firstentry = FALSE;
    }

    return highest;
}

VisVideoDepth visual_video_depth_get_highest_nogl (int depthflag)
{
    VisVideoDepth depth;

    depth = visual_video_depth_get_highest (depthflag);

    /* Get previous depth if the highest is openGL */
    if (depth == VISUAL_VIDEO_DEPTH_GL) {
        depth = visual_video_depth_get_prev (depthflag, depth);

        /* Is it still on openGL ? Return an error */
        if (depth == VISUAL_VIDEO_DEPTH_GL)
            return VISUAL_VIDEO_DEPTH_ERROR;

    } else {
        return depth;
    }

    return VISUAL_VIDEO_DEPTH_NONE;
}

int visual_video_depth_is_sane (VisVideoDepth depth)
{
    int count = 0;
    int i = 1;

    if (depth == VISUAL_VIDEO_DEPTH_NONE)
        return TRUE;

    if (depth >= VISUAL_VIDEO_DEPTH_ENDLIST)
        return FALSE;

    while (i < VISUAL_VIDEO_DEPTH_ENDLIST) {
        if ((i & depth) > 0)
            count++;

        if (count > 1)
            return FALSE;

        i <<= 1;
    }

    return TRUE;
}

int visual_video_depth_value_from_enum (VisVideoDepth depth)
{
    switch (depth) {
        case VISUAL_VIDEO_DEPTH_8BIT:
            return 8;

        case VISUAL_VIDEO_DEPTH_16BIT:
            return 16;

        case VISUAL_VIDEO_DEPTH_24BIT:
            return 24;

        case VISUAL_VIDEO_DEPTH_32BIT:
            return 32;

        default:
            return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
    }

    return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
}

VisVideoDepth visual_video_depth_enum_from_value (int depthvalue)
{
    switch (depthvalue) {
        case  8: return VISUAL_VIDEO_DEPTH_8BIT;
        case 16: return VISUAL_VIDEO_DEPTH_16BIT;
        case 24: return VISUAL_VIDEO_DEPTH_24BIT;
        case 32: return VISUAL_VIDEO_DEPTH_32BIT;

        default:
            return VISUAL_VIDEO_DEPTH_NONE;
    }

    return VISUAL_VIDEO_DEPTH_NONE;
}

int visual_video_bpp_from_depth (VisVideoDepth depth)
{
    switch (depth) {
        case VISUAL_VIDEO_DEPTH_8BIT:  return 1;
        case VISUAL_VIDEO_DEPTH_16BIT: return 2;
        case VISUAL_VIDEO_DEPTH_24BIT: return 3;
        case VISUAL_VIDEO_DEPTH_32BIT: return 4;
        case VISUAL_VIDEO_DEPTH_GL:    return 0;

        default:
            return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
    }

    return -VISUAL_ERROR_IMPOSSIBLE;
}
