/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_onetone.c,v 1.2 2006-09-19 19:05:47 synap Exp $
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

/* FIXME TODO:
 *
 * config UI.
 * fix for other depths than 32bits
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

#define MAX(x,y)	(x > y ? x : y)

typedef struct {
	int enabled;
	int color;
	int blend;
	int blendavg;
	int invert;

	unsigned char tabler[256];
	unsigned char tableg[256];
	unsigned char tableb[256];
} OnetonePrivate;

int lv_onetone_init (VisPluginData *plugin);
int lv_onetone_cleanup (VisPluginData *plugin);
int lv_onetone_events (VisPluginData *plugin, VisEventQueue *events);
int lv_onetone_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_onetone_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void RebuildTable (OnetonePrivate *priv);
static inline int depthof(OnetonePrivate *priv, int c);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_onetone_palette,
		.video = lv_onetone_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = FALSE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM, //".[avs]",

		.plugname = "avs_onetone",
		.name = "Libvisual AVS Transform: onetone element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: onetone element",
		.help = "This is the onetone element for the libvisual AVS system",

		.init = lv_onetone_init,
		.cleanup = lv_onetone_cleanup,
		.events = lv_onetone_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_onetone_init (VisPluginData *plugin)
{
	OnetonePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
		VISUAL_PARAM_LIST_ENTRY_COLOR ("color", 255, 0, 0, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 1, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 1, VISUAL_PARAM_LIMIT_INTEGER(0, 1000), ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("invert", 0, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (OnetonePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_onetone_cleanup (VisPluginData *plugin)
{
	OnetonePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_onetone_events (VisPluginData *plugin, VisEventQueue *events)
{
	OnetonePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "enabled"))
					priv->enabled = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "color")) {
					VisColor *color = visual_param_entry_get_color (param);
					priv->color = visual_color_to_uint32 (color);

					RebuildTable (priv);
				} else if (visual_param_entry_is (param, "blend"))
					priv->blend = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "blendavg"))
					priv->blendavg = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "invert"))
					priv->invert = visual_param_entry_get_integer (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_onetone_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_onetone_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	OnetonePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint32_t *buf = visual_video_get_pixels (video);
	int x, y;
	int c,d;
	int diff;

	if (priv->enabled == 0)
		return 0;

	diff = (video->pitch / video->bpp) - (video->width);

//	if (isBeat&0x80000000) return 0;

	if (priv->blend) {
		for (y = 0; y < video->height; y++) {
			for (x = 0; x < video->width; x++) {
				d = depthof(priv, *buf);
				c = priv->tableb[d] | (priv->tableg[d]<<8) | (priv->tabler[d]<<16);
				// *buf = BLEND(*buf, c);
				buf++;
			}

			buf += diff;
		}
	} else if (priv->blendavg) {
		for (y = 0; y < video->height; y++) {
			for (x = 0; x < video->width; x++) {
				d = depthof(priv, *buf);
				c = priv->tableb[d] | (priv->tableg[d]<<8) | (priv->tabler[d]<<16);
				// *buf = BLEND_AVG(*buf, c);
				buf++;
			}

			buf += diff;
		}
	} else {
		for (y = 0; y < video->height; y++) {
			for (x = 0; x < video->width; x++) {
				d = depthof(priv, *buf);
				*buf = priv->tableb[d] | (priv->tableg[d]<<8) | (priv->tabler[d]<<16);
				buf++;
			}

			buf += diff;
		}
	}

	return 0;
}

static inline int depthof(OnetonePrivate *priv, int c)
{
	int r= MAX(MAX((c & 0xFF), ((c & 0xFF00)>>8)), (c & 0xFF0000)>>16);
	return priv->invert ? 255 - r : r;
}

static void RebuildTable(OnetonePrivate *priv)
{
	int i;
	for (i=0;i<256;i++)
		priv->tableb[i] = (unsigned char)((i / 255.0) * (float)(priv->color & 0xFF));
	for (i=0;i<256;i++)
		priv->tableg[i] = (unsigned char)((i / 255.0) * (float)((priv->color & 0xFF00) >> 8));
	for (i=0;i<256;i++)
		priv->tabler[i] = (unsigned char)((i / 255.0) * (float)((priv->color & 0xFF0000) >> 16));
}
