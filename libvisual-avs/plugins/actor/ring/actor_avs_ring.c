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

#include <libvisual/libvisual.h>

typedef struct {
} RingPrivate;

int lv_ring_init (VisPluginData *plugin);
int lv_ring_cleanup (VisPluginData *plugin);
int lv_ring_requisition (VisPluginData *plugin, int *width, int *height);
int lv_ring_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_ring_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_ring_palette (VisPluginData *plugin);
int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_ring_requisition,
		.palette = lv_ring_palette,
		.render = lv_ring_render,
		.depth = VISUAL_VIDEO_DEPTH_32BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_ring",
		.name = "Libvisual AVS Render: ring element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: ring element",
		.help = "This is the ring scope element for the libvisual AVS system",

		.init = lv_ring_init,
		.cleanup = lv_ring_cleanup,
		.events = lv_ring_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_ring_init (VisPluginData *plugin)
{
	RingPrivate *priv;

	priv = visual_mem_new0 (RingPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	return 0;
}

int lv_ring_cleanup (VisPluginData *plugin)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_ring_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_ring_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_ring_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_ring_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
	
			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_ring_palette (VisPluginData *plugin)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return 0;
}

