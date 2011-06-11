/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_math.c,v 1.13 2006/02/13 20:54:08 synap Exp $
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
int visual_math_vectorized_multiplier_floats_const_float (float *dest, float *src, visual_size_t n, float multiplier)
{
	float *d = dest;
	float *s = src;

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
	} else if (visual_cpu_get_3dnow ()) {
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
int visual_math_vectorized_add_floats_const_float (float *dest, float *src, visual_size_t n, float adder)
{
	float *d = dest;
	float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_adder[4];

		packed_adder[0] = adder;
		packed_adder[1] = adder;
		packed_adder[2] = adder;
		packed_adder[3] = adder;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) + adder;

			d++;
			s++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_adder) : "memory");


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
	} else if (visual_cpu_get_3dnow ()) {
		float packed_adder[2];

		packed_adder[0] = adder;
		packed_adder[1] = adder;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[adder], %%mm0"
			 :: [adder] "m" (*packed_adder));

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
		(*d) = (*s) + adder;

		d++;
		s++;
	}

	return VISUAL_OK;
}

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
int visual_math_vectorized_substract_floats_const_float (float *dest, float *src, visual_size_t n, float substracter)
{
	float *d = dest;
	float *s = src;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_substracter[4];

		packed_substracter[0] = substracter;
		packed_substracter[1] = substracter;
		packed_substracter[2] = substracter;
		packed_substracter[3] = substracter;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) - substracter;

			d++;
			s++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_substracter) : "memory");


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
	} else if (visual_cpu_get_3dnow ()) {
		float packed_substracter[2];

		packed_substracter[0] = substracter;
		packed_substracter[1] = substracter;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[substracter], %%mm0"
			 :: [substracter] "m" (*packed_substracter));

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
		(*d) = (*s) - substracter;

		d++;
		s++;
	}

	return VISUAL_OK;
}

int visual_math_vectorized_multiplier_floats_floats (float *dest, float *src1, float *src2, visual_size_t n)
{
	float *d = dest;
	float *s1 = src1;
	float *s2 = src2;

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
	} else if (visual_cpu_get_3dnow ()) {
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
int visual_math_vectorized_floats_to_int32s (int32_t *ints, float *flts, visual_size_t n)
{
	float *s = flts;
	int32_t *d = ints;

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
int visual_math_vectorized_int32s_to_floats (float *flts, int32_t *ints, visual_size_t n)
{
	int32_t *s = ints;
	float *d = flts;

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
int visual_math_vectorized_floats_to_int32s_multiply (int32_t *ints, float *flts, visual_size_t n, float multiplier)
{
	float *s = flts;
	int32_t *d = ints;

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
int visual_math_vectorized_int32s_to_floats_multiply (float *flts, int32_t *ints, visual_size_t n, float multiplier)
{
	int32_t *s = ints;
	float *d = flts;

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
int visual_math_vectorized_floats_to_int32s_multiply_denormalise (int32_t *ints, float *flts, visual_size_t n, float multiplier)
{
	float *s = flts;
	int32_t *d = ints;

	visual_log_return_val_if_fail (flts != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (ints != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_3dnow ()) {
		float packed_multiplier[2];
		float packed_normalise_mul[2];
		float packed_adder[2];

		packed_multiplier[0] = multiplier;
		packed_multiplier[1] = multiplier;

		packed_normalise_mul[0] = 0.5;
		packed_normalise_mul[1] = 0.5;

		packed_adder[0] = 1;
		packed_adder[1] = 1;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t movq %[multiplier], %%mm0"
			 "\n\t movq %[normalise_mul], %%mm6"
			 "\n\t movq %[adder], %%mm7"

			 :: [multiplier] "m" (*packed_multiplier),
			 [normalise_mul] "m" (*packed_normalise_mul),
			 [adder] "m" (*packed_adder));

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
int visual_math_vectorized_sqrt_floats (float *dest, float *src, visual_size_t n)
{
	float *d = dest;
	float *s = src;

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
int visual_math_vectorized_complex_to_norm (float *dest, float *real, float *imag, visual_size_t n)
{
	float *d = dest;
	float *r = real;
	float *i = imag;

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
int visual_math_vectorized_complex_to_norm_scale (float *dest, float *real, float *imag, visual_size_t n, float scaler)
{
	float *d = dest;
	float *r = real;
	float *i = imag;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (real != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (imag != NULL, -VISUAL_ERROR_NULL);

	if (visual_cpu_get_sse () && n >= 16) {
		float packed_scaler[4];

		packed_scaler[0] = scaler;
		packed_scaler[1] = scaler;
		packed_scaler[2] = scaler;
		packed_scaler[3] = scaler;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * scaler;

			d++;
			r++;
			i++;

			n--;
		}

		__asm __volatile
			("\n\t movups (%0), %%xmm7"
			 :: "r" (packed_scaler) : "memory");

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
		*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * scaler;

		d++;
		r++;
		i++;
	}

	return VISUAL_OK;
}

/**
 * @}
 */

