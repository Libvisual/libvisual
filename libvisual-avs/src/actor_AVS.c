/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "avs_parse.h"

typedef struct {
} AVSPrivate;

int act_avs_init (VisPluginData *plugin);
int act_avs_cleanup (VisPluginData *plugin);
int act_avs_requisition (VisPluginData *plugin, int *width, int *height);
int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_avs_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_avs_palette (VisPluginData *plugin);
int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_avs_requisition,
		.palette = act_avs_palette,
		.render = act_avs_render,
		.depth = VISUAL_VIDEO_DEPTH_32BIT,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs",
		.name = "Libvisual advanced visual studio plugin",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The libvisual advanced visual studio plugin",
		.help = "",

		.init = act_avs_init,
		.cleanup = act_avs_cleanup,
		.events = act_avs_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_avs_init (VisPluginData *plugin)
{
	AVSPrivate *priv;

	priv = visual_mem_new0 (AVSPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);
	
	return 0;
}

int act_avs_cleanup (VisPluginData *plugin)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int act_avs_requisition (VisPluginData *plugin, int *width, int *height)
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

int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	
	visual_video_set_dimension (video, width, height);

	return 0;
}

int act_avs_events (VisPluginData *plugin, VisEventQueue *events)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_avs_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_avs_palette (VisPluginData *plugin)
{
	return NULL;
}

int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	
	return 0;
}

