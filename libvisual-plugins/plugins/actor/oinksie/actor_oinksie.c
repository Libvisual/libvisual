/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"
#include "oinksie.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
	OinksiePrivate       priv1;
	OinksiePrivate       priv2;

	int                  color_mode;

	VisVideoComposeFunc	 currentcomp;
} OinksiePrivContainer;

static int         act_oinksie_init        (VisPluginData *plugin);
static void        act_oinksie_cleanup     (VisPluginData *plugin);
static void        act_oinksie_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_oinksie_resize      (VisPluginData *plugin, int width, int height);
static int         act_oinksie_events      (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_oinksie_palette     (VisPluginData *plugin);
static void        act_oinksie_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void act_oinksie_set_color_mode (OinksiePrivContainer *self, int mode);
static void act_oinksie_set_acid_palette (OinksiePrivContainer *self, int palette);

static void compose_blend1_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend2_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend3_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend4_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend5_32_c (VisVideo *dest, VisVideo *src);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_oinksie_requisition,
		.palette     = act_oinksie_palette,
		.render      = act_oinksie_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "oinksie",
		.name     = "oinksie plugin",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>",
		.version  = "0.1",
		.about    = N_("Libvisual Oinksie visual plugin"),
		.help     = N_("This is the libvisual plugin for the Oinksie visual"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = act_oinksie_init,
		.cleanup  = act_oinksie_cleanup,
		.events   = act_oinksie_events,
		.plugin   = &actor
	};

	return &info;
}

static int act_oinksie_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_integer ("color_mode", N_("Color mode"),
                                                          1,
                                                          NULL),
                                visual_param_new_integer ("acid_palette", N_("Acid palette"),
                                                          0,
                                                          NULL),
                                NULL);

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

	OinksiePrivContainer *priv = visual_mem_new0 (OinksiePrivContainer, 1);
	visual_plugin_set_private (plugin, priv);

	priv->priv1.pal_cur = visual_palette_new (256);
	priv->priv1.pal_old = visual_palette_new (256);

	priv->priv2.pal_cur = visual_palette_new (256);
	priv->priv2.pal_old = visual_palette_new (256);

	VisRandomContext *rcontext = visual_plugin_get_random_context (plugin);
	priv->priv1.rcontext = rcontext;
	priv->priv2.rcontext = rcontext;

	oinksie_init (&priv->priv1, 64, 64);
	oinksie_init (&priv->priv2, 64, 64);

	act_oinksie_set_color_mode (priv, 1);
	act_oinksie_set_acid_palette (priv, 0);

	return TRUE;
}

static void act_oinksie_cleanup (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = visual_plugin_get_private (plugin);

	oinksie_quit (&priv->priv1);
	oinksie_quit (&priv->priv2);

	visual_palette_free (priv->priv1.pal_cur);
	visual_palette_free (priv->priv1.pal_old);

	visual_palette_free (priv->priv2.pal_cur);
	visual_palette_free (priv->priv2.pal_old);

	visual_mem_free (priv);
}

static void act_oinksie_requisition (VisPluginData *plugin, int *width, int *height)
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
}

static void act_oinksie_resize (VisPluginData *plugin, int width, int height)
{
	OinksiePrivContainer *priv = visual_plugin_get_private (plugin);

	oinksie_size_set (&priv->priv1, width, height);
	oinksie_size_set (&priv->priv2, width, height);
}

static int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events)
{
	OinksiePrivContainer *priv = visual_plugin_get_private (plugin);
	VisEvent ev;
	VisParam *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_oinksie_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_has_name (param, "color mode")) {
					act_oinksie_set_color_mode (priv, visual_param_get_value_integer (param));
				} else if (visual_param_has_name (param, "acid palette")) {
					act_oinksie_set_acid_palette (priv, visual_param_get_value_integer (param));
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_oinksie_palette (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = visual_plugin_get_private (plugin);
	VisPalette *pal;

	pal = oinksie_palette_get (&priv->priv1);

	return pal;
}

static void act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	OinksiePrivContainer *priv = visual_plugin_get_private (plugin);

	/* Left audio */
	VisBuffer *pcmbuf1 = visual_buffer_new_wrap_data (priv->priv1.audio.pcm[0], sizeof (priv->priv1.audio.pcm[0]), FALSE);
	visual_audio_get_sample (audio, pcmbuf1, VISUAL_AUDIO_CHANNEL_LEFT);

	VisBuffer *spmbuf1 = visual_buffer_new_wrap_data (priv->priv1.audio.freq[0], sizeof (priv->priv1.audio.freq[0]), FALSE);
	visual_audio_get_spectrum_for_sample (spmbuf1, pcmbuf1, FALSE);
	visual_buffer_unref(pcmbuf1);
	visual_buffer_unref(spmbuf1);

	/* Right audio */
	VisBuffer *pcmbuf2 = visual_buffer_new_wrap_data (priv->priv1.audio.pcm[1], sizeof (priv->priv1.audio.pcm[1]), FALSE);
	visual_audio_get_sample (audio, pcmbuf2, VISUAL_AUDIO_CHANNEL_RIGHT);

	VisBuffer *spmbuf2 = visual_buffer_new_wrap_data (priv->priv1.audio.freq[1], sizeof (priv->priv1.audio.freq[1]), FALSE);
	visual_audio_get_spectrum_for_sample (spmbuf2, pcmbuf2, FALSE);
	visual_buffer_unref (pcmbuf2);
	visual_buffer_unref (spmbuf2);

	/* Mix channels */
	VisBuffer *pcmmix = visual_buffer_new_wrap_data (priv->priv1.audio.pcm[2], sizeof (priv->priv1.audio.pcm[2]), FALSE);
	visual_audio_get_sample_mixed_simple (audio, pcmmix, 2, VISUAL_AUDIO_CHANNEL_LEFT, VISUAL_AUDIO_CHANNEL_RIGHT);

	VisBuffer *spmmix = visual_buffer_new_wrap_data (priv->priv1.audio.freqsmall, sizeof (priv->priv1.audio.freqsmall), FALSE);
	visual_audio_get_spectrum_for_sample (spmmix, pcmmix, FALSE);
	visual_buffer_unref (pcmmix);
	visual_buffer_unref (spmmix);

	/* Duplicate for second oinksie instance */
	visual_mem_copy (priv->priv2.audio.pcm, priv->priv1.audio.pcm, sizeof (priv->priv1.audio.pcm));
	visual_mem_copy (priv->priv2.audio.freq, priv->priv1.audio.freq, sizeof (priv->priv1.audio.freq));
	visual_mem_copy (priv->priv2.audio.freqsmall, priv->priv1.audio.freqsmall, sizeof (priv->priv1.audio.freqsmall));

	/* Audio energy */
	priv->priv1.audio.energy = 0 /*audio->energy*/;
	priv->priv2.audio.energy = 0 /*audio->energy*/;

	/* Let's get rendering */

	VisVideoDepth video_depth = visual_video_get_depth (video);

	if (video_depth == VISUAL_VIDEO_DEPTH_8BIT) {
		oinksie_sample (&priv->priv1);

		/* FIXME this is not pitch safe, will screw up region buffers.
		 * pass on the VisVideo to the plugin, and be pitch safe.
		 */
		priv->priv1.drawbuf = visual_video_get_pixels (video);
		oinksie_render (&priv->priv1);
	} else {
		int width  = visual_video_get_width (video);
		int height = visual_video_get_height (video);

		VisVideo *vid1 = visual_video_new_with_buffer (width, height, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_palette (vid1, oinksie_palette_get (&priv->priv1));

		VisVideo *vid2 = visual_video_new_with_buffer (width, height, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_palette (vid2, oinksie_palette_get (&priv->priv2));

		priv->priv1.drawbuf = visual_video_get_pixels (vid1);
		priv->priv2.drawbuf = visual_video_get_pixels (vid2);

		oinksie_sample (&priv->priv1);
		oinksie_sample (&priv->priv2);

		oinksie_render (&priv->priv1);
		oinksie_render (&priv->priv2);

		visual_video_blit (video, vid1, 0, 0, FALSE);

		visual_video_set_compose_type (vid2, VISUAL_VIDEO_COMPOSE_TYPE_CUSTOM);
		visual_video_set_compose_function (vid2, priv->currentcomp);
		visual_video_blit (video, vid2, 0, 0, TRUE);

		visual_video_unref (vid1);
		visual_video_unref (vid2);
	}
}

static void act_oinksie_set_color_mode (OinksiePrivContainer *self, int mode)
{
	self->color_mode = mode;

	switch (self->color_mode) {
		case 0:	 self->currentcomp = compose_blend1_32_c; break;
		case 1:	 self->currentcomp = compose_blend2_32_c; break;
		case 2:	 self->currentcomp = compose_blend3_32_c; break;
		case 3:	 self->currentcomp = compose_blend4_32_c; break;
		case 4:	 self->currentcomp = compose_blend5_32_c; break;
		default: self->currentcomp = compose_blend2_32_c; break;
	}
}

static void act_oinksie_set_acid_palette (OinksiePrivContainer *self, int palette)
{
	self->priv1.config.acidpalette = palette;
}

static void compose_blend1_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	int src_width  = visual_video_get_width	 (src);
	int src_height = visual_video_get_height (src);

	for (i = 0; i < src_height; i++) {
		for (j = 0; j < src_width; j++) {
			destbuf[0] = (alpha * (destbuf[0] - srcbuf[0]) >> 8) + srcbuf[0];
			destbuf[1] = (alpha * (destbuf[1] - srcbuf[1]) >> 8) + srcbuf[1];
			destbuf[2] = (alpha * (destbuf[2] - srcbuf[2]) >> 8) + srcbuf[2];

			destbuf += 4;
			srcbuf  += 4;
		}
	}
}

static void compose_blend2_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	int src_width  = visual_video_get_width	 (src);
	int src_height = visual_video_get_height (src);

	for (i = 0; i < src_height; i++) {
		for (j = 0; j < src_width; j++) {
			destbuf[0] = (destbuf[0] * (destbuf[0] - srcbuf[0]) >> 8) + srcbuf[0];
			destbuf[1] = (alpha      * (destbuf[1] - srcbuf[1]) >> 8) + srcbuf[1];
			destbuf[2] = (0          * (destbuf[2] - srcbuf[2]) >> 8) + srcbuf[2];

			destbuf += 4;
			srcbuf  += 4;
		}
	}
}

static void compose_blend3_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	int src_width  = visual_video_get_width	 (src);
	int src_height = visual_video_get_height (src);

	for (i = 0; i < src_height; i++) {
		for (j = 0; j < src_width; j++) {
			destbuf[0] = (0          * (destbuf[0] - srcbuf[0]) >> 8) + srcbuf[0];
			destbuf[1] = (alpha      * (destbuf[1] - srcbuf[1]) >> 8) + srcbuf[1];
			destbuf[2] = (destbuf[0] * (destbuf[2] - srcbuf[2]) >> 8) + srcbuf[2];

			destbuf += 4;
			srcbuf  += 4;
		}
	}
}

static void compose_blend4_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);
	uint8_t alpha = 128;

	int src_width  = visual_video_get_width	 (src);
	int src_height = visual_video_get_height (src);

	for (i = 0; i < src_height; i++) {
		for (j = 0; j < src_width; j++) {
			destbuf[0] = (destbuf[0] * (destbuf[0] - srcbuf[0]) >> 8) + srcbuf[0];
			destbuf[1] = (alpha      * (destbuf[1] - srcbuf[1]) >> 8) + srcbuf[1];
			destbuf[2] = (srcbuf[0]  * (destbuf[2] - srcbuf[2]) >> 8) + srcbuf[2];

			destbuf += 4;
			srcbuf  += 4;
		}
	}
}

static void compose_blend5_32_c (VisVideo *dest, VisVideo *src)
{
	int i, j;

	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);

    int src_width  = visual_video_get_width  (src);
    int src_height = visual_video_get_height (src);

	for (i = 0; i < src_height; i++) {
		for (j = 0; j < src_width; j++) {
			destbuf[0] = (destbuf[0] * (destbuf[0] - srcbuf[0]) >> 8) + srcbuf[0];
			destbuf[1] = (srcbuf[0]  * (destbuf[1] - srcbuf[1]) >> 8) + srcbuf[1];
			destbuf[2] = (destbuf[0] * (destbuf[2] - srcbuf[2]) >> 8) + srcbuf[2];

			destbuf += 4;
			srcbuf  += 4;
		}
	}
}
