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

typedef enum trans_runnable TransRunnable;

enum trans_runnable {
    TRANS_RUNNABLE_INIT,
    TRANS_RUNNABLE_FRAME,
    TRANS_RUNNABLE_BEAT,
    TRANS_RUNNABLE_PIXEL,
};

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsVariable var_d, var_b;

    // params
    int enabled;
    int x;
    int y;
    int color;
    int blend;
    int blendavg;
    int onbeat;
    int x2;
    int y2;
    int beatdur;

    // Others
    int cur_x, cur_y;


} InterleavePrivate;

int lv_interleave_init (VisPluginData *plugin);
int lv_interleave_cleanup (VisPluginData *plugin);
int lv_interleave_events (VisPluginData *plugin, VisEventQueue *events);
int lv_interleave_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_interleave_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_interleave_palette,
        .video = lv_interleave_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_interleave",
        .name = "Libvisual AVS Transform: interleave element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: interleave element",
        .help = "This is the interleave element for the libvisual AVS system",

        .init = lv_interleave_init,
        .cleanup = lv_interleave_cleanup,
        .events = lv_interleave_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_interleave_init (VisPluginData *plugin)
{
    InterleavePrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Interleave"),
        VISUAL_PARAM_LIST_INTEGER("x", 1, VISUAL_PARAM_LIMIT_NONE, "x"),
        VISUAL_PARAM_LIST_INTEGER("y", 1, VISUAL_PARAM_LIMIT_NONE, "y"),
        VISUAL_PARAM_LIST_INTEGER("color", 0, VISUAL_PARAM_LIMIT_NONE, "Color"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_NONE, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("blendavg", 0, VISUAL_PARAM_LIMIT_NONE, "Blend average"),
        VISUAL_PARAM_LIST_INTEGER("onbeat", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Onbeat"),
        VISUAL_PARAM_LIST_INTEGER("x2", 1, VISUAL_PARAM_LIMIT_NONE, "x2"),
        VISUAL_PARAM_LIST_INTEGER("y2", 1, VISUAL_PARAM_LIMIT_NONE, "y2"),
        VISUAL_PARAM_LIST_INTEGER("beatdur", 4, VISUAL_PARAM_LIMIT_NONE, "Beat duration"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (InterleavePrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();

    avs_runnable_variable_bind(priv->vm, "d", &priv->var_d);
    avs_runnable_variable_bind(priv->vm, "b", &priv->var_b);

    priv->cur_x = priv->cur_y = 1.0;

    return 0;
}

int lv_interleave_cleanup (VisPluginData *plugin)
{
    InterleavePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(InterleavePrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(InterleavePrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_interleave_events (VisPluginData *plugin, VisEventQueue *events)
{
    InterleavePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "x"))
                    priv->x = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "y"))
                    priv->y = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "color"))
                    priv->color = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "x2"))
                    priv->x2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "y2"))
                    priv->y2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "beatdur"))
                    priv->beatdur = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_interleave_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_interleave_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    InterleavePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    if (isBeat&0x80000000) return 0;
    if (!priv->enabled) return 0;
    int ystat=0;
    int yp=0;

    double sc1=(priv->beatdur+512.0-64.0)/512.0;
    priv->cur_x=(priv->cur_x*sc1+priv->x*(1.0-sc1));
    priv->cur_y=(priv->cur_y*sc1+priv->y*(1.0-sc1));

    if (isBeat && priv->onbeat)
    {
        priv->cur_x=priv->x2;
        priv->cur_y=priv->y2;
    }
    int tx=(int)priv->cur_x;
    int ty=(int)priv->cur_y;
    int xos=0;

    int *p=framebuffer;
    int j;
    if (!ty)
    {
        ystat=1;
    }
    if (tx > 0)
    {
        xos=(w%tx)/2;
    }
    if (ty > 0) yp=(h%ty)/2;
    if (tx>=0 && ty >=0) for (j=0;j<h;j++)
    {
        int xstat=0;
        if (ty && ++yp>=ty)
        {
            ystat=!ystat;
            yp=0;
        }
        int l=w;

        if (!ystat) // this line is pure color
        {
            if (priv->blend)
            {
                while (l--)
                {
                    *p=BLEND(*p, priv->color);
                    p++;
                }
            }
            else if (priv->blendavg)
            {
                while (l--)
                {
                    *p=BLEND_AVG(*p, priv->color);
                    p++;
                }
            }
            else
            {
                while (l--)
                {
                    *p++=priv->color;
                }
            }
        }
        else if (tx)
        {
            if (priv->blend)
            {
                int xo=xos;
                while (l>0)
                {
                    int l2=min(l,tx-xo);
                    xo=0;
                    l-=l2;
                    if (xstat) p+=l2;
                    else while (l2--) { *p=BLEND(*p,priv->color); p++; }
                    xstat=!xstat;
                }
            }
            else if (priv->blendavg)
            {
                int xo=xos;
                while (l>0)
                {
                    int l2=min(l,tx-xo);
                    xo=0;
                    l-=l2;
                    if (xstat) p+=l2;
                    else while (l2--) { *p=BLEND_AVG(*p,priv->color); p++; }
                    xstat=!xstat;
                }
            }
            else 
            {
                int xo=xos;
                while (l>0)
                {
                    int l2=min(l,tx-xo);
                    xo=0;
                    l-=l2;
                    if (xstat) p+=l2;
                    else while (l2--) { *p++=priv->color; }
                    xstat=!xstat;
                }
            }
        }
        else p+=w;
    }

    return 0;
}

