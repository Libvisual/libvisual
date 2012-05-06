#ifndef _LV_VIDEO_FILL_HPP
#define _LV_VIDEO_FILL_HPP

#include "lv_video.h"
#include "lv_color.h"

namespace LV {

  class VideoFill
  {
  public:

      static void fill_color_index8 (Video& video, Color const& color);
      static void fill_color_rgb16  (Video& video, Color const& color);
      static void fill_color_rgb24  (Video& video, Color const& color);
      static void fill_color_argb32 (Video& video, Color const& color);
  };

} // LV namespace

#endif // _LV_VIDEO_FILL_HPP
