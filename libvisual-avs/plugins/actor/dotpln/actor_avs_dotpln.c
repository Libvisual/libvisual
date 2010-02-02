/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_dotpln.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stdotpln.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs_matrix.h"

#define NUM_WIDTH 64

typedef struct {
    AvsGlobalProxy *proxy;

    int rotvel;
    int angle;
    VisPalette pal;
    float r;
    float atable[NUM_WIDTH*NUM_WIDTH];
    float vtable[NUM_WIDTH*NUM_WIDTH];
    float ctable[NUM_WIDTH*NUM_WIDTH];
    int color_tab[64];

} DotplnPrivate;

int lv_dotpln_init (VisPluginData *plugin);
int lv_dotpln_cleanup (VisPluginData *plugin);
int lv_dotpln_requisition (VisPluginData *plugin, int *width, int *height);
int lv_dotpln_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_dotpln_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_dotpln_palette (VisPluginData *plugin);
int lv_dotpln_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

void initcolortab (DotplnPrivate *priv);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_dotpln_requisition,
		.palette = lv_dotpln_palette,
		.render = lv_dotpln_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_dotpln",
		.name = "Libvisual AVS Render: dot plane element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: dot plane element",
		.help = "This is the dot plane scope element for the libvisual AVS system",

		.init = lv_dotpln_init,
		.cleanup = lv_dotpln_cleanup,
		.events = lv_dotpln_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_dotpln_init (VisPluginData *plugin)
{
	DotplnPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("rotvel", 1, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("angle", 0x10, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("r", 0, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (DotplnPrivate, 1);
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

	return 0;
}

int lv_dotpln_cleanup (VisPluginData *plugin)
{
	DotplnPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_dotpln_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_dotpln_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_dotpln_events (VisPluginData *plugin, VisEventQueue *events)
{
	DotplnPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;
    int init_colortab;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dotpln_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "rotvel"))
					priv->source = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "angle"))
					priv->place = visual_param_entry_get_integer (param);
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

VisPalette *lv_dotpln_palette (VisPluginData *plugin)
{
	DotplnPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_dotpln_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DotplnPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int width = video->width;
	int height = video->height;
    int isBeat = proxy->isBeat;

    if (isBeat&0x80000000) return 0;
    float btable[NUM_WIDTH];
    int fo, p;
    float matrix[16],matrix2[16];
    matrixRotate(matrix,2,r);
    matrixRotate(matrix2,1,(float)priv->angle);
    matrixMultiply(matrix,matrix2);
    matrixTranslate(matrix2,0.0f,-20.0f,400.0f);
    matrixMultiply(matrix,matrix2);

    memcpy(btable,&priv->atable[0],sizeof(float)*NUM_WIDTH);
    for (fo = 0; fo < NUM_WIDTH; fo ++)
    {
        float *i, *o, *v, *ov;
        int *c,*oc;
        int t=(NUM_WIDTH-(fo+2))*NUM_WIDTH;
        i = &priv->atable[t];
        o = &priv->atable[t+NUM_WIDTH];
        v = &priv->vtable[t];
        ov = &priv->vtable[t+NUM_WIDTH];
        c = &priv->ctable[t];
        oc = &priv->ctable[t+NUM_WIDTH];
        if (fo == NUM_WIDTH-1) 
        {
            unsigned char *sd = (unsigned char *)&proxy->audiodata[0][0][0];
            i = btable;
            for (p = 0; p < NUM_WIDTH; p ++)
            {
                register int t;
                t=max(sd[0],sd[1]);
                t=max(t,sd[2]);
                *o = (float)t;
                t>>=2;
                if (t > 63) t=63;
                *oc++=priv->color_tab[t];

                *ov++ = (*o++ - *i++) / 90.0f;
                sd+=3;
            }
        }
        else for (p = 0; p < NUM_WIDTH; p ++)
        {
            *o = *i++ + *v;
            if (*o < 0.0f) *o=0.0f;
            *ov++ = *v++ - 0.15f*(*o++/255.0f);
            *oc++ = *c++;
        
        }
    }

    float adj=width*440.0f/640.0f;
    float adj2=height*440.0f/480.0f;
    if (adj2 < adj) adj=adj2;
    for (fo = 0; fo < NUM_WIDTH; fo ++)
    {
        int f = (priv->r < 90.0 || priv->r > 270.0) ? NUM_WIDTH-fo-1 : fo;
        float dw=350.0f/(float)NUM_WIDTH, w = -(NUM_WIDTH*0.5f)*dw;
        float q = (f - NUM_WIDTH*0.5f)*dw;
        int *ct = &priv->ctable[f*NUM_WIDTH];
        float *at = &priv->atable[f*NUM_WIDTH];
        int da=1;
        if (priv->r < 180.0)
        {
            da=-1;
            dw=-dw;
            w=-w+dw;
            ct += NUM_WIDTH-1;
            at += NUM_WIDTH-1;
        }
        for (p = 0; p < NUM_WIDTH; p ++)
        {
            float x, y, z;
            matrixApply(matrix,w,64.0f-*at,q,&x,&y,&z);
            z = adj / z;
            register int ix = (int) (x * z) + (width/2);
            register int iy = (int) (y * z) + (height/2);
            if (iy >= 0 && iy < height && ix >= 0 && ix < width) 
            {
                BLEND_LINE(framebuffer + iy*width + ix,*ct);
            }
            w+=dw;
            ct+=da;
            at+=da;
        }
    }
    r += priv->rotvel/5.0f;
    if (r >= 360.0f) r -= 360.0f;
    if (r < 0.0f) r += 360.0f;

    return 0;
}

void initcolortab(DotplnPrivate *priv)
{
    int x,r,g,b,dr,dg,db,t;
    int colors[5];

    for( t = 0; t < 5; t++)
        colors[t] = visual_color_to_int32(priv->pal->colors[t]);

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
            priv->color_tab[t*16+x]=(r>>16)|((g>>16)<<8)|((b>>16)<<16);
            r+=dr;g+=dg;b+=db;
        }
    }
}

