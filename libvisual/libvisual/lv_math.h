/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.h,v 1.4 2006-01-18 12:27:29 synap Exp $
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
int visual_math_vectorized_multiplier_floats_const_float (float *vector, visual_size_t n, float multiplier);
int visual_math_vectorized_add_floats_const_float (float *vector, visual_size_t n, float adder);
int visual_math_vectorized_substract_floats_const_float (float *vector, visual_size_t n, float substracter);

int visual_math_vectorized_floats_to_ints (float *flts, int32_t *ints, visual_size_t n);
int visual_math_vectorized_ints_to_floats (int32_t *ints, float *flts, visual_size_t n);

int visual_math_vectorized_floats_to_ints_multiply (float *flts, int32_t *ints, visual_size_t n, float multiplier);
int visual_math_vectorized_ints_to_floats_multiply (int32_t *ints, float *flts, visual_size_t n, float multiplier);

int visual_math_vectorized_sqrt_floats (float *vector, visual_size_t n);


/* FIXME add many more to suite both rectangle and audio systems 100% */
/* FIXME also look into things we might be able to generalize from VisVideo. */
/* FIXME add complex number handling and vectorized complex number to real conversion. */
/* FIXME provide with source and dest when possible, source and dest can always be the same. */

VISUAL_END_DECLS

#endif /* _LV_MATH_H */
