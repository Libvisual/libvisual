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

#include "config.h"
#include "lv_math.h"
#include "lv_common.h"
#include "lv_bits.h"
#include "lv_cpu.h"
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

void visual_math_simd_mul_floats_float (float *dest, const float *src, visual_size_t n, float k)
{
	float *d = dest;
	const float *s = src;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	/* FIXME check what is faster on AMD (sse or 3dnow) */
	if (visual_cpu_has_sse () && n >= 16) {
		float packed_multiplier[4];

		packed_multiplier[0] = k;
		packed_multiplier[1] = k;
		packed_multiplier[2] = k;
		packed_multiplier[3] = k;

		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) * k;

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
	} else if (visual_cpu_has_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = k;
		packed_multiplier[1] = k;

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		(*d) = (*s) * k;

		d++;
		s++;
	}
}

void visual_math_simd_add_floats_float (float *dest, const float *src, visual_size_t n, float k)
{
	float *d = dest;
	const float *s = src;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_sse () && n >= 16) {
		float packed_adder[4];

		packed_adder[0] = k;
		packed_adder[1] = k;
		packed_adder[2] = k;
		packed_adder[3] = k;

		while (!VISUAL_ALIGNED(d, 16)) {
			(*d) = (*s) + k;

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
	} else if (visual_cpu_has_3dnow ()) {
		float packed_adder[2];

		packed_adder[0] = k;
		packed_adder[1] = k;

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		(*d) = (*s) + k;

		d++;
		s++;
	}
}

void visual_math_simd_mul_floats_floats (float *dest, const float *src1, const float *src2, visual_size_t n)
{
	float *d = dest;
	const float *s1 = src1;
	const float *s2 = src2;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src1 != NULL);
	visual_return_if_fail (src2 != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_sse () && n >= 16) {
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
	} else if (visual_cpu_has_3dnow ()) {
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
	}

#endif /* VISUAL_ARCH_X86) || VISUAL_ARCH_X86_64 */

	while (n--) {
		(*d) = (*s1) * (*s2);

		d++;
		s1++;
		s2++;
	}
}

void visual_math_simd_floats_to_int32s (int32_t *ints, const float *flts, visual_size_t n)
{
	const float *s = flts;
	int32_t *d = ints;

	visual_return_if_fail (flts != NULL);
	visual_return_if_fail (ints != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_3dnow ()) {

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

	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = *s;

		d++;
		s++;
	}
}

void visual_math_simd_int32s_to_floats (float *flts, const int32_t *ints, visual_size_t n)
{
	const int32_t *s = ints;
	float *d = flts;

	visual_return_if_fail (flts != NULL);
	visual_return_if_fail (ints != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_3dnow ()) {

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = *s;

		d++;
		s++;
	}
}

void visual_math_simd_floats_to_int32s_mul_float (int32_t *ints, const float *flts, visual_size_t n, float k)
{
	const float *s = flts;
	int32_t *d = ints;

	visual_return_if_fail (flts != NULL);
	visual_return_if_fail (ints != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = k;
		packed_multiplier[1] = k;

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
	}

#endif

	while (n--) {
		*d = (float) *s * k;

		d++;
		s++;
	}
}

void visual_math_simd_int32s_to_floats_mul_float (float *flts, const int32_t *ints, visual_size_t n, float k)
{
	const int32_t *s = ints;
	float *d = flts;

	visual_return_if_fail (flts != NULL);
	visual_return_if_fail (ints != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	if (visual_cpu_has_3dnow ()) {
		float packed_multiplier[2];

		packed_multiplier[0] = k;
		packed_multiplier[1] = k;

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
	}
#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = (float) *s * k;

		d++;
		s++;
	}
}

void visual_math_simd_floats_to_int32s_mul_float_denormalise (int32_t *ints, const float *flts, visual_size_t n, float k)
{
	const float *s = flts;
	int32_t *d = ints;

	visual_return_if_fail (flts != NULL);
	visual_return_if_fail (ints != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	if (visual_cpu_has_3dnow ()) {
		float packed_multiplier[2];
		float packed_normalise_mul[2];
		float packed_adder[2];

		packed_multiplier[0] = k;
		packed_multiplier[1] = k;

		packed_normalise_mul[0] = 0.5;
		packed_normalise_mul[1] = 0.5;

		packed_adder[0] = 1;
		packed_adder[1] = 1;

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = (float) (((*s) + 1) * 0.5) * k;

		d++;
		s++;
	}
}

void visual_math_simd_sqrt_floats (float *dest, const float *src, visual_size_t n)
{
	float *d = dest;
	const float *s = src;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_sse () && n >= 16) {
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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = sqrtf (*s);

		d++;
		s++;
	}
}

void visual_math_simd_complex_norm (float *dest, const float *real, const float *imag, visual_size_t n)
{
	float *d = dest;
	const float *r = real;
	const float *i = imag;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (real != NULL);
	visual_return_if_fail (imag != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_sse () && n >= 16) {

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	while (n--) {
		*d = sqrtf (((*r) * (*r)) + ((*i) * (*i)));

		d++;
		r++;
		i++;
	}
}

void visual_math_simd_complex_norm_mul_float (float *dest, const float *real, const float *imag, visual_size_t n, float k)
{
	float *d = dest;
	const float *r = real;
	const float *i = imag;

	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (real != NULL);
	visual_return_if_fail (imag != NULL);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_sse () && n >= 16) {
		float packed_scaler[4];

		packed_scaler[0] = k;
		packed_scaler[1] = k;
		packed_scaler[2] = k;
		packed_scaler[3] = k;

		while (!VISUAL_ALIGNED(d, 16)) {
			*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * k;

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
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */


	while (n--) {
		*d = sqrtf (((*r) * (*r)) + ((*i) * (*i))) * k;

		d++;
		r++;
		i++;
	}
}
