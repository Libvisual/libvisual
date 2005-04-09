/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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
#include <gettext.h>

#include "lv_mem.h"
#include "lv_common.h"
#include "lv_log.h"
#include "lv_error.h"
#include "lv_cpu.h"

/* FIXME sse, sse2, altivec versions, optionally with prefetching and such
 * with checking for optimal scan lines */

/* Optimize mem function prototypes */
static void *mem_copy_c (void *dest, const void *src, size_t n);
static void *mem_copy_mmx (void *dest, const void *src, size_t n);
static void *mem_copy_mmx2 (void *dest, const void *src, size_t n);
static void *mem_copy_sse (void *dest, const void *src, size_t n);
static void *mem_copy_sse2 (void *dest, const void *src, size_t n);
static void *mem_copy_altivec (void *dest, const void *src, size_t n);

static void *mem_set_c (void *dest, int c, size_t n);
static void *mem_set_mmx (void *dest, int c, size_t n);
static void *mem_set_mmx2 (void *dest, int c, size_t n);
static void *mem_set_sse (void *dest, int c, size_t n);
static void *mem_set_sse2 (void *dest, int c, size_t n);
static void *mem_set_altivec (void *dest, int c, size_t n);

/* Optimal performance functions set by visual_mem_initialize(). */
VisMemCopyFunc visual_mem_copy = mem_copy_c;
VisMemSetFunc visual_mem_set = mem_set_c;


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
int visual_mem_initialize ()
{
	/* Arranged from slow to fast, so the slower version gets overloaded
	 * every time */

	visual_mem_copy = mem_copy_c;
	visual_mem_set = mem_set_c;
	
	if (visual_cpu_get_mmx () > 0) {
		visual_mem_copy = mem_copy_mmx;
		visual_mem_set = mem_set_mmx;
	}

	if (visual_cpu_get_mmx2 () > 0) {
		visual_mem_copy = mem_copy_mmx2;
		visual_mem_set = mem_set_mmx2;
	}

	return VISUAL_OK;
}

/**
 * Allocates @a nbytes of memory initialized to 0.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 * 
 * @return On success, a pointer to a new allocated memory initialized
 * to 0 of size @a nbytes, on failure, program is aborted. 
 */
void *visual_mem_malloc0 (visual_size_t nbytes)
{
	void *buf;

	visual_log_return_val_if_fail (nbytes > 0, NULL);

	buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory"), nbytes);

		return NULL;
	}
	
	visual_mem_set (buf, 0, nbytes);

	return buf;
}

/**
 * Reallocates memory, can be used to grow a buffer.
 *
 * @param ptr Pointer that is to be reallocated.
 * @param nbytes The size of the new buffer.
 *
 * @return On success, a pointer to the new reallocated memory, on failure NULL.
 */
void *visual_mem_realloc (void *ptr, visual_size_t nbytes)
{
	return realloc (ptr, nbytes);
}

/**
 * Frees allocated memory.
 *
 * @param ptr Frees memory to which ptr points to.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MEM_NULL on failure.
 */
int visual_mem_free (void *ptr)
{
	visual_log_return_val_if_fail (ptr != NULL, -VISUAL_ERROR_MEM_NULL);

	free (ptr);

	return VISUAL_OK;
}


/* Optimize mem functions */
static void *mem_copy_c (void *dest, const void *src, size_t n)
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

static void *mem_copy_mmx (void *dest, const void *src, size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

#ifdef VISUAL_ARCH_X86
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

static void *mem_copy_mmx2 (void *dest, const void *src, size_t n)
{
	uint32_t *d = dest;
	const uint32_t *s = src;
	uint8_t *dc = dest;
	const uint8_t *sc = src;

#ifdef VISUAL_ARCH_X86
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

static void *mem_copy_sse (void *dest, const void *src, size_t n)
{

}

static void *mem_copy_sse2 (void *dest, const void *src, size_t n)
{

}

static void *mem_copy_altivec (void *dest, const void *src, size_t n)
{

}


static void *mem_set_c (void *dest, int c, size_t n)
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

static void *mem_set_mmx (void *dest, int c, size_t n)
{
	uint32_t *d = dest;
	uint8_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xff) |
		((c << 8) & 0xff00) |
		((c << 16) & 0xff0000) |
		((c << 24) & 0xff000000);
	uint8_t setflag8 = c & 0xff;

#ifdef VISUAL_ARCH_X86
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

static void *mem_set_mmx2 (void *dest, int c, size_t n)
{
	uint32_t *d = dest;
	uint8_t *dc = dest;
	uint32_t setflag32 =
		(c & 0xff) |
		((c << 8) & 0xff00) |
		((c << 16) & 0xff0000) |
		((c << 24) & 0xff000000);
	uint8_t setflag8 = c & 0xff;

#ifdef VISUAL_ARCH_X86
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

static void *mem_set_sse (void *dest, int c, size_t n)
{

}

static void *mem_set_sse2 (void *dest, int c, size_t n)
{

}

static void *mem_set_altivec (void *dest, int c, size_t n)
{

}


/**
 * @}
 */

