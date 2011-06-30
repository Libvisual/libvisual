/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_nfclr.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stnfclr.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    int nf, cf, df;
    int color, blend;

} NfclrPrivate;

int lv_nfclr_init (VisPluginData *plugin);
int lv_nfclr_cleanup (VisPluginData *plugin);
int lv_nfclr_requisition (VisPluginData *plugin, int *width, int *height);
int lv_nfclr_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_nfclr_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_nfclr_palette (VisPluginData *plugin);
int lv_nfclr_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_nfclr_requisition,
		.palette = lv_nfclr_palette,
		.render = lv_nfclr_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_nfclr",
		.name = "Libvisual AVS Render: On-beat clear element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: On-beat clear element",
		.help = "This is the On-beat clear element for the libvisual AVS system",

		.init = lv_nfclr_init,
		.cleanup = lv_nfclr_cleanup,
		.events = lv_nfclr_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_nfclr_init (VisPluginData *plugin)
{
	NfclrPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0, VISUAL_PARAM_LIMIT_NONE, "Blend to color"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("nf", 1, VISUAL_PARAM_LIMIT_NONE, "Clear every N beats"),
		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (NfclrPrivate, 1);
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

int lv_nfclr_cleanup (VisPluginData *plugin)
{
	NfclrPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_nfclr_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_nfclr_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_nfclr_events (VisPluginData *plugin, VisEventQueue *events)
{
	NfclrPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_nfclr_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "blend"))
					priv->blend = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "nf"))
					priv->nf = visual_param_entry_get_integer (param);
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

VisPalette *lv_nfclr_palette (VisPluginData *plugin)
{
	NfclrPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

#define NO_MMX

int lv_nfclr_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	NfclrPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;
    int p=0;
    char *t=(char *)&visdata[1][0][0];
    int np=0;
    if (isBeat&0x80000000) return 0;

    if (isBeat)
    {
        if (nf && ++cf >= nf)
        {
            cf=df=0;
            int i=w*h;
            int c=color;
            if (!blend) __asm
            {
                mov ecx, i
                mov edi, framebuffer
                mov eax, c
                rep stosd
            } 
            else 
            {
#ifdef NO_MMX
                while (i--)
                {
                    *framebuffer=BLEND_AVG(*framebuffer,color);
                    framebuffer++;
                }
#else
                {
                    int icolor[2]={this->color,this->color};
                    int vc[2] = { ~((1<<7)|(1<<15)|(1<<23)),~((1<<7)|(1<<15)|(1<<23))};
                    i/=4;
                    __asm
                    {
                        movq mm6, vc
                        movq mm7, icolor
                        psrlq mm7, 1
                        pand mm7, mm6
                        mov edx, i
                        mov edi, framebuffer
                        _l1:
                        movq mm0, [edi]

                        movq mm1, [edi+8]
                        psrlq mm0, 1

                        pand mm0, mm6
                        psrlq mm1, 1

                        paddb mm0, mm7
                        pand mm1, mm6

                        movq [edi], mm0
                        paddb mm1, mm7

                        movq [edi+8], mm1
                        add edi, 16
                        dec edx
                        jnz _l1
                        emms
                    }
                }
#endif
            }
        }
    }
    else if (++df >= nf)
    {
        df=0;
    //  memset(framebuffer,0,w*h*4);
    }

    return 0;
}

