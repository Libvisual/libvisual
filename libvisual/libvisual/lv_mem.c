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

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
#include <x86intrin.h>
#endif

/* Standard C fallbacks */
static void *mem_set16_c (void *dest, int c, visual_size_t n);
static void *mem_set32_c (void *dest, int c, visual_size_t n);
static void *mem_copy_pitch_c (void *dest, const void *src, int pitch1, int pitch2, int width, int rows);

/* x86 SIMD optimized versions */
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static void *mem_set16_simd_x86 (void *dest, int c, visual_size_t n);

static void *mem_set32_simd_x86 (void *dest, int c, visual_size_t n);
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
	if (visual_cpu_has_sse ()) {
		visual_mem_set16 = mem_set16_simd_x86;
		visual_mem_set32 = mem_set32_simd_x86;
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
	uint32_t setflag32 = (c & 0xffff) | ((c << 16) & 0xffff0000);
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

static void *mem_set16_simd_x86 (void *dest, int c, visual_size_t n)
{
	// FIXME: Do we need 'dest' to be aligned for this to be performing at optimal speed?

	const uint16_t copy    = c & 0xffff;
	const uint32_t copy_2x = (copy & 0xffff) | ((copy << 16) & 0xffff0000);
	const __m64    copy_4x = _mm_set_pi32 (copy_2x, copy_2x);

	__m64 *m64_ptr = (__m64 *) dest;

	// Copy 32 copies each iteration
	while (n >= 32) {
		_mm_stream_pi (m64_ptr, copy_4x);
		_mm_stream_pi (m64_ptr + 1, copy_4x);
		_mm_stream_pi (m64_ptr + 2, copy_4x);
		_mm_stream_pi (m64_ptr + 3, copy_4x);
		_mm_stream_pi (m64_ptr + 4, copy_4x);
		_mm_stream_pi (m64_ptr + 5, copy_4x);
		_mm_stream_pi (m64_ptr + 6, copy_4x);
		_mm_stream_pi (m64_ptr + 7, copy_4x);
		m64_ptr += 8;
		n -= 32;
	}

	uint32_t *uint32_ptr = (uint32_t *) m64_ptr;

	while (n >= 2) {
		*uint32_ptr++ = copy_2x;
		n -= 2;
	}

	uint16_t *uint16_ptr = (uint16_t *) uint32_ptr;
	const uint16_t setflag16 = c & 0xffff;

	while (n--)
		*uint16_ptr++ = setflag16;

	return dest;
}

static void *mem_set32_simd_x86 (void *dest, int c, visual_size_t n)
{
	// FIXME: Do we need 'dest' to be aligned for this to be performing at optimal speed?

	const uint32_t copy = c;
	const __m64    copy_2x = _mm_set_pi32 (copy, copy);

	__m64 *m64_ptr = (__m64 *) dest;

	// Copy 16 copies each iteration
	while (n >= 16) {
		_mm_stream_pi (m64_ptr, copy_2x);
		_mm_stream_pi (m64_ptr + 1, copy_2x);
		_mm_stream_pi (m64_ptr + 2, copy_2x);
		_mm_stream_pi (m64_ptr + 3, copy_2x);
		_mm_stream_pi (m64_ptr + 4, copy_2x);
		_mm_stream_pi (m64_ptr + 5, copy_2x);
		_mm_stream_pi (m64_ptr + 6, copy_2x);
		_mm_stream_pi (m64_ptr + 7, copy_2x);
		m64_ptr += 8;
		n -= 16;
	}

	uint32_t *uint32_ptr = (uint32_t *) m64_ptr;
	while (n--)
		*uint32_ptr++ = copy;

	return dest;
}

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
