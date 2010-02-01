/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_invert.c,v 1.2 2006-09-19 19:05:47 synap Exp $
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

typedef struct {
	int enabled;
} InvertPrivate;

int lv_invert_init (VisPluginData *plugin);
int lv_invert_cleanup (VisPluginData *plugin);
int lv_invert_events (VisPluginData *plugin, VisEventQueue *events);
int lv_invert_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_invert_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_invert_palette,
		.video = lv_invert_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = FALSE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM, //".[avs]",

		.plugname = "avs_invert",
		.name = "Libvisual AVS Transform: invert element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: invert element",
		.help = "This is the invert element for the libvisual AVS system",

		.init = lv_invert_init,
		.cleanup = lv_invert_cleanup,
		.events = lv_invert_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_invert_init (VisPluginData *plugin)
{
	InvertPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Invert"),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (InvertPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_invert_cleanup (VisPluginData *plugin)
{
	InvertPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_invert_events (VisPluginData *plugin, VisEventQueue *events)
{
	InvertPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "enabled"))
					priv->enabled = visual_param_entry_get_integer (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_invert_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_invert_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	InvertPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint32_t *buf = visual_video_get_pixels (video);
	int x, y;
	int diff;

	if (priv->enabled == 0)
		return 0;

	diff = (video->pitch / video->bpp) - (video->width);

	for (y = 0; y < video->height; y++) {
		for (x = 0; x < video->width; x++) {
			*buf = 0x00ffffff & (0xffffff ^ *buf);

			buf++;
		}

		buf += diff;
	}

    return 0;
}

