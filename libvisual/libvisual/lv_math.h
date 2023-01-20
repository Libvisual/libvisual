/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

#ifndef _LV_MATH_H
#define _LV_MATH_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

#ifdef __cplusplus

#include <algorithm>

namespace LV {

  // empty at the moment

} // LV namespce

#endif /* __cplusplus */

/**
 * @defgroup VisMath VisMath
 * @{
 */

#define VISUAL_MATH_PI 3.141592653589793238462643383279502884197169399f

LV_BEGIN_DECLS

/**
 * Checks if the given value is a power of 2.
 *
 * @param n Value to be checked if it's being a power of 2.
 *
 * @return TRUE if power of 2, FALSE if not.
 */
LV_API int visual_math_is_power_of_2 (int n);

/**
 * Rounds up to the next highest power of 2.
 *
 * @param n Value to round up
 *
 * @return Next highest power of 2
 */
LV_API unsigned int visual_math_round_power_of_2 (unsigned int n);

/**
 * Multiplies an array of floats with a constant multiplicand, using SIMD instructions on supported CPUs
 *
 * @note Destination and source maty be the same.
 *
 * @param dest  pointer to the destination float array
 * @param src   pointer to the source float array
 * @param k     constant multiplicand
 * @param count number of elements
 */
LV_API void visual_math_simd_mul_floats_float (float *LV_RESTRICT dest, const float *LV_RESTRICT src, float k, visual_size_t count);

/**
 * Adds an array of floats with a constant addend, using SIMD instructions on supported CPUs.
 *
 * @note Destination and source may be the same.
 *
 * @param dest  pointer to the destination float array
 * @param src   pointer to the source float array
 * @param k     constant addend
 * @param count number of elements
 */
LV_API void visual_math_simd_add_floats_float (float *LV_RESTRICT dest, const float *LV_RESTRICT src, float k, visual_size_t count);

LV_API void visual_math_simd_mul_floats_floats (float *LV_RESTRICT dest, const float *LV_RESTRICT src1, const float *LV_RESTRICT src2, visual_size_t count);

/**
 * Converts an array of floats to 32-bit integers, using SIMD instructions on supported CPUs.
 *
 * @param ints  pointer to the destination int32_t array
 * @param flts  pointer to the source float array
 * @param count number of elements
 */
LV_API void visual_math_simd_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, visual_size_t count);

/**
 * Converts an array of integers to floats, using SIMD instructions on supported CPUs.
 *
 * @param flts  pointer to the destination float array
 * @param ints  pointer to the source int32_t array
 * @param count number of elements
 */
LV_API void visual_math_simd_int32s_to_floats (float *LV_RESTRICT flts, const int32_t *LV_RESTRICT ints, visual_size_t count);

/**
 * Converts an array of floats to integers and multiplies it with a const multiplicand, using SIMD instructions on
 * supported CPUs.
 *
 * @param ints  pointer to the destination int32_t array
 * @param flts  pointer to the source float array
 * @param k     constant multiplicand
 * @param count number of elements
 */
LV_API void visual_math_simd_floats_to_int32s_mul_float (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count);

/**
 * Converts an array of integers to floats and multiplies it with a const multiplier.
 *
 * @param flts  pointer to the destination float array.
 * @param ints  pointer to the source int32_t array.
 * @param k     constant multiplicand
 * @param count number of elements
 */
LV_API void visual_math_simd_int32s_to_floats_mul_float (float *LV_RESTRICT flts, const int32_t *LV_RESTRICT ints, float k, visual_size_t count);

/**
 * Denormalizes an array of floats, each in [0.0, 1.0], to integers, using SIMD instructions on supported CPUs.
 *
 * Each float value is first clamped to [0.0, 1.0], and then scaled to [0.0, k]
 *
 * @param ints  pointer to the destination int32_t array.
 * @param flts  pointer to the source float array.
 * @param k     constant multiplicand
 * @param count number of elements
 */
LV_API void visual_math_simd_denorm_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count);

/**
 * Denormalizes an array of floats, each in [-1.0, 1.0], to integers, using SIMD instructions on supported CPUs.
 *
 * Each float value is first clamped to [-1.0, 1.0], and then scaled to [0.0, k].
 *
 * @see visual_math_simd_denorm_floats_to_int32()
 *
 * @param ints  pointer to the destination int32_t array.
 * @param flts  pointer to the source float array.
 * @param k     constant multiplicand
 * @param count number of elements
 */
LV_API void visual_math_simd_denorm_neg_floats_to_int32s (int32_t *LV_RESTRICT ints, const float *LV_RESTRICT flts, float k, visual_size_t count);

/**
 * Calculates the square root of each float element in the input array, using SIMD instructions on supported CPUs.
 *
 * @note This function works best with data sizes larger than 16 or equal to 16.
 *
 * @param dest  array to hold the results in
 * @param src   array of floats
 * @param count number of elements
 */
LV_API void visual_math_simd_sqrt_floats (float *LV_RESTRICT dest, const float *LV_RESTRICT src, visual_size_t count);

/**
 * Calculates the norm of a list of complex numbers, using SIMD instructions on supported CPUs. The real and imaginary
 * parts are given in two separate arrays.
 *
 * @param dest  array to hold the results in
 * @param real  array of real parts
 * @param imag  array of imaginary parts
 * @param count number of elements
 */
LV_API void visual_math_simd_complex_norm (float *LV_RESTRICT dest, const float *LV_RESTRICT real, const float *LV_RESTRICT imag, visual_size_t count);

/**
 * Calculates the scaled norm of a list of complex numbers, using SIMD instructions on supported CPUs. The real and
 * imaginary parts are given in two separate arrays.
 *
 * @param dest  array to hold the results in
 * @param real  array of real parts
 * @param imag  array of imaginary parts
 * @param k     const multiplcand
 * @param count number of elements
 */
LV_API void visual_math_simd_complex_scaled_norm (float *LV_RESTRICT dest, const float *LV_RESTRICT real, const float *LV_RESTRICT imag, float k, visual_size_t count);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_MATH_H */
