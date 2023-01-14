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
#include "lv_fourier.h"
#include "lv_common.h"
#include "lv_math.h"
#include <cmath>
#include <vector>
#include <unordered_map>

// Log scale settings
#define AMP_LOG_SCALE_THRESHOLD0    0.001f
#define AMP_LOG_SCALE_DIVISOR       6.908f  // divisor = -log threshold

namespace LV {

  namespace {

    enum DFTMethod
    {
        DFT_METHOD_BRUTE_FORCE,
        DFT_METHOD_FFT
    };

    class DFTCache
    {
    public:

        class Entry
        {
        public:

            std::vector<float> bitrevtable;
            std::vector<float> sintable;
            std::vector<float> costable;

            // FIXME: Eliminate this constructor
            Entry () {}

            Entry (DFTMethod method, unsigned int sample_count);

        private:

            void fft_bitrev_table_init (unsigned int sample_count);
            void fft_cossin_table_init (unsigned int sample_count);
            void dft_cossin_table_init (unsigned int sample_count);
        };

        Entry const& get_entry (DFTMethod method, unsigned int sample_count);

    private:

        typedef std::unordered_map<unsigned int, Entry> Table;

        Table m_cache;
    };

    DFTCache dft_cache;

  } // anonymous namespace


  class DFT::Impl
  {
  public:
      unsigned int       sample_count;
      unsigned int       spectrum_size;
      unsigned int       samples_out;
      DFTMethod          method;
      std::vector<float> real;
      std::vector<float> imag;

      Impl (unsigned int samples_out, unsigned int samples_in);

      void perform_brute_force (float const* input);
      void perform_fft_radix2_dit (float const* input);

      DFTMethod best_method (unsigned int sample_count);
  };


  namespace {

    DFTCache::Entry const& DFTCache::get_entry (DFTMethod method, unsigned int sample_count)
    {
        auto entry = m_cache.find (sample_count);
        if (entry != m_cache.end ())
            return entry->second;

        m_cache.emplace (sample_count, Entry {method, sample_count});

        return m_cache[sample_count];
    }

    DFTCache::Entry::Entry (DFTMethod method, unsigned int sample_count)
    {
        switch (method) {
            case DFT_METHOD_BRUTE_FORCE:
                dft_cossin_table_init (sample_count);
                break;

            case DFT_METHOD_FFT:
                fft_bitrev_table_init (sample_count);
                fft_cossin_table_init (sample_count);
                break;
        }
    }

    void DFTCache::Entry::fft_bitrev_table_init (unsigned int sample_count)
    {
        bitrevtable.clear ();
        bitrevtable.reserve (sample_count);

        for (unsigned int i = 0; i < sample_count; i++)
            bitrevtable.push_back (i);

        unsigned int j = 0;

        for (unsigned int i = 0; i < sample_count; i++) {
            if (j > i) {
                std::swap (bitrevtable[i], bitrevtable[j]);
            }

            unsigned int m = sample_count >> 1;

            while (m >= 1 && j >= m) {
                j -= m;
                m >>= 1;
            }

            j += m;
        }
    }

    void DFTCache::Entry::fft_cossin_table_init (unsigned int sample_count)
    {
        unsigned int dft_size = 2;
        unsigned int tab_size = 0;

        while (dft_size <= sample_count) {
            tab_size++;
            dft_size <<= 1;
        }

        sintable.clear ();
        sintable.reserve (tab_size);

        costable.clear ();
        costable.reserve (tab_size);

        dft_size = 2;

        while (dft_size <= sample_count) {
            float theta = -2.0f * VISUAL_MATH_PI / dft_size;

            costable.push_back (std::cos (theta));
            sintable.push_back (std::sin (theta));

            dft_size <<= 1;
        }
    }

    void DFTCache::Entry::dft_cossin_table_init (unsigned int sample_count)
    {
        sintable.clear ();
        sintable.reserve (sample_count);

        costable.clear ();
        costable.reserve (sample_count);

        for (unsigned int i = 0; i < sample_count; i++) {
            float theta = (-2.0f * VISUAL_MATH_PI * i) / sample_count;

            costable.push_back (std::cos (theta));
            sintable.push_back (std::sin (theta));
        }
    }

  } // anonymous namespace

  DFT::DFT (unsigned int samples_out, unsigned int samples_in)
      : m_impl (new Impl (samples_out, samples_in))
  {
      // empty
  }

  DFT::DFT (DFT&& rhs)
      : m_impl {std::move (rhs.m_impl)}
  {
      // nothing
  }

  DFT::~DFT ()
  {
      // empty
  }

  DFT& DFT::operator= (DFT&& rhs)
  {
      m_impl.swap (rhs.m_impl);
      return *this;
  }

  void DFT::perform (float *output, float const* input)
  {
      visual_return_if_fail (output != nullptr);
      visual_return_if_fail (input  != nullptr);

      switch (m_impl->method) {
          case DFT_METHOD_BRUTE_FORCE:
              m_impl->perform_brute_force (input);
              break;

          case DFT_METHOD_FFT:
              m_impl->perform_fft_radix2_dit (input);
              break;
      }

      visual_math_simd_complex_scaled_norm (output, m_impl->real.data (), m_impl->imag.data (),
                                            1.0 / m_impl->sample_count, m_impl->samples_out);
  }

  void DFT::log_scale (float *output, float const* input, unsigned int size)
  {
      visual_return_if_fail (output != nullptr);
      visual_return_if_fail (input  != nullptr);

      return log_scale_standard (output, input, size);
  }

  void DFT::log_scale_standard (float *output, float const* input, unsigned int size)
  {
      visual_return_if_fail (output != nullptr);
      visual_return_if_fail (input  != nullptr);

      return log_scale_custom (output, input, size, AMP_LOG_SCALE_DIVISOR);
  }

  void DFT::log_scale_custom (float* output, float const* input, unsigned int size, float log_scale_divisor)
  {
      visual_return_if_fail (output != nullptr);
      visual_return_if_fail (input  != nullptr);

      for (unsigned int i = 0; i < size; i++) {
          if (input[i] > AMP_LOG_SCALE_THRESHOLD0)
              output[i] = 1.0f + log (input[i]) / log_scale_divisor;
          else
              output[i] = 0.0f;
      }
  }

  DFT::Impl::Impl (unsigned int samples_out_, unsigned int samples_in_)
      : sample_count  (samples_in_),
		spectrum_size (sample_count/2 + 1),
        samples_out   (std::min (samples_out_, spectrum_size)),
        method        (best_method (sample_count)),
        real          (sample_count),
        imag          (sample_count)
  {
      // empty
  }

  DFTMethod DFT::Impl::best_method (unsigned int sample_count)
  {
      if (visual_math_is_power_of_2 (sample_count))
          return DFT_METHOD_FFT;
      else
          return DFT_METHOD_BRUTE_FORCE;
  }

  void DFT::Impl::perform_brute_force (float const* input)
  {
      DFTCache::Entry const& fcache = dft_cache.get_entry (method, sample_count);

      for (unsigned int i = 0; i < spectrum_size; i++) {
          float xr = 0.0f;
          float xi = 0.0f;

          float wr = 1.0f;
          float wi = 0.0f;

          for (unsigned int j = 0; j < sample_count; j++) {
              xr += input[j] * wr;
              xi += input[j] * wi;

              float wtemp = wr;

              wr = wr    * fcache.costable[i] - wi * fcache.sintable[i];
              wi = wtemp * fcache.sintable[i] + wi * fcache.costable[i];
          }

          real[i] = xr;
          imag[i] = xi;
      }
  }

  void DFT::Impl::perform_fft_radix2_dit (float const* input)
  {
    DFTCache::Entry const& fcache = dft_cache.get_entry (method, sample_count);

    for (unsigned int i = 0; i < sample_count; i++) {
        unsigned int idx = fcache.bitrevtable[i];

        if (idx < sample_count)
            real[i] = input[idx];
        else
            real[i] = 0;
    }

    unsigned int dft_size = 2;
    unsigned int t = 0;

    while (dft_size <= sample_count) {
        float wpr = fcache.costable[t];
        float wpi = fcache.sintable[t];

        float wr = 1.0f;
        float wi = 0.0f;

        unsigned int half_dft_size = dft_size >> 1;

        for (unsigned int m = 0; m < half_dft_size; m++) {
            for (unsigned int i = m; i < sample_count; i += dft_size) {
                unsigned int j = i + half_dft_size;

                float tempr = wr * real[j] - wi * imag[j];
                float tempi = wr * imag[j] + wi * real[j];

                real[j] = real[i] - tempr;
                imag[j] = imag[i] - tempi;

                real[i] += tempr;
                imag[i] += tempi;
            }

            float wtemp;

            wr = (wtemp = wr) * wpr - wi * wpi;
            wi = wi * wpr + wtemp * wpi;
        }

        dft_size <<= 1;
        t++;
    }
  }

} // LV namespace
