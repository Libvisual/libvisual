#ifndef LV_AUDIO_CONVERT_H
#define LV_AUDIO_CONVERT_H

#include "lv_buffer.h"

LV_BEGIN_DECLS

void audio_sample_convert_from_float (VisBuffer *dest, VisBuffer *src, int int_size, int is_signed);
void audio_sample_convert_to_float   (VisBuffer *dest, VisBuffer *src, int int_size, int is_signed);
void audio_sample_convert            (VisBuffer *dest, VisBuffer *src, int dsize, int ssize, int dsigned, int ssigned);

LV_END_DECLS

#endif
