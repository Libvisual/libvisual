/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_ring.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
 */
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "lvavs_pipeline.h"

typedef struct {
	LVAVSPipeline *pipeline;
	int			 place;
	VisPalette		 pal;
	int effect;
	int num_colors;
	int colors[16];
	int color_pos;
	
	int size;
	int source;
	AVSGfxColorCycler	*cycler;
} RingPrivate;

int lv_ring_init (VisPluginData *plugin);
int lv_ring_cleanup (VisPluginData *plugin);
int lv_ring_requisition (VisPluginData *plugin, int *width, int *height);
int lv_ring_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_ring_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_ring_palette (VisPluginData *plugin);
int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_ring_requisition,
		.palette = lv_ring_palette,
		.render = lv_ring_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_ring",
		.name = "Libvisual AVS Render: ring element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: ring element",
		.help = "This is the ring scope element for the libvisual AVS system",

		.init = lv_ring_init,
		.cleanup = lv_ring_cleanup,
		.events = lv_ring_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_ring_init (VisPluginData *plugin)
{
	RingPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num_colors", 2),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 0x10),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("source", 0),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (RingPrivate, 1);

	priv->pipeline = visual_object_get_private(VISUAL_OBJECT( plugin));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 1);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_container_add_many (paramcontainer, params);

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

	visual_palette_free_colors (&priv->pal);

	return 0;
}

int lv_ring_cleanup (VisPluginData *plugin)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_ring_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_ring_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_ring_events (VisPluginData *plugin, VisEventQueue *events)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_ring_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "effect"))
					priv->effect = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "num_colors"))
					priv->num_colors = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "size"))
					priv->size = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "source"))
					priv->source = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "palette")) {
					VisPalette *pal;

					pal = visual_param_entry_get_palette (param);

					visual_palette_free_colors (&priv->pal);
					visual_palette_allocate_colors (&priv->pal, pal->ncolors);
					visual_palette_copy (&priv->pal, pal);

					if (priv->cycler != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->cycler));

					priv->cycler = avs_gfx_color_cycler_new (&priv->pal);
					avs_gfx_color_cycler_set_mode (priv->cycler, AVS_GFX_COLOR_CYCLER_TYPE_TIME);
					avs_gfx_color_cycler_set_time (priv->cycler, avs_config_standard_color_cycler_time ());
				}

				break;

			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_ring_palette (VisPluginData *plugin)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

#if 0
int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int hx = video->width / 2;
	int hy = video->height / 2;
	int x, y, ox, oy;
	int i;
	float a = 0;
	float add = (2 * 3.1415) / 100;
	float size_mult;
	uint32_t *buf = priv->pipeline->framebuffer;
        float visdata[2][2][1024];
	VisColor *col;

	memcpy(visdata, priv->pipeline->audiodata, sizeof(visdata));

	if (priv->place == 1)
		hx += video->width / 4;
	else if (priv->place == 0)
		hx -= video->width / 4;
	
	if (video->width > video->height)
		size_mult = (float) video->height * ((float) priv->size / 64.00);
	else	
		size_mult = (float) video->width * ((float) priv->size / 64.00);
	
        float val = (visdata[priv->type][0][0] + 1) / 2;
	ox = (cos (a) * (size_mult + (val)));
	oy = (sin (a) * (size_mult + (val)));

	a += add;

	for (i = 0; i < 50; i++) {


		val = (visdata[priv->type][0][i + 1] + 1) / 2;
		x = (cos (a) * (size_mult + (val)));
		y = (sin (a) * (size_mult + (val)));
		
		a += add;

		col = avs_gfx_color_cycler_run (priv->cycler);
		
		avs_gfx_line_non_naieve_ints (video, ox + hx, hy - oy, x + hx, hy - y, col);
		avs_gfx_line_non_naieve_ints (video, ox + hx, oy + hy, x + hx, y + hy, col);

		visual_object_unref (VISUAL_OBJECT (col));

		ox = x;
		oy = y;

	}

	return 0;
}
#endif

int lv_ring_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
  RingPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
  int x;
  int current_color;
  unsigned char *fa_data;
  char center_channel[1024];
  int which_ch=(priv->effect>>2)&3;
  int y_pos=(priv->effect>>4);
  unsigned char visdata[2][2][1024];
  int w = video->width * 2, h = video->height;
  int s, c, i;
  for(s = 0; s < 2; s++) for(c = 0; c < 2; c++) for(i = 0; i < 1024; i++) 
	visdata[s][c][i] = (priv->pipeline->audiodata[s][c][i] + 1) / 2 * UCHAR_MAX;

  if (priv->pipeline->isBeat&0x80000000) return 0;
  if (!priv->num_colors) return 0;
  priv->color_pos++;
  if (priv->color_pos >= priv->num_colors * 64) priv->color_pos=0;

  {
    int p=priv->color_pos/64;
    int r=priv->color_pos&63;
    int c1,c2;
    int r1,r2,r3;
    c1=priv->colors[p];
    if (p+1 < priv->num_colors)
      c2=priv->colors[p+1];
    else c2=priv->colors[0];

    r1=(((c1&255)*(63-r))+((c2&255)*r))/64;
    r2=((((c1>>8)&255)*(63-r))+(((c2>>8)&255)*r))/64;
    r3=((((c1>>16)&255)*(63-r))+(((c2>>16)&255)*r))/64;
    
    current_color=r1|(r2<<8)|(r3<<16);
  }

  if (which_ch>=2)
  {
    for (x = 0; x < 1024; x ++) center_channel[x]=visdata[priv->source?0:1][0][x]/2+visdata[priv->source?0:1][1][x]/2;
  }
  if (which_ch < 2) fa_data=(unsigned char *)&visdata[priv->source?0:1][which_ch][0];
  else fa_data=(unsigned char *)center_channel;

  {
    double s=priv->size/32.0;
      int lx,ly;
#define min(a, b) (a<b?a:b)
	  double is=min((h*s),(w*s));
	  int c_x;
    int c_y=h/2;
    if (y_pos == 2) c_x = w/2;
    else if (y_pos == 0) c_x=(w/4);
    else c_x=w/2+w/4;
	  {
		  int q=0;
      double a=0.0;
      double sca;
      if (!priv->source) sca=0.1 + ((fa_data[q]^128)/255.0)*0.9;
      else sca=0.1 + ((fa_data[q*2]/2+fa_data[q*2+1]/2)/255.0)*0.9;
      int n_seg=1;
      lx=c_x+(cos(a)*is*sca);
      ly=c_y+(sin(a)*is*sca);

		  for (q = 1; q <= 80; q += n_seg)
		  {
        int tx,ty;
        a -= 3.14159*2.0 / 80.0 * n_seg;
        if (!priv->source) sca=0.1 + ((fa_data[q>40?80-q:q]^128)/255.0)*0.90;
        else sca=0.1 + ((fa_data[q>40?(80-q)*2:q*2]/2+fa_data[q>40?(80-q)*2+1:q*2+1]/2)/255.0)*0.9;
        tx=c_x+(cos(a)*is*sca);
        ty=c_y+(sin(a)*is*sca);

			  if ((tx >= 0 && tx < w && ty >= 0 && ty < h) ||
            (lx >= 0 && lx < w && ly >= 0 && ly < h))
        {
           VisColor color;
           visual_color_from_uint32(&color, 0xffffffff);
           avs_gfx_line_non_naieve_ints (video, tx, ty, lx, ly, &color);
           //line(framebuffer,tx,ty,lx,ly,w,h,current_color,(priv->pipeline->mode&0xff0000)>>16);
        }
        lx=tx;
        ly=ty;
      }
	  }
  }
  return 0;
}


