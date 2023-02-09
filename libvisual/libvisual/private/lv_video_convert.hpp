/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
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

#ifndef _LV_VIDEO_CONVERT_HPP
#define _LV_VIDEO_CONVERT_HPP

#include "lv_video.h"

namespace LV {

class VideoConvert {
public:
  static void convert_get_smallest(Video &dst, Video const &src, int &width,
                                   int &height);

  static void index8_to_rgb16(Video &dst, Video const &src);
  static void index8_to_rgb24(Video &dst, Video const &src);
  static void index8_to_argb32(Video &dst, Video const &src);

  static void rgb16_to_index8(Video &dst, Video const &src);
  static void rgb16_to_rgb24(Video &dst, Video const &src);
  static void rgb16_to_argb32(Video &dst, Video const &src);

  static void rgb24_to_index8(Video &dst, Video const &src);
  static void rgb24_to_rgb16(Video &dst, Video const &src);
  static void rgb24_to_argb32(Video &dst, Video const &src);

  static void argb32_to_index8(Video &dst, Video const &src);
  static void argb32_to_rgb16(Video &dst, Video const &src);
  static void argb32_to_rgb24(Video &dst, Video const &src);

  static void flip_pixel_bytes_color16(Video &dst, Video const &src);
  static void flip_pixel_bytes_color24(Video &dst, Video const &src);
  static void flip_pixel_bytes_color32(Video &dst, Video const &src);
};
} // namespace LV

#endif // _LV_VIDEO_CONVERT_HPP
