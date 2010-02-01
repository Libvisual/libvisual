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

    // params
    int enabled;
    int nPoints;
    int distance;
    int alpha;
    int rotation;
    int rotationinc;
    int distance2;
    int alpha2;
    int rotationinc2;
    int rgb;
    int blend;
    int blendavg;
    float speed;
    int onbeat;
    float a;
    int _distance;
    int _alpha;
    int _rotationinc;
    int _rgb;
    float status;

} InterfPrivate;

int lv_interf_init (VisPluginData *plugin);
int lv_interf_cleanup (VisPluginData *plugin);
int lv_interf_events (VisPluginData *plugin, VisEventQueue *events);
int lv_interf_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_interf_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_interf_palette,
        .video = lv_interf_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_interf",
        .name = "Libvisual AVS Transform: interf element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: interf element",
        .help = "This is the interf element for the libvisual AVS system",

        .init = lv_interf_init,
        .cleanup = lv_interf_cleanup,
        .events = lv_interf_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_interf_init (VisPluginData *plugin)
{
    InterfPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable interferences"),
        VISUAL_PARAM_LIST_INTEGER("nPoints", 2, VISUAL_PARAM_LIMIT_NONE, "Number of points"),
        VISUAL_PARAM_LIST_INTEGER("rotation", 0, VISUAL_PARAM_LIMIT_NONE, "Rotation"),
        VISUAL_PARAM_LIST_INTEGER("distance", 10, VISUAL_PARAM_LIMIT_NONE, "Distance"),
        VISUAL_PARAM_LIST_INTEGER("alpha", 128, VISUAL_PARAM_LIMIT_NONE, "Alpha"),
        VISUAL_PARAM_LIST_INTEGER("rotationinc", 0, VISUAL_PARAM_LIMIT_NONE, "Init rotation"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_NONE, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("blendavg", 0, VISUAL_PARAM_LIMIT_NONE, "Blend average"),
        VISUAL_PARAM_LIST_INTEGER("distance2", 32, VISUAL_PARAM_LIMIT_NONE, "Onbeat distance"),
        VISUAL_PARAM_LIST_INTEGER("alpha2", 192, VISUAL_PARAM_LIMIT_NONE, "Onbeat alpha"),
        VISUAL_PARAM_LIST_INTEGER("rotationinc2", 25, VISUAL_PARAM_LIMIT_NONE, "Onbeat rotation"),
        VISUAL_PARAM_LIST_INTEGER("rgb", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Separate RGB"),
        VISUAL_PARAM_LIST_INTEGER("onbeat", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable OnBeat"),
        VISUAL_PARAM_LIST_DOUBLE("speed", 0.2, VISUAL_PARAM_LIMIT_NONE, "OnBeat Speed"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (InterfPrivate, 1);

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

int lv_interf_cleanup (VisPluginData *plugin)
{
    InterfPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_interf_events (VisPluginData *plugin, VisEventQueue *events)
{
    InterfPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                else if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "nPoints"))
                    priv->nPoints = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "rotation")) {
                    priv->rotation = visual_param_entry_get_integer(param);
                    priv->a = (float)rotation/255*(float)M_PI*2;
                } else if(visual_param_entry_is(param, "distance"))
                    priv->distance = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "alpha"))
                    priv->alpha = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "rotationinc"))
                    priv->rotationinc = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "distance2"))
                    priv->distance2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "alpha2"))
                    priv->alpha2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "rotationinc2"))
                    priv->rotationinc2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "rgb"))
                    priv->rgb = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "speed"))
                    priv->speed = visual_param_entry_get_integer(param);

                priv->status = PI;
                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_interf_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_interf_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    InterfPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    int pnts=priv->nPoints;
    int x,y;
    int i;
    int mask=0;
    float s;

    if (isBeat&0x80000000) return 0;
    if (!priv->enabled) return 0;
    if (pnts == 0) return 0;

    float angle=(float)(2*PI)/pnts;
    
    if (priv->onbeat && isBeat)
        if (priv->status >= PI)
            priv->status=0;

    s = (float)sin(priv->status);
    priv->_rotationinc = priv->rotationinc + (int)((float)(priv->rotationinc2-priv->rotationinc) * s);
    priv->_alpha = priv->alpha + (int)((float)(priv->alpha2-priv->alpha) * s);
    priv->_distance = priv->distance + (int)((float)(priv->distance2-priv->distance) * s);

    a=(float)priv->rotation/255*(float)PI*2;

    int xpoints[MAX_POINTS],ypoints[MAX_POINTS];

    int minx=0, maxx=0;
    int miny=0, maxy=0;

    for (i=0;i<pnts;i++)
    {
        xpoints[i] = (int)(cos(a)*priv->_distance);
        ypoints[i] = (int)(sin(a)*priv->_distance);
        if (ypoints[i] > miny) miny=ypoints[i];
        if (-ypoints[i] > maxy) maxy=-ypoints[i];
        if (xpoints[i] > minx) minx=xpoints[i];
        if (-xpoints[i] > maxx) maxx=-xpoints[i];
        a += angle;
    }

    unsigned char *bt=priv->proxy->blendtable[_alpha];

    int *outp=fbout;
    for (y = 0; y < h; y ++)
    {
        int yp=ypoints[i];
        int yoffs[MAX_POINTS];
        for (i = 0; i < pnts; i ++)
        {
            if (y >= ypoints[i] && y-ypoints[i] < h)
                yoffs[i]=(y-ypoints[i])*w;
            else yoffs[i]=-1;
        }
        if (priv->rgb && (pnts==3 || pnts==6))
        {
            if (pnts == 3) for (x = 0; x < w; x ++)
            {
                int r=0,g=0,b=0;
                int xp;
                xp=x-xpoints[0];
                if (xp >= 0 && xp < w && yoffs[0]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[0]];
                    r=bt[pix&0xff];
                }
                xp=x-xpoints[1];
                if (xp >= 0 && xp < w && yoffs[1]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[1]];
                    g=bt[(pix>>8)&0xff];
                }
                xp=x-xpoints[2];
                if (xp >= 0 && xp < w && yoffs[2]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[2]];
                    b=bt[(pix>>16)&0xff];
                }
                *outp++ = r|(g<<8)|(b<<16);
            }    
            else for (x = 0; x < w; x ++)
            {
                int r=0,g=0,b=0;
                int xp;
                xp=x-xpoints[0];
                if (xp >= 0 && xp < w && yoffs[0]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[0]];
                    r=bt[pix&0xff];
                }
                xp=x-xpoints[1];
                if (xp >= 0 && xp < w && yoffs[1]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[1]];
                    g=bt[(pix>>8)&0xff];
                }
                xp=x-xpoints[2];
                if (xp >= 0 && xp < w && yoffs[2]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[2]];
                    b=bt[(pix>>16)&0xff];
                }
                xp=x-xpoints[3];
                if (xp >= 0 && xp < w && yoffs[3]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[3]];
                    r+=bt[pix&0xff];
                }
                xp=x-xpoints[4];
                if (xp >= 0 && xp < w && yoffs[4]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[4]];
                    g+=bt[(pix>>8)&0xff];
                }
                xp=x-xpoints[5];
                if (xp >= 0 && xp < w && yoffs[5]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[5]];
                    b+=bt[(pix>>16)&0xff];
                }
                if (r > 255) r=255;
                if (g > 255) g=255;
                if (b > 255) b=255;
                *outp++ = r|(g<<8)|(b<<16);
            }    
        }
        else if (y > miny && y < h-maxy && minx+maxx < w) // no y clipping required
        {
            for (x = 0; x < minx; x ++)
            {
                int r=0,g=0,b=0;
                for (i = 0; i < pnts; i++)
                {
                    int xp=x-xpoints[i];
                    if (xp >= 0 && xp < w)
                    {
                        int pix=framebuffer[xp+yoffs[i]];
                        r+=bt[pix&0xff];
                        g+=bt[(pix>>8)&0xff];
                        b+=bt[(pix>>16)&0xff];
                    }
                }
                if (r > 255) r=255;
                if (g > 255) g=255;
                if (b > 255) b=255;
                *outp++ = r|(g<<8)|(b<<16);
            }
            int *lfb=framebuffer+x;
            for (; x < w-maxx; x ++)
            {
                int r=0,g=0,b=0;
                for (i = 0; i < pnts; i++)
                {
                    int pix=lfb[yoffs[i]-xpoints[i]];
                    r+=bt[pix&0xff];
                    g+=bt[(pix>>8)&0xff];
                    b+=bt[(pix>>16)&0xff];
                }
                if (r > 255) r=255;
                if (g > 255) g=255;
                if (b > 255) b=255;
                lfb++;
                *outp++ = r|(g<<8)|(b<<16);
            }
            for (; x < w; x ++)
            {
                int r=0,g=0,b=0;
                for (i = 0; i < pnts; i++)
                {
                    int xp=x-xpoints[i];
                    if (xp >= 0 && xp < w)
                    {
                        int pix=framebuffer[xp+yoffs[i]];
                        r+=bt[pix&0xff];
                        g+=bt[(pix>>8)&0xff];
                        b+=bt[(pix>>16)&0xff];
                    }
                }
                if (r > 255) r=255;
                if (g > 255) g=255;
                if (b > 255) b=255;
                *outp++ = r|(g<<8)|(b<<16);
            }
        }
        else for (x = 0; x < w; x ++)
        {
            int r=0,g=0,b=0;
            for (i = 0; i < pnts; i++)
            {
                int xp=x-xpoints[i];
                if (xp >= 0 && xp < w && yoffs[i]!=-1)
                {
                    int pix=framebuffer[xp+yoffs[i]];
                    r+=bt[pix&0xff];
                    g+=bt[(pix>>8)&0xff];
                    b+=bt[(pix>>16)&0xff];
                }
            }
            if (r > 255) r=255;
            if (g > 255) g=255;
            if (b > 255) b=255;
            *outp++ = r|(g<<8)|(b<<16);
        }
    }

    priv->rotation+=priv->_rotationinc;
    priv->rotation=priv->rotation>255 ? priv->rotation-255 : priv->rotation;
    priv->rotation=priv->rotation<-255 ? priv->rotation+255 : priv->rotation;

    priv->status += priv->speed;
    priv->status=min(priv->status, (float)PI);
    if (priv->status<-PI) priv->status = (float) PI;

    int *p = framebuffer;
    int *d = fbout;

    if (!priv->blend && !priv->blendavg) return 1;
    if (priv->blendavg)
    {
        int i=w*h/4;
        while (i--)
        {
            p[0] = BLEND_AVG(p[0], d[0]);
            p[1] = BLEND_AVG(p[1], d[1]);
            p[2] = BLEND_AVG(p[2], d[2]);
            p[3] = BLEND_AVG(p[3], d[3]);
            p+=4;
            d+=4;
        }
    }
    else
        mmx_addblend_block(p,d,w*h);

    return 0;
}

