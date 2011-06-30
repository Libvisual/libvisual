/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_timescope.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
//#include <sttimescope.h>

#include <libvisual/libvisual.h>

#include "lvavs_pipeline.h"
#include "avs_common.h"

typedef struct {
    LVAVSPipeline *pipeline;

    int enabled;
    int color;
    int blend;
    int blendavg;
    int nbands;
    int x;
    int oldh;
    int which_ch;

} TimescopePrivate;

int lv_timescope_init (VisPluginData *plugin);
int lv_timescope_cleanup (VisPluginData *plugin);
int lv_timescope_requisition (VisPluginData *plugin, int *width, int *height);
int lv_timescope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_timescope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_timescope_palette (VisPluginData *plugin);
int lv_timescope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (TimescopePrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_timescope_requisition,
		.palette = lv_timescope_palette,
		.render = lv_timescope_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_timescope",
		.name = "Libvisual AVS Render: timescope element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: timescope element",
		.help = "This is the timescope scope element for the libvisual AVS system",

		.init = lv_timescope_init,
		.cleanup = lv_timescope_cleanup,
		.events = lv_timescope_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_timescope_init (VisPluginData *plugin)
{
	TimescopePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("color", 2),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0x10),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("which_ch", 0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("nbands", 0),
		VISUAL_PARAM_LIST_END
	};
	
    priv = visual_mem_new0 (TimescopePrivate, 1);
    priv->pipeline = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->pipeline == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many(paramcontainer, params);

	return 0;
}

int lv_timescope_cleanup (VisPluginData *plugin)
{
	TimescopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_timescope_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_timescope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_timescope_events (VisPluginData *plugin, VisEventQueue *events)
{
	TimescopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_timescope_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "enabled"))
					priv->enabled = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "color"))
					priv->color = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "blend"))
					priv->blend = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "blendavg"))
					priv->blendavg = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "which_ch"))
					priv->which_ch = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "nbands"))
					priv->nbands = visual_param_entry_get_integer (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_timescope_palette (VisPluginData *plugin)
{
	TimescopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_timescope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    TimescopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    LVAVSPipeline *pipeline = priv->pipeline;
    int *framebuffer = visual_video_get_pixels(video);
    int w = video->width;
    int h = video->height;

    int i,j;
    int c;
    char center_channel[576];
    unsigned char *fa_data;

    if (!priv->enabled) return 0;
    if (pipeline->isBeat&0x80000000) return 0;

    if (priv->which_ch >=2)
    {
        for (j = 0; j < 576; j ++) center_channel[j]=pipeline->audiodata[1][0][j]/2+pipeline->audiodata[1][1][j]/2;
        fa_data=(unsigned char *)center_channel;
    }
    else fa_data=(unsigned char *)&pipeline->audiodata[1][priv->which_ch][0];

    priv->x++;
    priv->x %= w;
    framebuffer+=priv->x;
    int r,g,b;
    r=priv->color&0xff;
    g=(priv->color>>8)&0xff;
    b=(priv->color>>16)&0xff;
    for (i=0;i<h;i++)
    {
        c = (int)pipeline->audiodata[0][0][(i*priv->nbands)/h] & 0xFF;
        c = (r*c)/256 + (((g*c)/256)<<8) + (((b*c)/256)<<16);
        if (priv->blend == 2)
            BLEND_LINE(framebuffer,c,  pipeline->blendtable, pipeline->blendmode);
        else if (priv->blend == 1)
            framebuffer[0]=BLEND(framebuffer[0],c);
        else if (priv->blendavg)
            framebuffer[0]=BLEND_AVG(framebuffer[0],c);
        else
            framebuffer[0]=c;
        framebuffer+=w;
    }

    return 0;
}

