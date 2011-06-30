/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_clear.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <omp.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "lvavs_pipeline.h"
#include "avs.h"

typedef struct {
    LVAVSPipeline *pipeline;
    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable;
    AvsNumber beat, clear;

    // params
    char *enabled;

} ClearPrivate;

int lv_clear_init (VisPluginData *plugin);
int lv_clear_cleanup (VisPluginData *plugin);
int lv_clear_events (VisPluginData *plugin, VisEventQueue *events);
int lv_clear_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_clear_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

//void smp_render(ClearPrivate *priv, float visdata[2][2][1024], int isBeat, unsigned int *framebuffer, unsigned int *fbout, int w, int h);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int load_runnable(ClearPrivate *priv, char *buf) {
	AvsRunnable *obj = avs_runnable_new(priv->ctx);
	avs_runnable_set_variable_manager(obj, priv->vm);
	priv->runnable = obj;
	avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
	return 0;
}

static int run_runnable(ClearPrivate *priv) {
	avs_runnable_execute(priv->runnable);
	return 0;
}

const VisPluginInfo *get_plugin_info(int *count);
const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_clear_palette,
		.video = lv_clear_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_clearscreen",
		.name = "Libvisual AVS Transform: clear element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: clear element",
		.help = "This is the clear element for the libvisual AVS system",

		.init = lv_clear_init,
		.cleanup = lv_clear_cleanup,
		.events = lv_clear_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_clear_init (VisPluginData *plugin)
{
	ClearPrivate *priv;
	VisParamEntry *entry;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING ("enabled", "clear=1;"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER("test", 1),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (ClearPrivate, 1);

	priv->pipeline = (LVAVSPipeline *)visual_object_get_private(VISUAL_OBJECT(plugin));
	visual_object_ref(VISUAL_OBJECT(priv->pipeline));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many (paramcontainer, params);

/*
	entry = visual_param_container_get(paramcontainer, "test");
	visual_param_entry_min_set_integer(entry, -0xf7);
	visual_param_entry_max_set_integer(entry, 0xf7);
	visual_param_entry_set_annotation(entry, "Test and test again damnit yyyyyeaaaaahhhh");
	visual_param_entry_default_set_integer(entry, 35);
*/
	priv->ctx = avs_runnable_context_new();
	priv->vm = avs_runnable_variable_manager_new();

	avs_runnable_variable_bind(priv->vm, "clear", &priv->clear);
	avs_runnable_variable_bind(priv->vm, "beat", &priv->beat);

	return 0;
}

int lv_clear_cleanup (VisPluginData *plugin)
{
	ClearPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_object_unref(VISUAL_OBJECT(priv->pipeline));

	if(priv->enabled != NULL)
		visual_mem_free(priv->enabled);

	visual_mem_free (priv);

	return 0;
}

int lv_clear_events (VisPluginData *plugin, VisEventQueue *events)
{
	ClearPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
                                if (visual_param_entry_is (param, "enabled")) {
				    if(priv->enabled != NULL)
					visual_mem_free(priv->enabled);

                                    priv->enabled = strdup(visual_param_entry_get_string(param));
				    load_runnable(priv, priv->enabled);
				}

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_clear_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_clear_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ClearPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	priv->beat = priv->pipeline->isBeat;

	run_runnable(priv);

	if(priv->clear) {
		visual_mem_set(priv->pipeline->fbout, 0, sizeof(int) * video->width * video->height);
		visual_mem_set(priv->pipeline->framebuffer, 0, sizeof(int) * video->width * video->height);
		priv->clear = FALSE;
	}
	return 0;
}

