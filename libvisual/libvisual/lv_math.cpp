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
#include "lv_math.h"
#include "lv_common.h"
#include "lv_math_orc.h"
#include <algorithm>
#include <cassert>
#include <cmath>

// NOTE: The ORC SIMD routines use uint32_t instead of int32_t for some reason. We are forced
// to perform signed to unsigned casts for routines accepting 32-bit integer vectors.

namespace LV::Math {

  void simd_add (std::span<float> dst, std::span<float const> src, float adder)
  {
      assert (dst.data () != src.data ());

      auto count {static_cast<int> (std::min (dst.size (), src.size ()))};
      ::simd_add_floats_float (dst.data (), src.data (), adder, count);
  }

  void simd_mul (std::span<float> dst, std::span<float const> src, float k)
  {
      assert (dst.data () != src.data ());

      auto count {static_cast<int> (std::min (dst.size (), src.size ()))};
      ::simd_mul_floats_float (dst.data (), src.data (), k, count);
  }

  void simd_mul (std::span<float> dst, std::span<float const> src1, std::span<float const> src2)
  {
      assert (dst.data () != src1.data ());
      assert (src1.data () != src2.data ());

      auto count {static_cast<int> (std::min (dst.size (), std::min (src1.size (), src2.size ())))};
      ::simd_mul_floats_floats (dst.data (), src1.data (), src2.data (), count);
  }

  void simd_convert (std::span<int32_t> ints, std::span<float const> flts)
  {
      auto count {static_cast<int> (std::min (ints.size (), flts.size ()))};
      ::simd_floats_to_int32s (reinterpret_cast<std::uint32_t*> (ints.data ()), flts.data (), count);
  }

  void simd_convert (std::span<float> flts, std::span<std::int32_t const> ints)
  {
      auto count {static_cast<int> (std::min (flts.size (), ints.size ()))};
      ::simd_int32s_to_floats (flts.data (), reinterpret_cast<std::uint32_t const*> (ints.data ()), count);
  }

  void simd_mul (std::span<std::int32_t> ints, std::span<float const> flts, float k)
  {
      auto count {static_cast<int> (std::min (ints.size (), flts.size ()))};
      ::simd_floats_to_int32s_mul_float (reinterpret_cast<std::uint32_t*> (ints.data ()), flts.data (), k, count);
  }

  void simd_mul (std::span<float> flts, std::span<std::int32_t const> ints, float k)
  {
      auto count {static_cast<int> (std::min (flts.size (), ints.size ()))};
      ::simd_int32s_to_floats_mul_float (flts.data (), reinterpret_cast<std::uint32_t const*> (ints.data ()), k, count);
  }

  void simd_convert_denorm (std::span<std::int32_t> ints, std::span<float const> flts, float k)
  {
      auto count {static_cast<int> (std::min (ints.size (), flts.size ()))};
      ::simd_denorm_floats_to_int32s (reinterpret_cast<std::uint32_t*> (ints.data ()), flts.data (), k, count);
  }

  void simd_convert_denorm_neg (std::span<std::int32_t> ints, std::span<float const> flts, float k)
  {
      auto count {static_cast<int> (std::min (ints.size (), flts.size ()))};
      ::simd_denorm_neg_floats_to_int32s (reinterpret_cast<std::uint32_t*> (ints.data ()), flts.data (), k, count);
  }

  void simd_sqrt (std::span<float> dst, std::span<float const> src)
  {
      assert (dst.data () != src.data ());

      auto count {static_cast<int> (std::min (dst.size (), src.size ()))};
      ::simd_sqrt_floats (dst.data (), src.data (), count);
  }

  void simd_complex_norm (std::span<float> dst, std::span<float const> real, std::span<float const> imag)
  {
      assert (real.data () != dst.data ());
      assert (real.size () == imag.size ());

      auto count {static_cast<int> (std::min (dst.size (), real.size()))};
      ::simd_complex_norm (dst.data (), real.data (), imag.data (), count);
  }

  void simd_complex_scaled_norm (std::span<float> dst, std::span<float const> real, std::span<float const> imag, float k)
  {
      assert (real.data () != dst.data ());
      assert (real.data () != imag.data ());
      assert (real.size () == imag.size ());

      auto size {static_cast<int> (std::min (dst.size (), real.size ()))};
      ::simd_complex_scaled_norm (dst.data (), real.data (), imag.data (), k, size);
  }

} // LV::Math namespace

int visual_math_is_power_of_2 (int n)
{
	return (n > 0) && !(n & (n - 1));
}

unsigned int visual_math_round_power_of_2 (unsigned int n)
{
    n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
#if SIZEOF_INT > 4
	n |= n >> 32;
#endif
	n++;

	return n;
}

void visual_math_simd_mul_floats_float (float *LV_RESTRICT dest, const float *LV_RESTRICT src, float k, visual_size_t count)
{
    simd_mul_floats_float (dest, src, k, (int) count);
}

void visual_math_simd_add_floats_float (float *LV_RESTRICT dest, const float *LV_RESTRICT src, float adder, visual_size_t count)
{
    simd_add_floats_float (dest, src, adder, (int) count);
}

void visual_math_simd_mul_floats_floats (float *LV_RESTRICT dest, const float *LV_RESTRICT src1, const float *LV_RESTRICT src2, visual_size_t count)
{
    simd_mul_floats_floats (dest, src1, src2, (int) count);
}

void visual_math_simd_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, visual_size_t count)
{
    simd_floats_to_int32s ((uint32_t *LV_RESTRICT) ints, flts, (int) count);
}

void visual_math_simd_int32s_to_floats (float *LV_RESTRICT flts, const int32_t *LV_RESTRICT ints, visual_size_t count)
{
    simd_int32s_to_floats (flts, (const uint32_t *LV_RESTRICT) ints, (int) count);
}

void visual_math_simd_floats_to_int32s_mul_float (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count)
{
    simd_floats_to_int32s_mul_float ((uint32_t *LV_RESTRICT) ints, flts, k, (int) count);
}

void visual_math_simd_int32s_to_floats_mul_float (float *LV_RESTRICT flts, const int32_t *LV_RESTRICT ints, float k, visual_size_t count)
{
    simd_int32s_to_floats_mul_float (flts, (const uint32_t *LV_RESTRICT) ints, k, (int) count);
}

void visual_math_simd_denorm_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count)
{
    simd_denorm_floats_to_int32s ((uint32_t *LV_RESTRICT) ints, flts, k, (int) count);
}

void visual_math_simd_denorm_neg_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count)
{
    simd_denorm_neg_floats_to_int32s ((uint32_t *LV_RESTRICT) ints, flts, k, (int) count);
}

void visual_math_simd_sqrt_floats (float *LV_RESTRICT dest, const float *LV_RESTRICT src, visual_size_t count)
{
    simd_sqrt_floats (dest, src, (int) count);
}

void visual_math_simd_complex_norm (float *LV_RESTRICT dest, const float *LV_RESTRICT real, const float *LV_RESTRICT imag, visual_size_t count)
{
    simd_complex_norm (dest, real, imag, (int) count);
}

void visual_math_simd_complex_scaled_norm (float *LV_RESTRICT dest, const float *LV_RESTRICT real, const float *LV_RESTRICT imag, float k, visual_size_t count)
{
    simd_complex_scaled_norm (dest, real, imag, k, (int) count);
}
