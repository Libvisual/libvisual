/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_dotfnt.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stdotfnt.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs_matrix.h"

#define NUM_ROT_DIV 30
#define NUM_ROT_HEIGHT 256
typedef struct {
    float r, dr;
    float h, dh;
    float ax, ay;
    int c;
} FountainPoint;

typedef struct {
    AvsGlobalProxy *proxy;
    
    FountainPoint points[NUM_ROT_HEIGHT][NUM_ROT_DIV];
    int color_tab[64];
    float r;

    int rotvel, angle;
    VisPalette pal;

} DotfntPrivate;

int lv_dotfnt_init (VisPluginData *plugin);
int lv_dotfnt_cleanup (VisPluginData *plugin);
int lv_dotfnt_requisition (VisPluginData *plugin, int *width, int *height);
int lv_dotfnt_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_dotfnt_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_dotfnt_palette (VisPluginData *plugin);
int lv_dotfnt_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

void initcolortab(DotfntPrivate *priv);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_dotfnt_requisition,
		.palette = lv_dotfnt_palette,
		.render = lv_dotfnt_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_dotfnt",
		.name = "Libvisual AVS Render: dotfnt element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: dotfnt element",
		.help = "This is the dotfnt scope element for the libvisual AVS system",

		.init = lv_dotfnt_init,
		.cleanup = lv_dotfnt_cleanup,
		.events = lv_dotfnt_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_dotfnt_init (VisPluginData *plugin)
{
	DotfntPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("rotvel", 16, VISUAL_PARAM_LIMIT_NONE, "Rotation"),
		VISUAL_PARAM_LIST_ENTRY ("palette", "Palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 0x10, VISUAL_PARAM_LIMIT_NONE, "Size"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("type", 0, VISUAL_PARAM_LIMIT_NONE, "Type"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (DotfntPrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 5);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_container_add_many_proxy (paramcontainer, params);

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

	visual_palette_free_colors (&priv->pal);

    visual_color_set(priv->pal->colors[0], 24, 107, 28);
    visual_color_set(priv->pal->colors[1], 35, 10, 255);
    visual_color_set(priv->pal->colors[2], 116, 29, 42);
    visual_color_set(priv->pal->colors[3], 217, 54, 144);
    visual_color_set(priv->pal->colors[4], 255, 136, 107);

    initcolortab(priv);

    priv->angle = -20;

	return 0;
}

int lv_dotfnt_cleanup (VisPluginData *plugin)
{
	DotfntPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_dotfnt_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_dotfnt_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_dotfnt_events (VisPluginData *plugin, VisEventQueue *events)
{
	DotfntPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;
    int init_colortab = 0;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dotfnt_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "rotvel"))
					priv->rotvel = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "angle"))
					priv->angle = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "r")) {
                    int rr = visual_param_entry_get_integer (param);
                    priv->r = rr/32.0f;
				} else if (visual_param_entry_is (param, "palette")) {
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
                    init_colortab = 1;
				}

				break;

			default:
				break;
		}
	}

    if(init_colortab)
        initcolortab(priv);

	return 0;
}

VisPalette *lv_dotfnt_palette (VisPluginData *plugin)
{
	DotfntPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_dotfnt_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DotfntPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int i=w*h;
    int *p=framebuffer;

    if (isBeat&0x80000000) return 0;
    int fo, p;
    float matrix[16],matrix2[16];
    matrixRotate(matrix,2,r);
    matrixRotate(matrix2,1,(float)priv->angle);
    matrixMultiply(matrix,matrix2);
    matrixTranslate(matrix2,0.0f,-20.0f,400.0f);
    matrixMultiply(matrix,matrix2);


    FountainPoint pb[NUM_ROT_DIV];
    FountainPoint *in, *out;
    memcpy(pb,&points[0],sizeof(pb));
    fo = NUM_ROT_HEIGHT-2;
    do      // transform points and remove old ones
    {
        float booga = 1.3f / (fo+100);
        in =  &points[fo][0];       
        out = &points[fo+1][0];
        for (p = 0; p < NUM_ROT_DIV; p ++)
        {
            *out = *in;
            out->r += out->dr;
            out->dh += 0.05f;
            out->dr += booga;
            out->h += out->dh;
            out++;
            in++;       
        }
    } while (fo--);

    out = &points[0][0];
    in = pb;
    { // create new points
        float a;
        unsigned char *sd = (unsigned char *) proxy->audiodata[1][0];
        for (p = 0; p < NUM_ROT_DIV; p ++)
        {
            int t;
            if (p >= NUM_ROT_DIV) t= (sd[576] ^ 128);
            else t= (*sd ^ 128);
            sd++;
            t*=5;
            t/=4;
            t-=64;
            if (isBeat) t+=128;
            if (t > 255) t=255;
//      t+=sd[576]^128;
  //    t/=2;
            out->r = 1.0f;
            float dr = t/200.0f;
            if (dr < 0) dr = -dr;
            out->h = 250;
            dr += 1.0;
            out->dh = -dr * (100.0f + (out->dh - in->dh)) / 100.0f * 2.8f;
            t = t/4;
            if (t > 63) t = 63;
            out->c = color_tab[t];
            a = p* 3.14159f * 2.0f / NUM_ROT_DIV;
            out->ax=(float)sin(a);
            out->ay=(float)cos(a);
            out->dr =0.0;
            out++;
            in++;
        }
    }

    float adj=width*440.0f/640.0f;
    float adj2=height*440.0f/480.0f;
    if (adj2 < adj) adj=adj2;
    in = &points[0][0];
    for (fo = 0; fo < NUM_ROT_HEIGHT; fo ++)
    {
        for (p = 0; p < NUM_ROT_DIV; p ++)
        {
            register float x, y, z;
            matrixApply(matrix,in->ax*in->r,in->h,in->ay*in->r,&x,&y,&z);
            z = adj / z;
            if (z > 0.0000001)
            {
                register int ix = (int) (x * z) + video->width/2;
                register int iy = (int) (y * z) + video->height/2;
                if (iy >= 0 && iy < video->height && ix >= 0 && ix < video->width) 
                {
                    BLEND_LINE(framebuffer + iy*video->width + ix,in->c);
                }
            }
            in++;
        }
    }
    priv->r += priv->rotvel/5.0f;
    if (priv->r >= 360.0f) priv->r -= 360.0f;
    if (priv->r < 0.0f) priv->r += 360.0f;

    return 0;
}

void initcolortab(DotfntPrivate *priv)
{
    int x,r,g,b,dr,dg,db,t;
    uint32_t colors[5];

    for(t = 0; t < 5; t++)
    {
        colors[t] = visual_color_to_uint32(priv->pal->colors[t]);
    }
    
    for (t=0; t < 4; t ++)
    {
        r=(colors[t]&255)<<16;
        g=((colors[t]>>8)&255)<<16;
        b=((colors[t]>>16)&255)<<16;
        dr=(((colors[t+1]&255)-(colors[t]&255))<<16)/16;
        dg=((((colors[t+1]>>8)&255)-((colors[t]>>8)&255))<<16)/16;
        db=((((colors[t+1]>>16)&255)-((colors[t]>>16)&255))<<16)/16;
        for (x = 0; x < 16; x ++)
        {
            color_tab[t*16+x]=(r>>16)|((g>>16)<<8)|((b>>16)<<16);
            r+=dr;g+=dg;b+=db;
        }
    }
}

