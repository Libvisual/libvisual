/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.h,v 1.8 2006-09-19 18:28:51 synap Exp $
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

#include <libvisual/lv_common.h>

VISUAL_BEGIN_DECLS

/* prototypes */
int visual_math_vector_multiplier_floats_scalar_float (float *dest, const float *src, visual_size_t n, float multiplier);
int visual_math_vector_add_floats_scalar_float (float *dest, const float *src, visual_size_t n, float addend);
int visual_math_vector_substract_floats_scalar_float (float *dest, const float *src, visual_size_t n, float subtrahend);

int visual_math_vector_multiplier_floats_floats (float *dest, const float *src1, const float *src2, visual_size_t n);

int visual_math_vector_floats_to_int32s (int32_t *ints, const float *flts, visual_size_t n);
int visual_math_vector_int32s_to_floats (float *flts, const int32_t *ints, visual_size_t n);

int visual_math_vector_floats_to_int32s_multiply (int32_t *ints, const float *flts, visual_size_t n, float multiplier);
int visual_math_vector_int32s_to_floats_multiply (float *flts, const int32_t *ints, visual_size_t n, float multiplier);

int visual_math_vector_floats_to_int32s_multiply_denormalise (int32_t *ints, const float *flts, visual_size_t n,
		float multiplier);

int visual_math_vector_sqrt_floats (float *dest, const float *src, visual_size_t n);

int visual_math_vector_complex_to_norm (float *dest, const float *real, const float *imag, visual_size_t n);
int visual_math_vector_complex_to_norm_scale (float *dest, const float *real, const float *imag, visual_size_t n,
		float factor);

/* FIXME add many more to suite both rectangle and audio systems 100% */
/* FIXME also look into things we might be able to generalize from VisVideo. */

VISUAL_END_DECLS

#endif /* _LV_MATH_H */
