/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_rotstar.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <strotstar.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    VisPalette pal;
    int num_colors;

    int color_pos;
    double r1;
} RotstarPrivate;

int lv_rotstar_init (VisPluginData *plugin);
int lv_rotstar_cleanup (VisPluginData *plugin);
int lv_rotstar_requisition (VisPluginData *plugin, int *width, int *height);
int lv_rotstar_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_rotstar_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_rotstar_palette (VisPluginData *plugin);
int lv_rotstar_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (RotstarPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_rotstar_requisition,
		.palette = lv_rotstar_palette,
		.render = lv_rotstar_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_rotstar",
		.name = "Libvisual AVS Render: rotstar element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: rotstar element",
		.help = "This is the rotstar element for the libvisual AVS system",

		.init = lv_rotstar_init,
		.cleanup = lv_rotstar_cleanup,
		.events = lv_rotstar_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_rotstar_init (VisPluginData *plugin)
{
	RotstarPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY ("palette", "Colors"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num_colors", 1, VISUAL_PARAM_LIMIT_NONE, "Number of colors"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (RotstarPrivate, 1);
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

int lv_rotstar_cleanup (VisPluginData *plugin)
{
	RotstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int lv_rotstar_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_rotstar_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_rotstar_events (VisPluginData *plugin, VisEventQueue *events)
{
	RotstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_rotstar_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "num_colors"))
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

VisPalette *lv_rotstar_palette (VisPluginData *plugin)
{
	RotstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_rotstar_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	RotstarPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;

    int x,y,c;
    int current_color;
    uint32_t *colors = visual_mem_malloc(priv->num_colors);

    for(i = 0; i < priv->num_colors; i++)
        colors[i] = visual_color_to_uint32(priv->pal->colors[i]);

    if (isBeat&0x80000000) return 0;
    if (!priv->num_colors) return 0;
    priv->color_pos++;
    if (priv->color_pos >= priv->num_colors * 64) priv->color_pos=0;

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

    x=(int) (cos(r1)*w/4.0);
    y=(int) (sin(r1)*h/4.0);
    for (c = 0; c < 2; c ++)
    {
        double r2=-r1;
        int s=0;
        int t;
        int a,b;
        int nx, ny;
        int lx,ly,l;
        a=x;
        b=y;

        for (l = 3; l < 14; l ++)
            if (proxy->audiodata[0][c][l] > s &&
            proxy->audiodata[0][c][l] > proxy->audiodata[0][c][l+1]+4 &&
            proxy->audiodata[0][c][l] > proxy->audiodata[0][c][l-1]+4)
            {
                s=proxy->audiodata[0][c][l];
            }

        if (c==1) { a=-a; b=-b; }

        double vw,vh;
        vw=w/8.0*(s+9)/88.0;
        vh=h/8.0*(s+9)/88.0;

        nx=(int) (cos(r2)*vw);
        ny=(int) (sin(r2)*vh);
        lx = w/2+a+nx;
        ly = h/2+b+ny;

        r2+=3.14159*4.0/5.0;

        for (t = 0; t < 5; t ++)
        {
            int nx, ny;
            nx=(int) (cos(r2)*vw+w/2+a);
            ny=(int) (sin(r2)*vh+h/2+b);
            r2+=3.14159*4.0/5.0;
            line(framebuffer,lx,ly,nx,ny,w,h,current_color,(prox->line_blend_mode&0xff0000)>>16);
            lx=nx;
            ly=ny;
        }
    }
    r1+=0.1;

    return 0;
}

