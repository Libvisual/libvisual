/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_bspin.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stbspin.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    int enabled;
    VisPalette pal;
    int mode;

    int last_a;
    int lx[2][2], ly[2][2];
    double r_v[2];
    double v[2];
    double dir[2];
} BspinPrivate;

int lv_bspin_init (VisPluginData *plugin);
int lv_bspin_cleanup (VisPluginData *plugin);
int lv_bspin_requisition (VisPluginData *plugin, int *width, int *height);
int lv_bspin_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_bspin_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_bspin_palette (VisPluginData *plugin);
int lv_bspin_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (BspinPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisActorPlugin actor[] = {{
        .requisition = lv_bspin_requisition,
        .palette = lv_bspin_palette,
        .render = lv_bspin_render,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_8BIT |
            VISUAL_VIDEO_DEPTH_32BIT

    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "avs_bspin",
        .name = "Libvisual AVS Render: bspin element",
        .author = "Dennis Smit <ds@nerds-incorporated.org>",
        .version = "0.1",
        .about = "The Libvisual AVS Render: bspin element",
        .help = "This is the bspin scope element for the libvisual AVS system",

        .init = lv_bspin_init,
        .cleanup = lv_bspin_cleanup,
        .events = lv_bspin_events,

        .plugin = VISUAL_OBJECT (&actor[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_bspin_init (VisPluginData *plugin)
{
    BspinPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 3, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable bass spin effect"),
        VISUAL_PARAM_LIST_ENTRY ("palette"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("mode", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (BspinPrivate, 1);
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_palette_allocate_colors (&priv->pal, 2);

    for (i = 0; i < priv->pal.ncolors; i++) {
        priv->pal.colors[i].r = 0xff;
        priv->pal.colors[i].g = 0xff;
        priv->pal.colors[i].b = 0xff;
    }

    visual_param_container_add_many_proxy (paramcontainer, params);

    visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

    visual_palette_free_colors (&priv->pal);

    priv->r_v[0] = M_PI;
    priv->r_v[1] = 0.0;
    priv->dir[0] = -1.0;
    priv->dir[1] = 1.0;

    return 0;
}

int lv_bspin_cleanup (VisPluginData *plugin)
{
    BspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_mem_free (priv);

    return 0;
}

int lv_bspin_requisition (VisPluginData *plugin, int *width, int *height)
{
    return 0;
}

int lv_bspin_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
    visual_video_set_dimension (video, width, height);

    return 0;
}

int lv_bspin_events (VisPluginData *plugin, VisEventQueue *events)
{
    BspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                lv_bspin_dimension (plugin, ev.event.resize.video,
                        ev.event.resize.width, ev.event.resize.height);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "source"))
                    priv->source = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "place"))
                    priv->place = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "size"))
                    priv->size = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "type"))
                    priv->type = visual_param_entry_get_integer (param);
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

VisPalette *lv_bspin_palette (VisPluginData *plugin)
{
    BspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    return NULL;
}

int lv_bspin_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    BspinPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    int w = video->width;
    int h = video->height;

    int y,x;
    if (isBeat&0x80000000) return 0;
    for (y = 0; y < 2; y ++)
    {
        if (!(priv->enabled&(1<<y))) continue;
        unsigned char *fa_data=(unsigned char *)visdata[0][y];
        int xp,yp;
        int ss=min(h/2,(w*3)/8);
        double s=(double)ss;
        int c_x = (!y?w/2-ss/2:w/2+ss/2);
        int a=0,d=0;
        int nc=1;
        int oc6 = visual_color_to_uint32(priv->pal->colors[y]);
        for (x = 0; x < 44; x ++)
        {
            d+=fa_data[x];
        }
        
        a=(d*512)/(priv->last_a+30*256);
        
        priv->last_a=d;

        if (a > 255) a =255;
        priv->v[y] = 0.7*(max(a-104,12)/96.0) + 0.3*priv->v[y];
        priv->r_v[y] += 3.14159/6.0 * priv->v[y] * priv->dir[y];

        s *= a*1.0/256.0f;
        yp=(int)(sin(r_v[y])*s);
        xp=(int)(cos(r_v[y])*s);
        if (priv->mode==0)
        {
            if (priv->lx[0][y] || priv->ly[0][y]) line(framebuffer,lx[0][y],ly[0][y],xp+c_x,yp+h/2,w,h,oc6,(proxy->line_blend_mode&0xff0000)>>16);
            lx[0][y]=xp+c_x;
            ly[0][y]=yp+h/2;
            line(framebuffer,c_x,h/2,c_x+xp,h/2+yp,w,h,oc6,(proxy->line_blend_mode&0xff0000)>>16);
            if (lx[1][y] || ly[1][y]) line(framebuffer,lx[1][y],ly[1][y],c_x-xp,h/2-yp,w,h,oc6,(proxy->line_blend_mode&0xff0000)>>16);
            lx[1][y]=c_x-xp;
            ly[1][y]=h/2-yp;
            line(framebuffer,c_x,h/2,c_x-xp,h/2-yp,w,h,oc6,(proxy->line_blend_mode&0xff0000)>>16);
        }   
        else if (mode==1)
        {
            if (lx[0][y] || ly[0][y])
            {
                int points[6] = { c_x,h/2, lx[0][y], ly[0][y], xp+c_x,yp+h/2 };
                my_triangle(framebuffer,points,w,h,oc6);
            }
            lx[0][y]=xp+c_x;
            ly[0][y]=yp+h/2;
            if (lx[1][y] || ly[1][y])
            {
                int points[6] = { c_x,h/2, lx[1][y], ly[1][y], c_x-xp,h/2-yp };
                my_triangle(framebuffer,points,w,h,oc6);
            }
            lx[1][y]=c_x-xp;
            ly[1][y]=h/2-yp;
        }
    }


    return 0;
}

short get_data (BspinPrivate *priv, VisAudio *audio, int index)
{
    if (priv->type == 0)
        return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SCOPE, priv->source, index) >> 9;
    else    
        return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SPECTRUM, priv->source, index);

    return 0;
}

