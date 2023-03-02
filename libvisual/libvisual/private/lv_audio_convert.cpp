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

#include "config.h"
#include "lv_audio_convert.hpp"
#include "lv_audio.h"
#include "lv_mem.h"
#include <array>
#include <concepts>
#include <type_traits>
#include <limits>

using namespace LV;

namespace {

  template <typename D, typename S>
  constexpr bool is_same_signedness_v =
      (std::is_signed_v<D> && std::is_signed_v<S>) ||
      (std::is_unsigned_v<D> && std::is_unsigned_v<S>);

  template <std::signed_integral T>
  constexpr T half_range ()
  {
      return std::numeric_limits<T>::max ();
  }

  template <std::unsigned_integral T>
  constexpr T half_range ()
  {
      return std::numeric_limits<T>::max () / 2 + 1;
  }

  template <std::signed_integral T>
  constexpr T zero ()
  {
      return 0;
  }

  template <std::unsigned_integral T>
  constexpr T zero ()
  {
      return std::numeric_limits<T>::max () / 2 + 1;
  }

  template <typename D, typename S>
  constexpr int shifter()
  {
      if constexpr (sizeof (S) > sizeof (D))
          return int (sizeof (S) - sizeof (D)) << 3;
      else
          return int (sizeof (D) - sizeof (S)) << 3;
  }

  // same format conversion
  template <typename T>
  inline void convert_sample_array (T* dst, T const* src, std::size_t count)
  {
      visual_mem_copy (dst, src, sizeof (T) * count);
  }

  // signed->unsigned int conversion (same width)
  template <std::unsigned_integral D, std::signed_integral S>
  requires (sizeof (D) == sizeof (S))
  inline void convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      constexpr auto a {zero<D> ()};

      auto src_end {src + count};

      while (src != src_end) {
          *dst = *src + a;
          dst++;
          src++;
      }
  }

  // unsigned->signed int conversion (same width)
  template <std::signed_integral D, std::unsigned_integral S>
  requires (sizeof (D) == sizeof (S))
  inline void convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      constexpr auto a {zero<S> ()};

      auto src_end {src + count};

      while (src != src_end) {
          *dst = *src - a;
          dst++;
          src++;
      }
  }

  // int->float conversions
  template <std::integral S>
  inline void convert_sample_array (float* dst, S const* src, std::size_t count)
  {
      constexpr float a {1.0 / float (half_range<S> ())};
      constexpr float b {-zero<S>() * a};

      S const* src_end = src + count;

      while (src != src_end) {
          *dst = *src * a + b;
          dst++;
          src++;
      }
  }

  // float->int conversions
  template <std::integral D>
  inline void convert_sample_array (D* dst, float const* src, std::size_t count)
  {
      constexpr auto a {float (half_range<D> ())};
      constexpr auto b {zero<D> ()};

      auto src_end {src + count};

      while (src != src_end) {
          *dst = *src * a + b;
          dst++;
          src++;
      }
  }

  // narrowing/widening int conversion (same signedness)
  template <std::integral D, std::integral S>
  requires (is_same_signedness_v<D, S> && sizeof (D) != sizeof (S))
  inline void convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      constexpr auto shift {shifter<D, S> ()};

      auto src_end {src + count};

      if constexpr (sizeof (S) > sizeof (D)) {
          // narrowing
          while (src != src_end) {
              *dst = *src >> shift;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = *src << shift;
              dst++;
              src++;
          }
      }
  }

  // narrowing/widening unsigned->signed int conversion
  template <std::signed_integral D, std::unsigned_integral S>
  requires (sizeof (D) != sizeof (S))
  inline void convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      constexpr auto a {zero<D>()};
      constexpr auto shift {shifter<D, S> ()};

      auto src_end {src + count};

      if constexpr (sizeof (D) < sizeof (S)) {
          // narrowing
          while (src != src_end) {
              *dst = D(*src >> shift) - a;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = D(*src << shift) - a;
              dst++;
              src++;
          }
      }
  }

  // narrowing/widening signed->unsigned int conversion
  template <std::unsigned_integral D, std::signed_integral S>
  requires (sizeof (D) != sizeof (S))
  inline void convert_sample_array (D* dst, S const* src, std::size_t count)
  {
      constexpr auto a {zero<D>()};
      constexpr auto shift {shifter<D, S> ()};

      auto src_end {src + count};

      if constexpr (sizeof (D) < sizeof (S)) {
          // narrowing
          while (src != src_end) {
              *dst = D(*src >> shift) + a;
              dst++;
              src++;
          }
      } else {
          // widening
          while (src != src_end) {
              *dst = D(*src << shift) + a;
              dst++;
              src++;
          }
      }
  }

  template <typename D, typename S>
  void convert (void* dst, void const* src, std::size_t size)
  {
      convert_sample_array (static_cast<D*> (dst), static_cast<S const*> (src), size / sizeof (S));
  }

  using ConvertFunc = void (*) (void*, void const*, std::size_t);

  constexpr ConvertFunc convert_func_table[7][7] = {
      {
          convert<uint8_t, uint8_t>,
          convert<uint8_t, int8_t>,
          convert<uint8_t, uint16_t>,
          convert<uint8_t, int16_t>,
          convert<uint8_t, uint32_t>,
          convert<uint8_t, int32_t>,
          convert<uint8_t, float>
      },

      {
          convert<int8_t, uint8_t>,
          convert<int8_t, int8_t>,
          convert<int8_t, uint16_t>,
          convert<int8_t, int16_t>,
          convert<int8_t, uint32_t>,
          convert<int8_t, int32_t>,
          convert<int8_t, float>
      },

      {
          convert<uint16_t, uint8_t>,
          convert<uint16_t, int8_t>,
          convert<uint16_t, uint16_t>,
          convert<uint16_t, int16_t>,
          convert<uint16_t, uint32_t>,
          convert<uint16_t, int32_t>,
          convert<uint16_t, float>
      },

      {
          convert<int16_t, uint8_t>,
          convert<int16_t, int8_t>,
          convert<int16_t, uint16_t>,
          convert<int16_t, int16_t>,
          convert<int16_t, uint32_t>,
          convert<int16_t, int32_t>,
          convert<int16_t, float>
      },

      {
          convert<uint32_t, uint8_t>,
          convert<uint32_t, int8_t>,
          convert<uint32_t, uint16_t>,
          convert<uint32_t, int16_t>,
          convert<uint32_t, uint32_t>,
          convert<uint32_t, int32_t>,
          convert<uint32_t, float>
      },

      {
          convert<int32_t, uint8_t>,
          convert<int32_t, int8_t>,
          convert<int32_t, uint16_t>,
          convert<int32_t, int16_t>,
          convert<int32_t, uint32_t>,
          convert<int32_t, int32_t>,
          convert<int32_t, float>
      },

      {
          convert<float, uint8_t>,
          convert<float, int8_t>,
          convert<float, uint16_t>,
          convert<float, int16_t>,
          convert<float, uint32_t>,
          convert<float, int32_t>,
          convert<float, float>
      }
  };

  template <typename T>
  inline void deinterleave_stereo_sample_array (T* dest1, T* dest2, T const* src, std::size_t count)
  {
      auto src_end {src + count};

      while (src != src_end) {
          *dest1 = src[0];
          *dest2 = src[1];

          dest1++;
          dest2++;
          src += 2;
      }
  }

  template <typename T>
  void deinterleave_stereo (void* dest1, void* dest2, void const* src, std::size_t size)
  {
      deinterleave_stereo_sample_array (static_cast<T*> (dest1), static_cast<T*> (dest2), static_cast<T const*> (src), size / sizeof (T));
  }

  constexpr std::array deinterleave_stereo_func_table {
      deinterleave_stereo<uint8_t>,
      deinterleave_stereo<int8_t>,
      deinterleave_stereo<uint16_t>,
      deinterleave_stereo<int16_t>,
      deinterleave_stereo<uint32_t>,
      deinterleave_stereo<int32_t>,
      deinterleave_stereo<float>
  };

} // anonymous namespace

namespace LV {

  void AudioConvert::convert_samples (BufferPtr const&         dest,
                                      VisAudioSampleFormatType dest_format,
                                      BufferConstPtr const&    src,
                                      VisAudioSampleFormatType src_format)
    {
        auto dbuf {dest->get_data ()};
        auto sbuf {src->get_data ()};
        auto size {src->get_size ()};

        const int i = int (dest_format) - 1;
        const int j = int (src_format)  - 1;

        convert_func_table[i][j] (dbuf, sbuf, size);
    }

  void AudioConvert::deinterleave_stereo_samples (BufferPtr const&         dest1,
                                                  BufferPtr const&         dest2,
                                                  BufferConstPtr const&    src,
                                                  VisAudioSampleFormatType format)
  {
      auto dbuf1 {dest1->get_data ()};
      auto dbuf2 {dest2->get_data ()};
      auto sbuf  {src->get_data ()};
      auto size  {src->get_size ()};

      const int i = int (format) - 1;

      deinterleave_stereo_func_table[i] (dbuf1, dbuf2, sbuf, size);
  }

} // LV namespace
