#ifndef _LV_VIDEO_PRIVATE_HPP
#define _LV_VIDEO_PRIVATE_HPP

#include "lv_video.h"
#include "lv_rectangle.h"
#include "lv_palette.h"
#include "lv_color.h"
#include <vector>

namespace LV {

  class Video::Impl
  {
  public:

      int                width;
      int                height;
      VisVideoDepth      depth;
      int                bpp;
      int                pitch;

      BufferPtr          buffer;
      std::vector<void*> pixel_rows;
      Palette            palette;

      VideoConstPtr      parent;
      Rect               extents;

      VisVideoComposeType compose_type;
      VisVideoComposeFunc compose_func;
      Color*              colorkey;
      uint8_t             alpha;

      Impl ();

      ~Impl ();

      void precompute_row_table ();
  };

} // LV namespace

#endif // _LV_VIDEO_PRIVATE_HPP
