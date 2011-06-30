/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_svp.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
 */
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stsvp.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    char m_library[1024];

} SvpPrivate;

int lv_svp_init (VisPluginData *plugin);
int lv_svp_cleanup (VisPluginData *plugin);
int lv_svp_requisition (VisPluginData *plugin, int *width, int *height);
int lv_svp_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_svp_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_svp_palette (VisPluginData *plugin);
int lv_svp_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

void set_library(SvpPrivate *priv);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_svp_requisition,
		.palette = lv_svp_palette,
		.render = lv_svp_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_svp",
		.name = "Libvisual AVS Render: SVP Loader element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: SVP Loader element",
		.help = "This is the SVP Loader element for the libvisual AVS system",

		.init = lv_svp_init,
		.cleanup = lv_svp_cleanup,
		.events = lv_svp_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_svp_init (VisPluginData *plugin)
{
	SvpPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING("m_library", "", "SVP/UVS Library");
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (SvpPrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 1);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_container_add_many_proxy (paramcontainer, params);

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

	visual_palette_free_colors (&priv->pal);

	return 0;
}

int lv_svp_cleanup (VisPluginData *plugin)
{
	SvpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_svp_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_svp_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_svp_events (VisPluginData *plugin, VisEventQueue *events)
{
	SvpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_svp_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "m_library"))
					priv->m_library = visual_param_entry_get_string (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_svp_palette (VisPluginData *plugin)
{
	SvpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_svp_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	SvpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;

    return 0;
}

