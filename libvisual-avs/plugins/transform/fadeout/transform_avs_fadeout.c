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

    uint8_t fadtab[3][256];
    int fadelen, color;
} FadeoutPrivate;

int lv_fadeout_init (VisPluginData *plugin);
int lv_fadeout_cleanup (VisPluginData *plugin);
int lv_fadeout_events (VisPluginData *plugin, VisEventQueue *events);
int lv_fadeout_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_fadeout_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_fadeout_palette,
        .video = lv_fadeout_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_fadeout",
        .name = "Libvisual AVS Transform: fadeout element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: fadeout element",
        .help = "This is the fadeout element for the libvisual AVS system",

        .init = lv_fadeout_init,
        .cleanup = lv_fadeout_cleanup,
        .events = lv_fadeout_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_fadeout_init (VisPluginData *plugin)
{
    FadeoutPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("fadelen", 0, VISUAL_PARAM_LIMIT_NONE, "Fade velocity"),
        VISUAL_PARAM_LIST_INTEGER("color", 0, VISUAL_PARAM_LIMIT_NONE, "Fade to color"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (FadeoutPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    return 0;
}

int lv_fadeout_cleanup (VisPluginData *plugin)
{
    FadeoutPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

void trans_maketab(FadeoutPrivate *priv)
{
  int rseek=priv->color&0xff;
  int gseek=(priv->color>>8)&0xff;
  int bseek=(priv->color>>16)&0xff;
  int x;
  for (x = 0; x < 256; x ++)
  {
    int r=x;
    int g=x;
    int b=x;
    if (r <= rseek-priv->fadelen) r+=priv->fadelen;
    else if (r >= rseek+priv->fadelen) r-=priv->fadelen;
    else r=rseek;

    if (g <= gseek-priv->fadelen) g+=priv->fadelen;
    else if (g >= gseek+priv->fadelen) g-=priv->fadelen;
    else g=gseek;
    if (b <= bseek-priv->fadelen) b+=priv->fadelen;
    else if (b >= bseek+priv->fadelen) b-=priv->fadelen;
    else b=bseek;

    priv->fadtab[0][x]=r;
    priv->fadtab[1][x]=g;
    priv->fadtab[2][x]=b;
  }
}

int lv_fadeout_events (VisPluginData *plugin, VisEventQueue *events)
{
    FadeoutPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "fadelen"))
                    priv->fadelen = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "color"))
                    priv->color = visual_param_entry_get_integer(param);

                trans_maketab(priv);
                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_fadeout_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define NO_MMX

int lv_fadeout_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    FadeoutPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

  if (isBeat&0x80000000) return 0;
  if (!priv->fadelen) return 0;
    if (
#ifdef NO_MMX
    1
#else   
    color
#endif
    )
    {
      unsigned char *t=(unsigned char *)framebuffer;
      int x=w*h;
      while (x--)
      {
          t[0]=priv->fadtab[0][t[0]];
          t[1]=priv->fadtab[1][t[1]];
          t[2]=priv->fadtab[2][t[2]];
          t+=4;
      }
  }
#ifndef NO_MMX
  else
  {
        int l=(w*h);
        char fadj[8];
        int x;
    unsigned char *t=fadtab[0];
        for (x = 0; x < 8; x ++) fadj[x]=this->fadelen;
        __asm 
        {
            mov edx, l
            mov edi, framebuffer
            movq mm7, [fadj]
            shr edx, 3
      align 16
        _l1:
            movq mm0, [edi]

            movq mm1, [edi+8]

            movq mm2, [edi+16]
            psubusb mm0, mm7

            movq mm3, [edi+24]
            psubusb mm1, mm7

            movq [edi], mm0
            psubusb mm2, mm7

            movq [edi+8], mm1
            psubusb mm3, mm7

            movq [edi+16], mm2

            movq [edi+24], mm3

            add edi, 8*4

            dec edx
            jnz _l1

            mov edx, l
            sub eax, eax

            and edx, 7
            jz _l3

            sub ebx, ebx
            sub ecx, ecx

            mov esi, t
        _l2:
            mov al, [edi]
            mov bl, [edi+1]
            mov cl, [edi+2]
            sub al, [esi+eax]
            sub bl, [esi+ebx]
            sub cl, [esi+ecx]
            mov [edi], al
            mov [edi+1], bl
            mov [edi+2], cl
            add edi, 4
            dec edx
            jnz _l2
        _l3:
            emms
        }
  }
#endif
    timingLeave(1);
  return 0;


    return 0;
}

