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

    int enabled;
    int blend, blendavg, smax;
    uint8_t *depthBuffer;
    int oldx, oldy;
    int staticgrain;
    uint8_t randtab[491];
    int randtab_pos;

} GrainPrivate;

int lv_grain_init (VisPluginData *plugin);
int lv_grain_cleanup (VisPluginData *plugin);
int lv_grain_events (VisPluginData *plugin, VisEventQueue *events);
int lv_grain_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_grain_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_grain_palette,
        .video = lv_grain_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_grain",
        .name = "Libvisual AVS Transform: grain element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: grain element",
        .help = "This is the grain element for the libvisual AVS system",

        .init = lv_grain_init,
        .cleanup = lv_grain_cleanup,
        .events = lv_grain_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_grain_init (VisPluginData *plugin)
{
    GrainPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enabled"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("blendavg", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend average"),
        VISUAL_PARAM_LIST_INTEGER("smax", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 100), ""),
        VISUAL_PARAM_LIST_INTEGER("staticgrain", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Static grain"),

        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (GrainPrivate, 1);

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

    return 0;
}

int lv_grain_cleanup (VisPluginData *plugin)
{
    GrainPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(GrainPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(GrainPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_grain_events (VisPluginData *plugin, VisEventQueue *events)
{
    GrainPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                else if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "smax"))
                    priv->smax = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "staticgrain"))
                    priv->staticgrain = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_grain_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

void trans_reinit(GrainPrivate *priv, int w, int h)
{
    int x,y;
    unsigned char *p;
    if (depthBuffer != NULL)
        visual_mem_free(priv->depthBuffer);
    priv->depthBuffer = visual_mem_malloc(w*h*2);
    p = depthBuffer;
    if (p)
        for (y=0;y<h;y++)
            for (x=0;x<w;x++)
            {
                *p++ = (rand()%255);
                *p++ = (rand()%100);
            }

}

uint8_t __inline fastrandbyte(GrainPrivate *priv)
{
  uint8_t r=randtab[priv->randtab_pos];
  priv->randtab_pos++;
  if (!(priv->randtab_pos&15))
  {
    priv->randtab_pos+=rand()%73;
  }
  if (priv->randtab_pos >= 491) priv->randtab_pos-=491;
  return r;
}
int lv_grain_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    GrainPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    if (isBeat&0x80000000) return 0;
    if (!enabled) return 0;

    int smax_sc = (priv->smax*255)/100;
    int *p;
    unsigned char *q;
    int l=w*h;

    if (w != priv->oldx || h != priv->oldy)
    {
        trans_reinit(priv, w, h);
        oldx = w;
        oldy = h;
    }
    priv->randtab_pos+=rand()%300;
    if (priv->randtab_pos >= 491) priv->randtab_pos-=491;

    p = framebuffer;
    q = priv->depthBuffer;
    if (priv->staticgrain)
    {
        if (priv->blend)
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (q[1] < smax_sc) 
                    {
                        int s = q[0];
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = BLEND(*p, c);
                }
                p++;
                q+=2;
            }
        }
        else if (priv->blendavg)
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (q[1] < smax_sc) 
                    {
                        int s = q[0];
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = BLEND_AVG(*p, c);
                }
                p++;
                q+=2;
            }
        }
        else
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (q[1] < smax_sc) 
                    {
                        int s = q[0];
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = c;
                }
                p++;
                q+=2;
            }
        }
    }
    else
    {
        if (priv->blend)
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (trans_fastrandbyte(priv) < smax_sc) 
                    {
                        int s = trans_fastrandbyte(priv);
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = BLEND(*p, c);
                }
                p++;
                q+=2;
            }
        }
        else if (priv->blendavg)
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (trans_fastrandbyte(priv) < smax_sc) 
                    {
                        int s = trans_fastrandbyte(priv);
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = BLEND_AVG(*p, c);
                }
                p++;
                q+=2;
            }
        }
        else
        {
            while (l--)
            {
                if (*p)
                {
                    int c=0;
                    if (trans_fastrandbyte(priv) < smax_sc) 
                    {
                        int s = trans_fastrandbyte(priv);
                        int r=(((p[0]&0xff0000)*s)>>8);
                        if (r > 0xff0000) r=0xff0000;
                        c|=r&0xff0000;
                        r=(((p[0]&0xff00)*s)>>8);
                        if (r > 0xff00) r=0xff00;
                        c|=r&0xff00;
                        r=(((p[0]&0xff)*s)>>8);
                        if (r > 0xff) r=0xff;
                        c|=r;
                    }
                    *p = c;
                }
                p++;
                q+=2;
            }
        }
    }
    return 0;
}

