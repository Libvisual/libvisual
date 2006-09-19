/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.c,v 1.14 2006-09-19 18:28:51 synap Exp $
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

#include "lv_bits.h"
#include "lv_math.h"

/* This file is getting big and bloated because of the large chunks of simd code. When all is in place we'll take a serious
 * look how we can reduce this. For example by using macros for common blocks. */

/* SMALL TODO LIST:
 * Benchmark all the code very well.
 */

/**
 * @defgroup VisMath VisMath
 * @{
 */

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
int visual_math_vector_multiplier_floats_scalar_float (float *dest, const float *src, visual_size_t n, float multiplier)
{
	float *d = dest;
	const float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	/* FIXME check what is faster on AMD (sse or 3dnow) */
	if (visual_cpu_get_sse () && n >= 16) {
		float packed_multiplier[4];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;
		packed_multiplier[2] = multiplier;
		packed_multiplier[3] = multiplier;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) * multiplier;

			d++;
			s++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_multiplier) : "memory");


		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm1"
				 "\n\t movups 32(%0), %%xmm2"
				 "\n\t movups 48(%0), %%xmm3"
				 "\n\t mulps %%xmm7, %%xmm0"
				 "\n\t mulps %%xmm7, %%xmm1"
				 "\n\t mulps %%xmm7, %%xmm2"
				 "\n\t mulps %%xmm7, %%xmm3"
				 "\n\t movntps %%xmm0, (%1)"
				 "\n\t movntps %%xmm1, 16(%1)"
				 "\n\t movntps %%xmm2, 32(%1)"
				 "\n\t movntps %%xmm3, 48(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}
#endif /* VISUAL_ARCH_X86 */
	} else if (visual_cpu_get_3dnow () && n >= 14) {
		float packed_multiplier[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 :: [multiplier] "m" (*packed_multiplier));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfmul %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%1)"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%1)"
				 "\n\t pfmul %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%1)"
				 "\n\t pfmul %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%1)"
				 "\n\t pfmul %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%1)"
				 "\n\t pfmul %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%1)"
				 "\n\t pfmul %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%1)"
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
		(*d) = (*s) * multiplier;

		d++;
		s++;
	}

	return VISUAL_OK;
}

/**
 * Adds an array of floats with one constant addend. The same destination and source arrays
 * are allowed. With the right cpu features in place this function is very optimized.
 *
 * @param dest Pointer to the destination float array.
 * @param src Pointer to the source float array.
 * @param n The number of items in the array.
 * @param addend The constant addend that is added to every entry in the source array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vector_add_floats_scalar_float (float *dest, const float *src, visual_size_t n, float addend)
{
	float *d = dest;
	const float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_addend[4];

		packed_addend[0] = addend;
		packed_addend[1] = addend;
		packed_addend[2] = addend;
		packed_addend[3] = addend;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) + addend;

			d++;
			s++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_addend) : "memory");


		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm1"
				 "\n\t movups 32(%0), %%xmm2"
				 "\n\t movups 48(%0), %%xmm3"
				 "\n\t addps %%xmm7, %%xmm0"
				 "\n\t addps %%xmm7, %%xmm1"
				 "\n\t addps %%xmm7, %%xmm2"
				 "\n\t addps %%xmm7, %%xmm3"
				 "\n\t movntps %%xmm0, (%1)"
				 "\n\t movntps %%xmm1, 16(%1)"
				 "\n\t movntps %%xmm2, 32(%1)"
				 "\n\t movntps %%xmm3, 48(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}
#endif /* VISUAL_ARCH_X86 */
	} else if (visual_cpu_get_3dnow () && n >= 14) {
		float packed_addend[2];

		packed_addend[0] = addend;
		packed_addend[1] = addend;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[addend], %%mm0"
			 :: [addend] "m" (*packed_addend));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfadd %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%1)"
				 "\n\t pfadd %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%1)"
				 "\n\t pfadd %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%1)"
				 "\n\t pfadd %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%1)"
				 "\n\t pfadd %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%1)"
				 "\n\t pfadd %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%1)"
				 "\n\t pfadd %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%1)"
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
		(*d) = (*s) + addend;

		d++;
		s++;
	}

	return VISUAL_OK;
}

/**
 * Substracts an array of floats with one constant subtrahend. The same destination and source arrays
 * are allowed. With the right cpu features in place this function is very optimized.
 *
 * @param dest Pointer to the destination float array.
 * @param src Pointer to the source float array.
 * @param n The number of items in the array.
 * @param subtrahend The constant subtrahend that is subtrahend from every entry in the source array.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vector_substract_floats_scalar_float (float *dest, const float *src, visual_size_t n, float subtrahend)
{
	float *d = dest;
	const float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_subtrahend[4];

		packed_subtrahend[0] = subtrahend;
		packed_subtrahend[1] = subtrahend;
		packed_subtrahend[2] = subtrahend;
		packed_subtrahend[3] = subtrahend;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) - subtrahend;

			d++;
			s++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_subtrahend) : "memory");


		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm1"
				 "\n\t movups 32(%0), %%xmm2"
				 "\n\t movups 48(%0), %%xmm3"
				 "\n\t subps %%xmm7, %%xmm0"
				 "\n\t subps %%xmm7, %%xmm1"
				 "\n\t subps %%xmm7, %%xmm2"
				 "\n\t subps %%xmm7, %%xmm3"
				 "\n\t movntps %%xmm0, (%1)"
				 "\n\t movntps %%xmm1, 16(%1)"
				 "\n\t movntps %%xmm2, 32(%1)"
				 "\n\t movntps %%xmm3, 48(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}
#endif /* VISUAL_ARCH_X86 */
	} else if (visual_cpu_get_3dnow () && n >= 14) {
		float packed_subtrahend[2];

		packed_subtrahend[0] = subtrahend;
		packed_subtrahend[1] = subtrahend;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[subtrahend], %%mm0"
			 :: [subtrahend] "m" (*packed_subtrahend));

		while (n > 14) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t pfsub %%mm0, %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq %%mm1, (%1)"
				 "\n\t pfsub %%mm0, %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t movq %%mm2, 8(%1)"
				 "\n\t pfsub %%mm0, %%mm3"
				 "\n\t movq 24(%0), %%mm4"
				 "\n\t movq %%mm3, 16(%1)"
				 "\n\t pfsub %%mm0, %%mm4"
				 "\n\t movq 32(%0), %%mm5"
				 "\n\t movq %%mm4, 24(%1)"
				 "\n\t pfsub %%mm0, %%mm5"
				 "\n\t movq 40(%0), %%mm6"
				 "\n\t movq %%mm5, 32(%1)"
				 "\n\t pfsub %%mm0, %%mm6"
				 "\n\t movq 48(%0), %%mm7"
				 "\n\t movq %%mm6, 40(%1)"
				 "\n\t pfsub %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%1)"
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
		(*d) = (*s) - subtrahend;

		d++;
		s++;
	}

	return VISUAL_OK;
}

int visual_math_vector_multiplier_floats_floats (float *dest, const float *src1, const float *src2, visual_size_t n)
{
	float *d = dest;
	const float *s1 = src1;
	const float *s2 = src2;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s1) * (*s2);

			d++;
			s1++;
			s2++;

			n--;
		}

		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t prefetchnta 256(%1)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm1"
				 "\n\t movups 32(%0), %%xmm2"
				 "\n\t movups 48(%0), %%xmm3"
				 "\n\t movups (%1), %%xmm4"
				 "\n\t movups 16(%1), %%xmm5"
				 "\n\t movups 32(%1), %%xmm6"
				 "\n\t movups 48(%1), %%xmm7"
				 "\n\t mulps %%xmm4, %%xmm0"
				 "\n\t mulps %%xmm5, %%xmm1"
				 "\n\t mulps %%xmm6, %%xmm2"
				 "\n\t mulps %%xmm7, %%xmm3"
				 "\n\t movntps %%xmm0, (%2)"
				 "\n\t movntps %%xmm1, 16(%2)"
				 "\n\t movntps %%xmm2, 32(%2)"
				 "\n\t movntps %%xmm3, 48(%2)"
				 :: "r" (s1), "r" (s2), "r" (d) : "memory");

			d += 16;
			s1 += 16;
			s2 += 16;

			n -= 16;
		}
#endif /* VISUAL_ARCH_X86 */
	} else if (visual_cpu_get_3dnow () && n >= 8) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (n > 8) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm0"
				 "\n\t movq 8(%0), %%mm1"
				 "\n\t movq 16(%0), %%mm2"
				 "\n\t movq 24(%0), %%mm3"
				 "\n\t movq (%1), %%mm4"
				 "\n\t movq 8(%1), %%mm5"
				 "\n\t movq 16(%1), %%mm6"
				 "\n\t movq 24(%1), %%mm7"
				 "\n\t pfmul %%mm4, %%mm0"
				 "\n\t pfmul %%mm5, %%mm1"
				 "\n\t pfmul %%mm6, %%mm2"
				 "\n\t pfmul %%mm7, %%mm3"
				 "\n\t movq %%mm0, (%2)"
				 "\n\t movq %%mm1, 8(%2)"
				 "\n\t movq %%mm2, 16(%2)"
				 "\n\t movq %%mm3, 24(%2)"
				 :: "r" (s1), "r" (s2), "r" (d) : "memory");

			d += 8;
			s1 += 8;
			s2 += 8;

			n -= 8;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		(*d) = (*s1) * (*s2);

		d++;
		s1++;
		s2++;
	}

	return VISUAL_OK;
}

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
int visual_math_vector_floats_to_int32s (int32_t *ints, const float *flts, visual_size_t n)
{
	int32_t *d = ints;
	const float *s = flts;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

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
				 "\n\t movq %%mm0, (%1)"
				 "\n\t movq %%mm1, 8(%1)"
				 "\n\t movq %%mm2, 16(%1)"
				 "\n\t movq %%mm3, 24(%1)"
				 "\n\t movq %%mm4, 32(%1)"
				 "\n\t movq %%mm5, 40(%1)"
				 "\n\t movq %%mm6, 48(%1)"
				 "\n\t movq %%mm7, 56(%1)"
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
int visual_math_vector_int32s_to_floats (float *flts, const int32_t *ints, visual_size_t n)
{
	float *d = flts;
	const int32_t *s = ints;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

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
				 "\n\t movq %%mm0, (%1)"
				 "\n\t movq %%mm1, 8(%1)"
				 "\n\t movq %%mm2, 16(%1)"
				 "\n\t movq %%mm3, 24(%1)"
				 "\n\t movq %%mm4, 32(%1)"
				 "\n\t movq %%mm5, 40(%1)"
				 "\n\t movq %%mm6, 48(%1)"
				 "\n\t movq %%mm7, 56(%1)"
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

/**
 * Converts an array of floats to integers and multiplies it with a const multiplier.
 * With the right cpu features in place this function is very optimized.
 * 
 * @param ints Pointer to the destination int32_t array.
 * @param flts Pointer to the source float array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every entry is multiplied.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vector_floats_to_int32s_multiply (int32_t *ints, const float *flts, visual_size_t n, float multiplier)
{
	int32_t *d = ints;
	const float *s = flts;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 :: [multiplier] "m" (*packed_multiplier));

		while (n > 6) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t movq 16(%0), %%mm3"
				 "\n\t pfmul %%mm0, %%mm1"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t pfmul %%mm0, %%mm3"
				 "\n\t pf2id %%mm1, %%mm4"
				 "\n\t pf2id %%mm2, %%mm5"
				 "\n\t pf2id %%mm3, %%mm6"
				 "\n\t movq %%mm4, (%1)"
				 "\n\t movq %%mm5, 8(%1)"
				 "\n\t movq %%mm6, 16(%1)"
			 :: "r" (s), "r" (d) : "memory");

			d += 6;
			s += 6;

			n -= 6;
		}


		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		*d = (float) *s * multiplier;

		d++;
		s++;
	}

	return VISUAL_OK;
}

/**
 * Converts an array of integers to floats and multiplies it with a const multiplier.
 * With the right cpu features in place this function is very optimized.
 * 
 * @param flts Pointer to the destination float array.
 * @param ints Pointer to the source int32_t array.
 * @param n The number of items in the array.
 * @param multiplier The constant multiplier with which every entry is multiplied.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vector_int32s_to_floats_multiply (float *flts, const int32_t *ints, visual_size_t n, float multiplier)
{
	float *d = flts;
	const int32_t *s = ints;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
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
				 "\n\t movq %%mm1, (%1)"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t movq %%mm2, 8(%1)"
				 "\n\t pfmul %%mm0, %%mm3"
				 "\n\t movq %%mm3, 16(%1)"
				 "\n\t pfmul %%mm0, %%mm4"
				 "\n\t movq %%mm4, 24(%1)"
				 "\n\t pfmul %%mm0, %%mm5"
				 "\n\t movq %%mm5, 32(%1)"
				 "\n\t pfmul %%mm0, %%mm6"
				 "\n\t movq %%mm6, 40(%1)"
				 "\n\t pfmul %%mm0, %%mm7"
				 "\n\t movq %%mm7, 48(%1)"
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
		*d = (float) *s * multiplier;

		d++;
		s++;
	}

	return VISUAL_OK;
}

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
int visual_math_vector_floats_to_int32s_multiply_denormalise (int32_t *ints, const float *flts, visual_size_t n,
		float multiplier)
{
	int32_t *d = ints;
	const float *s = flts;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];
		float packed_normalise_mul[2];
		float packed_addend[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

		packed_normalise_mul[0] = 0.5;
		packed_normalise_mul[1] = 0.5;

		packed_addend[0] = 1;
		packed_addend[1] = 1;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 "\n\t movq %[normalise_mul], %%mm6"
			 "\n\t movq %[addend], %%mm7"

			 :: [multiplier] "m" (*packed_multiplier),
			 [normalise_mul] "m" (*packed_normalise_mul),
			 [addend] "m" (*packed_addend));

		while (n > 4) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t movq (%0), %%mm1"
				 "\n\t movq 8(%0), %%mm2"
				 "\n\t pfadd %%mm7, %%mm1"
				 "\n\t pfadd %%mm7, %%mm2"
				 "\n\t pfmul %%mm6, %%mm1"
				 "\n\t pfmul %%mm6, %%mm2"
				 "\n\t pfmul %%mm0, %%mm1"
				 "\n\t pfmul %%mm0, %%mm2"
				 "\n\t pf2id %%mm1, %%mm4"
				 "\n\t pf2id %%mm2, %%mm5"
				 "\n\t movq %%mm4, (%1)"
				 "\n\t movq %%mm5, 8(%1)"
			 :: "r" (s), "r" (d) : "memory");

			d += 4;
			s += 4;

			n -= 4;
		}


		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	}

	while (n--) {
		*d = (float) (((*s) + 1) * 0.5) * multiplier;

		d++;
		s++;
	}

	return VISUAL_OK;
}

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
int visual_math_vector_sqrt_floats (float *dest, const float *src, visual_size_t n)
{
	float *d = dest;
	const float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (*s);

			d++;
			n--;
		}

		while (n > 16) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm1"
				 "\n\t movups 32(%0), %%xmm2"
				 "\n\t movups 48(%0), %%xmm3"
				 "\n\t sqrtps %%xmm0, %%xmm4"
				 "\n\t sqrtps %%xmm1, %%xmm5"
				 "\n\t sqrtps %%xmm2, %%xmm6"
				 "\n\t sqrtps %%xmm3, %%xmm7"
				 "\n\t movntps %%xmm4, (%1)"
				 "\n\t movntps %%xmm5, 16(%1)"
				 "\n\t movntps %%xmm6, 32(%1)"
				 "\n\t movntps %%xmm7, 48(%1)"
				 :: "r" (s), "r" (d) : "memory");

			d += 16;
			s += 16;

			n -= 16;
		}
#endif /* VISUAL_ARCH_X86 */
	}

	while (n--) {
		*d = sqrtf (*s);

		d++;
		s++;
	}

	return VISUAL_OK;
}

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
int visual_math_vector_complex_to_norm (float *dest, const float *real, const float *imag, visual_size_t n)
{
	float *d = dest;
	const float *r = real;
	const float *i = imag;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (real != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (imag != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (((*r) * (*r)) + ((*i) * (*i)));

			d++;
			r++;
			i++;

			n--;
		}

		while (n > 8) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t prefetchnta 256(%1)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm2"
				 "\n\t movups (%1), %%xmm1"
				 "\n\t movups 16(%1), %%xmm3"
				 "\n\t mulps %%xmm0, %%xmm0"
				 "\n\t mulps %%xmm1, %%xmm1"
				 "\n\t mulps %%xmm2, %%xmm2"
				 "\n\t mulps %%xmm3, %%xmm3"
				 "\n\t addps %%xmm0, %%xmm1"
				 "\n\t addps %%xmm2, %%xmm3"
				 "\n\t sqrtps %%xmm1, %%xmm0"
				 "\n\t sqrtps %%xmm3, %%xmm2"
				 "\n\t movntps %%xmm0, (%2)"
				 "\n\t movntps %%xmm2, 16(%2)"
				 :: "r" (r), "r" (i), "r" (d) : "memory");

			d += 8;
			i += 8;
			r += 8;

			n -= 8;
		}
#endif /* VISUAL_ARCH_X86 */
	}

	while (n--) {
		*d = sqrtf (((*r) * (*r)) + ((*i) * (*i)));

		d++;
		r++;
		i++;
	}

	return VISUAL_OK;
}

/**
 * Vectorized complex to norm conversion and result value factor. Will make norm values from a real and imaginary
 * array, after the conversion has been made it will be multiplied by the factor.
 *
 * @param dest Pointer to the destination float array.
 * @param real Pointer to the real part float array.
 * @param imag pointer to the imaginary part float array.
 * @param n The number of elements to be converted.
 * @param factor The factor that is used to scale the result value.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_NULL on failure.
 */
int visual_math_vector_complex_to_norm_scale (float *dest, const float *real, const float *imag, visual_size_t n,
		float factor)
{
	float *d = dest;
	const float *r = real;
	const float *i = imag;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (real != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (imag != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_factor[4];

		packed_factor[0] = factor;
		packed_factor[1] = factor;
		packed_factor[2] = factor;
		packed_factor[3] = factor;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * factor;

			d++;
			r++;
			i++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_factor) : "memory");

		while (n > 8) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t prefetchnta 256(%1)"
				 "\n\t movups (%0), %%xmm0"
				 "\n\t movups 16(%0), %%xmm2"
				 "\n\t movups (%1), %%xmm1"
				 "\n\t movups 16(%1), %%xmm3"
				 "\n\t mulps %%xmm0, %%xmm0"
				 "\n\t mulps %%xmm1, %%xmm1"
				 "\n\t mulps %%xmm2, %%xmm2"
				 "\n\t mulps %%xmm3, %%xmm3"
				 "\n\t addps %%xmm0, %%xmm1"
				 "\n\t addps %%xmm2, %%xmm3"
				 "\n\t sqrtps %%xmm1, %%xmm0"
				 "\n\t sqrtps %%xmm3, %%xmm2"
				 "\n\t mulps %%xmm7, %%xmm0"
				 "\n\t mulps %%xmm7, %%xmm2"
				 "\n\t movntps %%xmm0, (%2)"
				 "\n\t movntps %%xmm2, 16(%2)"
				 :: "r" (r), "r" (i), "r" (d) : "memory");

			d += 8;
			i += 8;
			r += 8;

			n -= 8;
		}
#endif /* VISUAL_ARCH_X86 */
	}

	while (n--) {
		*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * factor;

		d++;
		r++;
		i++;
	}

	return VISUAL_OK;
}

/**
 * @}
 */

