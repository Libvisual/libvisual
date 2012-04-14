/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_lv_scope.c,v 1.21 2006/01/27 20:19:17 synap Exp $
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

#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define PCM_SIZE	1024

typedef struct {
	VisPalette *pal;
	VisBuffer	pcm;
} ScopePrivate;

static int lv_scope_init (VisPluginData *plugin);
static int lv_scope_cleanup (VisPluginData *plugin);
static int lv_scope_requisition (VisPluginData *plugin, int *width, int *height);
static int lv_scope_resize (VisPluginData *plugin, int width, int height);
static int lv_scope_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *lv_scope_palette (VisPluginData *plugin);
static int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_scope_requisition,
		.palette = lv_scope_palette,
		.render = lv_scope_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_scope",
		.name = "libvisual scope",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = N_("Libvisual scope plugin"),
		.help = N_("This is a test plugin that'll display a simple scope"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_scope_init,
		.cleanup = lv_scope_cleanup,
		.events = lv_scope_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	return &info;
}

static int lv_scope_init (VisPluginData *plugin)
{
	ScopePrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (ScopePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->pal = visual_palette_new (256);

	visual_buffer_init_allocate (&priv->pcm, sizeof (float) * PCM_SIZE, visual_buffer_destroyer_free);

	return 0;
}

static int lv_scope_cleanup (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free (priv->pal);

	visual_object_unref (VISUAL_OBJECT (&priv->pcm));

	visual_mem_free (priv);

	return 0;
}

static int lv_scope_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 2 || (reqw / 2) % 2)
		reqw--;

	while (reqh % 2 || (reqh / 2) % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

static int lv_scope_resize (VisPluginData *plugin, int width, int height)
{
	return 0;
}

static int lv_scope_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_scope_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *lv_scope_palette (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisColor *pal_colors = visual_palette_get_colors (priv->pal);
	int i;

	for (i = 0; i < 256; i++) {
		pal_colors[i].r = i;
		pal_colors[i].g = i;
		pal_colors[i].b = i;
	}

	return priv->pal;
}

static int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisColor col;
	float *pcmbuf;
	int i, y, y_old;
	uint8_t *buf;

	if (video == NULL)
		return -1;

	y = video->height >> 1;

	visual_audio_get_sample_mixed (audio, &priv->pcm, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

	pcmbuf = visual_buffer_get_data (&priv->pcm);

	visual_color_set (&col, 0, 0, 0);
	visual_video_fill_color (video, &col);

	buf = (uint8_t *) visual_video_get_pixels (video);

	y_old = video->height / 2;
	for (i = 0; i < video->width; i++) {
		int j;

		y = (video->height / 2) + (pcmbuf[(i >> 1) % PCM_SIZE] * (video->height / 4));

		if (y > y_old) {
			for (j = y_old; j < y; j++)
				buf[(j * video->pitch) + i] = 255;
		} else {
			for (j = y; j < y_old; j++)
				buf[(j * video->pitch) + i] = 255;
		}
	}

	return 0;
}

