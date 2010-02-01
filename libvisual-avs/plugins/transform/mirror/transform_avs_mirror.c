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

#define HORIZONTAL1 1
#define HORIZONTAL2 2
#define VERTICAL1   4
#define VERTICAL2   8

#define D_HORIZONTAL1 0
#define D_HORIZONTAL2 8
#define D_VERTICAL1   16
#define D_VERTICAL2   24
#define M_HORIZONTAL1 0xFF
#define M_HORIZONTAL2 0xFF00
#define M_VERTICAL1   0xFF0000
#define M_VERTICAL2   0xFF000000

typedef enum trans_runnable TransRunnable;

typedef struct {
    AVSGlobalProxy *proxy;

    // params
    int enabled;
    int mode;
    int onbeat;
    int smooth;
    int slower;

    // Others
    int rbeat;


} MirrorPrivate;

int lv_mirror_init (VisPluginData *plugin);
int lv_mirror_cleanup (VisPluginData *plugin);
int lv_mirror_events (VisPluginData *plugin, VisEventQueue *events);
int lv_mirror_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_mirror_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_mirror_palette,
        .video = lv_mirror_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_mirror",
        .name = "Libvisual AVS Transform: mirror element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: mirror element",
        .help = "This is the mirror element for the libvisual AVS system",

        .init = lv_mirror_init,
        .cleanup = lv_mirror_cleanup,
        .events = lv_mirror_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_mirror_init (VisPluginData *plugin)
{
    MirrorPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Mirror effect"),
        VISUAL_PARAM_LIST_INTEGER("mode", HORIZONTAL1, VISUAL_PARAM_LIMIT_NONE, "Mode"),
        VISUAL_PARAM_LIST_INTEGER("onbeat", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable onbeat"),
        VISUAL_PARAM_LIST_INTEGER("smooth", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable smooth transitions"),
        VISUAL_PARAM_LIST_INTEGER("slower", 4, VISUAL_PARAM_LIMIT_NONE, "Faster/Slower"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (MirrorPrivate, 1);

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

int lv_mirror_cleanup (VisPluginData *plugin)
{
    MirrorPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(MirrorPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(MirrorPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_mirror_events (VisPluginData *plugin, VisEventQueue *events)
{
    MirrorPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                else if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "mode"))
                    priv->mode = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "smooth"))
                    priv->smooth = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "slower"))
                    priv->slower = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_mirror_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

static unsigned int __inline BLEND_ADAPT(unsigned int a, unsigned int b, /*float*/int divisor)
{
    return ((((a >> 4) & 0x0F0F0F) * (16-divisor) + (((b >> 4) & 0x0F0F0F) * divisor)));
}

int lv_mirror_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    MirrorPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

    int hi,t,j,halfw,halfh,m,d;
    int *thismode=&priv->mode;
    static int lastMode;
    static int divisors=0;
    static int inc=0;
    int divis;
    int *fbp;

    if (isBeat&0x80000000) return 0;
    if (!priv->enabled) return 0;
    
    t=w*h;
    halfw = w / 2 ;
    halfh = h / 2 ;

    if (priv->onbeat)
    {
        if (isBeat)   rbeat = (rand()%16) & priv->mode;
        thismode=&rbeat;
    }

    if (*thismode != priv->lastMode)
    {
        int dif = *thismode ^ priv->lastMode;
        int i;
        for (i=1,m=0xFF,d=0;i<16;i<<=1,m<<=8,d+=8)
            if (dif & i) 
            {
                inc = (inc & ~m) | ((priv->lastMode & i) ? 0xFF : 1) << d;
                if (!(divisors & m)) divisors = (divisors & ~m) | ((priv->lastMode & i) ? 16 : 1) << d;
            }
            priv->lastMode = *thismode;
    }

    fbp=framebuffer;
    if (*thismode & VERTICAL1 || (priv->smooth && (divisors & 0x00FF0000)))
    {
        divis = (divisors & M_VERTICAL1) >> D_VERTICAL1;
        for ( hi=0 ; hi < h ; hi++) 
        {
            if (priv->smooth && divis)
            {
                int *tmp=fbp+w-1;
                int n=halfw;
                while (n--) *tmp-- = BLEND_ADAPT(*tmp, *fbp++, divis); 
            }
            else
            {
                int *tmp=fbp+w-1;
                int n=halfw;
                while (n--) *tmp-- = *fbp++;
            }
            fbp+=halfw;
        }
    }
    fbp=framebuffer;
    if (*thismode & VERTICAL2 || (priv->smooth && (divisors & 0xFF000000)))
    {
        divis = (divisors & M_VERTICAL2) >> D_VERTICAL2;
        for ( hi=0 ; hi < h ; hi++)
        {
            if (priv->smooth && divis)
            {
                int *tmp=fbp+w-1;
                int n=halfw;
                while (n--) *fbp++ = BLEND_ADAPT(*fbp,*tmp--,divis);
            }
            else
            {
                int *tmp=fbp+w-1;
                int n=halfw;
                while (n--) *fbp++ = *tmp--;
            }
            fbp+=halfw;
        }
    }
    fbp=framebuffer;
    j=t-w;
    if (*thismode & HORIZONTAL1 || (priv->smooth && (divisors & 0x000000FF)))
    {
        divis = (divisors & M_HORIZONTAL1) >> D_HORIZONTAL1;
        for ( hi=0 ; hi < halfh ; hi++) 
        {
            if (priv->smooth && divis) 
            {
                int n=w;
                while (n--) fbp++[j]=BLEND_ADAPT(fbp[j], *fbp, divis); 
            }
            else 
            {
                memcpy(fbp+j,fbp,w*sizeof(int));
                fbp+=w;
            }
            j-=2*w;
        }
    }
    fbp=framebuffer;
    j=t-w;
    if (*thismode & HORIZONTAL2 || (priv->smooth && (divisors & 0x0000FF00)))
    {
        divis = (divisors & M_HORIZONTAL2) >> D_HORIZONTAL2;
        for ( hi=0 ; hi < halfh ; hi++) 
        {
            if (priv->smooth && divis) 
            {
                int n=w;
                while (n--)
                    *fbp++ = BLEND_ADAPT(*fbp, fbp[j], divis); 
            }
            else 
            {
                memcpy(fbp,fbp+j,w*sizeof(int));
                fbp+=w;
            }
            j-=2*w;
        }
    }

    if (priv->smooth && !(++framecount % priv->slower))
    {
        int i;
        for (i=1,m=0xFF,d=0;i<16;i<<=1,m<<=8,d+=8)
        {
            if (divisors & m)
                divisors = (divisors & ~m) | ((((divisors & m) >> d) + (unsigned char)((inc & m) >> d)) % 16) << d;
        }
    }

    return 0;
}

