#ifndef _LV_VIDEO_BLIT_HPP
#define _LV_VIDEO_BLIT_HPP

#include "lv_video.h"

namespace LV {

  class VideoBlit
  {
  public:

      static void blit_overlay_noalpha      (Video* dest, Video* src);
      static void blit_overlay_alphasrc     (Video* dest, Video* src);
      static void blit_overlay_colorkey     (Video* dest, Video* src);
      static void blit_overlay_surfacealpha (Video* dest, Video* src);
      static void blit_overlay_surfacealphacolorkey (Video* dest, Video* src);

      static void blit_overlay_alphasrc_mmx (Video* dest, Video* src);
  };
}

#endif /* _LV_VIDEO_BLIT_HPP */
