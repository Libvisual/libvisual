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

#include <libvisual/lv_buffer.h>

/**
 * @defgroup VisAudio VisAudio
 * @{
 */

#define VISUAL_AUDIO(obj) (VISUAL_CHECK_CAST ((obj), VisAudio))

#define VISUAL_AUDIO_CHANNEL_LEFT   "front left 1"
#define VISUAL_AUDIO_CHANNEL_RIGHT  "front right 1"

#define VISUAL_AUDIO_CHANNEL_CATEGORY_FRONT "front"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_REAR  "rear"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_RIGHT "left"
#define VISUAL_AUDIO_CHANNEL_CATEGORY_LEFT  "right"

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

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>
#include <string>
#include <cstdarg>

namespace LV {

  class LV_API Audio
  {
  public:

      Audio ();

      ~Audio ();

      bool get_sample (BufferPtr const& buffer, std::string const& channel_name);

      void get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, ...);
      void get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, va_list args);

      void get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, ...);
      void get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, va_list args);

      void get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string const& channel_name, bool normalised);

      void get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string const& channel_name, bool normalised, float multiplier);

      static void get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& sample, bool normalised);

      static void get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& sample, bool normalised, float multiplier);

      void input (BufferPtr const& buffer,
                  VisAudioSampleRateType rate,
                  VisAudioSampleFormatType format,
                  VisAudioSampleChannelType channeltype);

      void input (BufferPtr const& buffer,
                  VisAudioSampleRateType rate,
                  VisAudioSampleFormatType format,
                  std::string const& channel_name);

      static void normalise_spectrum (BufferPtr const& buffer);

  private:

      class Impl;

      ScopedPtr<Impl> m_impl;
  };

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus
typedef LV::Audio VisAudio;
#else
typedef struct _VisAudio VisAudio;
struct _VisAudio;
#endif

LV_BEGIN_DECLS

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

LV_API visual_size_t visual_audio_sample_rate_get_length (VisAudioSampleRateType rate);
LV_API visual_size_t visual_audio_sample_format_get_size (VisAudioSampleFormatType format);
LV_API int visual_audio_sample_format_is_signed (VisAudioSampleFormatType format);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_AUDIO_H */
