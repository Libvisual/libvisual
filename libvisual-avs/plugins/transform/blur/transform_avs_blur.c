/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_blur.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <omp.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "lvavs_pipeline.h"

double PI = M_PI;

#define MASK_SH1 (~(((1<<7)|(1<<15)|(1<<23))<<1))
#define MASK_SH2 (~(((3<<6)|(3<<14)|(3<<22))<<2))
#define MASK_SH3 (~(((7<<5)|(7<<13)|(7<<21))<<3))
#define MASK_SH4 (~(((15<<4)|(15<<12)|(15<<20))<<4))
static unsigned int mmx_mask1[2]={MASK_SH1,MASK_SH1};
static unsigned int mmx_mask2[2]={MASK_SH2,MASK_SH2};
static unsigned int mmx_mask3[2]={MASK_SH3,MASK_SH3};
static unsigned int mmx_mask4[2]={MASK_SH4,MASK_SH4};

#define DIV_2(x) ((( x ) & MASK_SH1)>>1)
#define DIV_4(x) ((( x ) & MASK_SH2)>>2)
#define DIV_8(x) ((( x ) & MASK_SH3)>>3)
#define DIV_16(x) ((( x ) & MASK_SH4)>>4)

typedef struct {
    LVAVSPipeline *pipeline;

    // params
    int enabled, roundmode;

} BlurPrivate;

int lv_blur_init (VisPluginData *plugin);
int lv_blur_cleanup (VisPluginData *plugin);
int lv_blur_events (VisPluginData *plugin, VisEventQueue *events);
int lv_blur_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_blur_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);


void smp_render(int this_thread, int max_threads, BlurPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info(int *count);
const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_blur_palette,
		.video = lv_blur_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_blur",
		.name = "Libvisual AVS Transform: blur element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: blur element",
		.help = "This is the blur element for the libvisual AVS system",

		.init = lv_blur_init,
		.cleanup = lv_blur_cleanup,
		.events = lv_blur_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_blur_init (VisPluginData *plugin)
{
	BlurPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 2),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("roundmode", 0),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (BlurPrivate, 1);

	priv->pipeline = (LVAVSPipeline *)visual_object_get_private(VISUAL_OBJECT(plugin));
	visual_object_ref(VISUAL_OBJECT(priv->pipeline));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add (paramcontainer, params);

	return 0;
}

int lv_blur_cleanup (VisPluginData *plugin)
{
	BlurPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_blur_events (VisPluginData *plugin, VisEventQueue *events)
{
	BlurPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
                if (visual_param_entry_is (param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "roundmode"))
                    priv->roundmode = visual_param_entry_get_integer(param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_blur_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_blur_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	BlurPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    	uint8_t isBeat = priv->pipeline->isBeat;
#pragma omp parallel 
{
	int i = 0, num_threads = omp_get_num_threads();
	#pragma omp for
        for(i = num_threads - 1; i>=0; i--)
		smp_render(i, num_threads, priv, priv->pipeline->audiodata, priv->pipeline->isBeat, priv->pipeline->framebuffer, priv->pipeline->fbout, video->width, video->height);
	priv->pipeline->swap = 1;
}
	return 0;
}

void smp_render(int this_thread, int max_threads, BlurPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
  if (!priv->enabled) return;

  int roundmode = priv->roundmode;
  int enabled = priv->enabled;

  int *f = (int *) framebuffer;
  int *of = (int *) fbout;

  int start_l = ( this_thread * h ) / max_threads;
  int end_l;

  if (this_thread >= max_threads - 1) end_l = h;
  else end_l = ( (this_thread+1) * h ) / max_threads;  

  int outh=end_l-start_l;
  if (outh<1) return;

  int skip_pix=start_l*w;

  f += skip_pix;
  of+= skip_pix;

  int at_top=0, at_bottom=0;

  if (!this_thread) at_top=1;
  if (this_thread >= max_threads - 1) at_bottom=1;

  enabled = 3;
  if (enabled == 2) 
  {
    // top line
    if (at_top)
    {
      int *f2=f+w;
      int x;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x03030303; adj_tl2 = 0x04040404; }
      // top left
      *of++=DIV_2(f[0])+DIV_4(f[0])+DIV_8(f[1])+DIV_8(f2[0]) + adj_tl; f++; f2++;
	    // top center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_2(f[0]) + DIV_8(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + adj_tl2;
		    of[1]=DIV_2(f[1]) + DIV_8(f[1]) + DIV_8(f[2]) + DIV_8(f[0]) + DIV_8(f2[1]) + adj_tl2;
		    of[2]=DIV_2(f[2]) + DIV_8(f[2]) + DIV_8(f[3]) + DIV_8(f[1]) + DIV_8(f2[2]) + adj_tl2;
		    of[3]=DIV_2(f[3]) + DIV_8(f[3]) + DIV_8(f[4]) + DIV_8(f[2]) + DIV_8(f2[3]) + adj_tl2;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_2(f[0]) + DIV_8(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + adj_tl2;
        f++;
        f2++;
      }
      // top right
      *of++=DIV_2(f[0])+DIV_4(f[0]) + DIV_8(f[-1])+DIV_8(f2[0]) + adj_tl; f++; f2++;
    }


	  // middle block
    {
      int y=outh-at_top-at_bottom;
      unsigned int adj_tl1=0,adj_tl2=0;
      unsigned long adj2=0;
      if (roundmode) { adj_tl1=0x04040404; adj_tl2=0x05050505; adj2=0x0505050505050505; }
      while (y--)
      {
        int x;
        int *f2=f+w;
        int *f3=f-w;
      
        // left edge
        *of++=DIV_2(f[0])+DIV_8(f[0])+DIV_8(f[1])+DIV_8(f2[0])+DIV_8(f3[0])+adj_tl1; f++; f2++; f3++;

        // middle of line
#ifdef NO_MMX
        x=(w-2)/4;
        if (roundmode)
        {
	        while (x--)
	        {
		        of[0]=DIV_2(f[0]) + DIV_4(f[0]) + DIV_16(f[1]) + DIV_16(f[-1]) + DIV_16(f2[0]) + DIV_16(f3[0]) + 0x05050505;
		        of[1]=DIV_2(f[1]) + DIV_4(f[1]) + DIV_16(f[2]) + DIV_16(f[0]) + DIV_16(f2[1]) + DIV_16(f3[1]) + 0x05050505;
		        of[2]=DIV_2(f[2]) + DIV_4(f[2]) + DIV_16(f[3]) + DIV_16(f[1]) + DIV_16(f2[2]) + DIV_16(f3[2]) + 0x05050505;
		        of[3]=DIV_2(f[3]) + DIV_4(f[3]) + DIV_16(f[4]) + DIV_16(f[2]) + DIV_16(f2[3]) + DIV_16(f3[3]) + 0x05050505;
            f+=4;
            f2+=4;
            f3+=4;
            of+=4;
          }
        }
        else
        {
	        while (x--)
	        {
		        of[0]=DIV_2(f[0]) + DIV_4(f[0]) + DIV_16(f[1]) + DIV_16(f[-1]) + DIV_16(f2[0]) + DIV_16(f3[0]);
		        of[1]=DIV_2(f[1]) + DIV_4(f[1]) + DIV_16(f[2]) + DIV_16(f[0]) + DIV_16(f2[1]) + DIV_16(f3[1]);
		        of[2]=DIV_2(f[2]) + DIV_4(f[2]) + DIV_16(f[3]) + DIV_16(f[1]) + DIV_16(f2[2]) + DIV_16(f3[2]);
		        of[3]=DIV_2(f[3]) + DIV_4(f[3]) + DIV_16(f[4]) + DIV_16(f[2]) + DIV_16(f2[3]) + DIV_16(f3[3]);
            f+=4;
            f2+=4;
            f3+=4;
            of+=4;
          }
        }
#else
        {
          __asm 
          {
            mov ecx, w
            mov edx, ecx
            mov ebx, edx
            neg ebx
            mov esi, f
            mov edi, of
            sub ecx, 2
            shr ecx, 2
            movq mm1, [esi-4]
            align 16
mmx_light_blur_loop:
            movq mm0, [esi]

            movq mm2, [esi+4]
            pand mm0, mmx_mask1

            movq mm5, mm2
            psrl mm0, 1

            movq mm7, [esi+8]
            movq mm4, mm0

            pand mm1, mmx_mask4
            pand mm4, mmx_mask1

            movq mm3, [esi+edx*4]
            psrl mm4, 1
           
            paddb mm0, mm4
            pand mm2, mmx_mask4

            movq mm4, [esi+ebx*4]
            pand mm3, mmx_mask4

            pand mm4, mmx_mask4

            psrl mm1, 4
            pand mm7, mmx_mask1

            movq mm6, [esi+12]

            psrl mm2, 4
            add esi, 16

            psrl mm3, 4

            paddb mm0, mm1
            psrl mm4, 4

            movq mm1, mm6
            psrl mm7, 1

            paddb mm2, mm3
            paddb mm0, mm4

            movq mm3, [esi+edx*4-8]
            paddb mm0, mm2

            movq mm4, [esi+ebx*4-8]
            paddb mm0, [adj2]

            pand mm6, mmx_mask4

            movq [edi],mm0
            pand mm5, mmx_mask4

            movq mm0, mm7
            pand mm3, mmx_mask4

            psrl mm6, 4
            pand mm0, mmx_mask1

            pand mm4, mmx_mask4
            psrl mm5, 4

            psrl mm0, 1
            paddb mm7, mm6

            paddb mm7, mm0
            add edi, 16

            psrl mm3, 4

            psrl mm4, 4
            paddb mm5, mm3

            paddb mm7, mm4
            dec ecx

            paddb mm7, mm5
            paddb mm7, [adj2]

            movq [edi-8],mm7
            
            jnz mmx_light_blur_loop
            mov of, edi
            mov f, esi           
          };
          f2=f+w; // update these bitches
          f3=f-w;
        }
#endif
        x=(w-2)&3;
	      while (x--)
	      {
		      *of++=DIV_2(f[0]) + DIV_4(f[0]) + DIV_16(f[1]) + DIV_16(f[-1]) + DIV_16(f2[0]) + DIV_16(f3[0]) + adj_tl2;
          f++;
          f2++;
          f3++;
        }

        // right block
        *of++=DIV_2(f[0])+DIV_8(f[0])+DIV_8(f[-1])+DIV_8(f2[0])+DIV_8(f3[0])+adj_tl1; f++;
	    }
    }
    // bottom block
    if (at_bottom)
    {
      int *f2=f-w;
      int x;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x03030303; adj_tl2 = 0x04040404; }
      // bottom left
      *of++=DIV_2(f[0])+DIV_4(f[0])+DIV_8(f[1])+DIV_8(f2[0]) + adj_tl; f++; f2++;
	    // bottom center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_2(f[0]) + DIV_8(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + adj_tl2;
		    of[1]=DIV_2(f[1]) + DIV_8(f[1]) + DIV_8(f[2]) + DIV_8(f[0]) + DIV_8(f2[1]) + adj_tl2;
		    of[2]=DIV_2(f[2]) + DIV_8(f[2]) + DIV_8(f[3]) + DIV_8(f[1]) + DIV_8(f2[2])+adj_tl2;
		    of[3]=DIV_2(f[3]) + DIV_8(f[3]) + DIV_8(f[4]) + DIV_8(f[2]) + DIV_8(f2[3])+adj_tl2;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_2(f[0]) + DIV_8(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0])+adj_tl2;
        f++;
        f2++;
      }
      // bottom right
      *of++=DIV_2(f[0])+DIV_4(f[0]) + DIV_8(f[-1])+DIV_8(f2[0])+adj_tl; f++; f2++;
    }
  }
  else if (enabled == 3) // more blur
  {
    // top line
    if (at_top) {
      int *f2=f+w;
      int x;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x02020202; adj_tl2 = 0x01010101; }
      // top left
      *of++=DIV_2(f[1])+DIV_2(f2[0]) + adj_tl2; f++; f2++;
	    // top center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_2(f2[0]) + adj_tl;
		    of[1]=DIV_4(f[2]) + DIV_4(f[0]) + DIV_2(f2[1]) +adj_tl;
		    of[2]=DIV_4(f[3]) + DIV_4(f[1]) + DIV_2(f2[2]) + adj_tl;
		    of[3]=DIV_4(f[4]) + DIV_4(f[2]) + DIV_2(f2[3]) + adj_tl;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_2(f2[0])+adj_tl;
        f++;
        f2++;
      }
      // top right
      *of++=DIV_2(f[-1])+DIV_2(f2[0])+adj_tl2; f++; f2++;
    }


	  // middle block
    {
      int y=outh-at_top-at_bottom;
      int adj_tl1=0,adj_tl2=0;
      unsigned long adj2=0;
      if (roundmode) { adj_tl1=0x02020202; adj_tl2=0x03030303; adj2=0x0303030303030303; }

      while (y--)
      {
        int x;
        int *f2=f+w;
        int *f3=f-w;
      
        // left edge
        *of++=DIV_2(f[1])+DIV_4(f2[0])+DIV_4(f3[0]) + adj_tl1; f++; f2++; f3++;

        // middle of line
#ifdef NO_MMX
        x=(w-2)/4;
        if (roundmode)
        {
	        while (x--)
	        {
		        of[0]=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + DIV_4(f3[0]) + 0x03030303;
		        of[1]=DIV_4(f[2]) + DIV_4(f[0]) + DIV_4(f2[1]) + DIV_4(f3[1]) + 0x03030303;
		        of[2]=DIV_4(f[3]) + DIV_4(f[1]) + DIV_4(f2[2]) + DIV_4(f3[2]) + 0x03030303;
		        of[3]=DIV_4(f[4]) + DIV_4(f[2]) + DIV_4(f2[3]) + DIV_4(f3[3]) + 0x03030303;
            f+=4; f2+=4; f3+=4; of+=4;
          }
        }
        else
        {
	        while (x--)
	        {
		        of[0]=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + DIV_4(f3[0]);
		        of[1]=DIV_4(f[2]) + DIV_4(f[0]) + DIV_4(f2[1]) + DIV_4(f3[1]);
		        of[2]=DIV_4(f[3]) + DIV_4(f[1]) + DIV_4(f2[2]) + DIV_4(f3[2]);
		        of[3]=DIV_4(f[4]) + DIV_4(f[2]) + DIV_4(f2[3]) + DIV_4(f3[3]);
            f+=4; f2+=4; f3+=4; of+=4;
          }
        }
#else
        {
          __asm 
          {
            mov ecx, w
            mov edx, ecx
            mov ebx, edx
            neg ebx
            mov esi, f
            mov edi, of
            sub ecx, 2
            shr ecx, 2
            movq mm1, [esi-4]
            align 16
mmx_heavy_blur_loop:
            movq mm2, [esi+4]
            pxor mm0, mm0

            movq mm5, mm2
            pxor mm7, mm7

            movq mm3, [esi+edx*4]
            pand mm1, mmx_mask2

            movq mm4, [esi+ebx*4]
            pand mm2, mmx_mask2

            pand mm3, mmx_mask2
            pand mm4, mmx_mask2

            psrl mm1, 2

            movq mm6, [esi+12]
            psrl mm2, 2

            psrl mm3, 2

            paddb mm0, mm1
            psrl mm4, 2

            movq mm1, mm6

            paddb mm2, mm3
            paddb mm0, mm4

            movq mm3, [esi+edx*4+8]
            paddb mm0, mm2

            movq mm4, [esi+ebx*4+8]
            paddb mm0, [adj2]

            pand mm6, mmx_mask2

            movq [edi],mm0
            pand mm5, mmx_mask2

            pand mm3, mmx_mask2
            add esi, 16

            psrl mm6, 2
            pand mm4, mmx_mask2

            psrl mm5, 2
            paddb mm7, mm6

            psrl mm3, 2
            add edi, 16

            psrl mm4, 2
            paddb mm5, mm3

            paddb mm7, mm4

            paddb mm7, mm5
            paddb mm7, [adj2]

            movq [edi-8],mm7
                        
            dec ecx
            jnz mmx_heavy_blur_loop
            mov of, edi
            mov f, esi           
          };
          f2=f+w; // update these bitches
          f3=f-w;
        }
#endif
        x=(w-2)&3;
	      while (x--)
	      {
		      *of++=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + DIV_4(f3[0]) + adj_tl2;
          f++;
          f2++;
          f3++;
        }

        // right block
        *of++=DIV_2(f[-1])+DIV_4(f2[0])+DIV_4(f3[0]) + adj_tl1; f++;
	    }
    }

    // bottom block
    if (at_bottom)
    {
      int *f2=f-w;
      int x;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x02020202; adj_tl2 = 0x01010101; }
      // bottom left
      *of++=DIV_2(f[1])+DIV_2(f2[0]) + adj_tl2; f++; f2++;
	    // bottom center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_2(f2[0])+adj_tl;
		    of[1]=DIV_4(f[2]) + DIV_4(f[0]) + DIV_2(f2[1])+adj_tl;
		    of[2]=DIV_4(f[3]) + DIV_4(f[1]) + DIV_2(f2[2])+adj_tl;
		    of[3]=DIV_4(f[4]) + DIV_4(f[2]) + DIV_2(f2[3])+adj_tl;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_4(f[1]) + DIV_4(f[-1]) + DIV_2(f2[0])+adj_tl;
        f++;
        f2++;
      }
      // bottom right
      *of++=DIV_2(f[-1])+DIV_2(f2[0])+adj_tl2; f++; f2++;
    }
  }
  else
  {
    // top line
    if (at_top) 
    {
      int *f2=f+w;
      int x;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x02020202; adj_tl2 = 0x03030303; }
      // top left
      *of++=DIV_2(f[0])+DIV_4(f[1])+DIV_4(f2[0]) + adj_tl; f++; f2++;
	    // top center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_4(f[0]) + DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + adj_tl2;
		    of[1]=DIV_4(f[1]) + DIV_4(f[2]) + DIV_4(f[0]) + DIV_4(f2[1]) + adj_tl2;
		    of[2]=DIV_4(f[2]) + DIV_4(f[3]) + DIV_4(f[1]) + DIV_4(f2[2]) + adj_tl2;
		    of[3]=DIV_4(f[3]) + DIV_4(f[4]) + DIV_4(f[2]) + DIV_4(f2[3]) + adj_tl2;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_4(f[0]) + DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + adj_tl2;
        f++;
        f2++;
      }
      // top right
      *of++=DIV_2(f[0])+DIV_4(f[-1])+DIV_4(f2[0]) + adj_tl; f++; f2++;
    }


	  // middle block
    {
      int y=outh-at_top-at_bottom;
      int adj_tl1=0,adj_tl2=0;
      unsigned long adj2=0;
      if (roundmode) { adj_tl1=0x03030303; adj_tl2=0x04040404; adj2=0x0404040404040404; }
      while (y--)
      {
        int x;
        int *f2=f+w;
        int *f3=f-w;
      
        // left edge
        *of++=DIV_4(f[0])+DIV_4(f[1])+DIV_4(f2[0])+DIV_4(f3[0])+adj_tl1; f++; f2++; f3++;

        // middle of line
#ifdef NO_MMX
        x=(w-2)/4;
        if (roundmode)
        {
	        while (x--)
	        {
		        of[0]=DIV_2(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + DIV_8(f3[0]) + 0x04040404;
		        of[1]=DIV_2(f[1]) + DIV_8(f[2]) + DIV_8(f[0]) + DIV_8(f2[1]) + DIV_8(f3[1]) + 0x04040404;
		        of[2]=DIV_2(f[2]) + DIV_8(f[3]) + DIV_8(f[1]) + DIV_8(f2[2]) + DIV_8(f3[2]) + 0x04040404;
		        of[3]=DIV_2(f[3]) + DIV_8(f[4]) + DIV_8(f[2]) + DIV_8(f2[3]) + DIV_8(f3[3]) + 0x04040404;
            f+=4; f2+=4; f3+=4; of+=4;
          }
        }
        else
        {
	        while (x--)
	        {
		        of[0]=DIV_2(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + DIV_8(f3[0]);
		        of[1]=DIV_2(f[1]) + DIV_8(f[2]) + DIV_8(f[0]) + DIV_8(f2[1]) + DIV_8(f3[1]);
		        of[2]=DIV_2(f[2]) + DIV_8(f[3]) + DIV_8(f[1]) + DIV_8(f2[2]) + DIV_8(f3[2]);
		        of[3]=DIV_2(f[3]) + DIV_8(f[4]) + DIV_8(f[2]) + DIV_8(f2[3]) + DIV_8(f3[3]);
            f+=4; f2+=4; f3+=4; of+=4;
          }
        }
#else
        {
          __asm 
          {
            mov ecx, w
            mov edx, ecx
            mov ebx, edx
            neg ebx
            mov esi, f
            mov edi, of
            sub ecx, 2
            shr ecx, 2
            movq mm1, [esi-4]
            align 16
mmx_normal_blur_loop:
            movq mm0, [esi]

            movq mm2, [esi+4]
            pand mm0, mmx_mask1

            movq mm5, mm2

            movq mm7, [esi+8]
            pand mm1, mmx_mask3

            movq mm3, [esi+edx*4]
            pand mm2, mmx_mask3

            movq mm4, [esi+ebx*4]
            pand mm3, mmx_mask3

            psrl mm0, 1
            pand mm4, mmx_mask3

            psrl mm1, 3
            pand mm7, mmx_mask1

            movq mm6, [esi+12]
            psrl mm2, 3

            add esi, 16
            psrl mm3, 3

            paddb mm0, mm1
            psrl mm4, 3

            movq mm1, mm6

            paddb mm2, mm3
            paddb mm0, mm4

            movq mm3, [esi+edx*4-8]
            paddb mm0, mm2

            movq mm4, [esi+ebx*4-8]
            paddb mm0, [adj2]

            pand mm6, mmx_mask3

            movq [edi],mm0
            pand mm5, mmx_mask3

            psrl mm7, 1
            pand mm3, mmx_mask3

            psrl mm6, 3
            pand mm4, mmx_mask3

            psrl mm5, 3
            paddb mm7, mm6

            add edi, 16
            psrl mm3, 3

            psrl mm4, 3
            paddb mm5, mm3

            paddb mm7, mm4
            dec ecx

            paddb mm7, mm5
            paddb mm7, [adj2]

            movq [edi-8],mm7
            
            jnz mmx_normal_blur_loop
            mov of, edi
            mov f, esi           
          };
          f2=f+w; // update these bitches
          f3=f-w;
        }
#endif
        x=(w-2)&3;
	      while (x--)
	      {
		      *of++=DIV_2(f[0]) + DIV_8(f[1]) + DIV_8(f[-1]) + DIV_8(f2[0]) + DIV_8(f3[0]) + adj_tl2;
          f++;
          f2++;
          f3++;
        }

        // right block
        *of++=DIV_4(f[0])+DIV_4(f[-1])+DIV_4(f2[0])+DIV_4(f3[0]) + adj_tl1; f++;
	    }
    }

    // bottom block
    if (at_bottom)
    {
      int *f2=f-w;
      int adj_tl=0, adj_tl2=0;
      if (roundmode) { adj_tl = 0x02020202; adj_tl2 = 0x03030303; }
      int x;
      // bottom left
      *of++=DIV_2(f[0])+DIV_4(f[1])+DIV_4(f2[0]) + adj_tl; f++; f2++;
	    // bottom center
      x=(w-2)/4;
	    while (x--)
	    {
		    of[0]=DIV_4(f[0]) + DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + adj_tl2;
		    of[1]=DIV_4(f[1]) + DIV_4(f[2]) + DIV_4(f[0]) + DIV_4(f2[1]) + adj_tl2;
		    of[2]=DIV_4(f[2]) + DIV_4(f[3]) + DIV_4(f[1]) + DIV_4(f2[2]) + adj_tl2;
		    of[3]=DIV_4(f[3]) + DIV_4(f[4]) + DIV_4(f[2]) + DIV_4(f2[3]) + adj_tl2;
        f+=4;
        f2+=4;
        of+=4;
	    }
      x=(w-2)&3;
	    while (x--)
	    {
		    *of++=DIV_4(f[0]) + DIV_4(f[1]) + DIV_4(f[-1]) + DIV_4(f2[0]) + adj_tl2;
        f++;
        f2++;
      }
      // bottom right
      *of++=DIV_2(f[0])+DIV_4(f[-1])+DIV_4(f2[0]) + adj_tl; f++; f2++;
    }
  }

#ifndef NO_MMX
  __asm emms;
#endif
  
	//timingLeave(0);
}



