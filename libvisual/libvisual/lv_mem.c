/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include <stdlib.h>
#include <string.h>

/* Standard C fallbacks */
static void *mem_set16_c (void *dest, int c, visual_size_t n);
static void *mem_set32_c (void *dest, int c, visual_size_t n);
static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);

/* x86 SIMD optimized versions */
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static void *mem_set16_mmx (void *dest, int c, visual_size_t n);
static void *mem_set16_mmx2 (void *dest, int c, visual_size_t n);

static void *mem_set32_mmx (void *dest, int c, visual_size_t n);
static void *mem_set32_mmx2 (void *dest, int c, visual_size_t n);
#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */


/* Optimal performance functions set by visual_mem_initialize(). */
VisMemCopyFunc visual_mem_copy = memcpy;
VisMemCopyPitchFunc visual_mem_copy_pitch = mem_copy_pitch_c;

VisMemSet8Func visual_mem_set = memset;
VisMemSet16Func visual_mem_set16 = mem_set16_c;
VisMemSet32Func visual_mem_set32 = mem_set32_c;


void visual_mem_initialize ()
{
	/* Arranged from slow to fast, so the slower version gets overloaded
	 * every time */

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

	if (visual_cpu_has_mmx ()) {
		visual_mem_set16 = mem_set16_mmx;
		visual_mem_set32 = mem_set32_mmx;
	}

	/* The k6-II and k6-III don't have mmx2, but of course can use the prefetch
	 * facility that 3dnow provides. */

	if (visual_cpu_has_mmx2 ()) {
		visual_mem_set16 = mem_set16_mmx2;
		visual_mem_set32 = mem_set32_mmx2;
	}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
}

void *visual_mem_malloc (visual_size_t nbytes)
{
	void *buf;

	visual_return_val_if_fail (nbytes > 0, NULL);

	buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, "Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory", nbytes);

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

void visual_mem_free (void *ptr)
{
	free (ptr);
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
