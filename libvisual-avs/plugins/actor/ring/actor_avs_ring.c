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

/* FIXME TODO:
 *
 * Color morpher.
 * config UI.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "avs.h"

typedef struct {
	int source;
	int place;
	VisPalette pal;
	int size;
	int type;
} RingPrivate;

int lv_ring_init (VisPluginData *plugin);
int lv_ring_cleanup (VisPluginData *plugin);
int lv_ring_requisition (VisPluginData *plugin, int *width, int *height);
int lv_ring_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_ring_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_ring_palette (VisPluginData *plugin);
int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (RingPrivate *priv, VisAudio *audio, int index);

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
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("source", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("place", 2),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 0x10),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("type", 0),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (RingPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 1);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_container_add_many (paramcontainer, params);

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

	visual_palette_free_colors (&priv->pal);

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
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_ring_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				printf ("proping param %s %d\n", visual_param_entry_get_name (param), visual_param_entry_get_integer (param));
				
				if (visual_param_entry_is (param, "source"))
					priv->source = visual_param_entry_get_integer (param);					
				else if (visual_param_entry_is (param, "place"))
					priv->place = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "size"))
					priv->size = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "type"))
					priv->type = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "palette")) {
					VisPalette *pal;

					pal = visual_param_entry_get_palette (param);

					visual_palette_free_colors (&priv->pal);
					visual_palette_allocate_colors (&priv->pal, pal->ncolors);
					visual_palette_copy (&priv->pal, pal);
				}

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
	int hx = video->width / 2;
	int hy = video->height / 2;
	int x, y, ox, oy;
	int i;
	float a = 0;
	float add = (2 * 3.1415) / 100;
	float size_mult;
	uint32_t *buf = video->pixels;
	VisColor col;

	if (priv->place == 1)
		hx += video->width / 4;
	else if (priv->place == 0)
		hx -= video->width / 4;
	
	col.r = 0xff;
	col.g = 0xff;
	col.b = 0xff;

	if (video->width > video->height)
		size_mult = (float) video->height * ((float) priv->size / 64.00);
	else	
		size_mult = (float) video->width * ((float) priv->size / 64.00);
	
	ox = (cos (a) * (size_mult + (get_data (priv, audio, 0))));
	oy = (sin (a) * (size_mult + (get_data (priv, audio, 0))));

	a += add;

	for (i = 0; i < 50; i++) {


		x = (cos (a) * (size_mult + (get_data (priv, audio, i + 1))));
		y = (sin (a) * (size_mult + (get_data (priv, audio, i + 1))));
		
		a += add;

		avs_gfx_line_non_naieve_ints (video, ox + hx, hy - oy, x + hx, hy - y, &col);
		avs_gfx_line_non_naieve_ints (video, ox + hx, oy + hy, x + hx, y + hy, &col);

		ox = x;
		oy = y;

	}

	return 0;
}

short get_data (RingPrivate *priv, VisAudio *audio, int index)
{
	if (priv->type == 0)
		return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SCOPE, priv->source, index) >> 9;
	else	
		return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SPECTRUM, priv->source, index);

	return 0;
}

