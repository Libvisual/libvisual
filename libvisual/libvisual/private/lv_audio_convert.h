#ifndef LV_AUDIO_CONVERT_H
#define LV_AUDIO_CONVERT_H

#include "lv_audio.h"

LV_BEGIN_DECLS

void visual_audio_sample_convert (VisBuffer *dest,
                                  VisAudioSampleFormatType dest_format,
                                  VisBuffer *src,
                                  VisAudioSampleFormatType src_format);

void visual_audio_sample_deinterleave_stereo (VisBuffer *dest1,
                                              VisBuffer *dest2,
                                              VisBuffer *src,
                                              VisAudioSampleFormatType format);

LV_END_DECLS

#endif
