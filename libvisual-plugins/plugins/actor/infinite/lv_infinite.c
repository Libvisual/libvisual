/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "renderer.h"
#include "display.h"

int act_infinite_init (VisPluginData *plugin);
int act_infinite_cleanup (VisPluginData *plugin);
int act_infinite_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_infinite_requisition (VisPluginData *plugin, int *width, int *height);
int act_infinite_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_infinite_palette (VisPluginData *plugin);
int act_infinite_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_infinite_requisition,
		.palette = act_infinite_palette,
		.render = act_infinite_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "infinite",
		.name = "infinite plugin",
		.author = "Original by: Julien Carme <julien.carme@acm.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The infinite visual plugin",
		.help = "This is the libvisual plugin for the infinite visual",

		.init = act_infinite_init,
		.cleanup = act_infinite_cleanup,
		.events = act_infinite_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);
	
	return info;
}

int act_infinite_init (VisPluginData *plugin)
{
	InfinitePrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	priv = visual_mem_new0 (InfinitePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->rcontext = visual_plugin_get_random_context (plugin);

	priv->plugwidth = 32;
	priv->plugheight = 32;

	visual_palette_allocate_colors (&priv->pal, 256);

	_inf_init_renderer (priv);
	_inf_load_random_effect(priv, &priv->current_effect);


	priv->color = visual_random_context_int_range (priv->rcontext, 0, NB_PALETTES - 1);
	_inf_change_color(priv, priv->old_color, priv->color, 256);
	priv->old_color = priv->color;

	priv->color = visual_random_context_int_range (priv->rcontext, 0, NB_PALETTES - 1);

	return 0;
}

int act_infinite_cleanup (VisPluginData *plugin)
{
	InfinitePrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	_inf_close_renderer (priv);

	visual_palette_free_colors (&priv->pal);
	visual_mem_free (priv);

	return 0;
}

int act_infinite_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	visual_log_return_val_if_fail (plugin != NULL, -1);

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

int act_infinite_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	InfinitePrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	visual_log_return_val_if_fail (video != NULL, -1);

	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	priv->plugwidth = width;
	priv->plugheight = height;

	visual_video_set_dimension (video, width, height);

	_inf_close_renderer (priv);

	if (video->depth != VISUAL_VIDEO_DEPTH_8BIT)
		return -1;

	_inf_init_renderer (priv);

	return 0;
}

int act_infinite_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_infinite_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_infinite_palette (VisPluginData *plugin)
{
	InfinitePrivate *priv;
	
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return &priv->pal;
}

int act_infinite_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	InfinitePrivate *priv;
	int i;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	visual_log_return_val_if_fail (video != NULL, -1);
	visual_log_return_val_if_fail (audio != NULL, -1);

	priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	for (i = 0; i < 512; i++) {
		priv->pcm_data[0][i] = audio->pcm[0][i];
		priv->pcm_data[1][i] = audio->pcm[1][i];
	}

	_inf_renderer (priv);
	_inf_display (priv, (uint8_t *) video->pixels);

	return 0;
}

