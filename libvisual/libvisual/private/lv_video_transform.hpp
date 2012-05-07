#ifndef _LV_VIDEO_SCALE_HPP
#define _LV_VIDEO_SCALE_HPP

#include "lv_video.h"

namespace LV {

  class VideoTransform
  {
  public:

      static void rotate_90  (Video& dest, Video const& src);
      static void rotate_180 (Video& dest, Video const& src);
      static void rotate_270 (Video& dest, Video const& src);

      static void mirror_x (Video& dest, Video const& src);
      static void mirror_y (Video& dest, Video const& src);

      static void scale_nearest_color8  (Video& dest, Video const& src);
      static void scale_nearest_color16 (Video& dest, Video const& src);
      static void scale_nearest_color24 (Video& dest, Video const& src);
      static void scale_nearest_color32 (Video& dest, Video const& src);

      static void scale_bilinear_color8  (Video& dest, Video const& src);
      static void scale_bilinear_color16 (Video& dest, Video const& src);
      static void scale_bilinear_color24 (Video& dest, Video const& src);
      static void scale_bilinear_color32 (Video& dest, Video const& src);

      static void scale_bilinear_color32_mmx (Video& dest, Video const& src);
  };

} // LV namespace

#endif /* _LV_VIDEO_SCALE_HPP */
