/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_movement.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
 * fix for other depths than 32bits
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs.h"

AvsNumber PI = M_PI;

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];

    int levels;

} ColorreductionPrivate;

int lv_colorreduction_init (VisPluginData *plugin);
int lv_colorreduction_cleanup (VisPluginData *plugin);
int lv_colorreduction_events (VisPluginData *plugin, VisEventQueue *events);
int lv_colorreduction_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_colorreduction_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_colorreduction_palette,
		.video = lv_colorreduction_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_colorreduction",
		.name = "Libvisual AVS Transform: colorreduction element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: colorreduction element",
		.help = "This is the colorreduction element for the libvisual AVS system",

		.init = lv_colorreduction_init,
		.cleanup = lv_colorreduction_cleanup,
		.events = lv_colorreduction_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_colorreduction_init (VisPluginData *plugin)
{
	ColorreductionPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (ColorreductionPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

    return 0;
}

int lv_colorreduction_cleanup (VisPluginData *plugin)
{
	ColorreductionPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int lv_colorreduction_events (VisPluginData *plugin, VisEventQueue *events)
{
	ColorreductionPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_colorreduction_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_colorreduction_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ColorreductionPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

  if (isBeat&0x80000000) return 0;

    int a,b,c;
    a = 8-priv->levels;
    b = 0xFF;
    while (a--) b=(b<<1)&0xFF;
    b |= (b<<16) | (b<<8);
    c = w*h;
    __asm {
        mov ebx, framebuffer;
        mov ecx, c;
        mov edx, b;
        lp:
        sub ecx, 4;
        test ecx, ecx;
        jz end;
        and dword ptr [ebx+ecx*4], edx;
        and dword ptr [ebx+ecx*4+4], edx;
        and dword ptr [ebx+ecx*4+8], edx;
        and dword ptr [ebx+ecx*4+12], edx;
        jmp lp;
        end:
    }

    return 0;
}

