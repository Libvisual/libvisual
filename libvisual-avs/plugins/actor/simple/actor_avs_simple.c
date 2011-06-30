/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_simple.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stsimple.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    int effect;
    int num_colors;
    VisPalette pal;
    int color_pos;

} SimplePrivate;

int lv_simple_init (VisPluginData *plugin);
int lv_simple_cleanup (VisPluginData *plugin);
int lv_simple_requisition (VisPluginData *plugin, int *width, int *height);
int lv_simple_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_simple_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_simple_palette (VisPluginData *plugin);
int lv_simple_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_simple_requisition,
		.palette = lv_simple_palette,
		.render = lv_simple_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_simple",
		.name = "Libvisual AVS Render: simple element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: simple element",
		.help = "This is the simple scope element for the libvisual AVS system",

		.init = lv_simple_init,
		.cleanup = lv_simple_cleanup,
		.events = lv_simple_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_simple_init (VisPluginData *plugin)
{
	SimplePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", (0|(2<<2)|(2<<4)), VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num_colors", 1, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (SimplePrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 1);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_container_add_many_proxy (paramcontainer, params);

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

	visual_palette_free_colors (&priv->pal);

	return 0;
}

int lv_simple_cleanup (VisPluginData *plugin)
{
	SimplePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_simple_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_simple_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_simple_events (VisPluginData *plugin, VisEventQueue *events)
{
	SimplePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_simple_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "effect"))
					priv->effect = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "num_colors"))
					priv->num_colors = visual_param_entry_get_integer (param);
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

VisPalette *lv_simple_palette (VisPluginData *plugin)
{
	SimplePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_simple_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	SimplePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int isBeat = proxy->isBeat;

    if (!num_colors) return 0;
    if (isBeat&0x80000000) return 0;
    int i;
    uint32_t *colors = visual_mem_malloc(priv->pal->nColors);

    for(i = 0; i < priv->pal->nColors; i++)
        colors[i] = visual_color_to_uint32(priv->pal->colors[i]);

    int x;
    float yscale = (float) h / 2.0f / 256.0f; 
    float xscale = 288.0f/w;
    int current_color;
    unsigned char *fa_data;
    char center_channel[576];
    int which_ch=(effect>>2)&3;
    int y_pos=(effect>>4)&3;
    priv->color_pos++;
    if (priv->color_pos >= priv->num_colors * 64) 
        priv->color_pos=0;

    {
        int p=priv->color_pos/64;
        int r=priv->color_pos&63;
        int c1,c2;
        int r1,r2,r3;
        c1=colors[p];
        if (p+1 < priv->num_colors)
            c2=colors[p+1];
        else c2=colors[0];

        r1=(((c1&255)*(63-r))+((c2&255)*r))/64;
        r2=((((c1>>8)&255)*(63-r))+(((c2>>8)&255)*r))/64;
        r3=((((c1>>16)&255)*(63-r))+(((c2>>16)&255)*r))/64;
    
        current_color=r1|(r2<<8)|(r3<<16);
    }

    if (which_ch>=2)
    {
        int w=0;
        if ((priv->effect&3)>1) 
            w=1;

        for (x = 0; x < 576; x ++) center_channel[x]=proxy->audiodata[w][0][x]/2+proxy->audiodata[w][1][x]/2;
    }
    if (which_ch < 2) fa_data=(unsigned char *)&proxy->audiodata[(priv->effect&3)>1?1:0][which_ch][0];
    else fa_data=(unsigned char *)center_channel;

    if (priv->effect&(1<<6))
    {
        switch (priv->effect&2)
        { 
        case 2: // dot scope
            {
                int yh = y_pos*h / 2;
                if (y_pos==2) yh=h/4;
                int ys = yh+(int)(yscale*128.0f);
                for (x = 0; x < w; x ++)
                {
                    float r=x*xscale;
                    float s1=r-(int)r;
                    float yr=(fa_data[(int)r]^128)*(1.0f-s1)+(fa_data[(int)r+1]^128)*(s1);
                    int y=yh + (int) (yr*yscale);
                    if (y >= 0 && y < h) framebuffer[x+y*w]=current_color;
                }
            }
        break;
        case 0: // dot analyzer
            {
                int h2=h/2;     
                float ys=yscale;
                float xs=200.0f/w;
                int adj=1;
                if (y_pos!=1) { ys=-ys; adj=0; }
                if (y_pos==2)
                {
                    h2 -= (int) (ys*256/2);
                }
                for (x = 0; x < w; x ++)
                {
                    float r=x*xs;
                    float s1=r-(int)r;
                    float yr=fa_data[(int)r]*(1.0f-s1)+fa_data[(int)r+1]*(s1);
                    int y=h2+adj+(int)(yr*ys-1.0f);
                    if (y >= 0 && y < h) framebuffer[x+w*y]=current_color;
                }
            }
        break;
        }
    }
    else
    {
        switch (priv->effect&3)
        {
            case 0: //solid analyzer
            {
                int h2=h/2;     
                float ys=yscale;
                float xs=200.0f/w;
                int adj=1;
                if (y_pos!=1) { ys=-ys; adj=0; }
                if (y_pos==2)
                {
                    h2 -= (int) (ys*256/2);
                }
                for (x = 0; x < w; x ++)
                {
                    float r=x*xs;
                    float s1=r-(int)r;
                    float yr=fa_data[(int)r]*(1.0f-s1)+fa_data[(int)r+1]*(s1);
                    line(framebuffer,x,h2-adj,x,h2 + adj + (int) (yr*ys - 1.0f),w,h,current_color,(proxy->line_blend_mode&0xff0000)>>16);
                }
            }
            break;
            case 1: // line analyzer
            {
                int yh = 0;
                int h2=h/2;
                int lx,ly,ox,oy;
                float xs= 1.0f/xscale*(288.0f/200.f);
                float ys=yscale;
                if (y_pos!=1) { ys=-ys; }
                if (y_pos == 2)
                    h2 -= (int) (ys*256/2);

                ly=h2 + (int) ((fa_data[0])*ys);
                lx=0;
                for (x = 1; x < 200; x ++)
                {
                    oy=h2 + (int) ((fa_data[x])*ys);
                    ox=(int) (x*xs);
                    line(framebuffer,lx,ly,ox,oy,w,h,current_color,(proxy->line_blend_mode&0xff0000)>>16);
                    ly=oy;
                    lx=ox;
                }
            }
            break;
            case 2: // line scope
            {
                float xs = 1.0f/xscale;
                int lx, ly,ox,oy;
                int yh;
                if (y_pos == 2)
                    yh = h/4;
                else yh = y_pos*h/ 2;
                lx=0;
                ly=yh + (int) ((int)(fa_data[0]^128)*yscale);;
                for (x = 1; x < 288; x ++)
                {
                    ox=(int)(x*xs);
                    oy = yh + (int) ((int)(fa_data[x]^128)*yscale);
                    line(framebuffer, lx,ly,ox,oy,w,h,current_color,(proxy->line_blend_mode&0xff0000)>>16);
                    lx=ox;
                    ly=oy;
                }
            } 
            break;
            case 3: // solid scope
            {
                int yh = y_pos*h / 2;
                if (y_pos==2) yh=h/4;
                int ys = yh+(int)(yscale*128.0f);
                for (x = 0; x < w; x ++)
                {
                    float r=x*xscale;
                    float s1=r-(int)r;
                    float yr=(fa_data[(int)r]^128)*(1.0f-s1)+(fa_data[(int)r+1]^128)*(s1);
                    line(framebuffer,x,ys-1,x,yh + (int) (yr*yscale),w,h,current_color,(proxy->line_blend_mode&0xff0000)>>16);
                }
            }
            break;
        }
    }

    return 0;
}

