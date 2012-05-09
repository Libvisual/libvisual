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
