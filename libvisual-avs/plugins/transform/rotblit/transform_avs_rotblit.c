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

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsVariable var_d, var_b;

    // params
    int zoom_scale, rot_dir, blend, beatch, beatch_speed, zoom_scale2, beatch_scale, subpixel;

    // Others
    int rot_rev;
    int scale_fpos;
    double rot_rev_pos;
    int l_w, l_h;
    int *mul;

} RotblitPrivate;

int lv_rotblit_init (VisPluginData *plugin);
int lv_rotblit_cleanup (VisPluginData *plugin);
int lv_rotblit_events (VisPluginData *plugin, VisEventQueue *events);
int lv_rotblit_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_rotblit_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_rotblit_palette,
        .video = lv_rotblit_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_rotblit",
        .name = "Libvisual AVS Transform: rotblit element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: rotblit element",
        .help = "This is the rotblit element for the libvisual AVS system",

        .init = lv_rotblit_init,
        .cleanup = lv_rotblit_cleanup,
        .events = lv_rotblit_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_rotblit_init (VisPluginData *plugin)
{
    RotblitPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("zoom_scale", 31, VISUAL_PARAM_LIMIT_BOOLEAN, "Zoom scale"),
        VISUAL_PARAM_LIST_INTEGER("rot_dir", 31, VISUAL_PARAM_LIMIT_BOOLEAN, "Rotate right/left"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend blitter"),
        VISUAL_PARAM_LIST_INTEGER("beatch", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable on-beat change"),
        VISUAL_PARAM_LIST_INTEGER("beatch_speed", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "On-beat speed"),
        VISUAL_PARAM_LIST_INTEGER("zoom_scale2", 31, VISUAL_PARAM_LIMIT_BOOLEAN, "Zoom scale 2"),
        VISUAL_PARAM_LIST_INTEGER("beatch_scale", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "On beat scale"),
        VISUAL_PARAM_LIST_INTEGER("subpixel", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Bilinear filtering"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (RotblitPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    priv->rot_rev = 1;
    priv->rot_rev_pos = 1.0;
    priv->mul = NULL;
    priv->scale_fpos = 31;

    return 0;
}

int lv_rotblit_cleanup (VisPluginData *plugin)
{
    RotblitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_rotblit_events (VisPluginData *plugin, VisEventQueue *events)
{
    RotblitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "zoom_scale"))
                    priv->zoom_scale = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "rot_dir"))
                    priv->rot_dir = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "beatch"))
                    priv->beatch = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "beatch_speed"))
                    priv->beatch_speed = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "zoom_scale2"))
                    priv->zoom_scale2 = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "beatch_scale"))
                    priv->beatch_scale = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "subpixel"))
                    priv->subpixel = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_rotblit_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define NO_MMX

int lv_rotblit_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    RotblitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    int y;

    if (priv->l_w != w || priv->l_h != h || !priv->mul) // generate width table
    {
        int x;
        if (priv->mul != NULL) visual_mem_free(priv->mul);
        priv->l_w=w;
        priv->l_h=h;
        priv->mul=(int *)visual_mem_malloc0(sizeof(int)*h);
        for (x = 0; x < h; x ++)
            priv->mul[x]=x*w;
    }
    if (isBeat&0x80000000) return 0;

    unsigned int *dest=(unsigned int *) fbout;
    unsigned int *src=(unsigned int *) framebuffer;
    unsigned int *bdest=(unsigned int *) framebuffer;

    if (isBeat && priv->beatch) 
    {
        rot_rev=-rot_rev;
    }
  
    if (!priv->beatch) priv->rot_rev=1;
    
    priv->rot_rev_pos+=(1.0/(1+priv->beatch_speed*4))*(priv->rot_rev-priv->rot_rev_pos);

    if (priv->rot_rev_pos > priv->rot_rev && priv->rot_rev>0) priv->rot_rev_pos=priv->rot_rev;
    if (priv->rot_rev_pos < priv->rot_rev && priv->rot_rev<0) priv->rot_rev_pos=priv->rot_rev;


    if (priv->isBeat && priv->beatch_scale) 
    {
        priv->scale_fpos=priv->zoom_scale2;
    }

    int f_val;
    if (priv->zoom_scale < priv->zoom_scale2) 
    {
        f_val=max(priv->scale_fpos,priv->zoom_scale); 
        if (priv->scale_fpos > priv->zoom_scale) priv->scale_fpos -= 3;
    }
    else 
    {
        f_val=min(priv->scale_fpos,priv->zoom_scale);
        if (priv->scale_fpos < priv->zoom_scale) priv->scale_fpos+=3;
    }
  
    double zoom = 1.0 + (f_val-31)/31.0;
    
    double theta=((priv->rot_dir-32))*priv->rot_rev_pos;
    double temp;
    int ds_dx, dt_dx, ds_dy, dt_dy, s, t, sstart, tstart;
    int x, offset=0;

    temp = cos((theta)*M_PI/180.0)*priv->zoom;
    ds_dx = (int) (temp*65536.0);
    dt_dy = (int) (temp*65536.0);
    temp = sin((theta)*M_PI/180.0)*priv->zoom;
    ds_dy = - (int) (temp*65536.0);
    dt_dx = (int) (temp*65536.0);
  
    s = sstart = -(((w-1)/2)*ds_dx + ((h-1)/2)*ds_dy) + (w-1)*(32768 + (1<<20));
    t = tstart = -(((w-1)/2)*dt_dx + ((h-1)/2)*dt_dy) + (h-1)*(32768 + (1<<20));
    int ds, dt;
    ds = (w-1)<<16;
    dt = (h-1)<<16;
    y = h;

    if (ds_dx <= -ds || ds_dx >= ds || dt_dx <= -dt || dt_dx >= dt);
    else while (y--) 
    {
        if (ds) s %= ds;
        if (dt) t %= dt;
        if (s < 0) s+=ds; if (t < 0) t+=dt;
        x = w;
        offset = y*w;


#define DO_LOOP(Z) while (x--) { Z; s += ds_dx; t += dt_dx; }
#define DO_LOOPS(Z) \
            if (ds_dx <= 0 && dt_dx <= 0) DO_LOOP(if (t < 0) t += dt; if (s < 0) s += ds; Z)  \
        else if (ds_dx <= 0) DO_LOOP(if (t >= dt) t -= dt; if (s < 0) s += ds; Z) \
            else if (dt_dx <= 0) DO_LOOP(if (t < 0) t += dt; if (s >= ds) s -= ds; Z) \
        else DO_LOOP(if (t >= dt) t -= dt; if (s >= ds) s -= ds; Z)


        if (priv->subpixel && priv->blend) DO_LOOPS(*dest++ = BLEND_AVG(*bdest++,BLEND4_16(src+(s>>16)+priv->mul[t>>16],w,s,t)))
        else if (priv->subpixel) DO_LOOPS(*dest++ = BLEND4_16(src+(s>>16)+priv->mul[t>>16],w,s,t))
        else if (!priv->blend) DO_LOOPS(*dest++ = src[(s>>16)+priv->mul[t>>16]])
        else DO_LOOPS(*dest++ = BLEND_AVG(*bdest++,src[(s>>16)+priv->mul[t>>16]]))

        s = (sstart += ds_dy);
        t = (tstart += dt_dy);
    }
#ifndef NO_MMX
  __asm emms;
#endif

    return 0;
}

