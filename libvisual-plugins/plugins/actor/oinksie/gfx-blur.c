/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-blur.c,v 1.12 2006/02/05 18:47:26 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "common.h"
#include "gfx-blur.h"

#include "gfx-misc.h"
#include "screen.h"

void _oink_gfx_blur_fade (OinksiePrivate *priv, uint8_t *buf, int fade)
{
	int i;
	uint8_t valuetab[256];

	if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		int fadeflag = fade | fade << 8 | fade << 16 | fade << 24;

		/* Prepare substraction register */
		__asm __volatile
			("\n\t movd %[fade], %%mm3"
			 "\n\t movd %[fade], %%mm4"
			 "\n\t psllq $32, %%mm3"
			 "\n\t por %%mm4, %%mm3"
			 :: [fade] "m" (fadeflag));


		for (i = 0; i < priv->screen_size; i += 8) {
			__asm __volatile
				("\n\t movq %[buf], %%mm0"
				 "\n\t psubsb %%mm3, %%mm0"
				 "\n\t movq %%mm0, %[buf]"
				 :: [buf] "m" (*buf));
		}

		__asm __volatile
			("\n\t emms");
#endif
	} else {
		for (i = 0; i < 256; i++)
			valuetab[i] = (i - fade) > 0 ? i - fade : 0;

		for (i = 0; i < priv->screen_size; i++)
		{
			*buf = valuetab[*buf]; buf++;
		}
	}
}

/* FIXME also make mmx versions for these, let's rock this babe! */
void _oink_gfx_blur_simple (OinksiePrivate *priv, uint8_t *buf)
{
	int i;

	for (i = 0; i < (priv->screen_size - priv->screen_width - 1); i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2] + buf[i + priv->screen_width]
			  + buf[i + 1 + (priv->screen_width)]) >> 2;
	}

	for (i = (priv->screen_size - priv->screen_width - 1); i < priv->screen_size - 2; i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2]) >> 1;
	}
}

void _oink_gfx_blur_middle (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;

	if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t pxor %%mm6, %%mm6"
			 ::);

		for (i = 0; i < scrsh; i += 4) {
			__asm __volatile
				("\n\t movd %[buf], %%mm0"
				 "\n\t movd %[add1], %%mm1"
				 "\n\t punpcklbw %%mm6, %%mm0"
				 "\n\t movd %[add2], %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm1"
				 "\n\t movd %[add3], %%mm3"
				 "\n\t punpcklbw %%mm6, %%mm2"
				 "\n\t paddw %%mm1, %%mm0"
				 "\n\t punpcklbw %%mm6, %%mm3"
				 "\n\t paddw %%mm2, %%mm0"
				 "\n\t paddw %%mm3, %%mm0"
				 "\n\t psrlw $2, %%mm0"
				 "\n\t packuswb %%mm6, %%mm0"
				 "\n\t movd %%mm0, %[buf]"
				 :: [buf] "m" (*(buf + i))
				 , [add1] "m" (*(buf + i + priv->screen_width))
				 , [add2] "m" (*(buf + i + priv->screen_width + 1))
				 , [add3] "m" (*(buf + i + priv->screen_width - 1)));
		}

		for (i = priv->screen_size - 1; i > scrsh; i -= 4) {
			__asm __volatile
				("\n\t movd %[buf], %%mm0"
				 "\n\t movd %[add1], %%mm1"
				 "\n\t punpcklbw %%mm6, %%mm0"
				 "\n\t movd %[add2], %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm1"
				 "\n\t movd %[add3], %%mm3"
				 "\n\t punpcklbw %%mm6, %%mm2"
				 "\n\t paddw %%mm1, %%mm0"
				 "\n\t punpcklbw %%mm6, %%mm3"
				 "\n\t paddw %%mm2, %%mm0"
				 "\n\t paddw %%mm3, %%mm0"
				 "\n\t psrlw $2, %%mm0"
				 "\n\t packuswb %%mm6, %%mm0"
				 "\n\t movd %%mm0, %[buf]"
				 :: [buf] "m" (*(buf + i))
				 , [add1] "m" (*(buf + i - priv->screen_width))
				 , [add2] "m" (*(buf + i - priv->screen_width + 1))
				 , [add3] "m" (*(buf + i - priv->screen_width - 1)));
		}

		__asm __volatile
			("\n\t emms");
#endif
	} else {
		for (i = 0; i < scrsh; i++)
		{
			*(buf + i) = (*(buf + i)
					+ *(buf + i + priv->screen_width)
					+ *(buf + i + priv->screen_width + 1)
					+ *(buf + i + priv->screen_width - 1)) >> 2;
		}

		for (i = priv->screen_size - 1; i > scrsh; i--)
		{
			*(buf + i) = (*(buf + i)
					+ *(buf + i - priv->screen_width)
					+ *(buf + i - priv->screen_width + 1)
					+ *(buf + i - priv->screen_width - 1)) >> 2;
		}
	}
}

void _oink_gfx_blur_midstrange (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;

	if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		__asm __volatile
			("\n\t pxor %%mm6, %%mm6"
			 ::);

		for (i = scrsh; i > 0; i -= 4) {
			__asm __volatile
				("\n\t movd %[buf], %%mm0"
				 "\n\t movd %[add1], %%mm1"
				 "\n\t punpcklbw %%mm6, %%mm0"
				 "\n\t movd %[add2], %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm1"
				 "\n\t movd %[add3], %%mm3"
				 "\n\t punpcklbw %%mm6, %%mm2"
				 "\n\t paddw %%mm1, %%mm0"
				 "\n\t punpcklbw %%mm6, %%mm3"
				 "\n\t paddw %%mm2, %%mm0"
				 "\n\t paddw %%mm3, %%mm0"
				 "\n\t psrlw $2, %%mm0"
				 "\n\t packuswb %%mm6, %%mm0"
				 "\n\t movd %%mm0, %[buf]"
				 :: [buf] "m" (*(buf + i))
				 , [add1] "m" (*(buf + i + priv->screen_width))
				 , [add2] "m" (*(buf + i + priv->screen_width + 1))
				 , [add3] "m" (*(buf + i + priv->screen_width - 1)));
		}

		for (i = scrsh; i < priv->screen_size - 2; i += 4) {
			__asm __volatile
				("\n\t movd %[buf], %%mm0"
				 "\n\t movd %[add1], %%mm1"
				 "\n\t punpcklbw %%mm6, %%mm0"
				 "\n\t movd %[add2], %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm1"
				 "\n\t movd %[add3], %%mm3"
				 "\n\t punpcklbw %%mm6, %%mm2"
				 "\n\t paddw %%mm1, %%mm0"
				 "\n\t punpcklbw %%mm6, %%mm3"
				 "\n\t paddw %%mm2, %%mm0"
				 "\n\t paddw %%mm3, %%mm0"
				 "\n\t psrlw $2, %%mm0"
				 "\n\t packuswb %%mm6, %%mm0"
				 "\n\t movd %%mm0, %[buf]"
				 :: [buf] "m" (*(buf + i))
				 , [add1] "m" (*(buf + i - priv->screen_width))
				 , [add2] "m" (*(buf + i - priv->screen_width + 1))
				 , [add3] "m" (*(buf + i - priv->screen_width - 1)));
		}

		__asm __volatile
			("\n\t emms");
#endif
	} else {

		for (i = scrsh; i > 0; i--)
		{
			buf[i] = (buf[i] 
					+ buf[i + priv->screen_width]
					+ buf[i + priv->screen_width + 1]
					+ buf[i + priv->screen_width - 1]) >> 2;
		}

		for (i = scrsh; i < priv->screen_size - 2; i++)
		{
			buf[i] = (buf[i]
					+ buf[i - priv->screen_width]
					+ buf[i - priv->screen_width + 1]
					+ buf[i - priv->screen_width - 1]) >> 2;
		}
	}
}

