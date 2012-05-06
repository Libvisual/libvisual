#ifndef _LV_VIDEO_PRIVATE_HPP
#define _LV_VIDEO_PRIVATE_HPP

#include "lv_video.h"
#include "lv_rect.h"
#include "lv_palette.h"
#include "lv_color.h"
#include <vector>

namespace LV {

  class Video::Impl
  {
  public:

      VisVideoDepth      depth;
      int                width;
      int                height;
      int                bpp;
      int                pitch;

      BufferPtr          buffer;
      std::vector<void*> rows;
      Palette            pal;

      VideoPtr           parent;
      Rect               extents;

      VisVideoComposeType compose_type;
      VisVideoComposeFunc compose_func;
      Color*              colorkey;
      uint8_t             alpha;

      Impl ();

      ~Impl ();
  };

} // LV namespace

#endif // _LV_VIDEO_PRIVATE_HPP
