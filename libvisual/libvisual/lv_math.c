/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.c,v 1.4 2006-01-18 12:27:29 synap Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <math.h>

#include "lv_math.h"

/**
 * @defgroup VisMath VisMath
 * @{
 */

int visual_math_vectorized_multiplier_floats_const_float (float *vector, visual_size_t n, float multiplier)
{
	float *d = vector;

	visual_log_return_val_if_fail (vector != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

#ifdef VISUAL_ARCH_X86
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 :: [multiplier] "m" (*packed_multiplier));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfmul %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%0)"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%0)"
				 "\n\t pfmul %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%0)"
				 "\n\t pfmul %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%0)"
				 "\n\t pfmul %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%0)"
				 "\n\t pfmul %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%0)"
				 "\n\t pfmul %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%0)"
				 :: "r" (d) : "memory");

			d += 14;

			n -= 14;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		(*d) *= multiplier;

		d++;
	}

	return VISUAL_OK;
}

int visual_math_vectorized_add_floats_const_float (float *vector, visual_size_t n, float adder)
{
	float *d = vector;

	visual_log_return_val_if_fail (vector != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_adder[2];

		packed_adder[0] = adder;
		packed_adder[1] = adder;

#ifdef VISUAL_ARCH_X86
		__asm __volatile
			("\n\t movq %[adder], %%mm0"
			 :: [adder] "m" (*packed_adder));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfadd %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%0)"
				 "\n\t pfadd %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%0)"
				 "\n\t pfadd %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%0)"
				 "\n\t pfadd %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%0)"
				 "\n\t pfadd %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%0)"
				 "\n\t pfadd %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%0)"
				 "\n\t pfadd %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%0)"
				 :: "r" (d) : "memory");

			d += 14;

			n -= 14;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		(*d) *= adder;

		d++;
	}

	return VISUAL_OK;
}

int visual_math_vectorized_substract_floats_const_float (float *vector, visual_size_t n, float substracter)
{
	float *d = vector;

	visual_log_return_val_if_fail (vector != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_substracter[2];

		packed_substracter[0] = substracter;
		packed_substracter[1] = substracter;

#ifdef VISUAL_ARCH_X86
		__asm __volatile
			("\n\t movq %[substracter], %%mm0"
			 :: [substracter] "m" (*packed_substracter));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfsub %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%0)"
				 "\n\t pfsub %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%0)"
				 "\n\t pfsub %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%0)"
				 "\n\t pfsub %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%0)"
				 "\n\t pfsub %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%0)"
				 "\n\t pfsub %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%0)"
				 "\n\t pfsub %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%0)"
				 :: "r" (d) : "memory");

			d += 14;

			n -= 14;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		(*d) *= substracter;

		d++;
	}

	return VISUAL_OK;
}


int visual_math_vectorized_floats_to_ints (float *flts, int32_t *ints, visual_size_t n)
{
	float *s = flts;
	int32_t *d = ints;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
#ifdef VISUAL_ARCH_X86

		while (n > 16) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t pf2id (%0), %%mm0"
				 "\n\t pf2id 8(%0), %%mm1"
				 "\n\t pf2id 16(%0), %%mm2"
				 "\n\t pf2id 24(%0), %%mm3"
				 "\n\t pf2id 32(%0), %%mm4"
				 "\n\t pf2id 40(%0), %%mm5"
				 "\n\t pf2id 48(%0), %%mm6"
				 "\n\t pf2id 56(%0), %%mm7"
				 "\n\t movntq %%mm0, (%1)"
				 "\n\t movntq %%mm1, 8(%1)"
				 "\n\t movntq %%mm2, 16(%1)"
				 "\n\t movntq %%mm3, 24(%1)"
				 "\n\t movntq %%mm4, 32(%1)"
				 "\n\t movntq %%mm5, 40(%1)"
				 "\n\t movntq %%mm6, 48(%1)"
				 "\n\t movntq %%mm7, 56(%1)"
				 :: "r" (d), "r" (s) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		*d = *s;

		d++;
		s++;
	}

	return VISUAL_OK;
}

int visual_math_vectorized_ints_to_floats (int32_t *ints, float *flts, visual_size_t n)
{
	int32_t *s = ints;
	float *d = flts;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
#ifdef VISUAL_ARCH_X86

		while (n > 16) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t pi2fd (%0), %%mm0"
				 "\n\t pi2fd 8(%0), %%mm1"
				 "\n\t pi2fd 16(%0), %%mm2"
				 "\n\t pi2fd 24(%0), %%mm3"
				 "\n\t pi2fd 32(%0), %%mm4"
				 "\n\t pi2fd 40(%0), %%mm5"
				 "\n\t pi2fd 48(%0), %%mm6"
				 "\n\t pi2fd 56(%0), %%mm7"
				 "\n\t movntq %%mm0, (%1)"
				 "\n\t movntq %%mm1, 8(%1)"
				 "\n\t movntq %%mm2, 16(%1)"
				 "\n\t movntq %%mm3, 24(%1)"
				 "\n\t movntq %%mm4, 32(%1)"
				 "\n\t movntq %%mm5, 40(%1)"
				 "\n\t movntq %%mm6, 48(%1)"
				 "\n\t movntq %%mm7, 56(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		*d = *s;

		d++;
		s++;
	}

	return VISUAL_OK;
}

int visual_math_vectorized_floats_to_ints_multiply (float *flts, int32_t *ints, visual_size_t n, float multiplier)
{
	return VISUAL_OK;
}

int visual_math_vectorized_ints_to_floats_multiply (int32_t *ints, float *flts, visual_size_t n, float multiplier)
{
	int32_t *s = ints;
	float *d = flts;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

#ifdef VISUAL_ARCH_X86
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 :: [multiplier] "m" (*packed_multiplier));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t pi2fd (%0), %%mm1"
				 "\n\t pi2fd 8(%0), %%mm2"
				 "\n\t pi2fd 16(%0), %%mm3"
				 "\n\t pi2fd 24(%0), %%mm4"
				 "\n\t pi2fd 32(%0), %%mm5"
				 "\n\t pi2fd 40(%0), %%mm6"
				 "\n\t pi2fd 48(%0), %%mm7"
				 "\n\t pfmul %%mm0, %%mm1"
				 "\n\t movntq %%mm1, (%1)"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t movntq %%mm2, 8(%1)"
				 "\n\t pfmul %%mm0, %%mm3"
				 "\n\t movntq %%mm3, 16(%1)"
				 "\n\t pfmul %%mm0, %%mm4"
				 "\n\t movntq %%mm4, 24(%1)"
				 "\n\t pfmul %%mm0, %%mm5"
				 "\n\t movntq %%mm5, 32(%1)"
				 "\n\t pfmul %%mm0, %%mm6"
				 "\n\t movntq %%mm6, 40(%1)"
				 "\n\t pfmul %%mm0, %%mm7"
				 "\n\t movntq %%mm7, 48(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 14;
			s += 14;

			n -= 14;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		*d = *s;

		d++;
		s++;
	}

	return VISUAL_OK;
}

/**
 * Vectorized square root for single precision floats. This function works best with data
 * sizes larger than 16 or equal to 16.
 *
 * @param vector The vector of floats of which the squere roots will be calculated.
 * @param n The number of floats in the vector.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vectorized_sqrt_floats (float *vector, visual_size_t n)
{
	float *d = vector;

	visual_log_return_val_if_fail (vector != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
#ifdef VISUAL_ARCH_X86
		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (*d);

			d++;
			n++;
		}

		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t movaps (%0), %%xmm0"
				 "\n\t movaps 16(%0), %%xmm1"
				 "\n\t movaps 32(%0), %%xmm2"
				 "\n\t movaps 48(%0), %%xmm3"
				 "\n\t sqrtps %%xmm0, %%xmm4"
				 "\n\t sqrtps %%xmm1, %%xmm5"
				 "\n\t sqrtps %%xmm2, %%xmm6"
				 "\n\t sqrtps %%xmm3, %%xmm7"
				 "\n\t movntps %%xmm4, (%0)"
				 "\n\t movntps %%xmm5, 16(%0)"
				 "\n\t movntps %%xmm6, 32(%0)"
				 "\n\t movntps %%xmm7, 48(%0)"
				 :: "r" (d) : "memory");

			d += 16;
			n -= 16;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */
	}

	while (n--) {
		*d = sqrtf (*d);

		d++;
	}

	return VISUAL_OK;
}

/*_
 * @}
 */

