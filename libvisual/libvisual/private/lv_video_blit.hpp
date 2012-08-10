/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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
