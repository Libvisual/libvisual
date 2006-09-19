/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem_ops.c,v 1.1 2006-09-19 18:28:51 synap Exp $
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

#include <config.h>

#include <string.h>
#include <stdlib.h>

#include "lv_mem_ops.h"
#include "lv_common.h"
#include "lv_log.h"
#include "lv_error.h"
#include "lv_cpu.h"
#include "lv_bits.h"

/* FIXME sse, altivec versions, optionally with prefetching and such
 * with checking for optimal cache lines */

/* Optimize mem function prototypes */
static void *mem_copy_c (void *dest, const void *src, visual_size_t n);
static void *mem_copy_mmx (void *dest, const void *src, visual_size_t n);
static void *mem_copy_mmx2 (void *dest, const void *src, visual_size_t n);
static void *mem_copy_3dnow (void *dest, const void *src, visual_size_t n);
static void *mem_copy_altivec (void *dest, const void *src, visual_size_t n);

static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);
static void *mem_copy_pitch_mmx (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);
static void *mem_copy_pitch_mmx2 (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);
static void *mem_copy_pitch_3dnow (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);

static void *mem_set8_c (void *dest, int c, visual_size_t n);
static void *mem_set8_mmx (void *dest, int c, visual_size_t n);
static void *mem_set8_mmx2 (void *dest, int c, visual_size_t n);
static void *mem_set8_altivec (void *dest, int c, visual_size_t n);

static void *mem_set16_c (void *dest, int c, visual_size_t n);
static void *mem_set16_mmx (void *dest, int c, visual_size_t n);
static void *mem_set16_mmx2 (void *dest, int c, visual_size_t n);
static void *mem_set16_altivec (void *dest, int c, visual_size_t n);

static void *mem_set32_c (void *dest, int c, visual_size_t n);
static void *mem_set32_mmx (void *dest, int c, visual_size_t n);
static void *mem_set32_mmx2 (void *dest, int c, visual_size_t n);
static void *mem_set32_altivec (void *dest, int c, visual_size_t n);

/* Optimal performance functions set by visual_mem_initialize(). */
VisMemCopyFunc visual_mem_copy = mem_copy_c;
VisMemCopyPitchFunc visual_mem_copy_pitch = mem_copy_pitch_c;

VisMemSet8Func visual_mem_set = mem_set8_c;
VisMemSet16Func visual_mem_set16 = mem_set16_c;
VisMemSet32Func visual_mem_set32 = mem_set32_c;


/**
 * @defgroup VisMem VisMem
 * @{
 */

/**
 * Initialize the memory functions. This is used to set the function pointers to the right optimized version.
 * It's legal to call visual_mem_initialize more than once in the same context if it's needed to reset the optimal
 * function pointers. This function bases it's choices upon the VisCPU system.
 *
 * Be aware that visual_mem_initialize() should be called to set the most optimize mem_copy() and
 * mem_set() functions is called. Be sure that visual_cpu_initialize() is called before this however if
 * possible the best solution is to just call visual_init() which will call all the libvisual initialize functions.
 *
 * return VISUAL_OK on succes.
 */
int visual_mem_ops_initialize ()
{
	/* Arranged from slow to fast, so the slower version gets overloaded
	 * every time */

	visual_mem_copy = mem_copy_c;
	visual_mem_copy_pitch = mem_copy_pitch_c;

	visual_mem_set = mem_set8_c;
	visual_mem_set16 = mem_set16_c;
	visual_mem_set32 = mem_set32_c;

	if (visual_cpu_get_mmx () > 0) {
		visual_mem_copy = mem_copy_mmx;
		visual_mem_copy_pitch = mem_copy_pitch_mmx;

		visual_mem_set = mem_set8_mmx;
		visual_mem_set16 = mem_set16_mmx;
		visual_mem_set32 = mem_set32_mmx;
	}

	/* The k6-II and k6-III don't have mmx2, but of course can use the prefetch
	 * facility that 3dnow provides. */
	if (visual_cpu_get_3dnow () > 0) {
		visual_mem_copy = mem_copy_3dnow;
		visual_mem_copy_pitch = mem_copy_pitch_3dnow;
	}

	if (visual_cpu_get_mmx2 () > 0) {
		visual_mem_copy = mem_copy_mmx2;
		visual_mem_copy_pitch = mem_copy_pitch_mmx2;

		visual_mem_set = mem_set8_mmx2;
		visual_mem_set16 = mem_set16_mmx2;
		visual_mem_set32 = mem_set32_mmx2;
	}

	return VISUAL_OK;
}

/* Optimize mem functions */
static void *mem_copy_c (void *dest, const void *src, visual_size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

	while (n >= 4) {
		*d++ = *s++;
		n -= 4;
	}

	dc = (uint8_t *) d;
	sc = (const uint8_t *) s;

	while (n--)
		*dc++ = *sc++;

	return dest;
}

static void *mem_copy_mmx (void *dest, const void *src, visual_size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	while (n >= 64) {
		__asm __volatile
			("\n\t movq (%0), %%mm0"
			 "\n\t movq 8(%0), %%mm1"
			 "\n\t movq 16(%0), %%mm2"
			 "\n\t movq 24(%0), %%mm3"
			 "\n\t movq 32(%0), %%mm4"
			 "\n\t movq 40(%0), %%mm5"
			 "\n\t movq 48(%0), %%mm6"
			 "\n\t movq 56(%0), %%mm7"
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

		n -= 64;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 4) {
		*d++ = *s++;
		n -= 4;
	}

	dc = (uint8_t *) d;
	sc = (const uint8_t *) s;

	while (n--)
		*dc++ = *sc++;

	return dest;
}

static void *mem_copy_mmx2 (void *dest, const void *src, visual_size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	while (n >= 64) {
		__asm __volatile
			("\n\t prefetchnta 256(%0)"
			 "\n\t prefetchnta 320(%0)"
			 "\n\t movq (%0), %%mm0"
			 "\n\t movq 8(%0), %%mm1"
			 "\n\t movq 16(%0), %%mm2"
			 "\n\t movq 24(%0), %%mm3"
			 "\n\t movq 32(%0), %%mm4"
			 "\n\t movq 40(%0), %%mm5"
			 "\n\t movq 48(%0), %%mm6"
			 "\n\t movq 56(%0), %%mm7"
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

		n -= 64;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 4) {
		*d++ = *s++;
		n -= 4;
	}

	dc = (uint8_t *) d;
	sc = (const uint8_t *) s;

	while (n--)
		*dc++ = *sc++;

	return dest;
}

static void *mem_copy_3dnow (void *dest, const void *src, visual_size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	while (n >= 64) {
		__asm __volatile
			("\n\t prefetch 256(%0)"
			 "\n\t prefetch 320(%0)"
			 "\n\t movq (%0), %%mm0"
			 "\n\t movq 8(%0), %%mm1"
			 "\n\t movq 16(%0), %%mm2"
			 "\n\t movq 24(%0), %%mm3"
			 "\n\t movq 32(%0), %%mm4"
			 "\n\t movq 40(%0), %%mm5"
			 "\n\t movq 48(%0), %%mm6"
			 "\n\t movq 56(%0), %%mm7"
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

		n -= 64;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 4) {
		*d++ = *s++;
		n -= 4;
	}

	dc = (uint8_t *) d;
	sc = (const uint8_t *) s;

	while (n--)
		*dc++ = *sc++;

	return dest;
}

static void *mem_copy_altivec (void *dest, const void *src, visual_size_t n)
{

}

/* Memcopy with pitch functions */
static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int width, int rows)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	int i;

	for (i = 0; i < rows; i++) {
		uint32_t *inner_d;
		const uint32_t *inner_s;
		uint8_t *inner_dc = (uint8_t*) d;
		const uint8_t *inner_sc = (const uint8_t*) s;
		int n = width;

                while (!VISUAL_ALIGNED(inner_dc, 4) && n > 4) {
			*inner_dc++ = *inner_sc++;
			n--;
		}

		inner_d = (uint32_t*) inner_dc;
		inner_s = (const uint32_t*) inner_sc;

		while (n >= 4) {
			*inner_d++ = *inner_s++;
			n -= 4;
		}

		inner_dc = (uint8_t*) inner_d;
		inner_sc = (const uint8_t*) inner_s;

		while (n--)
			*inner_dc++ = *inner_sc++;

		d = (uint32_t*)((uint8_t*) d + pitch1);
		s = (const uint32_t*)((const uint8_t*) s + pitch2);
	}

	return dest;
}

static void *mem_copy_pitch_mmx (void *dest, const void *src, int pitch1, int pitch2, int width, int rows)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	int i;

	for (i = 0; i < rows; i++) {
		uint32_t *inner_d;
		const uint32_t *inner_s;
		uint8_t *inner_dc = (uint8_t*) d;
		const uint8_t *inner_sc = (const uint8_t*) s;
		int n = width;

                while (!VISUAL_ALIGNED(inner_dc, 4) && n > 4) {
			*inner_dc++ = *inner_sc++;
			n--;
		}

		inner_d = (uint32_t*) inner_dc;
		inner_s = (const uint32_t*) inner_sc;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (n >= 64) {
			__asm __volatile
				("\n\t movq (%0), %%mm0"
				 "\n\t movq 8(%0), %%mm1"
				 "\n\t movq 16(%0), %%mm2"
				 "\n\t movq 24(%0), %%mm3"
				 "\n\t movq 32(%0), %%mm4"
				 "\n\t movq 40(%0), %%mm5"
				 "\n\t movq 48(%0), %%mm6"
				 "\n\t movq 56(%0), %%mm7"
				 "\n\t movq %%mm0, (%1)"
				 "\n\t movq %%mm1, 8(%1)"
				 "\n\t movq %%mm2, 16(%1)"
				 "\n\t movq %%mm3, 24(%1)"
				 "\n\t movq %%mm4, 32(%1)"
				 "\n\t movq %%mm5, 40(%1)"
				 "\n\t movq %%mm6, 48(%1)"
				 "\n\t movq %%mm7, 56(%1)"
				 :: "r" (inner_s), "r" (inner_d) : "memory");

			inner_d += 16;
			inner_s += 16;

			n -= 64;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

		while (n >= 4) {
			*inner_d++ = *inner_s++;
			n -= 4;
		}

		inner_dc = (uint8_t*) inner_d;
		inner_sc = (const uint8_t*) inner_s;

		while (n--)
			*inner_dc++ = *inner_sc++;

		d = (uint32_t*)((uint8_t*) d + pitch1);
		s = (const uint32_t*)((const uint8_t*) s + pitch2);
	}

	return dest;
}

static void *mem_copy_pitch_mmx2 (void *dest, const void *src, int pitch1, int pitch2, int width, int rows)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	int i;

	for (i = 0; i < rows; i++) {
		uint32_t *inner_d;
		const uint32_t *inner_s;
		uint8_t *inner_dc = (uint8_t*) d;
		const uint8_t *inner_sc = (const uint8_t*) s;
		int n = width;

                while (!VISUAL_ALIGNED(inner_dc, 4) && n > 4) {
			*inner_dc++ = *inner_sc++;
			n--;
		}

		inner_d = (uint32_t*) inner_dc;
		inner_s = (const uint32_t*) inner_sc;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (n >= 64) {
			__asm __volatile
				("\n\t prefetchnta 256(%0)"
				 "\n\t prefetchnta 320(%0)"
				 "\n\t movq (%0), %%mm0"
				 "\n\t movq 8(%0), %%mm1"
				 "\n\t movq 16(%0), %%mm2"
				 "\n\t movq 24(%0), %%mm3"
				 "\n\t movq 32(%0), %%mm4"
				 "\n\t movq 40(%0), %%mm5"
				 "\n\t movq 48(%0), %%mm6"
				 "\n\t movq 56(%0), %%mm7"
				 "\n\t movntq %%mm0, (%1)"
				 "\n\t movntq %%mm1, 8(%1)"
				 "\n\t movntq %%mm2, 16(%1)"
				 "\n\t movntq %%mm3, 24(%1)"
				 "\n\t movntq %%mm4, 32(%1)"
				 "\n\t movntq %%mm5, 40(%1)"
				 "\n\t movntq %%mm6, 48(%1)"
				 "\n\t movntq %%mm7, 56(%1)"
				 :: "r" (inner_s), "r" (inner_d) : "memory");

			inner_d += 16;
			inner_s += 16;

			n -= 64;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

		while (n >= 4) {
			*inner_d++ = *inner_s++;
			n -= 4;
		}

		inner_dc = (uint8_t*) inner_d;
		inner_sc = (const uint8_t*) inner_s;

		while (n--)
			*inner_dc++ = *inner_sc++;

		d = (uint32_t*)((uint8_t*) d + pitch1);
		s = (const uint32_t*)((const uint8_t*) s + pitch2);
	}

	return dest;
}

static void *mem_copy_pitch_3dnow (void *dest, const void *src, int pitch1, int pitch2, int width, int rows)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	int i;

	for (i = 0; i < rows; i++) {
		uint32_t *inner_d;
		const uint32_t *inner_s;
		uint8_t *inner_dc = (uint8_t*) d;
		const uint8_t *inner_sc = (const uint8_t*) s;
		int n = width;

                while (!VISUAL_ALIGNED(inner_dc, 4) && n > 4) {
			*inner_dc++ = *inner_sc++;
			n--;
		}

		inner_d = (uint32_t*) inner_dc;
		inner_s = (const uint32_t*) inner_sc;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		while (n >= 64) {
			__asm __volatile
				("\n\t prefetch 256(%0)"
				 "\n\t prefetch 320(%0)"
				 "\n\t movq (%0), %%mm0"
				 "\n\t movq 8(%0), %%mm1"
				 "\n\t movq 16(%0), %%mm2"
				 "\n\t movq 24(%0), %%mm3"
				 "\n\t movq 32(%0), %%mm4"
				 "\n\t movq 40(%0), %%mm5"
				 "\n\t movq 48(%0), %%mm6"
				 "\n\t movq 56(%0), %%mm7"
				 "\n\t movq %%mm0, (%1)"
				 "\n\t movq %%mm1, 8(%1)"
				 "\n\t movq %%mm2, 16(%1)"
				 "\n\t movq %%mm3, 24(%1)"
				 "\n\t movq %%mm4, 32(%1)"
				 "\n\t movq %%mm5, 40(%1)"
				 "\n\t movq %%mm6, 48(%1)"
				 "\n\t movq %%mm7, 56(%1)"
				 :: "r" (inner_s), "r" (inner_d) : "memory");

			inner_d += 16;
			inner_s += 16;

			n -= 64;
		}

		__asm __volatile
			("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

		while (n >= 4) {
			*inner_d++ = *inner_s++;
			n -= 4;
		}

		inner_dc = (uint8_t*) inner_d;
		inner_sc = (const uint8_t*) inner_s;

		while (n--)
			*inner_dc++ = *inner_sc++;

		d = (uint32_t*)((uint8_t*) d + pitch1);
		s = (const uint32_t*)((const uint8_t*) s + pitch2);
	}

	return dest;
}

/* Memset functions, 1 byte memset */
static void *mem_set8_c (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint8_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xff) |
		((c << 8) & 0xff00) |
		((c << 16) & 0xff0000) |
		((c << 24) & 0xff000000);
	uint8_t setflag8 = c & 0xff;

	while (n >= 4) {
		*d++ = setflag32;
		n -= 4;
	}

	dc = (uint8_t *) d;

	while (n--)
		*dc++ = setflag8;

	return dest;
}

static void *mem_set8_mmx (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint8_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xff) |
		((c << 8) & 0xff00) |
		((c << 16) & 0xff0000) |
		((c << 24) & 0xff000000);
	uint8_t setflag8 = c & 0xff;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 64) {
		__asm __volatile
			("\n\t movq %%mm0, (%0)"
			 "\n\t movq %%mm1, 8(%0)"
			 "\n\t movq %%mm2, 16(%0)"
			 "\n\t movq %%mm3, 24(%0)"
			 "\n\t movq %%mm4, 32(%0)"
			 "\n\t movq %%mm5, 40(%0)"
			 "\n\t movq %%mm6, 48(%0)"
			 "\n\t movq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 64;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 4) {
		*d++ = setflag32;
		n -= 4;
	}

	dc = (uint8_t *) d;

	while (n--)
		*dc++ = setflag8;

	return dest;
}

static void *mem_set8_mmx2 (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint8_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xff) |
		((c << 8) & 0xff00) |
		((c << 16) & 0xff0000) |
		((c << 24) & 0xff000000);
	uint8_t setflag8 = c & 0xff;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 64) {
		__asm __volatile
			("\n\t movntq %%mm0, (%0)"
			 "\n\t movntq %%mm1, 8(%0)"
			 "\n\t movntq %%mm2, 16(%0)"
			 "\n\t movntq %%mm3, 24(%0)"
			 "\n\t movntq %%mm4, 32(%0)"
			 "\n\t movntq %%mm5, 40(%0)"
			 "\n\t movntq %%mm6, 48(%0)"
			 "\n\t movntq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 64;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 4) {
		*d++ = setflag32;
		n -= 4;
	}

	dc = (uint8_t *) d;

	while (n--)
		*dc++ = setflag8;

	return dest;
}

static void *mem_set8_altivec (void *dest, int c, visual_size_t n)
{

}

/* Memset functions, 2 byte memset */
static void *mem_set16_c (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint16_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xffff) |
		((c << 16) & 0xffff0000);
	uint16_t setflag16 = c & 0xffff;

	while (n >= 2) {
		*d++ = setflag32;
		n -= 2;
	}

	dc = (uint16_t *) d;

	while (n--)
		*dc++ = setflag16;

	return dest;
}

static void *mem_set16_mmx (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint16_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xffff) |
		((c << 16) & 0xffff0000);
	uint16_t setflag16 = c & 0xffff;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 64) {
		__asm __volatile
			("\n\t movq %%mm0, (%0)"
			 "\n\t movq %%mm1, 8(%0)"
			 "\n\t movq %%mm2, 16(%0)"
			 "\n\t movq %%mm3, 24(%0)"
			 "\n\t movq %%mm4, 32(%0)"
			 "\n\t movq %%mm5, 40(%0)"
			 "\n\t movq %%mm6, 48(%0)"
			 "\n\t movq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 32;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 2) {
		*d++ = setflag32;
		n -= 2;
	}

	dc = (uint16_t *) d;

	while (n--)
		*dc++ = setflag16;

	return dest;
}

static void *mem_set16_mmx2 (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint16_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xffff) |
		((c << 16) & 0xffff0000);
	uint16_t setflag16 = c & 0xffff;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 32) {
		__asm __volatile
			("\n\t movntq %%mm0, (%0)"
			 "\n\t movntq %%mm1, 8(%0)"
			 "\n\t movntq %%mm2, 16(%0)"
			 "\n\t movntq %%mm3, 24(%0)"
			 "\n\t movntq %%mm4, 32(%0)"
			 "\n\t movntq %%mm5, 40(%0)"
			 "\n\t movntq %%mm6, 48(%0)"
			 "\n\t movntq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 32;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n >= 2) {
		*d++ = setflag32;
		n -= 2;
	}

	dc = (uint16_t *) d;

	while (n--)
		*dc++ = setflag16;

	return dest;
}

static void *mem_set16_altivec (void *dest, int c, visual_size_t n)
{

}

/* Memset functions, 4 byte memset */
static void *mem_set32_c (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

	while (n--)
		*d++ = setflag32;

	return dest;
}

static void *mem_set32_mmx (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 64) {
		__asm __volatile
			("\n\t movq %%mm0, (%0)"
			 "\n\t movq %%mm1, 8(%0)"
			 "\n\t movq %%mm2, 16(%0)"
			 "\n\t movq %%mm3, 24(%0)"
			 "\n\t movq %%mm4, 32(%0)"
			 "\n\t movq %%mm5, 40(%0)"
			 "\n\t movq %%mm6, 48(%0)"
			 "\n\t movq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 16;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n--) 
		*d++ = setflag32;

	return dest;
}

static void *mem_set32_mmx2 (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t movd (%0), %%mm0"
		 "\n\t movd (%0), %%mm1"
		 "\n\t psllq $32, %%mm1"
		 "\n\t por %%mm1, %%mm0"
		 "\n\t movq %%mm0, %%mm2"
		 "\n\t movq %%mm0, %%mm1"
		 "\n\t movq %%mm2, %%mm3"
		 "\n\t movq %%mm1, %%mm4"
		 "\n\t movq %%mm0, %%mm5"
		 "\n\t movq %%mm2, %%mm6"
		 "\n\t movq %%mm1, %%mm7"
		 :: "r" (&setflag32) : "memory");

	while (n >= 64) {
		__asm __volatile
			("\n\t movntq %%mm0, (%0)"
			 "\n\t movntq %%mm1, 8(%0)"
			 "\n\t movntq %%mm2, 16(%0)"
			 "\n\t movntq %%mm3, 24(%0)"
			 "\n\t movntq %%mm4, 32(%0)"
			 "\n\t movntq %%mm5, 40(%0)"
			 "\n\t movntq %%mm6, 48(%0)"
			 "\n\t movntq %%mm7, 56(%0)"
			 :: "r" (d) : "memory");

		d += 16;

		n -= 16;
	}

	__asm __volatile
		("\n\t emms");
#endif /* VISUAL_ARCH_X86 */

	while (n--)
		*d++ = setflag32;

	return dest;
}

static void *mem_set32_altivec (void *dest, int c, visual_size_t n)
{

}

/**
 * @}
 */

