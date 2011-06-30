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
};

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsVariable var_x, var_y, var_w, var_h, var_b, var_alpha;

    // params
    char *init, *frame, *beat;
    int blend, subpixel;

    // Others
    int max_d;
    int lastw, lasth;


} ShiftPrivate;

int lv_shift_init (VisPluginData *plugin);
int lv_shift_cleanup (VisPluginData *plugin);
int lv_shift_events (VisPluginData *plugin, VisEventQueue *events);
int lv_shift_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_shift_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_shift_palette,
        .video = lv_shift_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_shift",
        .name = "Libvisual AVS Transform: shift element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: shift element",
        .help = "This is the shift element for the libvisual AVS system",

        .init = lv_shift_init,
        .cleanup = lv_shift_cleanup,
        .events = lv_shift_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_shift_init (VisPluginData *plugin)
{
    ShiftPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_STRING("init", "d=0;", "Initialization code")
        VISUAL_PARAM_LIST_STRING("frame", "x=sin(d)*1.4; y=1.4*cos(d); d=d+0.01;", "Frame code"),
        VISUAL_PARAM_LIST_STRING("beat", "d=d+2.0;", "On-beat code"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("subpixel", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Bilenear filtering"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (ShiftPrivate, 1);

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

    avs_runnable_variable_bind(priv->vm, "x", &priv->var_x);
    avs_runnable_variable_bind(priv->vm, "y", &priv->var_y);
    avs_runnable_variable_bind(priv->vm, "w", &priv->var_w);
    avs_runnable_variable_bind(priv->vm, "h", &priv->var_h);
    avs_runnable_variable_bind(priv->vm, "b", &priv->var_b);
    avs_runnable_variable_bind(priv->vm, "alpha", &priv->var_alpha);

    return 0;
}

int lv_shift_cleanup (VisPluginData *plugin)
{
    ShiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(ShiftPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(ShiftPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_shift_events (VisPluginData *plugin, VisEventQueue *events)
{
    ShiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "init")) {
                    priv->init = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_INIT, priv->pixel);
                    priv->var_x = 0;
                    priv->var_y = 0;
                    priv->var_alpha = 0.5;
                    trans_run_runnable(priv, TRANS_RUNNABLE_INIT);
                } else if(visual_param_entry_is(param, "frame"))
                    priv->frame = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_FRAME, priv->pixel);
                } else if(visual_param_entry_is(param, "beat"))
                    priv->beat = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_BEAT, priv->pixel);
                } else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "subpixel"))
                    priv->subpixel = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_shift_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define NO_MMX

int lv_shift_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    ShiftPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    uint8_t *framebuffer = visual_video_malloc(w*h*video->pitch);
    uint8_t *fbout = visual_video_get_pixels (video);
    int *fbin=framebuffer;
    visual_mem_copy(framebuffer, fbout, w*h*video->pitch);

    priv->var_w=w;
    priv->var_h=h;
    priv->var_b=isBeat?1.0:0.0;

    if (isBeat&0x80000000) return 0;

    if (priv->lasth != h || priv->lastw != w) 
    { 
        priv->lastw=w;
        priv->lasth=h;
        priv->var_x=0; priv->var_y=0; priv->var_alpha=0.5; 
        trans_runnable_run(priv, TRANS_RUNNABLE_INIT);
    }

    trans_runnable_run(priv, TRANS_RUNNABLE_FRAME);
    if (isBeat) trans_runnable_run(priv, TRANS_RUNNABLE_BEAT);

    int doblend=priv->blend;
    int ialpha=127;
    if (doblend)
    {
        ialpha=(int)(*priv->var_alpha*255.0);
        if (ialpha <= 0) return 0;
        if (ialpha >= 255) doblend=0;
    }
    int *inptr=framebuffer;
    int *blendptr=framebuffer;
    int *outptr=fbout;
    int xa=(int)*var_x;
    int ya=(int)*var_y;
  
    // var_x, var_y at this point tell us how to shift, and blend also tell us what to do.
    if (!priv->subpixel) 
    {
        int endy=h+ya;
        int endx=w+xa;
        int x,y;
        if (endx > w) endx=w;
        if (endy > h) endy=h;
        if (ya < 0) inptr += -ya*w;
        if (ya > h) ya=h;
        if (xa > w) xa=w;
        for (y = 0; y < ya; y ++)
        {
            x=w; 
            if (!doblend) while (x--) *outptr++ = 0;
            else while (x--) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
        }
        for (; y < endy; y ++)
        {
            int *ip=inptr;
            if (xa < 0) inptr += -xa;
            if (!doblend)
            {
                for (x = 0; x < xa; x ++) *outptr++=0;
                for (; x < endx; x ++) *outptr++=*inptr++;
                for (; x < w; x ++) *outptr++=0;
            }
            else
            {
                for (x = 0; x < xa; x ++) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
                for (; x < endx; x ++) *outptr++ = BLEND_ADJ(*inptr++,*blendptr++,ialpha);
                for (; x < w; x ++) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
            }
            inptr=ip+w;
        }
        for (; y < h; y ++)
        {   
            x=w; 
            if (!doblend) while (x--) *outptr++ = 0;
            else while (x--) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
        }
    }
    else // bilinear filtering version
    {
    
        int xpart,ypart;

        {
            double vx=*var_x;
            double vy=*var_y;
            xpart=(int) ((vx - (int)vx)*255.0);
            if (xpart < 0) xpart=-xpart;
            else { xa++; xpart=255-xpart; }
            if (xpart < 0) xpart=0;
            if (xpart > 255) xpart=255;

            ypart=(int) ((vy - (int)vy)*255.0);
            if (ypart < 0) ypart=-ypart;
            else { ya++; ypart=255-ypart; }
            if (ypart < 0) ypart=0;
            if (ypart > 255) ypart=255;
        }

        int x,y;
        if (ya < 1-h) ya=1-h;
        if (xa < 1-w) xa=1-w;
        if (ya > h-1) ya=h-1;
        if (xa > w-1) xa=w-1;
        if (ya < 0) inptr += -ya*w;
        int endy=h-1+ya;
        int endx=w-1+xa;
        if (endx > w-1) endx=w-1;
        if (endy > h-1) endy=h-1;
        if (endx < 0) endx=0;
        if (endy < 0) endy=0;
        for (y = 0; y < ya; y ++)
        {
            x=w; 
            if (!doblend) while (x--) *outptr++ = 0;
            else while (x--) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
        }
        for (; y < endy; y ++)
        {
            int *ip=inptr;
            if (xa < 0) inptr += -xa;
            if (!doblend)
            {
                for (x = 0; x < xa; x ++) *outptr++=0;
                for (; x < endx; x ++) *outptr++=BLEND4((unsigned int *)inptr++,w,xpart,ypart);
                for (; x < w; x ++) *outptr++=0;
            }
            else
            {
                for (x = 0; x < xa; x ++) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
                for (; x < endx; x ++) *outptr++ = BLEND_ADJ(BLEND4((unsigned int *)inptr++,w,xpart,ypart),*blendptr++,ialpha);
                for (; x < w; x ++) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
            }
            inptr=ip+w;
        }
        for (; y < h; y ++)
        {   
            x=w; 
            if (!doblend) while (x--) *outptr++ = 0;
            else while (x--) *outptr++ = BLEND_ADJ(0,*blendptr++,ialpha);
        }
    }
#ifndef NO_MMX
    __asm emms;
#endif

    visual_mem_free(framebuffer);

    return 0;
}

