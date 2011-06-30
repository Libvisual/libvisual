/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_oscstar.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stoscstar.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    VisPalette pal;

    int effect;
    int num_colors;
    int size, rot;
    int color_pos;
    double m_r;

} OscstarPrivate;

int lv_oscstar_init (VisPluginData *plugin);
int lv_oscstar_cleanup (VisPluginData *plugin);
int lv_oscstar_requisition (VisPluginData *plugin, int *width, int *height);
int lv_oscstar_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_oscstar_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_oscstar_palette (VisPluginData *plugin);
int lv_oscstar_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (OscstarPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_oscstar_requisition,
		.palette = lv_oscstar_palette,
		.render = lv_oscstar_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_oscstar",
		.name = "Libvisual AVS Render: Oscilliscope Star element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: Oscilliscope element",
		.help = "This is the Oscilliscope Star scope element for the libvisual AVS system",

		.init = lv_oscstar_init,
		.cleanup = lv_oscstar_cleanup,
		.events = lv_oscstar_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_oscstar_init (VisPluginData *plugin)
{
	OscstarPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", (0|(2<<2)|(2<<4)), VISUAL_PARAM_LIMIT_NONE, "Effect"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num_colors", 1, VISUAL_PARAM_LIMIT_NONE, "Number of colors (max 16)"),
		VISUAL_PARAM_LIST_ENTRY ("palette", "Colors"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 8, VISUAL_PARAM_LIMIT_NONE, "Star Size"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("rot", 3, VISUAL_PARAM_LIMIT_NONE, "Rotation"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (OscstarPrivate, 1);
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

int lv_oscstar_cleanup (VisPluginData *plugin)
{
	OscstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_oscstar_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_oscstar_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_oscstar_events (VisPluginData *plugin, VisEventQueue *events)
{
	OscstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_oscstar_dimension (plugin, ev.event.resize.video,
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
				else if (visual_param_entry_is (param, "rot"))
					priv->rot = visual_param_entry_get_integer (param);
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

VisPalette *lv_oscstar_palette (VisPluginData *plugin)
{
	OscstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_oscstar_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	OscstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int isBeat = proxy->isBeat;
    int x;
    int current_color;
    unsigned char *fa_data;
    char center_channel[576];
    int which_ch=(priv->effect>>2)&3;
    int y_pos=(priv->effect>>4);

    uint32_t *colors = visual_mem_malloc(priv->pal->nColors);
    int i;

    for(i = 0; i < priv->pal->nColors)
    {
        colors[i] = visual_color_to_uint32(priv->pal->colors[i]);
    }

    if (isBeat&0x80000000) return 0;

    if (!priv->num_colors) return 0;
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
        for (x = 0; x < 576; x ++) center_channel[x]=proxy->audiodata[1][0][x]/2+proxy->audiodata[1][1][x]/2;
    }
    if (which_ch < 2) fa_data=(unsigned char *)&proxy->audiodata[1][which_ch][0];
    else fa_data=(unsigned char *)center_channel;

    {
        double s=size/32.0;
        int c_x;
        int is=min((int)(h*s),(int)(w*s));
        if (y_pos == 2) c_x = w/2;
        else if (y_pos == 0) c_x=(w/4);
        else c_x=w/2+w/4;
        {
            int q,ii=0;
            for (q = 0; q < 5; q ++)
            {
                double s,c;
                s=sin(m_r+q*(3.14159*2.0/5.0));
                c=cos(m_r+q*(3.14159*2.0/5.0));
                double p=0.0;
                if (y_pos == 2) c_x = w/2;
                else if (y_pos == 0) c_x=(w/4);
                int lx=c_x;
                int ly=h/2;
                int t=64;
                double dp=is/(double)t;
                double dfactor=1.0/1024.0f;
                double hw=is;
                while (t--)
                {
                    double ale=(((fa_data[ii]^128)-128)*dfactor*hw);
                    int x,y;
                    ii++;
                    x=c_x+(int)(c*p)-(int)(s*ale);
                    y=h/2+(int)(s*p)+(int)(c*ale);
                    if ((x >= 0 && x < w && y >= 0 && y < h) ||
                    (lx >= 0 && lx < w && ly >= 0 && ly < h))
                    {
                        line(framebuffer,x,y,lx,ly,w,h,current_color,(proxy->line_blend_mode&0xff0000)>>16);
                    }
                    lx=x;
                    ly=y;
                    p+=dp;
                    dfactor -= ((1.0/1024.0f)-(1.0/128.0f))/64.0f;
                }
            }

            priv->m_r+=0.01 * (double)priv->rot;
            if (priv->m_r >= 3.14159*2)
                priv->m_r -= 3.14159*2;
        }
    }

    return 0;
}

