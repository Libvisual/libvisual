/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include "oinksie.h"

typedef struct {
	OinksiePrivate priv1;
	OinksiePrivate priv2;

	int color_mode;
	
	int depth;
	uint8_t *tbuf1;
	uint8_t *tbuf2;
	uint8_t *buf1;
	uint8_t *buf2;
} OinksiePrivContainer;

static int alpha_blend1_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static int alpha_blend2_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static int alpha_blend3_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static int alpha_blend4_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static int alpha_blend5_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);


int act_oinksie_init (VisPluginData *plugin);
int act_oinksie_cleanup (VisPluginData *plugin);
int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height);
int act_oinksie_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_oinksie_palette (VisPluginData *plugin);
int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = act_oinksie_requisition,
		.palette = act_oinksie_palette,
		.render = act_oinksie_render,
		.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT,
	}};

	static VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "oinksie",
		.name = "oinksie plugin",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = N_("Libvisual Oinksie visual plugin"),
		.help = N_("This is the libvisual plugin for the Oinksie visual"),

		.init = act_oinksie_init,
		.cleanup = act_oinksie_cleanup,
		.events = act_oinksie_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_oinksie_init (VisPluginData *plugin)
{
	OinksiePrivContainer *priv;
	VisRandomContext *rcontext;
        VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("color mode", 1),
		VISUAL_PARAM_LIST_END
	};

	static VisParamEntry cmodeparamchoices[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Fair blended", 0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Turbelent temperature", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Acid summer", 2),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Perfect match", 3),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Sanity edge", 4),
		VISUAL_PARAM_LIST_END
	};
	
	VisUIWidget *hbox;
	VisUIWidget *label;
	VisUIWidget *popup;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (OinksiePrivContainer, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

        visual_param_container_add_many (paramcontainer, params);

	hbox = visual_ui_box_new (VISUAL_ORIENT_TYPE_HORIZONTAL);
	
	label = visual_ui_label_new (_("Color mode:"), FALSE);

	popup = visual_ui_popup_new ();
	visual_ui_widget_set_tooltip (popup, _("The coloring method (only works when the plugin is in 32 bits mode)"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (popup), visual_param_container_get (paramcontainer, "color mode"));
	visual_ui_choice_add_many (VISUAL_UI_CHOICE (popup), cmodeparamchoices);

	visual_ui_box_pack (VISUAL_UI_BOX (hbox), label);
	visual_ui_box_pack (VISUAL_UI_BOX (hbox), popup);

        visual_plugin_set_userinterface (plugin, hbox);
	
	visual_palette_allocate_colors (&priv->priv1.pal_cur, 256);
	visual_palette_allocate_colors (&priv->priv1.pal_old, 256);

	visual_palette_allocate_colors (&priv->priv2.pal_cur, 256);
	visual_palette_allocate_colors (&priv->priv2.pal_old, 256);

	rcontext = visual_plugin_get_random_context (plugin);
	priv->priv1.rcontext = rcontext;
	priv->priv2.rcontext = rcontext;

	oinksie_init (&priv->priv1, 64, 64);
	oinksie_init (&priv->priv2, 64, 64);
	
	return 0;
}

int act_oinksie_cleanup (VisPluginData *plugin)
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

	visual_palette_free_colors (&priv->priv1.pal_cur);
	visual_palette_free_colors (&priv->priv1.pal_old);

	visual_palette_free_colors (&priv->priv2.pal_cur);
	visual_palette_free_colors (&priv->priv2.pal_old);

	visual_mem_free (priv);

	return 0;
}

int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height)
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

int act_oinksie_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	
	visual_video_set_dimension (video, width, height);

	oinksie_size_set (&priv->priv1, video->width, video->height);
	oinksie_size_set (&priv->priv2, video->width, video->height);

	priv->depth = video->depth;
	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		if (priv->buf1)
			visual_mem_free (priv->buf1);

		if (priv->buf2)
			visual_mem_free (priv->buf2);

		if (priv->tbuf1)
			visual_mem_free (priv->tbuf1);

		if (priv->tbuf2)
			visual_mem_free (priv->tbuf2);

		priv->buf1 = visual_mem_malloc0 (video->size);
		priv->buf2 = visual_mem_malloc0 (video->size);
		priv->tbuf1 = visual_mem_malloc0 (video->size);
		priv->tbuf2 = visual_mem_malloc0 (video->size);
	}

	return 0;
}

int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_oinksie_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				if (visual_param_entry_is (param, "color mode"))
					priv->color_mode = visual_param_entry_get_integer (param);

				break;
								
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_oinksie_palette (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisPalette *pal;
	
	pal = oinksie_palette_get (&priv->priv1);

	return pal;
}

int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	OinksiePrivContainer *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisVideo transvid;
	int pitch;

	visual_mem_set (&transvid, 0, sizeof (VisVideo));

	visual_mem_copy (&priv->priv1.audio.freq, &audio->freq, sizeof (short) * 3 * 256);
	visual_mem_copy (&priv->priv2.audio.freq, &audio->freq, sizeof (short) * 3 * 256);

	visual_mem_copy (&priv->priv1.audio.pcm, &audio->pcm, sizeof (short) * 3 * 512);
	visual_mem_copy (&priv->priv2.audio.pcm, &audio->pcm, sizeof (short) * 3 * 512);

	priv->priv1.audio.energy = audio->energy;
	priv->priv2.audio.energy = audio->energy;

	if (priv->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		oinksie_sample (&priv->priv1);

		priv->priv1.drawbuf = video->pixels;
		oinksie_render (&priv->priv1);
	} else {
		oinksie_sample (&priv->priv1);
		oinksie_sample (&priv->priv2);
	
		priv->priv1.drawbuf = priv->buf1;
		priv->priv2.drawbuf = priv->buf2;

		oinksie_render (&priv->priv1);
		oinksie_render (&priv->priv2);

		visual_video_set_depth (&transvid, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_dimension (&transvid, video->width, video->height);
		
		pitch = video->width * video->bpp;

		visual_video_set_buffer (&transvid, priv->buf1);
		visual_video_depth_transform_to_buffer (priv->tbuf1, &transvid,
				oinksie_palette_get (&priv->priv1), priv->depth, video->pitch);

		visual_video_set_buffer (&transvid, priv->buf2);
		visual_video_depth_transform_to_buffer (priv->tbuf2, &transvid,
				oinksie_palette_get (&priv->priv2), priv->depth, video->pitch);

		switch (priv->color_mode) {
			case 0:
				alpha_blend1_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;
				
			case 1:
				alpha_blend2_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;

			case 2:
				alpha_blend3_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;
				
			case 3:
				alpha_blend4_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;
			case 4:
				alpha_blend5_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;

			default:
				alpha_blend2_32_c (video->pixels, priv->tbuf1, priv->tbuf2, transvid.size, 0.5);
			
				break;
		}
		
	}
	
	return 0;
}

static int alpha_blend1_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;
	
	for (i = 0; i < size; i++) {
		*dest = ((ialpha * (*src1 - *src2) >> 8) + *src2);
		*(dest + 1) = ((ialpha * (*(src1 + 1) - *(src2 + 1)) >> 8) + *(src2 + 1));
		*(dest + 2) = ((ialpha * (*(src1 + 2) - *(src2 + 2)) >> 8) + *(src2 + 2));

		dest += 4;
		src1 += 4;
		src2 += 4;
	}
	
	return 0;
}

static int alpha_blend2_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		*dest = ((*src1 * (*src1 - *src2) >> 8) + *src2);
		*(dest + 1) = ((ialpha * (*(src1 + 1) - *(src2 + 1)) >> 8) + *(src2 + 1));
		*(dest + 2) = ((0 * (*(src1 + 2) - *(src2 + 2)) >> 8) + *(src2 + 2));

		dest += 4;
		src1 += 4;
		src2 += 4;
	}

	return 0;
}

static int alpha_blend3_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		*dest = ((0 * (*src1 - *src2) >> 8) + *src2);
		*(dest + 1) = ((ialpha * (*(src1 + 1) - *(src2 + 1)) >> 8) + *(src2 + 1));
		*(dest + 2) = ((*src1 * (*(src1 + 2) - *(src2 + 2)) >> 8) + *(src2 + 2));

		dest += 4;
		src1 += 4;
		src2 += 4;
	}

	return 0;
}

static int alpha_blend4_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		*dest = ((*src1 * (*src1 - *src2) >> 8) + *src2);
		*(dest + 1) = ((ialpha * (*(src1 + 1) - *(src2 + 1)) >> 8) + *(src2 + 1));
		*(dest + 2) = ((*src2 * (*(src1 + 2) - *(src2 + 2)) >> 8) + *(src2 + 2));

		dest += 4;
		src1 += 4;
		src2 += 4;
	}

	return 0;
}

static int alpha_blend5_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		*dest = ((*src1 * (*src1 - *src2) >> 8) + *src2);
		*(dest + 1) = ((*src2 * (*(src1 + 1) - *(src2 + 1)) >> 8) + *(src2 + 1));
		*(dest + 2) = ((*src1 * (*(src1 + 2) - *(src2 + 2)) >> 8) + *(src2 + 2));

		dest += 4;
		src1 += 4;
		src2 += 4;
	}

	return 0;
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
