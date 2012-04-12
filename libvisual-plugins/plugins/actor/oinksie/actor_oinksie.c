/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_oinksie.c,v 1.36 2006/01/27 20:19:17 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include "oinksie.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info(void);

typedef struct {
	OinksiePrivate			 priv1;
	OinksiePrivate			 priv2;

	int				 color_mode;

	int				 depth;
	uint8_t				*tbuf1;
	uint8_t				*tbuf2;
	uint8_t				*buf1;
	uint8_t				*buf2;

	VisVideoCustomCompositeFunc	 currentcomp;
} OinksiePrivContainer;

static int composite_blend1_32_c (VisVideo *dest, VisVideo *src);
static int composite_blend2_32_c (VisVideo *dest, VisVideo *src);
static int composite_blend3_32_c (VisVideo *dest, VisVideo *src);
static int composite_blend4_32_c (VisVideo *dest, VisVideo *src);
static int composite_blend5_32_c (VisVideo *dest, VisVideo *src);

static int act_oinksie_init (VisPluginData *plugin);
static int act_oinksie_cleanup (VisPluginData *plugin);
static int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height);
static int act_oinksie_resize (VisPluginData *plugin, int width, int height);
static int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_oinksie_palette (VisPluginData *plugin);
static int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_oinksie_requisition,
		.palette = act_oinksie_palette,
		.render = act_oinksie_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "oinksie",
		.name = "oinksie plugin",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = N_("Libvisual Oinksie visual plugin"),
		.help = N_("This is the libvisual plugin for the Oinksie visual"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = act_oinksie_init,
		.cleanup = act_oinksie_cleanup,
		.events = act_oinksie_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	return &info;
}

static int act_oinksie_init (VisPluginData *plugin)
{
	OinksiePrivContainer *priv;
	VisRandomContext *rcontext;
        VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("color mode", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("acid palette", 0),
		VISUAL_PARAM_LIST_END
	};

/*
	static VisParamEntry cmodeparamchoices[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Fair blended", 0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Turbelent temperature", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Acid summer", 2),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Perfect match", 3),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Sanity edge", 4),
		VISUAL_PARAM_LIST_END
	};
*/

	/* FIXME: add UI to access the acid palette parameter */

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (OinksiePrivContainer, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many (paramcontainer, params);

	priv->priv1.pal_cur = visual_palette_new (256);
	priv->priv1.pal_old = visual_palette_new (256);

	priv->priv2.pal_cur = visual_palette_new (256);
	priv->priv2.pal_old = visual_palette_new (256);

	rcontext = visual_plugin_get_random_context (plugin);
	priv->priv1.rcontext = rcontext;
	priv->priv2.rcontext = rcontext;

	oinksie_init (&priv->priv1, 64, 64);
	oinksie_init (&priv->priv2, 64, 64);

	return 0;
}

static int act_oinksie_cleanup (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	oinksie_quit (&priv->priv1);
	oinksie_quit (&priv->priv2);

	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		if (priv->buf1)
			visual_mem_free (priv->buf1);

		if (priv->buf2)
			visual_mem_free (priv->buf2);

		if (priv->tbuf1)
			visual_mem_free (priv->tbuf1);

		if (priv->tbuf2)
			visual_mem_free (priv->tbuf2);
	}

	visual_palette_free (priv->priv1.pal_cur);
	visual_palette_free (priv->priv1.pal_old);

	visual_palette_free (priv->priv2.pal_cur);
	visual_palette_free (priv->priv2.pal_old);

	visual_mem_free (priv);

	return 0;
}

static int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

static int act_oinksie_resize (VisPluginData *plugin, int width, int height)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	oinksie_size_set (&priv->priv1, width, height);
	oinksie_size_set (&priv->priv2, width, height);

	return 0;
}

static int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_oinksie_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "color mode")) {
					priv->color_mode = visual_param_entry_get_integer (param);

					if (priv->color_mode == 0)
						priv->currentcomp = composite_blend1_32_c;
					else if (priv->color_mode == 1)
						priv->currentcomp = composite_blend2_32_c;
					else if (priv->color_mode == 2)
						priv->currentcomp = composite_blend3_32_c;
					else if (priv->color_mode == 3)
						priv->currentcomp = composite_blend4_32_c;
					else if (priv->color_mode == 4)
						priv->currentcomp = composite_blend5_32_c;
					else
						priv->currentcomp = composite_blend2_32_c;
				} else if (visual_param_entry_is (param, "acid palette")) {
					priv->priv1.config.acidpalette = visual_param_entry_get_integer (param);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *act_oinksie_palette (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisPalette *pal;

	pal = oinksie_palette_get (&priv->priv1);

	return pal;
}

static int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer			 pcmbuf1;
	VisBuffer			 pcmbuf2;
	VisBuffer			 pcmmix;
	VisBuffer			 spmbuf;

	/* Left audio */
	visual_buffer_set_data_pair (&pcmbuf1, priv->priv1.audio.pcm[0], sizeof (float) * 4096);
	visual_audio_get_sample (audio, &pcmbuf1, VISUAL_AUDIO_CHANNEL_LEFT);

	visual_buffer_set_data_pair (&spmbuf, &priv->priv1.audio.freq[0], sizeof (float) * 256);
	visual_audio_get_spectrum_for_sample (&spmbuf, &pcmbuf1, FALSE);

	/* Right audio */
	visual_buffer_set_data_pair (&pcmbuf2, priv->priv1.audio.pcm[1], sizeof (float) * 4096);
	visual_audio_get_sample (audio, &pcmbuf2, VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_buffer_set_data_pair (&spmbuf, priv->priv1.audio.freq[1], sizeof (float) * 256);
	visual_audio_get_spectrum_for_sample (&spmbuf, &pcmbuf2, FALSE);

	/* Mix channels */
	visual_buffer_set_data_pair (&pcmmix, priv->priv1.audio.pcm[2], sizeof (float) * 4096);
	visual_audio_sample_buffer_mix_many (&pcmmix, TRUE, 2, &pcmbuf1, &pcmbuf2, 1.0, 1.0);

	visual_buffer_set_data_pair (&spmbuf, priv->priv1.audio.freqsmall, sizeof (float) * 4);
	visual_audio_get_spectrum_for_sample (&spmbuf, &pcmmix, FALSE);

	/* Duplicate for second oinksie instance */
	visual_mem_copy (&priv->priv2.audio.pcm, &priv->priv1.audio.pcm, sizeof (float) * 4096 * 3);
	visual_mem_copy (&priv->priv2.audio.freq, &priv->priv1.audio.freq, sizeof (float) * 256 * 2);
	visual_mem_copy (&priv->priv2.audio.freqsmall, &priv->priv1.audio.freqsmall, sizeof (float) * 4);

	/* Audio energy */
	priv->priv1.audio.energy = audio->energy;
	priv->priv2.audio.energy = audio->energy;

	/* Let's get rendering */
	if (priv->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		oinksie_sample (&priv->priv1);

		/* FIXME this is not pitch safe, will screw up region buffers.
		 * pass on the VisVideo to the plugin, and be pitch safe.
		 */
		priv->priv1.drawbuf = visual_video_get_pixels (video);
		oinksie_render (&priv->priv1);
	} else {
		VisVideo *vid1;
		VisVideo *vid2;

		vid1 = visual_video_new ();
		vid2 = visual_video_new ();

		oinksie_sample (&priv->priv1);
		oinksie_sample (&priv->priv2);

		priv->priv1.drawbuf = priv->buf1;
		priv->priv2.drawbuf = priv->buf2;

		oinksie_render (&priv->priv1);
		oinksie_render (&priv->priv2);

		visual_video_set_depth (vid1, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_dimension (vid1, video->width, video->height);
		visual_video_set_buffer (vid1, priv->buf1);
		visual_video_set_palette (vid1, oinksie_palette_get (&priv->priv1));

		visual_video_blit_overlay (video, vid1, 0, 0, FALSE);

		visual_video_set_depth (vid2, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_dimension (vid2, video->width, video->height);
		visual_video_set_buffer (vid2, priv->buf2);
		visual_video_set_palette (vid2, oinksie_palette_get (&priv->priv2));

		visual_video_composite_set_type (vid2, VISUAL_VIDEO_COMPOSITE_TYPE_CUSTOM);
		visual_video_composite_set_function (vid2, priv->currentcomp);

		visual_video_blit_overlay (video, vid2, 0, 0, TRUE);

		visual_object_unref (VISUAL_OBJECT (vid1));
		visual_object_unref (VISUAL_OBJECT (vid2));
	}

	return 0;
}

static int composite_blend1_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((alpha * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((alpha * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((alpha * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int composite_blend2_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((*destbuf * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((alpha * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((0 * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int composite_blend3_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((0 * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((alpha * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((*destbuf * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int composite_blend4_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((*destbuf * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((alpha * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((*srcbuf * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int composite_blend5_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);

	for (i = 0; i < src->height; i++) {
		for (j = 0; j < src->width; j++) {
			*destbuf = ((*destbuf * (*destbuf - *srcbuf) >> 8) + *srcbuf);
			*(destbuf + 1) = ((*srcbuf * (*(destbuf + 1) - *(srcbuf + 1)) >> 8) + *(srcbuf + 1));
			*(destbuf + 2) = ((*destbuf * (*(destbuf + 2) - *(srcbuf + 2)) >> 8) + *(srcbuf + 2));

			destbuf += 4;
			srcbuf += 4;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

/* FIXME make a color mode out of this one as well (yeah I was fooling around with mmx) */
#if 0
static int alpha_blend3_32_mmx (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint32_t ialpha = (alpha * 255);
	int i;

	/* Reset some regs */
	__asm __volatile
		("\n\t emms"
		 "\n\t pxor %%mm6, %%mm6"
		 "\n\t pxor %%mm7, %%mm7"
		 ::: "mm6", "mm7");

	for (i = 0; i < size; i++) {
		__asm __volatile
			("\n\t movd %[spix1], %%mm0"
			 "\n\t movd %[spix2], %%mm1"
			 "\n\t movq %%mm0, %%mm3"       // [ 2 pixels ]
			 "\n\t movd %[alpha], %%mm2"    // [ alpha ]
			 "\n\t psllq $24, %%mm3"
			 "\n\t movq %%mm0, %%mm4"
			 "\n\t psrld $24, %%mm3"
			 "\n\t psrld $24, %%mm4"
			 "\n\t psllq $32, %%mm2"
			 "\n\t psllq $16, %%mm3"
			 "\n\t por %%mm4, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm0"  /* interleaving source 1 */
			 "\n\t por %%mm3, %%mm2"
			 "\n\t punpcklbw %%mm6, %%mm1"  /* interleaving source 2 */
			 "\n\t paddsw %%mm7, %%mm2"
			 "\n\t psubsw %%mm1, %%mm0"     /* (src - dest) part */
			 "\n\t pxor %%mm3, %%mm3"
			 "\n\t pmullw %%mm2, %%mm0"     /* alpha * (src - dest) */
			 "\n\t pxor %%mm4, %%mm4"
			 "\n\t psrlw $8, %%mm0"         /* / 256 */
			 "\n\t pxor %%mm2, %%mm2"
			 "\n\t paddb %%mm1, %%mm0"      /* + dest */
			 "\n\t packuswb %%mm0, %%mm0"
			 "\n\t pxor %%mm1, %%mm1"
			 "\n\t movd %%mm0, %[dest]"
			 : [dest] "=m" (*dest)
			 : [spix1] "m" (*src1)
			 , [spix2] "m" (*src2)
			 , [alpha] "m" (ialpha)
			 : "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7");

		dest += 4;
		src1 += 4;
		src2 += 4;
	}

	__asm __volatile
		("\n\t emms");

	return 0;
}

#endif
