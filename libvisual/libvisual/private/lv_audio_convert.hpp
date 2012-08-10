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

#ifndef LV_AUDIO_CONVERT_HPP
#define LV_AUDIO_CONVERT_HPP

#include "lv_audio.h"

namespace LV {

  class AudioConvert
  {
  public:

      static void convert_samples (BufferPtr const&         dest,
                                   VisAudioSampleFormatType dest_format,
                                   BufferConstPtr const&    src,
                                   VisAudioSampleFormatType src_format);

      static void deinterleave_stereo_samples (BufferPtr const&         dest1,
                                               BufferPtr const&         dest2,
                                               BufferConstPtr const&    src,
                                               VisAudioSampleFormatType format);
  };

} // LV namespace

#endif // LV_AUDIO_CONVERT_HPP
