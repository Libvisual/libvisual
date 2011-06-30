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


    int enabled;
    int quality;
    int quality2;
    int blend;
    int blendavg;
    int onbeat;
    int durFrames;
    int nF;
    int thisQuality;

} MosaicPrivate;

int lv_mosaic_init (VisPluginData *plugin);
int lv_mosaic_cleanup (VisPluginData *plugin);
int lv_mosaic_events (VisPluginData *plugin, VisEventQueue *events);
int lv_mosaic_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_mosaic_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_mosaic_palette,
        .video = lv_mosaic_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_mosaic",
        .name = "Libvisual AVS Transform: mosaic element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: mosaic element",
        .help = "This is the mosaic element for the libvisual AVS system",

        .init = lv_mosaic_init,
        .cleanup = lv_mosaic_cleanup,
        .events = lv_mosaic_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_mosaic_init (VisPluginData *plugin)
{
    MosaicPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Mosaic"),
        VISUAL_PARAM_LIST_INTEGER("quality", 50, VISUAL_PARAM_LIMIT_BOOLEAN, "Quality"),
        VISUAL_PARAM_LIST_INTEGER("quality2", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Quality 2"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("blendavg", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend average"),
        VISUAL_PARAM_LIST_INTEGER("onbeat", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "OnBeat"),
        VISUAL_PARAM_LIST_INTEGER("durFrames", 15, VISUAL_PARAM_LIMIT_BOOLEAN, "durFrames"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (MosaicPrivate, 1);

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

int lv_mosaic_cleanup (VisPluginData *plugin)
{
    MosaicPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_mosaic_events (VisPluginData *plugin, VisEventQueue *events)
{
    MosaicPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "quality"))
                    priv->quality = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "quality2"))
                    priv->quality2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "durFrames"))
                    priv->durFrames = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_mosaic_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_mosaic_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    MosaicPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    if (isBeat&0x80000000) return 0;

    if (!priv->enabled) return 0;

    if (priv->onbeat && isBeat)
    {
        priv->thisQuality=priv->quality2;
        priv->nF = priv->durFrames;
    }
    else if (!priv->nF) priv->thisQuality = priv->quality;

    if (priv->thisQuality<100)
    {
        int y;
        int *p = fbout;
        int *p2 = framebuffer;
        int sXInc = (w*65536) / priv->thisQuality;
        int sYInc = (h*65536) / priv->thisQuality;
        int ypos=(sYInc>>17);
        int dypos=0;

        for (y = 0; y < h; y ++)
        {
            int x=w;
            int *fbread=framebuffer+ypos*w;
            int dpos=0;
            int xpos=(sXInc>>17);
            int src=fbread[xpos];

            if (priv->blend)
            {
                while (x--)
                {
                    *p++ = BLEND(*p2++, src);
                    dpos+=1<<16;
                    if (dpos>=sXInc)
                    {
                        xpos+=dpos>>16;
                        if (xpos >= w) break;
                        src=fbread[xpos];
                        dpos-=sXInc;
                    }
                }
            }
            else if (priv->blendavg)
            {
                while (x--)
                {
                    *p++ = BLEND_AVG(*p2++, src);
                    dpos+=1<<16;
                    if (dpos>=sXInc)
                    {
                        xpos+=dpos>>16;
                        if (xpos >= w) break;
                        src=fbread[xpos];
                        dpos-=sXInc;
                    }
                }
            }
            else
            {
                while (x--)
                {
                    *p++ = src;
                    dpos+=1<<16;
                    if (dpos>=sXInc)
                    {
                        xpos+=dpos>>16;
                        if (xpos >= w) break;
                        src=fbread[xpos];
                        dpos-=sXInc;
                    }
                }
            }
            dypos+=1<<16;
            if (dypos>=sYInc)
            {
                ypos+=(dypos>>16);
                dypos-=sYInc;
                if (ypos >= h) break;
            }
        }
        rval=1;
    }

    if (nF)
    {
        nF--;
        if (nF)
        {
            int a = abs(quality - quality2) / durFrames;
            thisQuality += a * (quality2 > quality ? -1 : 1);
        }
    }

    return 0;
}

