/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *      Duilio J. Protti <dprotti@users.sourceforge.net>
 *      Chong Kai Xiong <descender@phreaker.net>
 *      Jean-Christophe Hoelt <jeko@ios-software.com>
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

#ifndef _LV_VIDEO_H
#define _LV_VIDEO_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_rectangle.h>
#include <libvisual/lv_buffer.h>
#include <libvisual/lv_gl.h>

/**
 * @defgroup VisVideo VisVideo
 * @{
 */

#define VISUAL_VIDEO(obj)                   (VISUAL_CHECK_CAST ((obj), VisVideo))
#define VISUAL_VIDEO_ATTR_OPTIONS(obj)      (VISUAL_CHECK_CAST ((obj), VisVideoAttrOptions))

#define VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(options, attr, val)  \
    options.gl_attrs[attr].attribute = attr;  \
    options.gl_attrs[attr].value = val;       \
    options.gl_attrs[attr].mutated = TRUE;


/* NOTE: The depth find helper code in lv_actor depends on an arrangment from low to high */
/**
 * Enumerate that defines video depths for use within plugins, libvisual functions, etc.
 */
typedef enum {
    VISUAL_VIDEO_DEPTH_NONE     = 0,    /**< No video surface flag. */
    VISUAL_VIDEO_DEPTH_8BIT     = 1,    /**< 8 bits indexed surface flag. */
    VISUAL_VIDEO_DEPTH_16BIT    = 2,    /**< 16 bits 5-6-5 surface flag. */
    VISUAL_VIDEO_DEPTH_24BIT    = 4,    /**< 24 bits surface flag. */
    VISUAL_VIDEO_DEPTH_32BIT    = 8,    /**< 32 bits surface flag. */
    VISUAL_VIDEO_DEPTH_GL       = 16,   /**< openGL surface flag. */
    VISUAL_VIDEO_DEPTH_ENDLIST  = 32,   /**< Used to mark the end of the depth list. */
    VISUAL_VIDEO_DEPTH_ERROR    = -1,   /**< Used when there is an error. */
    VISUAL_VIDEO_DEPTH_ALL      = VISUAL_VIDEO_DEPTH_8BIT
                                | VISUAL_VIDEO_DEPTH_16BIT
                                | VISUAL_VIDEO_DEPTH_24BIT
                                | VISUAL_VIDEO_DEPTH_32BIT
                                | VISUAL_VIDEO_DEPTH_GL /**< All graphical depths. */
} VisVideoDepth;

/**
 * Enumerate that defines video rotate types, used with the visual_video_rotate_*() functions.
 */
typedef enum {
    VISUAL_VIDEO_ROTATE_NONE = 0,   /**< No rotating. */
    VISUAL_VIDEO_ROTATE_90   = 1,   /**< 90 degrees rotate. */
    VISUAL_VIDEO_ROTATE_180  = 2,   /**< 180 degrees rotate. */
    VISUAL_VIDEO_ROTATE_270  = 3    /**< 270 degrees rotate. */
} VisVideoRotateDegrees;

/**
 * Enumerate that defines the video mirror types, used with the visual_video_mirror_*() functions.
 */
typedef enum {
    VISUAL_VIDEO_MIRROR_NONE = 0,   /**< No mirroring. */
    VISUAL_VIDEO_MIRROR_X    = 1,   /**< Mirror on the X ax. */
    VISUAL_VIDEO_MIRROR_Y    = 2    /**< Mirror on the Y ax. */
} VisVideoMirrorOrient;

/**
 * Enumerate that defines the different methods of scaling within VisVideo.
 */
typedef enum {
    VISUAL_VIDEO_SCALE_NEAREST  = 0,    /**< Nearest neighbour. */
    VISUAL_VIDEO_SCALE_BILINEAR = 1     /**< Bilinearly interpolated. */
} VisVideoScaleMethod;

/**
 * Enumerate that defines the different blitting methods for a VisVideo.
 */
typedef enum {
    VISUAL_VIDEO_COMPOSE_TYPE_NONE = 0,   /**< No compose set, use default. */
    VISUAL_VIDEO_COMPOSE_TYPE_SRC,        /**< Source alpha channel. */
    VISUAL_VIDEO_COMPOSE_TYPE_COLORKEY,   /**< Colorkey alpha. */
    VISUAL_VIDEO_COMPOSE_TYPE_SURFACE,    /**< One alpha channel for the complete surface. */
    VISUAL_VIDEO_COMPOSE_TYPE_SURFACECOLORKEY, /**< Use surface alpha on colorkey. */
    VISUAL_VIDEO_COMPOSE_TYPE_CUSTOM      /**< Custom compose function (looks up on the source VisVideo. */
} VisVideoComposeType;

typedef struct _VisVideoAttrOptions VisVideoAttrOptions;

#ifdef __cplusplus
namespace LV {
  class Video;
}
typedef LV::Video VisVideo;
#else
typedef struct _VisVideo VisVideo;
struct _VisVideo;
#endif

/** VisVideo custom compose method */
typedef void (*VisVideoComposeFunc)(VisVideo *dest, VisVideo *src);

struct _VisVideoAttrOptions {
    VisObject      object;
    VisVideoDepth  depth;
    VisGLAttrEntry gl_attrs[VISUAL_GL_ATTRIBUTE_LAST];
};

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>
#include <libvisual/lv_intrusive_ptr.hpp>

namespace LV {

  class Video;

  typedef IntrusivePtr<Video> VideoPtr;
  typedef IntrusivePtr<Video const> VideoConstPtr;

  class LV_API Video
  {
  public:

      /**
       * Creates a new empty Video object.
       */
      static VideoPtr create ();

      /**
       * Creates a new Video object with an allocated buffer.
       *
       * @param width The width for the new buffer.
       * @param height The height for the new buffer.
       * @param depth The depth being used.
       */
      static VideoPtr create (int width, int height, VisVideoDepth depth);

      static VideoPtr wrap (void* buffer, bool owner, int width, int height, VisVideoDepth depth);

      static VideoPtr create_sub (VideoConstPtr const& src, Rect const& srect);

      static VideoPtr create_sub (Rect const& drect, VideoConstPtr const& src, Rect const& srect);

      ~Video ();

      /**
       * Sets the video dimensions.
       *
       * @param width  width in pixels
       * @param height height in pixels
       */
      void set_dimension (int width, int height);

      int get_width () const;

      int get_height () const;

      /**
       * Sets the video depth.
       *
       * @param depth The depth choosen from the VisVideoDepth enumerate.
       */
      void set_depth (VisVideoDepth depth);

      VisVideoDepth get_depth () const;

      /**
       * Sets the pixel pitch i.e. row stride
       *
       * @note Use this only when the desired pitch is not equal to width * bytes per pixel.
       *
       * @param pitch pitch in bytes
       */
      void set_pitch (int pitch);

      /**
       * Returns the pitch.
       *
       * @return pitch in bytes
       */
      int get_pitch () const;

      /**
       * Returns the byte size of each pixel.
       *
       * @return pixel byte size
       */
      int get_bpp () const;

      /**
       * Sets the buffer to a given memory block
       *
       * @param ptr Pointer to memory block
       */
      void set_buffer (void* ptr);

      /**
       * Allocates a buffer for the VisVideo based on the set
       * dimensions and pixel format
       */
      bool allocate_buffer ();

      /**
       * Frees the buffer
       */
      void free_buffer ();

      /**
       * Checks if this object has a private allocated buffer.
       *
       * @return true if this has an allocated buffer, false otherwise
       */
      bool has_allocated_buffer () const;

      /**
       * Retrieves the Buffer object
       *
       * @return the Buffer object
       */
      BufferPtr get_buffer () const;

      /**
       * Sets all attributes.
       *
       * @param width  width in pixels
       * @param height height in pixels
       * @param pitch  pitch (row stride)
       * @param depth  colour depth
       */
      void set_attrs (int width, int height, int pitch, VisVideoDepth depth);

      /**
       * Copies the attributes from another Video.
       *
       * @param src Video object to copy attributes from
       */
      void copy_attrs (VideoConstPtr const& src);

      /**
       * Checks if this Video has the same attributes as another Video.
       *
       * @param src Video to compare against
       *
       * @return true if both videos have the same attributes, false otherwise
       */
      bool compare_attrs (VideoConstPtr const& src) const;

      /**
       * Checks if this Video has the same attributes as another Video, ignoring pitch comparisons.
       *
       * @param src Video to compare against
       *
       * @return true if both videos have the same attributes, false otherwise
       */
      bool compare_attrs_ignore_pitch (VideoConstPtr const& src) const;

      std::size_t get_size () const;

      /**
       * Sets the color palette.
       *
       * @param palette palette
       */
      void set_palette (Palette const& palette);

      /**
       * Returns the color palette.
       *
       * @return palette
       */
      Palette const& get_palette () const;

      /**
       * Returns the color palette.
       *
       * @return palette
       */
      Palette& get_palette ();

      /**
       * Returns a pointer to the pixel buffer.
       *
       * @see get_pixel_ptr
       *
       * @return pointer to pixel buffer
       */
      void* get_pixels () const;

      /**
       * Returns a pointer to a specific location in the pixel buffer.
       *
       * @see get_pixels
       *
       * @return pointer to pixel in buffer
       */
      void* get_pixel_ptr (int x, int y) const;

      /**
       * Returns the extents of this Video
       *
       * @return the extents
       */
      Rect const& get_extents () const;

      void set_compose_type (VisVideoComposeType type);
      void set_compose_colorkey (Color const& color);
      void set_compose_surface  (uint8_t alpha);
      void set_compose_function (VisVideoComposeFunc func);

      VisVideoComposeFunc get_compose_function (VideoConstPtr const& src, bool alpha);

      /**
       * This function blits a VisVideo into another VisVideo. Placement can be done and there
       * is support for the alpha channel.
       *
       * @param src   Video to blit
       * @param x     x-coordinate of blit position
       * @param y     y-coordinate of blit position
       * @param alpha set to true if alpha channel is to be checked
       */
      void blit (VideoConstPtr const& src, int x, int y, bool alpha);
      void blit (Rect const& drect, VideoConstPtr const& src, Rect const& srect, bool alpha);

      void compose (VideoConstPtr const& src, int x, int y, VisVideoComposeFunc func);
      void compose (Rect const& drect, VideoConstPtr const& src, Rect const& srect, VisVideoComposeFunc func);

      void blit_scale    (Rect const& drect, VideoConstPtr const& src, Rect const& srect, bool alpha, VisVideoScaleMethod scale_method);
      void compose_scale (Rect const& drect, VideoConstPtr const& src, Rect const& srect, VisVideoScaleMethod scale_method, VisVideoComposeFunc func);

      /**
       * Fills the entire pixel buffer with a constant alpha value
       *
       * @note Only applicable for pixel formats with an alpha channel
       *
       * @param alpha alpha value for filling
       */
      void fill_alpha (uint8_t alpha);

      void fill_alpha (uint8_t alpha, Rect const& area);

      /**
       * Performs a color fill.
       *
       * @param color fill color
       */
      void fill_color (Color const& color);

      void fill_color (Color const& color, Rect const& area);

      /**
       * Flips the pixel byte ordering of a video.
       *
       * @param src source Video
       */
      void flip_pixel_bytes (VideoConstPtr const& src);

      void rotate (VideoConstPtr const& src, VisVideoRotateDegrees degrees);
      void mirror (VideoConstPtr const& src, VisVideoMirrorOrient orient);

      /**
       * Performs a depth conversion of a video.
       *
       * @param src source Video
       */
      void convert_depth (VideoConstPtr const& src);

      /**
       * Scales a video.
       *
       * @param src    source Video
       * @param method scaling method to use
       */
      void scale (VideoConstPtr const& src, VisVideoScaleMethod method);

      /**
       * Scales a video and performs a depth conversion where necessary.
       *
       * @see scale
       *
       * @param src    source Video
       * @param method scaling method to use
       */
      void scale_depth (VideoConstPtr const& src, VisVideoScaleMethod method);

      static VideoPtr create_scale_depth (VideoConstPtr const& src,
                                          int                  width,
                                          int                  height,
                                          VisVideoDepth        depth,
                                          VisVideoScaleMethod  scale_method);
      void ref () const;

      void unref () const;

  private:

      friend class VideoConvert;
      friend class VideoTransform;
      friend class VideoFill;
      friend class VideoBlit;

      class Impl;

      ScopedPtr<Impl> m_impl;
      mutable unsigned int m_ref_count;

      Video ();
      Video (Video const&);
      Video& operator= (Video const&);
  };

  inline void intrusive_ptr_add_ref (Video* video)
  {
      video->ref ();
  }

  inline void intrusive_ptr_release (Video* video)
  {
      video->unref ();
  }

  inline void intrusive_ptr_add_ref (Video const* video)
  {
      video->ref ();
  }

  inline void intrusive_ptr_release (Video const* video)
  {
      video->unref ();
  }

} // LV namespace

#endif

LV_BEGIN_DECLS

LV_API VisVideo *visual_video_new (void);
LV_API VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth);
LV_API VisVideo *visual_video_new_wrap_buffer (void *buffer, int owner, int width, int height, VisVideoDepth depth);

LV_API void visual_video_ref   (VisVideo *video);
LV_API void visual_video_unref (VisVideo *video);

LV_API int  visual_video_allocate_buffer (VisVideo *video);
LV_API void visual_video_free_buffer (VisVideo *video);
LV_API int  visual_video_has_allocated_buffer (VisVideo *video);

LV_API void visual_video_copy_attrs (VisVideo *dest, VisVideo *src);

LV_API int visual_video_compare_attrs (VisVideo *src1, VisVideo *src2);
LV_API int visual_video_compare_attrs_ignore_pitch (VisVideo *src1, VisVideo *src2);

LV_API void        visual_video_set_palette (VisVideo *video, VisPalette *pal);
LV_API VisPalette* visual_video_get_palette (VisVideo *video);

LV_API void visual_video_set_buffer (VisVideo *video, void *ptr);

LV_API void visual_video_set_dimension (VisVideo *video, int width, int height);

LV_API void visual_video_set_attrs (VisVideo *video, int width, int height, int pitch, VisVideoDepth depth);

LV_API int visual_video_get_width  (VisVideo *video);
LV_API int visual_video_get_height (VisVideo *video);

LV_API void visual_video_set_pitch (VisVideo *video, int pitch);
LV_API int  visual_video_get_pitch (VisVideo *video);

LV_API void          visual_video_set_depth (VisVideo *video, VisVideoDepth depth);
LV_API VisVideoDepth visual_video_get_depth (VisVideo *video);

LV_API int visual_video_get_bpp (VisVideo *video);

LV_API visual_size_t visual_video_get_size (VisVideo *video);

LV_API void *visual_video_get_pixels    (VisVideo *video);
LV_API void *visual_video_get_pixel_ptr (VisVideo *video, int x, int y);

LV_API VisBuffer *visual_video_get_buffer (VisVideo *video);

LV_API VisRectangle *visual_video_get_extents (VisVideo *video);

LV_API VisVideo *visual_video_new_sub (VisVideo *src, VisRectangle *area);
LV_API VisVideo *visual_video_new_sub_by_values (VisVideo *src, int x, int y, int width, int height);
LV_API VisVideo *visual_video_new_sub_with_boundary (VisRectangle *drect, VisVideo *src, VisRectangle *srect);
LV_API VisVideo *visual_video_new_sub_all (VisVideo *src);

LV_API void visual_video_set_compose_type     (VisVideo *video, VisVideoComposeType type);
LV_API void visual_video_set_compose_colorkey (VisVideo *video, VisColor *color);
LV_API void visual_video_set_compose_surface  (VisVideo *video, uint8_t alpha);
LV_API void visual_video_set_compose_function (VisVideo *video, VisVideoComposeFunc compfunc);

LV_API VisVideoComposeFunc visual_video_get_compose_function (VisVideo *dest, VisVideo *src, int alpha);

LV_API void visual_video_blit_area          (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect, int alpha);
LV_API void visual_video_compose_area       (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect, VisVideoComposeFunc func);
LV_API void visual_video_blit_scale_area    (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect, int alpha, VisVideoScaleMethod scale_method);
LV_API void visual_video_compose_scale_area (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect, VisVideoScaleMethod scale_method, VisVideoComposeFunc compfunc);

LV_API void visual_video_blit (VisVideo *dest, VisVideo *src, int x, int y, int alpha);
LV_API void visual_video_compose (VisVideo *dest, VisVideo *src, int x, int y, VisVideoComposeFunc compfunc);

LV_API void visual_video_fill_alpha       (VisVideo *video, uint8_t density);
LV_API void visual_video_fill_alpha_area  (VisVideo *video, uint8_t density, VisRectangle *rect);
LV_API void visual_video_fill_color       (VisVideo *video, VisColor *color);
LV_API void visual_video_fill_color_area  (VisVideo *video, VisColor *color, VisRectangle *rect);

LV_API void visual_video_convert_depth    (VisVideo *dest, VisVideo *src);
LV_API void visual_video_flip_pixel_bytes (VisVideo *dest, VisVideo *src);

LV_API void visual_video_rotate (VisVideo *dest, VisVideo *src, VisVideoRotateDegrees degrees);
LV_API void visual_video_mirror (VisVideo *dest, VisVideo *src, VisVideoMirrorOrient orient);
LV_API void visual_video_scale  (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method);

LV_API void visual_video_scale_depth (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method);

LV_API VisVideo *visual_video_scale_depth_new (VisVideo*           src,
                                               int                 width,
                                               int                 height,
                                               VisVideoDepth       depth,
                                               VisVideoScaleMethod scale_method);

/**
 * Checks if a certain depth is supported by checking against an ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth that we want to test.
 *
 * @return TRUE when supported, FALSE when unsupported and -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
LV_API int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth);

/**
 * Get the previous depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the previous supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_next
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the previous supported depth.
 *
 * @return The previous supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_get_prev (int depthflag, VisVideoDepth depth);

/**
 * Get the next depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the next supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_prev
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the next supported depth.
 *
 * @return The next supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_get_next (int depthflag, VisVideoDepth depth);

/**
 * Return the lowest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The lowest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_get_lowest (int depthflag);

/**
 * Return the highest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_get_highest (int depthflag);

/**
 * Return the highest supported depth that is NOT openGL.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth that is not openGL or
 *  VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_get_highest_nogl (int depthflag);

/**
 * Checks if a certain value is a sane depth.
 *
 * @param depth Depth to be checked if it's sane.
 *
 * @return TRUE if the depth is sane, FALSE if the depth is not sane.
 */
LV_API int visual_video_depth_is_sane (VisVideoDepth depth);

/**
 * Returns the number of bits per pixel from a VisVideoDepth enumerate value.
 *
 * @param depth The VisVideodepth enumerate value from which the bits per pixel
 *  needs to be returned.
 *
 * @return The bits per pixel or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
LV_API int visual_video_depth_value_from_enum (VisVideoDepth depth);

/**
 * Returns a VisVideoDepth enumerate value from bits per pixel.
 *
 * @param depthvalue Integer containing the number of bits per pixel.
 *
 * @return The corespondending enumerate value or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
LV_API VisVideoDepth visual_video_depth_enum_from_value (int depthvalue);

/**
 * Returns the number of bytes per pixel from the VisVideoDepth enumerate.
 *
 * @param depth The VisVideodepth enumerate value from which the bytes per pixel
 *  needs to be returned.
 *
 * @return The number of bytes per pixel, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
LV_API int visual_video_bpp_from_depth (VisVideoDepth depth);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_VIDEO_H */
