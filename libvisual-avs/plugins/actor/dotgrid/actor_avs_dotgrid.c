/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_dotgrid.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stdotgrid.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    VisPalette pal;

    int num_colors;
    int spacing;
    int x_move;
    int y_move;
    int blend;

    int xp, yp;
    int color_pos;

} DotgridPrivate;

int lv_dotgrid_init (VisPluginData *plugin);
int lv_dotgrid_cleanup (VisPluginData *plugin);
int lv_dotgrid_requisition (VisPluginData *plugin, int *width, int *height);
int lv_dotgrid_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_dotgrid_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_dotgrid_palette (VisPluginData *plugin);
int lv_dotgrid_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (DotgridPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_dotgrid_requisition,
		.palette = lv_dotgrid_palette,
		.render = lv_dotgrid_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_dotgrid",
		.name = "Libvisual AVS Render: dotgrid element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: dotgrid element",
		.help = "This is the dotgrid scope element for the libvisual AVS system",

		.init = lv_dotgrid_init,
		.cleanup = lv_dotgrid_cleanup,
		.events = lv_dotgrid_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_dotgrid_init (VisPluginData *plugin)
{
	DotgridPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("num_colors", 1, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("spacing", 8, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("x_move", 128, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("y_move", 128, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 3, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (DotgridPrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 16);

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

int lv_dotgrid_cleanup (VisPluginData *plugin)
{
	DotgridPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_dotgrid_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_dotgrid_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_dotgrid_events (VisPluginData *plugin, VisEventQueue *events)
{
	DotgridPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dotgrid_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "num_colors"))
					priv->num_colors = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "spacing"))
					priv->spacing = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "x_move"))
					priv->x_move = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "y_move"))
					priv->y_move = visual_param_entry_get_integer (param);
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

VisPalette *lv_dotgrid_palette (VisPluginData *plugin)
{
	DotgridPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_dotgrid_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DotgridPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int x,y;
    int current_color;
    int colors[16];

    for(i = 0; i < 16; i++)
    {
        colors[i] = visual_color_to_uint32(priv->pal->colors[i]);
    }

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
    if (priv->spacing<2)priv->spacing=2;
    while (yp < 0) yp+=priv->spacing*256;
    while (xp < 0) xp+=priv->spacing*256;

    int sy=(yp>>8)%priv->spacing;
    int sx=(xp>>8)%priv->spacing;
    framebuffer += sy*w;
    for (y = sy; y < h; y += priv->spacing)
    {
        if (priv->blend==1)
            for (x = sx; x < w; x += priv->spacing)
            framebuffer[x]=BLEND(framebuffer[x],current_color);
        else if (priv->blend == 2)
            for (x = sx; x < w; x += priv->spacing)
            framebuffer[x]=BLEND_AVG(framebuffer[x],current_color);
        else if (priv->blend == 3)
            for (x = sx; x < w; x += priv->spacing)
                BLEND_LINE(framebuffer+x,current_color);
        else
            for (x = sx; x < w; x += priv->spacing)
        framebuffer[x]=current_color;
        framebuffer += w*priv->spacing;
    }
    xp+=priv->x_move;
    yp+=priv->y_move;

    return 0;
}

