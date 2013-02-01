/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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

      void set_buffer (void* ptr);
      void precompute_row_table ();
  };

} // LV namespace

#endif // _LV_VIDEO_PRIVATE_HPP
