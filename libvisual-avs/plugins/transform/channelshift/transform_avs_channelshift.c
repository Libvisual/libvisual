/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_channelshift.c,v 1.4 2006-09-19 19:05:47 synap Exp $
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
 * andere shifts afmaken
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
	int shift;
	int onbeat;
} ChannelshiftPrivate;

int lv_channelshift_init (VisPluginData *plugin);
int lv_channelshift_cleanup (VisPluginData *plugin);
int lv_channelshift_events (VisPluginData *plugin, VisEventQueue *events);
int lv_channelshift_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_channelshift_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

void channelshift_video (VisPluginData *plugin, VisVideo *video, int a, int b);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_channelshift_palette,
		.video = lv_channelshift_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = FALSE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM".[avs]",

		.plugname = "avs_channelshift",
		.name = "Libvisual AVS Transform: channelshift element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: channelshift element",
		.help = "This is the channelshift element for the libvisual AVS system",

		.init = lv_channelshift_init,
		.cleanup = lv_channelshift_cleanup,
		.events = lv_channelshift_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_channelshift_init (VisPluginData *plugin)
{
	ChannelshiftPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("shift", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 1000), ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeat", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 1000), ""),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (ChannelshiftPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_channelshift_cleanup (VisPluginData *plugin)
{
	ChannelshiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_channelshift_events (VisPluginData *plugin, VisEventQueue *events)
{
	ChannelshiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "shift"))
					priv->shift = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "onbeat"))
					priv->onbeat = visual_param_entry_get_integer (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_channelshift_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_channelshift_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ChannelshiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	/* FIXME on beat stuff, when on beat is there.! (VisAudio.. ) */

//	priv->shift = 1;
	switch (priv->shift) {
		case 0: /* RGB */
			break;

		case 1: /* RBG */
			channelshift_video (plugin, video, 0, 1);

			break;

		case 2: /* BRG */
			channelshift_video (plugin, video, 0, 1);

			break;

		case 3: /* BGR */
			channelshift_video (plugin, video, 0, 1);

			break;

		case 4: /* GBR */
			channelshift_video (plugin, video, 0, 1);

			break;

		case 5: /* GRB */
			channelshift_video (plugin, video, 0, 1);

			break;

		default: /* RGB */
			break;

	}
}

void channelshift_video (VisPluginData *plugin, VisVideo *video, int a, int b)
{
	uint8_t *buf = visual_video_get_pixels (video);
	int size = visual_video_get_size (video);
	uint8_t swap;
	int i;

	for (i = 0; i < size / 4; i++) {
		swap = buf[a];
		buf[a] = buf[b];
		buf[b] = swap;

		buf += 4;
	}
}

