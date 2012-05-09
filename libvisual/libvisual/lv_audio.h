/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_audio.h,v 1.23 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_AUDIO_H
#define _LV_AUDIO_H

#include <libvisual/lv_time.h>
#include <libvisual/lv_ringbuffer.h>

/**
 * @defgroup VisAudio VisAudio
 * @{
 */

#define VISUAL_AUDIO(obj)                    (VISUAL_CHECK_CAST ((obj), VisAudio))
#define VISUAL_AUDIO_SAMPLEPOOL(obj)         (VISUAL_CHECK_CAST ((obj), VisAudioSamplePool))
#define VISUAL_AUDIO_SAMPLEPOOL_CHANNEL(obj) (VISUAL_CHECK_CAST ((obj), VisAudioSamplePoolChannel))
#define VISUAL_AUDIO_SAMPLE(obj)             (VISUAL_CHECK_CAST ((obj), VisAudioSample))

#define VISUAL_AUDIO_CHANNEL_LEFT	"front left 1"
#define VISUAL_AUDIO_CHANNEL_RIGHT	"front right 1"

#define VISUAL_AUDIO_CHANNEL_CATEGORY_FRONT	"front"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_REAR	"rear"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_RIGHT	"left"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_LEFT	"right"

typedef enum {
	VISUAL_AUDIO_SAMPLE_RATE_NONE = 0,
	VISUAL_AUDIO_SAMPLE_RATE_8000,
	VISUAL_AUDIO_SAMPLE_RATE_11250,
	VISUAL_AUDIO_SAMPLE_RATE_22500,
	VISUAL_AUDIO_SAMPLE_RATE_32000,
	VISUAL_AUDIO_SAMPLE_RATE_44100,
	VISUAL_AUDIO_SAMPLE_RATE_48000,
	VISUAL_AUDIO_SAMPLE_RATE_96000,
	VISUAL_AUDIO_SAMPLE_RATE_LAST
} VisAudioSampleRateType;

typedef enum {
	VISUAL_AUDIO_SAMPLE_FORMAT_NONE = 0,
	VISUAL_AUDIO_SAMPLE_FORMAT_U8,
	VISUAL_AUDIO_SAMPLE_FORMAT_S8,
	VISUAL_AUDIO_SAMPLE_FORMAT_U16,
	VISUAL_AUDIO_SAMPLE_FORMAT_S16,
	VISUAL_AUDIO_SAMPLE_FORMAT_U32,
	VISUAL_AUDIO_SAMPLE_FORMAT_S32,
	VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
	VISUAL_AUDIO_SAMPLE_FORMAT_LAST
} VisAudioSampleFormatType;

typedef enum {
	VISUAL_AUDIO_SAMPLE_CHANNEL_NONE = 0,
	VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO
} VisAudioSampleChannelType;


typedef struct _VisAudio VisAudio;
typedef struct _VisAudioSamplePool VisAudioSamplePool;
typedef struct _VisAudioSamplePoolChannel VisAudioSamplePoolChannel;
typedef struct _VisAudioSample VisAudioSample;


LV_BEGIN_DECLS

/**
 * Creates a new VisAudio structure.
 *
 * @return A newly allocated VisAudio, or NULL on failure.
 */
LV_API VisAudio *visual_audio_new (void);
LV_API void visual_audio_free (VisAudio *audio);

LV_API int  visual_audio_get_sample (VisAudio *audio, VisBuffer *buffer, const char *channelid);
LV_API void visual_audio_get_sample_mixed_simple (VisAudio *audio, VisBuffer *buffer, unsigned int channels, ...);
LV_API void visual_audio_get_sample_mixed (VisAudio *audio, VisBuffer *buffer, int divide, unsigned int channels, ...);

LV_API void visual_audio_get_spectrum (VisAudio *audio, VisBuffer *buffer, int samplelen, const char *channelid, int normalised);
LV_API void visual_audio_get_spectrum_multiplied (VisAudio *audio, VisBuffer *buffer, int samplelen, const char *channelid, int normalised, float multiplier);
LV_API void visual_audio_get_spectrum_for_sample (VisBuffer *buffer, VisBuffer *sample, int normalised);
LV_API void visual_audio_get_spectrum_for_sample_multiplied (VisBuffer *buffer, VisBuffer *sample, int normalised, float multiplier);

LV_API void visual_audio_input (VisAudio *audio,
                                VisBuffer *buffer,
                                VisAudioSampleRateType rate,
                                VisAudioSampleFormatType format,
                                VisAudioSampleChannelType channeltype);

LV_API void visual_audio_input_channel (VisAudio *audio,
                                        VisBuffer *buffer,
                                        VisAudioSampleRateType rate,
                                        VisAudioSampleFormatType format,
                                        const char *channelid);

LV_API void visual_audio_normalise_spectrum (VisBuffer *buffer);

LV_API VisAudioSamplePool *visual_audio_samplepool_new (void);
LV_API int visual_audio_samplepool_add (VisAudioSamplePool *samplepool, VisAudioSample *sample, const char *channelid);
LV_API int visual_audio_samplepool_add_channel (VisAudioSamplePool *samplepool, VisAudioSamplePoolChannel *channel);
LV_API VisAudioSamplePoolChannel *visual_audio_samplepool_get_channel (VisAudioSamplePool *samplepool, const char *channelid);
LV_API void visual_audio_samplepool_flush_old (VisAudioSamplePool *samplepool);

VisAudioSamplePoolChannel *visual_audio_samplepool_channel_new (const char *channelid);
LV_API void visual_audio_samplepool_channel_add (VisAudioSamplePoolChannel *channel, VisAudioSample *sample);
LV_API void visual_audio_samplepool_channel_flush_old (VisAudioSamplePoolChannel *channel);

LV_API void visual_audio_sample_buffer_mix (VisBuffer *dest, VisBuffer *src, int divide, float multiplier);
LV_API void visual_audio_sample_buffer_mix_many (VisBuffer *dest, int divide, int channels, ...);

LV_API VisAudioSample *visual_audio_sample_new (VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate);
LV_API int visual_audio_sample_has_internal (VisAudioSample *sample);
LV_API void visual_audio_sample_transform_format (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleFormatType format);
LV_API void visual_audio_sample_transform_rate (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleRateType rate);
LV_API visual_size_t visual_audio_sample_rate_get_length (VisAudioSampleRateType rate);
LV_API visual_size_t visual_audio_sample_format_get_size (VisAudioSampleFormatType format);
LV_API int visual_audio_sample_format_is_signed (VisAudioSampleFormatType format);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_AUDIO_H */
