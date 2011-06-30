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

typedef enum trans_runnable TransRunnable;

enum trans_runnable {
    TRANS_RUNNABLE_INIT,
    TRANS_RUNNABLE_FRAME,
    TRANS_RUNNABLE_BEAT,
    TRANS_RUNNABLE_PIXEL,
};

typedef struct {
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];

    // params
    int scale, scale2, blend, beatch, subpixel;

} BlitPrivate;

int lv_blit_init (VisPluginData *plugin);
int lv_blit_cleanup (VisPluginData *plugin);
int lv_blit_events (VisPluginData *plugin, VisEventQueue *events);
int lv_blit_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_blit_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void trans_begin(BlitPrivate *priv, uint8_t isBeat, uint32_t *fbin, uint32_t *fbout);
static void trans_render(BlitPrivate *priv, uint8_t isBeat, uint32_t *fb, uint32_t *fbout);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_blit_palette,
		.video = lv_blit_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_blit",
		.name = "Libvisual AVS Transform: dynamic movement element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: dynamic movement element",
		.help = "This is the dynamic movement element for the libvisual AVS system",

		.init = lv_blit_init,
		.cleanup = lv_blit_cleanup,
		.events = lv_blit_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int trans_load_runnable(BlitPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(BlitPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_blit_init (VisPluginData *plugin)
{
	BlitPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("scale", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 100), "Blitter direction"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("scale2", 0, VISUAL_PARAM_LIMIT_NONE(0, 100), "Blitter direction (on beat)"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend blitter"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beatch", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable on-beat changes"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Bilenear filtering"),

		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (BlitPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_blit_cleanup (VisPluginData *plugin)
{
	BlitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_blit_events (VisPluginData *plugin, VisEventQueue *events)
{
	BlitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
                if (visual_param_entry_is (param, "scale"))
                    priv->scale = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "scale2"))
                    priv->scale2 = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "beatch"))
                    priv->beatch = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "subpixel"))
                    priv->subpixel = visual_param_entry_get_integer(param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_blit_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_blit_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	BlitPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

    return 0;
}


