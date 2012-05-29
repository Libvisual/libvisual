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

	int                  depth;
	uint8_t             *buf1;
	uint8_t             *buf2;

	VisVideoComposeFunc	 currentcomp;
} OinksiePrivContainer;

static void compose_blend1_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend2_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend3_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend4_32_c (VisVideo *dest, VisVideo *src);
static void compose_blend5_32_c (VisVideo *dest, VisVideo *src);

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
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
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

					switch (priv->color_mode) {
						case 0:  priv->currentcomp = compose_blend1_32_c; break;
						case 1:  priv->currentcomp = compose_blend2_32_c; break;
						case 2:  priv->currentcomp = compose_blend3_32_c; break;
						case 3:  priv->currentcomp = compose_blend4_32_c; break;
						case 4:  priv->currentcomp = compose_blend5_32_c; break;
						default: priv->currentcomp = compose_blend2_32_c; break;
					}
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
	VisBuffer			 *pcmbuf1;
	VisBuffer			 *pcmbuf2;
	VisBuffer			 *pcmmix;
	VisBuffer			 *spmbuf;

	/* Left audio */
	pcmbuf1 = visual_buffer_new_wrap_data (&priv->priv1.audio.pcm[0], sizeof (float) * 4096);
	visual_audio_get_sample (audio, pcmbuf1, VISUAL_AUDIO_CHANNEL_LEFT);

	spmbuf = visual_buffer_new_wrap_data (&priv->priv1.audio.freq[0], sizeof (float) * 256);
	visual_audio_get_spectrum_for_sample (spmbuf, pcmbuf1, FALSE);

	/* Right audio */
	pcmbuf2 = visual_buffer_new_wrap_data (priv->priv1.audio.pcm[1], sizeof (float) * 4096);
	visual_audio_get_sample (audio, pcmbuf2, VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_buffer_set_data_pair (spmbuf, priv->priv1.audio.freq[1], sizeof (float) * 256);
	visual_audio_get_spectrum_for_sample (spmbuf, pcmbuf2, FALSE);

	/* Mix channels */
	pcmmix = visual_buffer_new_wrap_data (priv->priv1.audio.pcm[2], sizeof (float) * 4096);
	visual_audio_sample_buffer_mix_many (pcmmix, TRUE, 2, pcmbuf1, pcmbuf2, 1.0, 1.0);

	visual_buffer_set_data_pair (spmbuf, priv->priv1.audio.freqsmall, sizeof (float) * 4);
	visual_audio_get_spectrum_for_sample (spmbuf, pcmmix, FALSE);

	visual_buffer_unref (pcmbuf1);
	visual_buffer_unref (pcmbuf2);
	visual_buffer_unref (spmbuf);
	visual_buffer_unref (pcmmix);

	/* Duplicate for second oinksie instance */
	visual_mem_copy (&priv->priv2.audio.pcm, &priv->priv1.audio.pcm, sizeof (float) * 4096 * 3);
	visual_mem_copy (&priv->priv2.audio.freq, &priv->priv1.audio.freq, sizeof (float) * 256 * 2);
	visual_mem_copy (&priv->priv2.audio.freqsmall, &priv->priv1.audio.freqsmall, sizeof (float) * 4);

	/* Audio energy */
	priv->priv1.audio.energy = 0 /*audio->energy*/;
	priv->priv2.audio.energy = 0 /*audio->energy*/;

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

		oinksie_sample (&priv->priv1);
		oinksie_sample (&priv->priv2);

		priv->priv1.drawbuf = priv->buf1;
		priv->priv2.drawbuf = priv->buf2;

		oinksie_render (&priv->priv1);
		oinksie_render (&priv->priv2);

		vid1 = visual_video_new_wrap_buffer (priv->buf1,
                                             FALSE,
                                             visual_video_get_width (video),
                                             visual_video_get_height (video),
                                             VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_palette (vid1, oinksie_palette_get (&priv->priv1));

		vid2 = visual_video_new_wrap_buffer (priv->buf2,
                                             FALSE,
                                             visual_video_get_width (video),
                                             visual_video_get_height (video),
                                             VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_palette (vid2, oinksie_palette_get (&priv->priv2));

		visual_video_blit (video, vid1, 0, 0, FALSE);
		visual_video_set_compose_type (vid2, VISUAL_VIDEO_COMPOSE_TYPE_CUSTOM);
		visual_video_set_compose_function (vid2, priv->currentcomp);

		visual_video_blit (video, vid2, 0, 0, TRUE);

		visual_video_unref (vid1);
		visual_video_unref (vid2);
	}

	return 0;
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
