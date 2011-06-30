/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_movement.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <string.h>
#include <sys/mman.h>
#include <math.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs.h"

AvsNumber PI = M_PI;

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];

    // params
    int enabled, color_clip;

} ColorreplacePrivate;

int lv_colorreplace_init (VisPluginData *plugin);
int lv_colorreplace_cleanup (VisPluginData *plugin);
int lv_colorreplace_events (VisPluginData *plugin, VisEventQueue *events);
int lv_colorreplace_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_colorreplace_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_colorreplace_palette,
		.video = lv_colorreplace_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_colorreplace",
		.name = "Libvisual AVS Transform: colorreplace element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: colorreplace element",
		.help = "This is the colorreplace element for the libvisual AVS system",

		.init = lv_colorreplace_init,
		.cleanup = lv_colorreplace_cleanup,
		.events = lv_colorreplace_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_colorreplace_init (VisPluginData *plugin)
{
	ColorreplacePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable plugin"),
        VISUAL_PARAM_LIST_INTEGER("color_clip", 0x00202020, VISUAL_PARAM_LIMIT_NONE, "Color clip"),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (ColorreplacePrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

    return 0;
}

int lv_colorreplace_cleanup (VisPluginData *plugin)
{
	ColorreplacePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int lv_colorreplace_events (VisPluginData *plugin, VisEventQueue *events)
{
	ColorreplacePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "color_clip"))
                    priv->color_clip = visual_param_entry_get_integer(param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_colorreplace_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_colorreplace_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ColorreplacePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

    unsigned int *f = (unsigned int *) pixels;
    int fs_r,fs_g,fs_b;
    int x=video->width * video->height;
    if (!enabled) return 0;

    fs_b=(priv->color_clip>>16)&255;
    fs_g=(priv->color_clip>>8)&255;
    fs_r=(priv->color_clip)&255;

    while (x--)
    {
        int r=f[0]&255;
        int g=(f[0]>>8)&255;
        int b=(f[0]>>16)&255;
        int a=(f[0]&0xff000000);
        if (r <= fs_r && g <= fs_g && b <= fs_b)
            f[0]=a|fs_r|(fs_g<<8)|(fs_b<<16);
        f++;
    }
    return 0;
}

