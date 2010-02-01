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

typedef struct {
    AVSGlobalProxy *proxy;

    // Params
    int enabled;

    // Others
    uint32_t *lastframe;
    int lastframe_len;

} WaterPrivate;

int lv_water_init (VisPluginData *plugin);
int lv_water_cleanup (VisPluginData *plugin);
int lv_water_events (VisPluginData *plugin, VisEventQueue *events);
int lv_water_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_water_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_water_palette,
        .video = lv_water_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_water",
        .name = "Libvisual AVS Transform: water element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: water element",
        .help = "This is the water element for the libvisual AVS system",

        .init = lv_water_init,
        .cleanup = lv_water_cleanup,
        .events = lv_water_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_water_init (VisPluginData *plugin)
{
    WaterPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable water effect"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (WaterPrivate, 1);

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

int lv_water_cleanup (VisPluginData *plugin)
{
    WaterPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_water_events (VisPluginData *plugin, VisEventQueue *events)
{
    WaterPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled")) {
                    priv->enabled = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_water_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define _R(x) (( x ) & 0xff)
#define _G(x) ((( x )) & 0xff00)
#define _B(x) ((( x )) & 0xff0000)
#define _RGB(r,g,b) (( r ) | (( g ) & 0xff00) | (( b ) & 0xff0000))

int trans_render(WaterPrivate *priv, uint8_t *framebuffer, uint8_t *fbout, int w, int h, int isBeat);
int trans_begin(WaterPrivate *priv, uint8_t *fbin, uint8_t *fbout, int w, int h, int isBeat);

int lv_water_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    WaterPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    uint8_t *framebuffer = visual_mem_malloc0(w * h * video->pitch);
    uint8_t fbout = visual_video_get_pixels (video);
    int *fbin=framebuffer;
    visual_mem_copy(framebuffer, fbout, w * h * video->pitch);

    trans_begin(priv, fbin, fbout, w, h, isBeat);

    if(isBeat & 0x80000000) return 0;

    trans_render(0,1,visdata,isBeat,framebuffer,fbout,w,h);

    return 0;
}

int trans_begin(WaterPrivate *priv, uint8_t *fbin, uint8_t *fbout, int w, int h, int isBeat)
{
  if (!enabled) return 0;

  if (!lastframe || w*h != lastframe_len)
  {
    if (lastframe) GlobalFree(lastframe);
    lastframe_len=w*h;
    lastframe=(unsigned int *)GlobalAlloc(GPTR,w*h*sizeof(int));
  }

  return max_threads;
}

#define NO_MMX

int trans_render(WaterPrivate *priv, uint8_t *framebuffer, uint8_t *fbout, int w, int h, int isBeat)
{
    if (!enabled) return;

    unsigned int *f = (unsigned int *) framebuffer;
    unsigned int *of = (unsigned int *) fbout;
    unsigned int *lfo = (unsigned int *) priv->lastframe;

    {

        // top line
        {
            int x;
    
            // left edge
            {
                int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                f++;

                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;

                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }

            // middle of line
            x=(w-2);
            while (x--)
            {
                int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                r += _R(f[-1]); g += _G(f[-1]); b += _B(f[-1]);
                r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                f++;

                r/=2; g/=2; b/=2;

                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;

                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }

            // right block
            {
                int r=_R(f[-1]); int g=_G(f[-1]); int b=_B(f[-1]);
                r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                f++;

                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;

                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }
        }


        // middle block
        {
            int y=h-at_top-at_bottom;
            while (y--)
            {
                int x;
      
                // left edge
                {
                    int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                    r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                    r += _R(f[-w]); g += _G(f[-w]); b += _B(f[-w]);
                    f++;

                    r/=2; g/=2; b/=2;

                    r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                    lfo++;

                    if (r < 0) r=0;
                    else if (r > 255) r=255;
                    if (g < 0) g=0;
                    else if (g > 255*256) g=255*256;
                    if (b < 0) b=0;
                    else if (b > 255*65536) b=255*65536;
                    *of++=_RGB(r,g,b);          
                }

                // middle of line
                x=(w-2);
#ifdef NO_MMX
                while (x--)
                {
                    int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                    r += _R(f[-1]); g += _G(f[-1]); b += _B(f[-1]);
                    r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                    r += _R(f[-w]); g += _G(f[-w]); b += _B(f[-w]);
                    f++;

                    r/=2; g/=2; b/=2;

                    r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                    lfo++;

                    if (r < 0) r=0;
                    else if (r > 255) r=255;
                    if (g < 0) g=0;
                    else if (g > 255*256) g=255*256;
                    if (b < 0) b=0;
                    else if (b > 255*65536) b=255*65536;
                    *of++=_RGB(r,g,b);          
                }
#else
        __asm
        {
          mov esi, f
          mov edi, of
          mov edx, lfo
          mov ecx, x
          mov ebx, w
          shl ebx, 2
          shr ecx, 1
          sub esi, ebx
          align 16
mmx_water_loop1:
          movd mm0, [esi+ebx+4]

          movd mm1, [esi+ebx-4]
          punpcklbw mm0, [zero]

          movd mm2, [esi+ebx*2]
          punpcklbw mm1, [zero]

          movd mm3, [esi]
          punpcklbw mm2, [zero]

          movd mm4, [edx]
          paddw mm0, mm1

          punpcklbw mm3, [zero]
          movd mm7, [esi+ebx+8]

          punpcklbw mm4, [zero]
          paddw mm2, mm3

          movd mm6, [esi+ebx]
          paddw mm0, mm2

          psrlw mm0, 1
          punpcklbw mm7, [zero]

          movd mm2, [esi+ebx*2+4]
          psubw mm0, mm4

          movd mm3, [esi+4]
          packuswb mm0, mm0

          movd [edi], mm0
          punpcklbw mm6, [zero]

          movd mm4, [edx+4]
          punpcklbw mm2, [zero]

          paddw mm7, mm6
          punpcklbw mm3, [zero]

          punpcklbw mm4, [zero]
          paddw mm2, mm3
          
          paddw mm7, mm2
          add edx, 8

          psrlw mm7, 1
          add esi, 8

          psubw mm7, mm4

          packuswb mm7, mm7

          movd [edi+4], mm7

          add edi, 8

          dec ecx
          jnz mmx_water_loop1

          add esi, ebx
          mov f, esi
          mov of, edi
          mov lfo, edx
        };
#endif
                // right block
                {
                    int r=_R(f[-1]); int g=_G(f[-1]); int b=_B(f[-1]);
                    r += _R(f[w]);  g += _G(f[w]);  b += _B(f[w]);
                    r += _R(f[-w]); g += _G(f[-w]); b += _B(f[-w]);
                    f++;

                    r/=2; g/=2; b/=2;

                    r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                    lfo++;

                    if (r < 0) r=0;
                    else if (r > 255) r=255;
                    if (g < 0) g=0;
                    else if (g > 255*256) g=255*256;
                    if (b < 0) b=0;
                    else if (b > 255*65536) b=255*65536;
                    *of++=_RGB(r,g,b);          
                }
            }
        }

        // bottom line
        {
            int x;
        
            // left edge
            {
                int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                r += _R(f[-w]);  g += _G(f[-w]);  b += _B(f[-w]);
                f++;
    
                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;
    
                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }
    
            // middle of line
            x=(w-2);
            while (x--)
            {
                int r=_R(f[1]); int g=_G(f[1]); int b=_B(f[1]);
                r += _R(f[-1]); g += _G(f[-1]); b += _B(f[-1]);
                r += _R(f[-w]);  g += _G(f[-w]);  b += _B(f[-w]);
                f++;
    
                r/=2; g/=2; b/=2;
    
                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;
    
                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }
    
            // right block
            {
                int r=_R(f[-1]); int g=_G(f[-1]); int b=_B(f[-1]);
                r += _R(f[-w]);  g += _G(f[-w]);  b += _B(f[-w]);
                f++;
    
                r-=_R(lfo[0]); g-=_G(lfo[0]); b-=_B(lfo[0]);
                lfo++;
    
                if (r < 0) r=0;
                else if (r > 255) r=255;
                if (g < 0) g=0;
                else if (g > 255*256) g=255*256;
                if (b < 0) b=0;
                else if (b > 255*65536) b=255*65536;
                *of++=_RGB(r,g,b);          
            }
        }
    }

  memcpy(priv->lastframe,framebuffer,w*sizeof(int));
  
#ifndef NO_MMX
    __asm emms;
#endif

}
