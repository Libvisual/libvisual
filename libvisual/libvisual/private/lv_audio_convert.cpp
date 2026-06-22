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
#include <algorithm>
#include <array>
#include <concepts>
#include <limits>
#include <span>
#include <type_traits>

using namespace LV;

namespace {

  // Checks whether two integral types are both signed or unsigned.
  template <std::integral T, std::integral U>
  constexpr bool is_same_signedness_v = std::is_signed_v<T> == std::is_signed_v<U>;

  // Checks whether an integral type is promoted to unsigned or signed int in arithmetic operations.
  template <std::integral T>
  constexpr bool int_promotable_v = sizeof (T) <= sizeof (int);

  // Checks if an integral type can be exactly representable with a float i.e. fits within the
  // mantissa without loss.
  template <std::integral T>
  constexpr bool is_float_representable_v = std::numeric_limits<T>::digits <= std::numeric_limits<float>::digits;

  // Type constraint for integral samples.
  template <typename T>
  concept integral_sample = std::integral<T> && int_promotable_v<T>;

  // Type constraint for signed integral samples.
  template <typename T>
  concept signed_integral_sample = std::signed_integral<T> && int_promotable_v<T>;

  // Type constraint for unsigned integral samples.
  template <typename T>
  concept unsigned_integral_sample = std::unsigned_integral<T> && int_promotable_v<T>;

  // Returns the 'bias' of an unsigned integral type i.e. the offset that can be added to the minimum value of its
  // signed counterpart to get 0.
  // Example: The bias for uint8_t is 128 as the minimum value of an int8_t is -128.
  template <unsigned_integral_sample T>
  constexpr auto bias () -> unsigned int
  {
      return 1U << (std::numeric_limits<T>::digits - 1);
  }

  // Converts an integral sample to an unsigned integral sample of the same width.
  // Reduces to an identity function if handed an unsigned integral sample.
  // Example: int16_t and uint16_t samples are both converted to uint16_t.
  template <integral_sample T>
  constexpr auto to_unsigned_sample (T x) -> std::make_unsigned_t<T>
  {
      if constexpr (std::is_signed_v<T>) {
          using U = std::make_unsigned_t<T>;
          return x + bias<U> ();
      } else {
          return x;
      }
  }

  // Converts an integral sample to an signed integral sample of the same width.
  // Reduces to an identity function if input is already unsigned.
  // Example: int16_t and uint16_t samples are both converted to uint16_t.
  template <integral_sample T>
  constexpr auto to_signed_sample (T x) -> std::make_signed_t<T>
  {
      if constexpr (!std::is_signed_v<T>) {
          return x - bias<T> ();
      } else {
          return x;
      }
  }

  // Converts an integral sample to a narrower or wider integral type with the
  // same signedness.
  //
  // Caveat: Due to the use of shifts for performance instead of float multiplications, the relative error in _widening_
  // conversions can get as large as 0.038 to 0.039% of the true value (e.g. converting from 8-bit to 16/32-bit).
  template <integral_sample T, integral_sample U>
  constexpr auto widen_or_narrow_sample (T x) -> U
      requires is_same_signedness_v<T, U>
  {
      constexpr auto n_t {std::numeric_limits<T>::digits};
      constexpr auto n_u {std::numeric_limits<U>::digits};

      if constexpr (n_t > n_u) {
          return x >> (n_t - n_u);
      } else if constexpr (n_t < n_u) {
          return x << (n_u - n_t);
      } else {
          return x;
      }
  }

  // Converts an integral sample to 32-bit float.
  template <integral_sample T>
  constexpr auto to_float_sample (T x) -> float
  {
      using U = std::make_unsigned_t<T>;
      using F = std::conditional_t<is_float_representable_v<T>, float, double>;

      constexpr F factor = 1.0 / (0.5 * std::numeric_limits<U>::max ());
      constexpr F one = 1.0;

      return to_unsigned_sample (x) * factor - one;
  }

  // Converts a 32-bit float sample to an integral type.
  template <integral_sample T>
  constexpr auto from_float_sample (float x) -> T
  {
      using U = std::make_unsigned_t<T>;
      using F = std::conditional_t<is_float_representable_v<T>, float, double>;

      constexpr F factor = 0.5 * std::numeric_limits<U>::max ();
      constexpr F one = 1.0;

      auto y {static_cast<U> ((x + one) * factor)};

      if constexpr (std::is_signed_v<T>) {
          return to_signed_sample (y);
      } else {
          return y;
      }
  }

  // Overload for same format conversions.
  template <typename T>
  constexpr void convert_samples (std::span<T> dst, std::span<T const> src)
  {
      auto count {std::min (src.size (), dst.size ())};
      visual_mem_copy (dst.data (), src.data (), count * sizeof (T));
  }

  // Overload for unsigned to unsigned integral conversions.
  template <unsigned_integral_sample D, integral_sample S>
  constexpr void convert_samples (std::span<D> dst, std::span<S const> src)
  {
      auto const count {std::min (src.size (), dst.size ())};
      std::transform (src.begin (), src.begin () + count, dst.begin (),
                      [=] (auto x) {
                          auto y {to_unsigned_sample (x)};
                          return widen_or_narrow_sample<decltype (y), D> (y);
                      });
  }

  // Overload for signed/unsigned to signed integral conversions.
  template <signed_integral_sample D, integral_sample S>
  constexpr void convert_samples (std::span<D> dst, std::span<S const> src)
  {
      auto const count {std::min (src.size (), dst.size ())};
      std::transform (src.begin (), src.begin () + count, dst.begin (),
                      [=] (auto x) {
                          auto y {to_signed_sample (x)};
                          return widen_or_narrow_sample<decltype (y), D> (y);
                      });
  }

  // Overload for integral to float conversions.
  template <integral_sample T>
  constexpr void convert_samples (std::span<float> dst, std::span<T const> src)
  {
      auto const count {std::min (src.size (), dst.size ())};
      std::transform (src.begin (), src.begin () + count, dst.begin (), to_float_sample<T>);
  }

  // Overload for float to integral conversions.
  template <integral_sample T>
  constexpr void convert_samples (std::span<T> dst, std::span<float const> src)
  {
      auto const count {std::min (src.size (), dst.size ())};
      std::transform (src.begin (), src.begin () + count, dst.begin (), from_float_sample<T>);
  }

  // Wrapper for convert_samples() for storing all of its variants in a single table.
  template <typename D, typename S>
  void convert (void* dst, void const* src, std::size_t count)
  {
      std::span const src_span {static_cast<S const*> (src), count};
      std::span const dst_span {static_cast<D*> (dst), count};
      convert_samples (dst_span, src_span);
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
