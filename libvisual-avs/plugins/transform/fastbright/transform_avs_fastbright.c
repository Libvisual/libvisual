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


typedef struct {
    AVSGlobalProxy *proxy;

    int tab[3][256];
    int dir;

} FastbrightPrivate;

int lv_fastbright_init (VisPluginData *plugin);
int lv_fastbright_cleanup (VisPluginData *plugin);
int lv_fastbright_events (VisPluginData *plugin, VisEventQueue *events);
int lv_fastbright_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_fastbright_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_fastbright_palette,
        .video = lv_fastbright_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_fastbright",
        .name = "Libvisual AVS Transform: fastbright element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: fastbright element",
        .help = "This is the fastbright element for the libvisual AVS system",

        .init = lv_fastbright_init,
        .cleanup = lv_fastbright_cleanup,
        .events = lv_fastbright_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_fastbright_init (VisPluginData *plugin)
{
    FastbrightPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("dir", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 2), "Brighten/Darken/Do nothing"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (FastbrightPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    int x;
    for(x = 0; x < 128; x++)
    {
        priv->tab[0][x]=x+x;
        priv->tab[1][x]=x<<9;
        priv->tab[2][x]=x<<17;
    }
    for(; x < 256; x++)
    {
        priv->tab[0][x]=255;
        priv->tab[1][x]=255<<8;
        priv->tab[2][x]=255<<16;
    }

    return 0;
}

int lv_fastbright_cleanup (VisPluginData *plugin)
{
    FastbrightPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_fastbright_events (VisPluginData *plugin, VisEventQueue *events)
{
    FastbrightPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "dir"))
                    priv->dir = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_fastbright_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define NO_MMX

int lv_fastbright_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    FastbrightPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;
    if (isBeat&0x80000000) return 0;
#ifdef NO_MMX // the non mmx x2 version really isn't any , in terms faster than normal brightness with no exclusions turned on
    {
        unsigned int *t=(unsigned int *)framebuffer;
        int x;
        unsigned int mask = 0x7F7F7F7F;

        x=w*h/2;
        if (dir == 0)
            while (x--)
            {
                unsigned int v1=t[0];
                unsigned int v2=t[1];
                v1=tab[0][v1&0xff]|tab[1][(v1>>8)&0xff]|tab[2][(v1>>16)&0xff]|(v1&0xff000000);
                v2=tab[0][v2&0xff]|tab[1][(v2>>8)&0xff]|tab[2][(v2>>16)&0xff]|(v2&0xff000000);
                t[0]=v1;
                t[1]=v2;
                t+=2;
            }
        else if (dir == 1)
            while (x--)
            {
                unsigned int v1=t[0]>>1;
                unsigned int v2=t[1]>>1;
                t[0]=v1&mask;
                t[1]=v2&mask;
                t+=2;
            }
    }
#else
    int mask[2] =
    {
        0x7F7F7F7F,
        0x7F7F7F7F,
    };
    int l=(w*h);
    if (dir == 0) __asm 
        {
            mov edx, l
            mov edi, framebuffer
      shr edx, 3 // 8 pixels at a time
      align 16
        _l1:
            movq mm0, [edi]
            movq mm1, [edi+8]
            movq mm2, [edi+16]
      paddusb mm0, mm0
            movq mm3, [edi+24]
      paddusb mm1, mm1
      paddusb mm2, mm2
      movq [edi], mm0
      paddusb mm3, mm3
      movq [edi+8], mm1
      movq [edi+16], mm2
      movq [edi+24], mm3

      add edi, 32

            dec edx
            jnz _l1

            mov edx, l
            and edx, 7
      shr edx, 1 // up the last 7 pixels (two at a time)
            jz _l3

        _l2:
            movq mm3, [edi]
      paddusb mm3, mm3
      movq [edi], mm3
            add edi, 8
            dec edx
            jnz _l2
        _l3:
            emms
        }
    else if (dir == 1) __asm 
        {
            mov edx, l
      movq mm7, [mask]
            mov edi, framebuffer
      shr edx, 3 // 8 pixels at a time
      align 16
        _lr1:
            movq mm0, [edi]
            movq mm1, [edi+8]

            movq mm2, [edi+16]
      psrl mm0, 1

            movq mm3, [edi+24]
      pand mm0, mm7
      
      psrl mm1, 1
      movq [edi], mm0

      psrl mm2, 1
      pand mm1, mm7

      movq [edi+8], mm1
      pand mm2, mm7

      psrl mm3, 1
      movq [edi+16], mm2

      pand mm3, mm7
      movq [edi+24], mm3

      add edi, 32

            dec edx
            jnz _lr1

            mov edx, l
            and edx, 7
      shr edx, 1 // up the last 7 pixels (two at a time)
            jz _lr3

        _lr2:
            movq mm3, [edi]
      psrl mm3, 1
      pand mm3, mm7
      movq [edi], mm3
            add edi, 8
            dec edx
            jnz _lr2
        _lr3:
            emms
        }
#endif
  return 0;
}

