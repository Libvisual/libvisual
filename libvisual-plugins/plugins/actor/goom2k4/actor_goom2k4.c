/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
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
#include "goom.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
	PluginInfo	*goominfo; /* The goom internal private struct */
} GoomPrivate;

static int         lv_goom_init        (VisPluginData *plugin);
static void        lv_goom_cleanup     (VisPluginData *plugin);
static void        lv_goom_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_goom_resize      (VisPluginData *plugin, int width, int height);
static int         lv_goom_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_goom_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_goom_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_goom_requisition,
		.palette     = lv_goom_palette,
		.render      = lv_goom_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "goom2k4",
		.name     = "libvisual goom2k4 plugin",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>, goom2k4 by: Jean-Christophe Hoelt <jeko@ios-software.com>",
		.version  = "0.1",
		.about    = N_("Libvisual goom2k4 plugin"),
		.help     = N_("This plugin adds support for the supercool goom2k4 plugin that is simply awesome"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init    = lv_goom_init,
		.cleanup = lv_goom_cleanup,
		.events  = lv_goom_events,
		.plugin  = &actor
	};

	return &info;
}

static int lv_goom_init (VisPluginData *plugin)
{
	GoomPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	priv = visual_mem_new0 (GoomPrivate, 1);
	visual_plugin_set_private (plugin, priv);

	priv->goominfo = goom_init (128, 128);

	return TRUE;
}

static void lv_goom_cleanup (VisPluginData *plugin)
{
	GoomPrivate *priv = visual_plugin_get_private (plugin);

	if (priv->goominfo != NULL)
		goom_close (priv->goominfo);

	visual_mem_free (priv);
}

static void lv_goom_requisition (VisPluginData *plugin, int *width, int *height)
{
	/* We don't change the value, we can handle anything */
}

static void lv_goom_resize (VisPluginData *plugin, int width, int height)
{
	GoomPrivate *priv = visual_plugin_get_private (plugin);

	goom_set_resolution (priv->goominfo, width, height);
}

static int lv_goom_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_goom_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *lv_goom_palette (VisPluginData *plugin)
{
	return NULL;
}

static void lv_goom_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	GoomPrivate *priv = visual_plugin_get_private (plugin);

	const int showinfo = TRUE;

	VisBuffer *pcmbuf1 = visual_buffer_new_allocate(sizeof (float) * 512);
	visual_audio_get_sample (audio, pcmbuf1, VISUAL_AUDIO_CHANNEL_LEFT);

	VisBuffer *pcmbuf2 = visual_buffer_new_allocate(sizeof (float) * 512);
	visual_audio_get_sample (audio, pcmbuf2, VISUAL_AUDIO_CHANNEL_RIGHT);

	float *buf1 = visual_buffer_get_data(pcmbuf1);
	float *buf2 = visual_buffer_get_data(pcmbuf2);

	short pcmdata[2][512];

	for (int i = 0; i < 512; i++) {
		pcmdata[0][i] = buf1[i] * 32767;
		pcmdata[1][i] = buf2[i] * 32767;
	}

	visual_buffer_unref(pcmbuf1);
	visual_buffer_unref(pcmbuf2);

	/* Retrieve the songinfo */
	VisSongInfo *songinfo = ((VisActorPlugin *) visual_plugin_get_specific (plugin))->songinfo;

	uint32_t *buf;

	/* FIXME goom should support setting a pointer, so we don't need that final visual_mem_copy */
	if (songinfo != NULL && visual_songinfo_get_age (songinfo) <= 1 && showinfo == TRUE) {
		VisSongInfoType songinfo_type = visual_songinfo_get_type (songinfo);

		if (songinfo_type == VISUAL_SONGINFO_TYPE_SIMPLE)
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, visual_songinfo_get_simple_name (songinfo), NULL);
		else if (songinfo_type == VISUAL_SONGINFO_TYPE_ADVANCED)
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, visual_songinfo_get_song (songinfo), NULL);
		else
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, NULL, NULL);
	}
	else {
		buf = goom_update (priv->goominfo, pcmdata, 0, 0, NULL, NULL);
	}

	uint8_t *vidbuf = visual_video_get_pixels (video);

	visual_mem_copy_pitch (vidbuf, buf, visual_video_get_pitch (video),
						   visual_video_get_pitch (video),
						   visual_video_get_pitch (video),
						   visual_video_get_height (video));
}
