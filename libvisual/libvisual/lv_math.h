/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.h,v 1.7 2006/02/13 20:54:08 synap Exp $
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

  template <typename T>
  T clamp (T value, T lower, T upper)
  {
      return std::max (lower, std::min (upper, value));
  }

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
 * Multiplies an array of floats with one constant multiplier. The same destination and source arrays
 * are allowed. With the right cpu features in place this function is very optimized.
 *
 * @param dest Pointer to the destination float array.
 * @param src Pointer to the source float array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every element in the source array is multiplied.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_multiplier_floats_const_float (float *dest, const float *src, visual_size_t n, float multiplier);

/**
 * Adds an array of floats with one constant adder. The same destination and source arrays
 * are allowed. With the right cpu features in place this function is very optimized.
 *
 * @param dest Pointer to the destination float array.
 * @param src Pointer to the source float array.
 * @param n The number of items in the array.
 * @param adder The constant adder that is added to every entry in the source array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_add_floats_const_float (float *dest, const float *src, visual_size_t n, float adder);

/**
 * Substracts an array of floats with one constant substracter. The same destination and source arrays
 * are allowed. With the right cpu features in place this function is very optimized.
 *
 * @param dest Pointer to the destination float array.
 * @param src Pointer to the source float array.
 * @param n The number of items in the array.
 * @param substracter The constant substracter that is substracter from every entry in the source array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_substract_floats_const_float (float *dest, const float *src, visual_size_t n, float substracter);

LV_API int visual_math_vectorized_multiplier_floats_floats (float *dest, const float *src1, const float *src2, visual_size_t n);

/**
 * Converts an array of floats to integers. With the right cpu features in place this function
 * is very optimized.
 *
 * @param ints Pointer to the destination int32_t array.
 * @param flts Pointer to the source float array.
 * @param n The number of items in the array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_floats_to_int32s (int32_t *ints, const float *flts, visual_size_t n);

/**
 * Converts an array of integers to floats. With the right cpu features in place this function
 * is very optimized.
 *
 * @param flts Pointer to the destination float array.
 * @param ints Pointer to the source int32_t array.
 * @param n The number of items in the array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_int32s_to_floats (float *flts, const int32_t *ints, visual_size_t n);

/**
 * Converts an array of floats to integers and multiplies it with a const multiplier.
 * With the right cpu features in place this function is very optimized.
 *
 * @param ints Pointer to the destination int32_t array.
 * @param flts Pointer to the source float array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every entry is multiplied.
 *
 * @return VISUAL_OK on success or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_floats_to_int32s_multiply (int32_t *ints, const float *flts, visual_size_t n, float multiplier);

/**
 * Converts an array of integers to floats and multiplies it with a const multiplier.
 * With the right cpu features in place this function is very optimized.
 *
 * @param flts Pointer to the destination float array.
 * @param ints Pointer to the source int32_t array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every entry is multiplied.
 *
 * @return VISUAL_OK on success or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_int32s_to_floats_multiply (float *flts, const int32_t *ints, visual_size_t n, float multiplier);

/**
 * Converts an array of floats to integers and multiplies it with a const multiplier. Furthermore
 * the float values are denormalized in the following way: -1.0 to 1.0 will be converted to:
 * 0.0 to 1.0. With the right cpu features in place this function is very optimized.
 *
 * @param ints Pointer to the destination int32_t array.
 * @param flts Pointer to the source float array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every entry is multiplied.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_floats_to_int32s_multiply_denormalise (int32_t *ints, const float *flts, visual_size_t n, float multiplier);

/**
 * Vectorized square root for single precision floats. This function works best with data
 * sizes larger than 16 or equal to 16.
 *
 * @param dest The destination vector of floats in which the results are placed.
 * @param src The source vector of floats of which the square roots will be calculated.
 * @param n The number of floats in the vector.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_sqrt_floats (float *dest, const float *src, visual_size_t n);

/**
 * Vectorized complex to norm conversion. Will make norm values from a real and imaginary
 * array.
 *
 * @param dest Pointer to the destination float array.
 * @param real Pointer to the real part float array.
 * @param imag pointer to the imaginary part float array.
 * @param n The number of elements to be converted.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_complex_to_norm (float *dest, const float *real, const float *imag, visual_size_t n);

/**
 * Vectorized complex to norm conversion and result value scaler. Will make norm values from a real and imaginary
 * array, after the conversion has been made it will be multiplied by the scaler.
 *
 * @param dest Pointer to the destination float array.
 * @param real Pointer to the real part float array.
 * @param imag pointer to the imaginary part float array.
 * @param n The number of elements to be converted.
 * @param scaler The scaler that is used to scale the result value.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_math_vectorized_complex_to_norm_scale (float *dest, const float *real, const float *imag, visual_size_t n, float scaler);

/* FIXME add many more to suite both rectangle and audio systems 100% */
/* FIXME also look into things we might be able to generalize from VisVideo. */
/* FIXME provide with source and dest when possible, source and dest can always be the same. */

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_MATH_H */
