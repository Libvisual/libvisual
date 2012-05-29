#ifndef _LV_VIDEO_CONVERT_HPP
#define _LV_VIDEO_CONVERT_HPP

#include "lv_video.h"

namespace LV {

  class VideoConvert
  {
  public:

      static void convert_get_smallest (Video& dest, Video const& src, int& width, int& height);

      static void index8_to_rgb16  (Video& dest, Video const& src);
      static void index8_to_rgb24  (Video& dest, Video const& src);
      static void index8_to_argb32 (Video& dest, Video const& src);

      static void rgb16_to_index8 (Video& dest, Video const& src);
      static void rgb16_to_rgb24  (Video& dest, Video const& src);
      static void rgb16_to_argb32 (Video& dest, Video const& src);

      static void rgb24_to_index8 (Video& dest, Video const& src);
      static void rgb24_to_rgb16  (Video& dest, Video const& src);
      static void rgb24_to_argb32 (Video& dest, Video const& src);

      static void argb32_to_index8 (Video& dest, Video const& src);
      static void argb32_to_rgb16  (Video& dest, Video const& src);
      static void argb32_to_rgb24  (Video& dest, Video const& src);

      static void flip_pixel_bytes_color16 (Video& dest, Video const& src);
      static void flip_pixel_bytes_color24 (Video& dest, Video const& src);
      static void flip_pixel_bytes_color32 (Video& dest, Video const& src);
  };
}

#endif // _LV_VIDEO_CONVERT_HPP
