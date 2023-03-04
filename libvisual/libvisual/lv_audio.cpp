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

#include "config.h"
#include "lv_audio.h"
#include "private/lv_audio_convert.hpp"
#include "private/lv_audio_stream.hpp"
#include "private/lv_string_hash.hpp"
#include "lv_common.h"
#include "lv_fourier.h"
#include "lv_math.h"
#include "lv_time.h"
#include <cstdarg>
#include <memory>
#include <unordered_map>
#include <vector>

namespace LV {

  class AudioChannel;

  typedef std::unique_ptr<AudioChannel> AudioChannelPtr;

  class Audio::Impl
  {
  public:

      using ChannelList = std::unordered_map<std::string, AudioChannelPtr, StringHash, std::equal_to<>>;

      ChannelList channels;

      void upload_to_channel (std::string_view name, BufferConstPtr const& samples, Time const& timestamp);

      AudioChannel* get_channel (std::string_view name) const;
  };

  class AudioChannel
  {
  public:

      std::string name;
      AudioStream stream;

      explicit AudioChannel (std::string_view name);

      ~AudioChannel ();

      // Non-copyable
      AudioChannel (AudioChannel const&) = delete;
      AudioChannel& operator= (AudioChannel const&) = delete;

      void add_samples (BufferConstPtr const& samples, Time const& timestamp);
  };

  namespace {

    void sample_buffer_mix (BufferPtr const& dest, BufferPtr const& src, float multiplier)
    {
        visual_return_if_fail (dest->get_size () == src->get_size ());

        auto dbuf = static_cast<float*> (dest->get_data ());
        auto const sbuf = static_cast<float const*> (src->get_data ());

        std::size_t scnt = dest->get_size () / sizeof (float);

        for (unsigned int i = 0; i < scnt; i++)
            dbuf[i] += sbuf[i] * multiplier;
    }

  } // anonymous

  void Audio::Impl::upload_to_channel (std::string_view name, BufferConstPtr const& samples, Time const& timestamp)
  {
      auto entry {channels.find (name)};

      if (entry == channels.end ()) {
          entry = channels.emplace (name, std::make_unique<AudioChannel> (name)).first;
      }

      entry->second->add_samples (samples, timestamp);
  }

  AudioChannel* Audio::Impl::get_channel (std::string_view name) const
  {
      auto entry = channels.find (name);
      return entry != channels.end () ? entry->second.get () : nullptr;
  }

  AudioChannel::AudioChannel (std::string_view name_)
      : name (name_)
  {}

  AudioChannel::~AudioChannel ()
  {
      // empty
  }

  void AudioChannel::add_samples (BufferConstPtr const& samples, Time const& timestamp)
  {
      stream.write (samples, timestamp);
  }

  Audio::Audio ()
      : m_impl (new Impl)
  {
      // empty
  }

  Audio::Audio (Audio&& rhs)
      : m_impl {std::move (rhs.m_impl)}
  {
      // empty
  }

  Audio::~Audio ()
  {
      // empty
  }

  Audio& Audio::operator= (Audio&& rhs)
  {
      m_impl.swap (rhs.m_impl);
      return *this;
  }

  bool Audio::get_sample (BufferPtr const& buffer, std::string_view channel_name)
  {
      auto channel = m_impl->get_channel (channel_name);

      if (!channel) {
          buffer->fill (0);
          return false;
      }

      channel->stream.read (buffer, buffer->get_size ());

      return true;
  }

  void Audio::get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, ...)
  {
      va_list args;

      va_start (args, channels);
      get_sample_mixed_simple (buffer, channels, args);
      va_end (args);
  }

  void Audio::get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, va_list args)
  {
      visual_return_if_fail (channels > 0);

      std::vector<std::string> channel_ids (channels);

      for (unsigned int i = 0; i < channels; i++)
          channel_ids[i] = va_arg (args, const char *);

      float factor = 1.0 / channels;

      auto channel_samples = Buffer::create (buffer->get_size ());
      buffer->fill (0);

      // The mixing loop
      for (unsigned int i = 0; i < channels; i++) {
          if (get_sample (channel_samples, channel_ids[i])) {
              sample_buffer_mix (buffer, channel_samples, factor);
          }
      }
  }

  void Audio::get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, ...)
  {
      va_list args;

      va_start (args, channels);
      get_sample_mixed (buffer, divide, channels, args);
      va_end (args);
  }

  void Audio::get_sample_mixed (BufferPtr const& buffer, bool divide, unsigned int channels, va_list args)
  {
      visual_return_if_fail (channels > 0);

      std::vector<std::string> channel_ids (channels);
      for (unsigned int i = 0; i < channels; i++) {
          channel_ids[i] = va_arg (args, const char *);
      }

      std::vector<double> channel_factors (channels);
      for (unsigned int i = 0; i < channels; i++)
          channel_factors[i] = va_arg (args, double);

      float factor = divide ? (1.0 / channels) : 1.0;

      auto channel_samples = Buffer::create (buffer->get_size ());
      buffer->fill (0);

      // The mixing loop
      for (unsigned int i = 0; i < channels; i++) {
          if (get_sample (channel_samples, channel_ids[i])) {
              sample_buffer_mix (buffer, channel_samples, channel_factors[i] * factor);
          }
      }
  }

  void Audio::get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string_view channel_name, bool normalised)
  {
      auto sample = Buffer::create (samplelen);

      if (get_sample (sample, channel_name))
          get_spectrum_for_sample (buffer, sample, normalised);
      else
          buffer->fill (0);
  }

  void Audio::get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string_view channel_name, bool normalised, float multiplier)
  {
      auto spectrum {Buffer::create (buffer->get_size ())};
      get_spectrum (spectrum, samplelen, channel_name, normalised);

      auto result_ptr {static_cast<float*> (buffer->get_data ())};
      auto spectrum_ptr {static_cast<float const*> (spectrum->get_data ())};
      auto count {buffer->get_size () / sizeof (float)};

      visual_math_simd_mul_floats_float (result_ptr, spectrum_ptr, multiplier, count);
  }

  void Audio::get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& sample, bool normalised)
  {
      DFT dft (buffer->get_size () / sizeof (float),
               sample->get_size () / sizeof (float));

      // Fourier analyze the pcm data
      dft.perform (static_cast<float*> (buffer->get_data ()),
                   static_cast<float*> (sample->get_data ()));

      if (normalised)
          normalise_spectrum (buffer);
  }

  void Audio::get_spectrum_for_sample (BufferPtr const& buffer, BufferConstPtr const& sample, bool normalised, float multiplier)
  {
      auto spectrum {Buffer::create (buffer->get_size ())};
      get_spectrum_for_sample (spectrum, sample, normalised);

      auto result_ptr {static_cast<float*> (buffer->get_data ())};
      auto spectrum_ptr {static_cast<float const*> (spectrum->get_data ())};
      auto count {buffer->get_size () / sizeof (float)};

      visual_math_simd_mul_floats_float (result_ptr, spectrum_ptr, multiplier, count);
  }

  void Audio::normalise_spectrum (BufferPtr const& buffer)
  {
      DFT::log_scale_standard (static_cast<float*> (buffer->get_data ()),
                               static_cast<float*> (buffer->get_data ()),
                               buffer->get_size () / sizeof (float));
  }

  void Audio::input (BufferPtr const&          buffer,
                     VisAudioSampleRateType    rate,
                     VisAudioSampleFormatType  format,
                     VisAudioSampleChannelType channeltype)
  {
      (void)rate;

      auto timestamp = Time::now ();

      auto sample_count = buffer->get_size () / visual_audio_sample_format_get_size (format);

      auto converted_buffer = Buffer::create (sample_count * sizeof (float));

      AudioConvert::convert_samples (converted_buffer,
                                     VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
                                     buffer,
                                     format);

      switch (channeltype) {
          case VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO: {
              auto samples1 = Buffer::create (sample_count/2 * sizeof (float));
              auto samples2 = Buffer::create (sample_count/2 * sizeof (float));

              AudioConvert::deinterleave_stereo_samples (samples1, samples2, converted_buffer, VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT);

              m_impl->upload_to_channel (VISUAL_AUDIO_CHANNEL_LEFT, samples1, timestamp);
              m_impl->upload_to_channel (VISUAL_AUDIO_CHANNEL_RIGHT, samples2, timestamp);

              return;
          }
          default: {
              visual_log (VISUAL_LOG_CRITICAL, "Only stereo input is currently supported");
              return;
          }
      }
  }

  void Audio::input (BufferPtr const&         buffer,
                     VisAudioSampleRateType   rate,
                     VisAudioSampleFormatType format,
                     std::string_view         channel_name)
  {
      (void)rate;

      auto timestamp = Time::now ();

      auto sample_count = buffer->get_size () / visual_audio_sample_format_get_size (format);

      auto converted_buffer = Buffer::create (sample_count * sizeof (float));

      AudioConvert::convert_samples (converted_buffer,
                                     VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
                                     buffer,
                                     format);

      m_impl->upload_to_channel (channel_name, converted_buffer, timestamp);
  }

} // LV namespace

visual_size_t visual_audio_sample_rate_get_length (VisAudioSampleRateType rate)
{
    visual_return_val_if_fail (rate < VISUAL_AUDIO_SAMPLE_RATE_LAST, 0);

    static visual_size_t ratelengthtable[] = {
        0,      // VISUAL_AUDIO_SAMPLE_RATE_NONE
        8000,   // VISUAL_AUDIO_SAMPLE_RATE_8000
        11250,  // VISUAL_AUDIO_SAMPLE_RATE_11250
        22500,  // VISUAL_AUDIO_SAMPLE_RATE_22500
        32000,  // VISUAL_AUDIO_SAMPLE_RATE_32000
        44100,  // VISUAL_AUDIO_SAMPLE_RATE_44100
        48000,  // VISUAL_AUDIO_SAMPLE_RATE_48000
        96000   // VISUAL_AUDIO_SAMPLE_RATE_96000
    };

    return ratelengthtable[rate];
}

visual_size_t visual_audio_sample_format_get_size (VisAudioSampleFormatType format)
{
    visual_return_val_if_fail (format < VISUAL_AUDIO_SAMPLE_FORMAT_LAST, 0);

    static int formatsizetable[] = {
        0, // VISUAL_AUDIO_SAMPLE_FORMAT_NONE
        1, // VISUAL_AUDIO_SAMPLE_FORMAT_U8
        1, // VISUAL_AUDIO_SAMPLE_FORMAT_S8
        2, // VISUAL_AUDIO_SAMPLE_FORMAT_U16
        2, // VISUAL_AUDIO_SAMPLE_FORMAT_S16
        4, // VISUAL_AUDIO_SAMPLE_FORMAT_U32
        4, // VISUAL_AUDIO_SAMPLE_FORMAT_S32
        4  // VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT
    };

    return formatsizetable[format];
}

int visual_audio_sample_format_is_signed (VisAudioSampleFormatType format)
{
    visual_return_val_if_fail (format < VISUAL_AUDIO_SAMPLE_FORMAT_LAST, -1);

    static bool formatsignedtable[] = {
        false, // VISUAL_AUDIO_SAMPLE_FORMAT_NONE
        false, // VISUAL_AUDIO_SAMPLE_FORMAT_U8
        true,  // VISUAL_AUDIO_SAMPLE_FORMAT_S8
        false, // VISUAL_AUDIO_SAMPLE_FORMAT_U16
        true,  // VISUAL_AUDIO_SAMPLE_FORMAT_S16
        false, // VISUAL_AUDIO_SAMPLE_FORMAT_U32
        true,  // VISUAL_AUDIO_SAMPLE_FORMAT_S32
        true   // VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT
    };

    return formatsignedtable[format];
}
