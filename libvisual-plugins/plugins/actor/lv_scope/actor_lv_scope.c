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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <libvisual/libvisual.h>

typedef struct {
	VisPalette pal;
} ScopePrivate;

int lv_scope_init (VisPluginData *plugin);
int lv_scope_cleanup (VisPluginData *plugin);
int lv_scope_requisition (VisPluginData *plugin, int *width, int *height);
int lv_scope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_scope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_scope_palette (VisPluginData *plugin);
int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = lv_scope_requisition,
		.palette = lv_scope_palette,
		.render = lv_scope_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_scope",
		.name = "libvisual scope",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = N_("Libvisual scope plugin"),
		.help = N_("This is a test plugin that'll display a simple scope"),

		.init = lv_scope_init,
		.cleanup = lv_scope_cleanup,
		.events = lv_scope_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_scope_init (VisPluginData *plugin)
{
	ScopePrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (ScopePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

int lv_scope_cleanup (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_mem_free (priv);

	return 0;
}

int lv_scope_requisition (VisPluginData *plugin, int *width, int *height)
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

int lv_scope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_scope_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_scope_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_scope_palette (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;
	
	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = i;
		priv->pal.colors[i].g = i;
		priv->pal.colors[i].b = i;
	}

	return &priv->pal;
}

int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	int adder = video->width > 512 ? (video->width - 512) / 2 : 0;
	int i, y;
	uint8_t *buf;

	if (video == NULL)
		return -1;

	y = video->height >> 1;
	
	buf = (uint8_t *) video->pixels;

	visual_mem_set (buf, 0, video->pitch * video->height);
	
	for (i = 0; i < video->width && i < 512; i++) {
		y = (video->height / 2) + (audio->pcm[2][i >> 1] >> 9);

		if (y < 0)
			y = 0;
		else if (y > video->height - 1)
			y = video->height - 1;
		
		buf[(y * video->pitch) + i + adder] = i;
	}

	return 0;
}

