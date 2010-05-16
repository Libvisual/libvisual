/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: morph_alphablend.c,v 1.19 2006/01/27 20:19:18 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

static inline int alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha);

/* alpha blenders */
static inline int alpha_blend_8_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);

static inline int alpha_blend_8_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_16_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_24_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_32_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);

int lv_morph_alpha_init (VisPluginData *plugin);
int lv_morph_alpha_cleanup (VisPluginData *plugin);
int lv_morph_alpha_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisMorphPlugin morph[] = {{
		.apply = lv_morph_alpha_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT  |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "alphablend",
		.name = "alphablend morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An alphablend morph plugin",
		.help = "This morph plugin morphs between two video sources using the alphablend method",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_alpha_init,
		.cleanup = lv_morph_alpha_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_morph_alpha_init (VisPluginData *plugin)
{
	return 0;
}

int lv_morph_alpha_cleanup (VisPluginData *plugin)
{
	return 0;
}

int lv_morph_alpha_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	visual_log_return_val_if_fail (dest != NULL, -1);
	visual_log_return_val_if_fail (src1 != NULL, -1);
	visual_log_return_val_if_fail (src2 != NULL, -1);

	alpha_blend_buffer (visual_video_get_pixels (dest),
			visual_video_get_pixels (src1),
			visual_video_get_pixels (src2),
			visual_video_get_size (dest), dest->depth, rate);

	return 0;
}

static inline int alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha)
{

	if (visual_cpu_get_mmx ()) {
		if (depth == VISUAL_VIDEO_DEPTH_8BIT)
			return alpha_blend_8_mmx (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_16BIT)
			return alpha_blend_16_mmx (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_24BIT)
			return alpha_blend_24_mmx (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_32BIT)
			return alpha_blend_32_mmx (dest, src1, src2, size, alpha);
	} else {
		if (depth == VISUAL_VIDEO_DEPTH_8BIT)
			return alpha_blend_8_c (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_16BIT)
			return alpha_blend_16_c (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_24BIT)
			return alpha_blend_24_c (dest, src1, src2, size, alpha);

		if (depth == VISUAL_VIDEO_DEPTH_32BIT)
			return alpha_blend_32_c (dest, src1, src2, size, alpha);

	}

	return -1;
}

/* FIXME TODO blends:   c       sse     mmx
 * 8                    x		x
 * 16                   x
 * 24                   x
 * 32                   x		x
 */

static inline int alpha_blend_8_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	_color16 *destr = (_color16 *) dest;
	_color16 *src1r = (_color16 *) src1;
	_color16 *src2r = (_color16 *) src2;
	int i;

	for (i = 0; i < size / 2; i++) {
		destr[i].r = ((ialpha * (src2r[i].r - src1r[i].r)) / 255 + src1r[i].r);
		destr[i].g = ((ialpha * (src2r[i].g - src1r[i].g)) / 255 + src1r[i].g);
		destr[i].b = ((ialpha * (src2r[i].b - src1r[i].b)) / 255 + src1r[i].b);
	}

	return 0;
}

static inline int alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_8_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint32_t aalpha = (alpha * 255);
	uint32_t ialpha = aalpha;
	int i;

	ialpha |= ialpha << 16;

#ifdef VISUAL_ARCH_X86
	__asm __volatile
		("\n\t pxor %%mm6, %%mm6"
		 ::);

	for (i = size; i > 4; i -= 4) {
		__asm __volatile
			("\n\t movd %[alpha], %%mm3"
			 "\n\t movd %[src2], %%mm0"
			 "\n\t psllq $32, %%mm3"
			 "\n\t movd %[alpha], %%mm2"
			 "\n\t movd %[src1], %%mm1"
			 "\n\t por %%mm3, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm0"  /* interleaving dest */
			 "\n\t punpcklbw %%mm6, %%mm1"  /* interleaving source */
			 "\n\t psubsw %%mm1, %%mm0"     /* (src - dest) part */
			 "\n\t pmullw %%mm2, %%mm0"     /* alpha * (src - dest) */
			 "\n\t psrlw $8, %%mm0"         /* / 256 */
			 "\n\t paddb %%mm1, %%mm0"      /* + dest */
			 "\n\t packuswb %%mm0, %%mm0"
			 "\n\t movd %%mm0, %[dest]"
			 : [dest] "=m" (*(dest + i))
			 : [src1] "m" (*(src1 + i))
			 , [src2] "m" (*(src2 + i))
			 , [alpha] "m" (ialpha));
	}

	while (i--)
		dest[i] = (aalpha * (src2[i] - src1[i])) / 255 + src1[i];

	__asm __volatile
		("\n\t emms");
#endif

	return 0;
}

static inline int alpha_blend_16_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	_color16 *destr = (_color16 *) dest;
	_color16 *src1r = (_color16 *) src1;
	_color16 *src2r = (_color16 *) src2;
	int i;

	for (i = 0; i < size / 2; i++) {
		destr[i].r = ((ialpha * (src2r[i].r - src1r[i].r)) / 255 + src1r[i].r);
		destr[i].g = ((ialpha * (src2r[i].g - src1r[i].g)) / 255 + src1r[i].g);
		destr[i].b = ((ialpha * (src2r[i].b - src1r[i].b)) / 255 + src1r[i].b);
	}

	return 0;
}

static inline int alpha_blend_24_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_32_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint32_t ialpha = (alpha * 255);
	int i;

#ifdef VISUAL_ARCH_X86
	__asm __volatile
		("\n\t pxor %%mm6, %%mm6"
		 ::);

	for (i = 0; i < size; i += 4) {
		__asm __volatile
			("\n\t movd %[src2], %%mm0"
			 "\n\t movd %[alpha], %%mm2"
			 "\n\t movd %[src1], %%mm1"
			 "\n\t psllq $32, %%mm2"
			 "\n\t movd %[alpha], %%mm3"
			 "\n\t movd %[alpha], %%mm4"
			 "\n\t psllq $16, %%mm3"
			 "\n\t por %%mm4, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm0"  /* interleaving dest */
			 "\n\t por %%mm3, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm1"  /* interleaving source */
			 "\n\t psubsw %%mm1, %%mm0"     /* (src - dest) part */
			 "\n\t pmullw %%mm2, %%mm0"     /* alpha * (src - dest) */
			 "\n\t psrlw $8, %%mm0"         /* / 256 */
			 "\n\t paddb %%mm1, %%mm0"      /* + dest */
			 "\n\t packuswb %%mm0, %%mm0"
			 "\n\t movd %%mm0, %[dest]"
			 : [dest] "=m" (*(dest + i))
			 : [src1] "m" (*(src1 + i))
			 , [src2] "m" (*(src2 + i))
			 , [alpha] "m" (ialpha));
	}

	__asm __volatile
		("\n\t emms");
#endif

	return 0;
}

