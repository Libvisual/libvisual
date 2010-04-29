// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2006 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_math.hpp,v 1.1 2006-09-12 14:22:46 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LV_MATH_HPP
#define LV_MATH_HPP

#include <libvisual/lv_math.h>

namespace Lv
{
  namespace Math
  {
    // type conversions

    template <typename To, typename From>
    void vector_convert (To *dest, const From *src, visual_size_t n);

    template <>
    inline void vector_convert (int32_t *dest, const float *src, visual_size_t n)
    {
        visual_math_vectorized_floats_to_int32s (dest, const_cast<float *> (src), n);
    }

    template <>
    inline void vector_convert (float *dest, const int32_t *src, visual_size_t n)
    {
        visual_math_vectorized_int32s_to_floats (dest, const_cast<int32_t *> (src), n);
    }

    // vector-scalar additions

    template <typename To, typename From, typename Addend>
    void vector_add (To *dest, const From *src, visual_size_t n, Addend addend);

    template <>
    inline void vector_add (float *dest, const float *src,
        visual_size_t n, float addend)
    {
        visual_math_vectorized_add_floats_const_float (dest, const_cast<float *> (src), n, addend);
    }

    // vector-scalar subtractions

    template <typename To, typename From, typename Subtrahend>
    void vector_subtract (To *dest, const From *src, visual_size_t n, Subtrahend subtrahend);

    template <>
    inline void vector_subtract (float *dest, const float *src,
        visual_size_t n, float subtrahend)
    {
        // NOTE: 'subtract' is spelled wrongly
        visual_math_vectorized_substract_floats_const_float (dest, const_cast<float *> (src), n, subtrahend);
    }

    // vector-scalar multiplications

    template <typename To, typename From, typename Multiplier>
    void vector_multiply (To *dest, const From *src, visual_size_t n, Multiplier multiplier);

    template <>
    inline void vector_multiply (float *dest, const float *src, visual_size_t n, float multiplier)
    {
        visual_math_vectorized_multiplier_floats_const_float (dest, const_cast<float *> (src), n, multiplier);
    }

    template <>
    inline void vector_multiply (int32_t *dest, const float *src, visual_size_t n, float multiplier)
    {
        visual_math_vectorized_floats_to_int32s_multiply (dest, const_cast<float *> (src), n, multiplier);
    }

    template <>
    inline void vector_multiply (float *dest, const int32_t *src, visual_size_t n, float multiplier)
    {
        visual_math_vectorized_int32s_to_floats_multiply (dest, const_cast<int32_t *> (src), n, multiplier);
    }

    // vector-scalar multiplications with denormalisations

    template <typename To, typename From, typename Multiplier>
    inline void vector_multiply_denormalise (To *dest, From *src, visual_size_t n, Multiplier multiplier);

    template <>
    inline void vector_multiply_denormalise (int32_t *dest, float *src, visual_size_t n, float multiplier)
    {
 	visual_math_vectorized_floats_to_int32s_multiply_denormalise (dest, src, n, multiplier);
    }

    // vector-vector multiplications

    template <typename To, typename From1, typename From2>
    void vector_multiply (To *dest, const From1 *src1, const From2 *src2, visual_size_t n);

    template <>
    inline void vector_multiply (float *dest, const float *src1, const float *src2,
        visual_size_t n)
    {
        visual_math_vectorized_multiplier_floats_floats (dest,
                                                         const_cast<float *> (src1),
                                                         const_cast<float *> (src2),
                                                         n);
    }

    // complex vector operations

    inline void vector_complex_to_norm (float *dest, const float *real, const float *imag,
        visual_size_t n)
    {
        visual_math_vectorized_complex_to_norm (dest,
                                                const_cast<float *> (real),
                                                const_cast<float *> (imag),
                                                n);
    }

    inline void vector_complex_to_norm_scale (float *dest, const float *real, const float *imag,
        visual_size_t n, float factor)
    {
        visual_math_vectorized_complex_to_norm_scale (dest,
                                                      const_cast<float *> (real),
                                                      const_cast<float *> (imag),
                                                      n,
                                                      factor);
    }

    // miscellaneous

    inline void vector_sqrt (float *dest, const float *src, visual_size_t n)
    {
        visual_math_vectorized_sqrt_floats (dest, const_cast<float *> (src), n);
    }

  } // namespace Math

} // namespace Lv

#endif // #ifndef LV_MATH_HPP
