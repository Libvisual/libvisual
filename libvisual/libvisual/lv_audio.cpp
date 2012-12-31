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
#include "private/lv_buffer_ring.hpp"
#include "lv_common.h"
#include "lv_fourier.h"
#include "lv_math.h"
#include "lv_time.h"
#include "lv_util.hpp"
#include <cstdarg>
#include <unordered_map>
#include <vector>

namespace LV {

  class AudioChannel;
  class AudioSample;

  typedef std::unique_ptr<AudioChannel> AudioChannelPtr;

  class Audio::Impl
  {
  public:

      typedef std::unordered_map<std::string, AudioChannelPtr> ChannelList;

      ChannelList channels;

      Impl ();

      ~Impl ();

      void add_channel (std::string const& name, AudioSample& samples);

      AudioChannel* get_channel (std::string const& name) const;
  };

  class AudioChannel
  {
  public:

      std::string   name;
      BufferRingPtr samples;
      Time          samples_timeout;
      float         factor;

      explicit AudioChannel (std::string const& name);

      ~AudioChannel ();

      // Non-copyable
      AudioChannel (AudioChannel const&) = delete;
      AudioChannel& operator= (AudioChannel const&) = delete;

      void add_samples (AudioSample& sample);
  };

  class AudioSample
  {
  public:

      Time                     timestamp;
      VisAudioSampleRateType   rate;
      VisAudioSampleFormatType format;
      BufferPtr                buffer;
      BufferPtr                processed;

      AudioSample (AudioSample const&) = delete;

      AudioSample (BufferPtr const&         buffer_,
                   Time const&              timestamp_,
                   VisAudioSampleFormatType format_,
                   VisAudioSampleRateType   rate_);

      ~AudioSample ();

      AudioSample& operator= (AudioSample const&) = delete;
  };

  namespace {

    void sample_destroy_func (BufferRingEntry& entry)
    {
        auto sample = static_cast<AudioSample*> (entry.func_data);
        delete sample;
    }

    int sample_size_func (BufferRingEntry& entry)
    {
        auto sample = static_cast<AudioSample*> (entry.func_data);

        return (sample->buffer->get_size () /
                visual_audio_sample_format_get_size (sample->format)) * sizeof (float);
    }

    BufferPtr sample_data_func (BufferRingEntry& entry)
    {
        auto sample = static_cast<AudioSample*> (entry.func_data);

        /* We have internal format ready */
        if (sample->processed) {
            return sample->processed;
        }

        sample->processed = Buffer::create ((sample->buffer->get_size () /
                                             visual_audio_sample_format_get_size (sample->format)) * sizeof (float));

        AudioConvert::convert_samples (sample->processed,
                                       VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
                                       sample->buffer,
                                       sample->format);

        return sample->processed;
    }

    void sample_buffer_mix (BufferPtr const& dest, BufferPtr const& src, bool divide, float multiplier)
    {
        visual_return_if_fail (dest->get_size () == src->get_size ());

        auto dbuf = static_cast<float*> (dest->get_data ());
        auto const sbuf = static_cast<float const*> (src->get_data ());

        std::size_t scnt = dest->get_size () / sizeof (float);

        /* FIXME make simd version of these */
        if (!divide) {
            if (multiplier == 1.0) {
                for (unsigned int i = 0; i < scnt; i++)
                    dbuf[i] += sbuf[i];
            } else {
                for (unsigned int i = 0; i < scnt; i++)
                    dbuf[i] += (sbuf[i] * multiplier);
            }
        } else {
            if (multiplier == 1.0) {
                for (unsigned int i = 0; i < scnt; i++)
                    dbuf[i] = (dbuf[i] + sbuf[i]) * 0.5;
            } else {
                for (unsigned int i = 0; i < scnt; i++)
                    dbuf[i] = (dbuf[i] + (sbuf[i] * multiplier)) * 0.5;
            }
        }
    }

  } // anonymous


  AudioSample::AudioSample (BufferPtr const&         buffer_,
                            Time const&              timestamp_,
                            VisAudioSampleFormatType format_,
                            VisAudioSampleRateType   rate_)
      : timestamp (timestamp_)
      , rate      (rate_)
      , format    (format_)
      , buffer    (buffer_)
  {
  }

  AudioSample::~AudioSample ()
  {
      // empty
  }

  Audio::Impl::Impl ()
  {
      // empty
  }

  Audio::Impl::~Impl ()
  {
      // empty
  }

  void Audio::Impl::add_channel (std::string const& name, AudioSample& samples)
  {
      auto channel = make_unique<AudioChannel> (name);
      channel->add_samples (samples);

      channels[name] = std::move (channel);
  }

  AudioChannel* Audio::Impl::get_channel (std::string const& name) const
  {
      auto entry = channels.find (name);
      return entry != channels.end () ? entry->second.get () : nullptr;
  }

  AudioChannel::AudioChannel (std::string const& name_)
      : name            (name_)
      , samples         (new BufferRing)
      , samples_timeout (Time (1, 0))
      , factor          (1.0)
  {}

  AudioChannel::~AudioChannel ()
  {
      // empty
  }

  void AudioChannel::add_samples (AudioSample& sample)
  {
      samples->add_function (sample_data_func,
                             sample_destroy_func,
                             sample_size_func,
                             &sample);
  }

  Audio::Audio ()
      : m_impl (new Impl)
  {
      // empty
  }

  Audio::~Audio ()
  {
      // empty
  }

  bool Audio::get_sample (BufferPtr const& buffer, std::string const& channel_name)
  {
      auto channel = m_impl->get_channel (channel_name);

      if (!channel) {
          buffer->fill (0);
          return false;
      }

      channel->samples->get_data_from_end (buffer.get (), buffer->get_size ());

      return true;
  }

  void Audio::get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, ...)
  {
      va_list args;

      va_start (args, channels);
      get_sample_mixed_simple (buffer, channels, args);
      va_end (args);
  }

  // FIXME: too much code duplication with get_sample_mixed
  void Audio::get_sample_mixed_simple (BufferPtr const& buffer, unsigned int channels, va_list args)
  {
      visual_return_if_fail (channels > 0);

      std::vector<std::string> chanids (channels);

      for (unsigned int i = 0; i < channels; i++)
          chanids[i] = va_arg (args, const char *);

      auto temp = Buffer::create (buffer->get_size ());
      buffer->fill (0);

      bool first = true;

      // The mixing loop
      for (unsigned int i = 0; i < channels; i++) {
          if (get_sample (temp, chanids[i])) {

              AudioChannel* channel = m_impl->get_channel (chanids[i]);

              if (first) {
                  sample_buffer_mix (buffer, temp, false, channel->factor);
                  first = false;
              } else {
                  sample_buffer_mix (buffer, temp, true, channel->factor);
              }
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

      std::vector<std::string> chanids  (channels);
      std::vector<double>      chanmuls (channels);

      for (unsigned int i = 0; i < channels; i++) {
          chanids[i] = va_arg (args, const char *);
      }

      for (unsigned int i = 0; i < channels; i++)
          chanmuls[i] = va_arg (args, double);

      auto temp = Buffer::create (buffer->get_size ());

      buffer->fill (0);

      bool first = true;

      // The mixing loop
      for (unsigned int i = 0; i < channels; i++) {
          if (get_sample (temp, chanids[i])) {
              if (first) {
                  sample_buffer_mix (buffer, temp, false, chanmuls[i]);
                  first = false;
              } else {
                  sample_buffer_mix (buffer, temp, divide, chanmuls[i]);
              }
          }
      }
  }

  void Audio::get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string const& channel_name, bool normalised)
  {
      auto sample = Buffer::create (samplelen);

      if (get_sample (sample, channel_name))
          get_spectrum_for_sample (buffer, sample, normalised);
      else
          buffer->fill (0);
  }

  void Audio::get_spectrum (BufferPtr const& buffer, std::size_t samplelen, std::string const& channel_name, bool normalised, float multiplier)
  {
      get_spectrum (buffer, samplelen, channel_name, normalised);

      auto data = static_cast<float*> (buffer->get_data ());
      std::size_t datasize = buffer->get_size () / sizeof (float);

      visual_math_simd_mul_floats_float (data, data, multiplier, datasize);
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
      get_spectrum_for_sample (buffer, sample, normalised);

      auto data = static_cast<float*> (buffer->get_data ());
      std::size_t datasize = buffer->get_size () / sizeof (float);

      visual_math_simd_mul_floats_float (data, data, multiplier, datasize);
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
      if (channeltype == VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO) {
          auto chan1 = Buffer::create (buffer->get_size () / 2);
          auto chan2 = Buffer::create (buffer->get_size () / 2);

          AudioConvert::deinterleave_stereo_samples (chan1, chan2, buffer, format);

          auto timestamp = Time::now ();

          auto sample1 = new AudioSample (chan1, timestamp, format, rate);
          m_impl->add_channel (VISUAL_AUDIO_CHANNEL_LEFT, *sample1);

          auto sample2 = new AudioSample (chan2, timestamp, format, rate);
          m_impl->add_channel (VISUAL_AUDIO_CHANNEL_RIGHT, *sample2);
      }
  }

  void Audio::input (BufferPtr const&         buffer,
                     VisAudioSampleRateType   rate,
                     VisAudioSampleFormatType format,
                     std::string const&       channel_name)
  {
      auto pcmbuf = Buffer::create ();
      pcmbuf->copy (buffer);

      auto timestamp = Time::now ();

      auto sample = new AudioSample (pcmbuf, timestamp, format, rate);
      m_impl->add_channel (channel_name, *sample);
  }

} // LV namespace

visual_size_t visual_audio_sample_rate_get_length (VisAudioSampleRateType rate)
{
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

    if (rate >= VISUAL_AUDIO_SAMPLE_RATE_LAST)
        return -1; /* FIXME decent error, also for format_get_size */

    return ratelengthtable[rate];
}

visual_size_t visual_audio_sample_format_get_size (VisAudioSampleFormatType format)
{
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

    if (format >= VISUAL_AUDIO_SAMPLE_FORMAT_LAST)
        return -1;

    return formatsizetable[format];
}

int visual_audio_sample_format_is_signed (VisAudioSampleFormatType format)
{
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

    if (format >= VISUAL_AUDIO_SAMPLE_FORMAT_LAST)
        return -1;

    return formatsignedtable[format];
}
