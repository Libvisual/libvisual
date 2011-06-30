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

// Integer Square Root function

// Uses factoring to find square root
// A 256 entry table used to work out the square root of the 7 or 8 most
// significant bits.  A power of 2 used to approximate the rest.
// Based on an 80386 Assembly implementation by Arne Steinarson

static unsigned const char sq_table[]=
{0, 16, 22, 27, 32, 35, 39, 42, 45, 48, 50, 53, 55, 57, 59, 61, 64, 65,
67, 69, 71, 73, 75, 76, 78, 80, 81, 83, 84, 86, 87, 89, 90, 91, 93, 94,
96, 97, 98, 99, 101, 102, 103, 104, 106, 107, 108, 109, 110, 112, 113,
114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 128,
128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141,
142, 143, 144, 144, 145, 146, 147, 148, 149, 150, 150, 151, 152, 153,
154, 155, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 163, 164,
165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175,
176, 176, 177, 178, 178, 179, 180, 181, 181, 182, 183, 183, 184, 185,
185, 186, 187, 187, 188, 189, 189, 190, 191, 192, 192, 193, 193, 194,
195, 195, 196, 197, 197, 198, 199, 199, 200, 201, 201, 202, 203, 203,
204, 204, 205, 206, 206, 207, 208, 208, 209, 209, 210, 211, 211, 212,
212, 213, 214, 214, 215, 215, 216, 217, 217, 218, 218, 219, 219, 220,
221, 221, 222, 222, 223, 224, 224, 225, 225, 226, 226, 227, 227, 228,
229, 229, 230, 230, 231, 231, 232, 232, 233, 234, 234, 235, 235, 236,
236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242, 243, 243,
244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 250, 250,
251, 251, 252, 252, 253, 253, 254, 254, 255};

static __inline unsigned long isqrt(unsigned long n)
{
  if (n >= 0x10000)
    if (n >= 0x1000000)
      if (n >= 0x10000000)
        if (n >= 0x40000000) return(sq_table[n >> 24] << 8);
    else                 return(sq_table[n >> 22] << 7);
      else
    if (n >= 0x4000000)  return(sq_table[n >> 20] << 6);
    else                 return(sq_table[n >> 18] << 5);
    else
      if (n >= 0x100000)
    if (n >= 0x400000)   return(sq_table[n >> 16] << 4);
    else                 return(sq_table[n >> 14] << 3);
      else
    if (n >= 0x40000)    return(sq_table[n >> 12] << 2);
    else                 return(sq_table[n >> 10] << 1);

  else
   if (n >= 0x100)
      if (n >= 0x1000)
        if (n >= 0x4000)     return(sq_table[n >> 8]);
    else                 return(sq_table[n >> 6] >> 1);
      else
    if (n >= 0x400)      return(sq_table[n >> 4] >> 2);
    else                 return(sq_table[n >> 2] >> 3);
    else
      if (n >= 0x10)
    if (n >= 0x40)       return(sq_table[n]      >> 4);
    else                 return(sq_table[n << 2] << 5);
      else
    if (n >= 0x4)        return(sq_table[n >> 4] << 6);
    else                 return(sq_table[n >> 6] << 7);

}

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
    char *init, *frame, *beat, *pixel;

    // Others
    int wt, lastw, lasth, tab, subpixel, blend;
    int *wmul;
    double max_d;


} DDMPrivate;

int lv_ddm_init (VisPluginData *plugin);
int lv_ddm_cleanup (VisPluginData *plugin);
int lv_ddm_events (VisPluginData *plugin, VisEventQueue *events);
int lv_ddm_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_ddm_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_ddm_palette,
        .video = lv_ddm_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_ddm",
        .name = "Libvisual AVS Transform: ddm element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: ddm element",
        .help = "This is the ddm element for the libvisual AVS system",

        .init = lv_ddm_init,
        .cleanup = lv_ddm_cleanup,
        .events = lv_ddm_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_ddm_init (VisPluginData *plugin)
{
    DDMPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_STRING("init", "", "u=1;t=0"),
        VISUAL_PARAM_LIST_STRING("frame", "", "t=t+u;t=min(100,t);t=max(0,t);u=if(equal(t,100),-1,u);u=if(equal(t,0),1,u)"),
        VISUAL_PARAM_LIST_STRING("beat", "", ""),
        VISUAL_PARAM_LIST_STRING("pixel", "", "d=d-sigmoid((t-50)/100,2)"),
        VISUAL_PARAM_LIST_INTEGER("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_INTEGER("subpixel", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Bilenear filtering"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (DDMPrivate, 1);

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

int lv_ddm_cleanup (VisPluginData *plugin)
{
    DDMPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(DDMPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(DDMPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_ddm_events (VisPluginData *plugin, VisEventQueue *events)
{
    DDMPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "init")) {
                    priv->init = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_INIT, priv->pixel);
                    trans_run_runnable(priv, TRANS_RUNNABLE_INIT);
                } else if(visual_param_entry_is(param, "frame"))
                    priv->frame = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_FRAME, priv->pixel);
                } else if(visual_param_entry_is(param, "beat"))
                    priv->beat = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_BEAT, priv->pixel);
                } else if(visual_param_entry_is(param, "pixel"))
                    priv->pixel = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_PIXEL, priv->pixel);
                else if(visual_param_entry_is(param, "blend"))
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

int lv_ddm_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_ddm_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    DDMPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;

    int *fbin=framebuffer;
    if (priv->lasth != h || priv->lastw != w || priv->tab || priv->wmul)
    {
        int y;
        priv->lastw=w;
        priv->lasth=h;
        priv->max_d=sqrt((w*w+h*h)/4.0);
        if (priv->wmul) visual_mem_free(priv->wmul)
        priv->wmul=visual_mem_malloc(sizeof(int)*h);
        for (y = 0; y < h; y ++) priv->wmul[y]=y*w;
        if (priv->tab) visual_mem_free(priv->tab);
        m_tab=NULL;
    }
    int imax_d=(int)(priv->max_d + 32.9);

    if (imax_d < 33) imax_d=33;

    if (!priv->tab)
        priv->tab=visual_mem_malloc0(sizeof(int)*imax_d);

    int x;

    if (isBeat&0x80000000) return 0;

    priv->var_b=isBeat?1.0:0.0;

    executeCode(codehandle[1],visdata);
    trans_runnable_execute(priv, TRANS_RUNNABLE_FRAME);
    if (isBeat) trans_runnable_execute(priv, TRANS_RUNNABLE_BEAT);
    for (x = 0; x < imax_d-32; x ++)
    {
        priv->var_d=x/(priv->max_d-1);
        trans_runnable_execute(priv, TRANS_RUNNABLE_PIXEL);
        priv->tab[x]=(int) (priv->var_d*256.0*priv->max_d/(x+1));
    }
    for (; x < imax_d; x ++)
    {
        priv->tab[x]=priv->tab[x-1];
    }

    priv->wt++;
    priv->wt&=63;

    {
        int w2=w/2;
        int h2=h/2;
        int y;
        for (y = 0; y < h; y ++)
        {
            int ty=y-h2;
            int x2=w2*w2+w2+ty*ty+256;
            int dx2=-2*w2;
            int yysc=ty;
            int xxsc=-w2;
            int x=w;
            if (priv->subpixel)
            {
                if (priv->blend)
                    while (x--)
                    {
                        int qd=priv->tab[isqrt(x2)];
                        int ow,oh;
                        int xpart,ypart;
                        x2+=dx2;
                        dx2+=2;
                        xpart=(qd*xxsc+128);
                        ypart=(qd*yysc+128);
                        ow = w2 + (xpart>>8);
                        oh = h2 + (ypart>>8);
                        xpart&=0xff;
                        ypart&=0xff;
                        xxsc++;
        
                        if (ow < 0) ow=0;
                        else if (ow >= w-1) ow=w-2;
                        if (oh < 0) oh=0;
                        else if (oh >= h-1) oh=h-2;

                        *fbout++=BLEND_AVG(BLEND4((unsigned int *)framebuffer+ow+priv->wmul[oh],w,xpart,ypart),*fbin++);
                    }
                    else
                        while (x--)
                        {
                            int qd=priv->tab[isqrt(x2)];
                            int ow,oh;
                            int xpart,ypart;
                            x2+=dx2;
                            dx2+=2;
                            xpart=(qd*xxsc+128);
                            ypart=(qd*yysc+128);
                            ow = w2 + (xpart>>8);
                            oh = h2 + (ypart>>8);
                            xpart&=0xff;
                            ypart&=0xff;
                            xxsc++;
        
                            if (ow < 0) ow=0;
                            else if (ow >= w-1) ow=w-2;
                            if (oh < 0) oh=0;
                            else if (oh >= h-1) oh=h-2;

                            *fbout++=BLEND4((unsigned int *)framebuffer+ow+priv->wmul[oh],w,xpart,ypart);
                        }
            }
            else
            {
                if (priv->blend) {
                    while (x--)
                    {
                        int qd=priv->tab[isqrt(x2)];
                        int ow,oh;
                        x2+=dx2;
                        dx2+=2;
                        ow = w2 + ((qd*xxsc+128)>>8);
                        xxsc++;
                        oh = h2 + ((qd*yysc+128)>>8);
    
                        if (ow < 0) ow=0;
                        else if (ow >= w) ow=w-1;
                        if (oh < 0) oh=0;
                        else if (oh >= h) oh=h-1;
    
                        *fbout++=BLEND_AVG(framebuffer[ow+priv->wmul[oh]],*fbin++);
                    }
                } else {
                    while (x--)
                    {
                        int qd=priv->tab[isqrt(x2)];
                        int ow,oh;
                        x2+=dx2;
                        dx2+=2;
                        ow = w2 + ((qd*xxsc+128)>>8);
                        xxsc++;
                        oh = h2 + ((qd*yysc+128)>>8);

                        if (ow < 0) ow=0;
                        else if (ow >= w) ow=w-1;
                        if (oh < 0) oh=0;
                        else if (oh >= h) oh=h-1;

                        *fbout++=framebuffer[ow+priv->wmul[oh]];
                    }
                }
            } 
        }
    }
#ifdef HAVE_MMX
  //if (subpixel) __asm emms;
#endif

    return 0;
}

