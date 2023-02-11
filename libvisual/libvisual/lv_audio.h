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

#ifndef _LV_AUDIO_H
#define _LV_AUDIO_H

#include <libvisual/lv_buffer.h>

/**
 * @defgroup VisAudio VisAudio
 * @{
 */

#define VISUAL_AUDIO_CHANNEL_LEFT  "left"
#define VISUAL_AUDIO_CHANNEL_RIGHT "right"

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

#include <memory>
#include <string_view>
#include <cstdarg>

namespace LV {

  /**
   * Multi-channel audio stream class.
   *
   * @note Samples are stored as 32-bit floating point PCM at 44.1kHz.
   */
  class LV_API Audio
  {
  public:

      /**
       * Default constructor
       */
      Audio ();

      Audio (Audio const&) = delete;

      /**
       * Move constructor
       */
      Audio (Audio&& rhs);

      /**
       * Destructor
       */
      ~Audio ();

      Audio& operator= (Audio const&) = delete;

      /**
       * Move assignment operator
       */
      Audio& operator= (Audio&& rhs);

      /**
       * Retrieves samples from a channel.
       *
       * @note Output samples will be truncated to fit the user-supplied buffer.
       *
       * @param[out] buffer  buffer to hold the retrieved samples (32-bit floating point PCM)
       * @param channel_name name of channel
       *
       * @return true if successful, false if channel does not exist
       */
      bool get_sample (BufferPtr const& buffer, std::string_view channel_name);

      /**
       * Returns samples downmixed by averaging a set of channels.
       *
       * @note Output samples will be truncated to fit the user-supplied buffer.
       *
       * @param[out] buffer buffer to hold the mixed samples (32-bit floating point PCM)
       * @param channels    number of channels
       * @param ...         list of channel names (each of type const char *)
       */
      void get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, ...);

      void get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, va_list args);

      /**
       * Returns samples downmixed by weighted summing or averaging a set of channels.
       *
       * @note Output samples will be truncated to fit the user-supplied buffer.
       *
       * @param[out] buffer buffer to hold the mixed samples (32-bit floating point PCM)
       * @param divide      perform averaging
       * @param channels    number of channels
       * @param ...         list of channel names (each of type const char *), followed by a list of respective weights (each of type double)
       */
      void get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, ...);

      void get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, va_list args);

      /**
       * Returns the amplitude spectrum of a set of samples from a channel.
       *
       * @note The output spectrum will be truncated to fit the user-supplied buffer.
       *
       * @param[out] buffer  buffer to hold the amplitude spectrum (32-bit floats)
       * @param sample_count number of samples to draw from channel
       * @param channel_name name of channel
       * @param normalised   normalise ampltitudes to [0.0, 1.0]
       */
      void get_spectrum (BufferPtr const& buffer, std::size_t sample_count, std::string_view channel_name, bool normalised);

      void get_spectrum (BufferPtr const& buffer, std::size_t sample_count, std::string_view channel_name, bool normalised, float multiplier);

      /**
       * Returns the amplitude spectrum of a set of samples.
       *
       * @note The output spectrum will be truncated to fit the user-supplied buffer.
       *
       * @param[out] buffer buffer to hold the ampltitude spectrum (32-bit floats)
       * @param samples     input samples
       * @param normalised  normalise ampltitudes to [0.0, 1.0]
       */
      static void get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& samples, bool normalised);

      static void get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& samples, bool normalised, float multiplier);

      static void normalise_spectrum (BufferPtr const& buffer);

      /**
       * Adds an interleaved set of samples to the stream.
       *
       * @param buffer       buffer containing the input samples
       * @param rate         sampling rate
       * @param format       sample format
       * @param channel_type channel format
       */
      void input (BufferPtr const& buffer,
                  VisAudioSampleRateType rate,
                  VisAudioSampleFormatType format,
                  VisAudioSampleChannelType channel_type);

      /**
       * Adds a set of channel samples to the stream.
       *
       * @param buffer       buffer containing the input samples
       * @param rate         sampling rate
       * @param format       sample format
       * @param channel_name name of channel
       */
      void input (BufferPtr const& buffer,
                  VisAudioSampleRateType rate,
                  VisAudioSampleFormatType format,
                  std::string_view channel_name);

  private:

      class Impl;

      std::unique_ptr<Impl> m_impl;
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

LV_NODISCARD LV_API VisAudio *visual_audio_new (void);

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
