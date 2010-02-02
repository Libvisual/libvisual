/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_parts.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stparts.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    int enabled;
    VisPalette pal;
    int maxdist, size, size2;
    int blend;

    int s_pos;

    double c[2];
    double v[2];
    double p[2];

} PartsPrivate;

int lv_parts_init (VisPluginData *plugin);
int lv_parts_cleanup (VisPluginData *plugin);
int lv_parts_requisition (VisPluginData *plugin, int *width, int *height);
int lv_parts_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_parts_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_parts_palette (VisPluginData *plugin);
int lv_parts_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (PartsPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_parts_requisition,
		.palette = lv_parts_palette,
		.render = lv_parts_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_parts",
		.name = "Libvisual AVS Render: Moving Particle element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: Moving Particle element",
		.help = "This is the Moving Particle element for the libvisual AVS system",

		.init = lv_parts_init,
		.cleanup = lv_parts_cleanup,
		.events = lv_parts_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_parts_init (VisPluginData *plugin)
{
	PartsPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_NONE, "Enable Moving Particle"),
		VISUAL_PARAM_LIST_ENTRY ("palette", "Colors"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("maxdist", 16, VISUAL_PARAM_LIMIT_NONE, "Distance From Center"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 8, VISUAL_PARAM_LIMIT_NONE, "Particle Size"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size2", 8, VISUAL_PARAM_LIMIT_NONE, "Particle Size (OnBeat)"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 2, VISUAL_PARAM_LIMIT_NONE, "Blending"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (PartsPrivate, 1);
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

    priv->s_pos = 8;
    priv->c[0]=priv->c[1]=0.0f;
    priv->v[0]=-0.01551;
    priv->v[1]=0.0;
    priv->p[0]=-0.6;
    priv->p[1]=0.3;

	return 0;
}

int lv_parts_cleanup (VisPluginData *plugin)
{
	PartsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_parts_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_parts_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_parts_events (VisPluginData *plugin, VisEventQueue *events)
{
	PartsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_parts_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "enabled"))
					priv->enabled = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "maxdist"))
					priv->maxdist = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "size"))
					priv->size = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "size2"))
					priv->size2 = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "blend"))
					priv->blend = visual_param_entry_get_integer (param);
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

VisPalette *lv_parts_palette (VisPluginData *plugin)
{
	PartsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_parts_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PartsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int isBeat = proxy->isBeat;

    if (!(enabled&1)) return 0;
    if (isBeat&0x80000000) return 0;
    int colors = visual_color_to_uint32(priv->pal.colors[0]);
    int xp,yp;
    int ss=min(h/2,(w*3)/8);
    int oc6 = colors;

    if (isBeat)
    {
        c[0]=((rand()%33)-16)/48.0f;
        c[1]=((rand()%33)-16)/48.0f;
    }


    v[0] -= 0.004*(p[0]-c[0]); 
    v[1] -= 0.004*(p[1]-c[1]); 

    p[0]+=v[0];
    p[1]+=v[1];

    v[0]*=0.991;
    v[1]*=0.991;

    xp=(int)(p[0]*(ss)*(priv->maxdist/32.0))+w/2;
    yp=(int)(p[1]*(ss)*(priv->maxdist/32.0))+h/2;
    if (isBeat && priv->enabled&2) 
        priv->s_pos=priv->size2;
    int sz=priv->s_pos;
    priv->s_pos=(priv->s_pos+priv->size)/2;
    if (sz <= 1) 
    {
        framebuffer += xp+(yp)*w;
        if (xp >= 0 && yp >= 0 && xp < w && yp < h) 
        {
            if (priv->blend==0)
                framebuffer[0]=colors;
            else if (priv->blend==2)
                framebuffer[0]=BLEND_AVG(framebuffer[0],colors);
            else if (priv->blend==3)
                BLEND_LINE(framebuffer,colors);
            else
                framebuffer[0]=BLEND(framebuffer[0],colors);
        }
        return 0;
    }
    if (sz > 128) sz=128;
    {
        int y;
        double md=sz*sz*0.25;
        yp-=sz/2;    
        for (y = 0; y < sz; y ++)
        {
            if (yp+y >= 0 && yp+y < h)
            {
                double yd=(y-sz*0.5);
                double l=sqrt(md-yd*yd);
                int xs=(int)(l+0.99);
                int x;
                if (xs < 1) xs=1;
                int xe=xp + xs;
                if (xe > w) xe=w;
                int xst=xp-xs;
                if (xst < 0) xst=0;
                int *f=&framebuffer[xst+(yp+y)*w];
                if (priv->blend == 0) for ( x = xst; x < xe; x ++) *f++=colors;
                else if (priv->blend == 2)
                    for ( x = xst; x < xe; x ++)
                    {
                        *f=BLEND_AVG(*f,colors);
                        f++;
                    }
                else if (priv->blend == 3)
                    for ( x = xst; x < xe; x ++)
                    {
                        BLEND_LINE(f++,colors);
                    }
                else
                    for ( x = xst; x < xe; x ++)
                    {
                        *f=BLEND(*f,colors);
                        f++;
                    }
            }
        }
    }

    return 0;
}

