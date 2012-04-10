/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem.c,v 1.30 2006/02/05 18:45:57 synap Exp $
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
#include "lv_mem.h"
#include "lv_common.h"
#include "lv_cpu.h"
#include "lv_bits.h"
#include <string.h>
#include <stdlib.h>
#include "gettext.h"


/* Standard C fallbacks */
static void *mem_copy_c (void *dest, const void *src, visual_size_t n);
static void *mem_set8_c (void *dest, int c, visual_size_t n);
static void *mem_set16_c (void *dest, int c, visual_size_t n);
static void *mem_set32_c (void *dest, int c, visual_size_t n);
static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);


/* x86 SIMD optimized versions */
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static void *mem_copy_3dnow (void *dest, const void *src, visual_size_t n);
static void *mem_copy_mmx (void *dest, const void *src, visual_size_t n);
static void *mem_copy_mmx2 (void *dest, const void *src, visual_size_t n);

static void *mem_copy_pitch_mmx (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);
static void *mem_copy_pitch_mmx2 (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);
static void *mem_copy_pitch_3dnow (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);

static void *mem_set8_mmx (void *dest, int c, visual_size_t n);
static void *mem_set8_mmx2 (void *dest, int c, visual_size_t n);

static void *mem_set16_mmx (void *dest, int c, visual_size_t n);
static void *mem_set16_mmx2 (void *dest, int c, visual_size_t n);

static void *mem_set32_mmx (void *dest, int c, visual_size_t n);
static void *mem_set32_mmx2 (void *dest, int c, visual_size_t n);
#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */


/* Optimal performance functions set by visual_mem_initialize(). */
VisMemCopyFunc visual_mem_copy = mem_copy_c;
VisMemCopyPitchFunc visual_mem_copy_pitch = mem_copy_pitch_c;

VisMemSet8Func visual_mem_set = mem_set8_c;
VisMemSet16Func visual_mem_set16 = mem_set16_c;
VisMemSet32Func visual_mem_set32 = mem_set32_c;



int visual_mem_initialize ()
{
	/* Arranged from slow to fast, so the slower version gets overloaded
	 * every time */

	visual_mem_copy = mem_copy_c;
	visual_mem_copy_pitch = mem_copy_pitch_c;

	visual_mem_set = mem_set8_c;
	visual_mem_set16 = mem_set16_c;
	visual_mem_set32 = mem_set32_c;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_mmx ()) {
		visual_mem_copy = mem_copy_mmx;
		visual_mem_copy_pitch = mem_copy_pitch_mmx;

		visual_mem_set = mem_set8_mmx;
		visual_mem_set16 = mem_set16_mmx;
		visual_mem_set32 = mem_set32_mmx;
	}

	/* The k6-II and k6-III don't have mmx2, but of course can use the prefetch
	 * facility that 3dnow provides. */
	if (visual_cpu_has_3dnow ()) {
		visual_mem_copy = mem_copy_3dnow;
		visual_mem_copy_pitch = mem_copy_pitch_3dnow;
	}

	if (visual_cpu_has_mmx2 ()) {
		visual_mem_copy = mem_copy_mmx2;
		visual_mem_copy_pitch = mem_copy_pitch_mmx2;

		visual_mem_set = mem_set8_mmx2;
		visual_mem_set16 = mem_set16_mmx2;
		visual_mem_set32 = mem_set32_mmx2;
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

	return VISUAL_OK;
}

void *visual_mem_malloc (visual_size_t nbytes)
{
	void *buf;

	visual_return_val_if_fail (nbytes > 0, NULL);

	buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory"), nbytes);

		return NULL;
	}

	return buf;
}

void *visual_mem_malloc0 (visual_size_t nbytes)
{
	void *buf;

	visual_return_val_if_fail (nbytes > 0, NULL);

	buf = visual_mem_malloc (nbytes);

	visual_mem_set (buf, 0, nbytes);

	return buf;
}

void *visual_mem_realloc (void *ptr, visual_size_t nbytes)
{
	return realloc (ptr, nbytes);
}

int visual_mem_free (void *ptr)
{
	/* FIXME remove eventually, we keep it for now for explicit debug */
	visual_return_val_if_fail (ptr != NULL, -VISUAL_ERROR_MEM_NULL);

	free (ptr);

	return VISUAL_OK;
}

static void *mem_copy_c (void *dest, const void *src, visual_size_t n)
{
	return memcpy(dest, src, n);
}

/* Memset functions, 1 byte memset */
static void *mem_set8_c (void *dest, int c, visual_size_t n)
{
	return memset(dest, c, n);
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

/* Memset functions, 4 byte memset */
static void *mem_set32_c (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

	while (n--)
		*d++ = setflag32;

	return dest;
}


/* Memcopy with pitch functions */
static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int row_bytes, int rows)
{
	uint8_t *d = dest;
	const uint8_t *s = src;
	int i;

	for (i = 0; i < rows; i++) {
		memcpy(d, s, row_bytes);

		d += pitch1;
		s += pitch2;
	}

	return dest;
}


#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

static void *mem_copy_mmx (void *dest, const void *src, visual_size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

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

	while (n >= 4) {
		*d++ = setflag32;
		n -= 4;
	}

	dc = (uint8_t *) d;

	while (n--)
		*dc++ = setflag8;

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

	while (n >= 2) {
		*d++ = setflag32;
		n -= 2;
	}

	dc = (uint16_t *) d;

	while (n--)
		*dc++ = setflag16;

	return dest;
}

static void *mem_set32_mmx (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

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

	while (n--)
		*d++ = setflag32;

	return dest;
}

static void *mem_set32_mmx2 (void *dest, int c, visual_size_t n)
{
	uint32_t *d = dest;
	uint32_t setflag32 = c;

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

	while (n--)
		*d++ = setflag32;

	return dest;
}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
