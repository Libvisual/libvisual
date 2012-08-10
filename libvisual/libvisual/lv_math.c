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
#include <math.h>

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
