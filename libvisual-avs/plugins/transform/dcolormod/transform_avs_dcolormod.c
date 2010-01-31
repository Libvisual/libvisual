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
    AvsVariable var_r, var_g, var_b, var_beat;

    // params
    char *init, *frame, *beat, *pixel;
    int recompute;

    // Others
    int tab_valid;
    uint8_t tab[768];
    int inited;

} DColormodPrivate;

int lv_dcolormod_init (VisPluginData *plugin);
int lv_dcolormod_cleanup (VisPluginData *plugin);
int lv_dcolormod_events (VisPluginData *plugin, VisEventQueue *events);
int lv_dcolormod_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_dcolormod_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_dcolormod_palette,
		.video = lv_dcolormod_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_dcolormod",
		.name = "Libvisual AVS Transform: dcolormod element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: dcolormod element",
		.help = "This is the dcolormod element for the libvisual AVS system",

		.init = lv_dcolormod_init,
		.cleanup = lv_dcolormod_cleanup,
		.events = lv_dcolormod_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_dcolormod_init (VisPluginData *plugin)
{
	DColormodPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("recompute", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Recompute every frame"),
        VISUAL_PARAM_LIST_STRING("init", "", ""),
        VISUAL_PARAM_LIST_STRING("frame", "", ""),
        VISUAL_PARAM_LIST_STRING("beat", "", ""),
        VISUAL_PARAM_LIST_STRING("pixel", "", ""),
        VISUAL_PARAM_LIST_INTEGER("example", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 10), "Load an example. This parameter is not included in the avs preset."),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (DColormodPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();

    avs_runnable_variable_bind(priv->vm, "red", &priv->var_r);
    avs_runnable_variable_bind(priv->vm, "green", &priv->var_g);
    avs_runnable_variable_bind(priv->vm, "blue", &priv->var_b);
    avs_runnable_variable_bind(priv->vm, "beat", &priv->var_beat);

    return 0;
}

int lv_dcolormod_cleanup (VisPluginData *plugin)
{
	DColormodPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int trans_load_runnable(DColormodPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(DColormodPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_dcolormod_events (VisPluginData *plugin, VisEventQueue *events)
{
	DColormodPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

                if(visual_param_entry_is(param, "init")) {
                    priv->init = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_INIT, priv->pixel);
                    trans_run_runnable(priv, TRANS_RUNNABLE_INIT);
                } else if(visual_param_entry_is(param, "frame"))
                    priv->frame = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_FRAME, priv->pixel);
                } else if(visual_param_entry_is(param, "beat"))
                    priv->beat = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_BEAT, priv->pixel);
                } else if(visual_param_entry_is(param, "pixel"))
                    priv->pixel = visual_param_entry_get_string(param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_PIXEL, priv->pixel);
                } else if(visual_param_entry_is(param, "color_dist"))
                    priv->pixel = visual_param_entry_get_string(param);
                else if(visual_param_entry_is(param, "recompute"))
                    priv->recompute = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "example"))
                    priv->example = visual_param_entry_get_integer(param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_dcolormod_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_dcolormod_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DColormodPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

    if (isBeat&0x80000000) return 0;

    *var_beat=isBeat?1.0:0.0;

    if (codehandle[3] && !inited) { executeCode(codehandle[3],visdata); inited=1; }
    executeCode(codehandle[1],visdata);
    trans_run_runnable(priv, TRANS_RUNNABLE_FRAME);

    if (isBeat)
        trans_run_runnable(priv, TRANS_RUNNABLE_BEAT);

    if (priv->recompute || !priv->tab_valid)
    {
        int x;
        unsigned char *t=priv->tab;
        for (x = 0; x < 256; x ++)
        {
            priv->var_r = priv->var_b = priv->var_g = x/255.0;
            trans_run_runnable(priv, TRANS_RUNNABLE_PIXEL);
            int r=(int) (priv->var_r*255.0 + 0.5);
            int g=(int) (priv->var_g*255.0 + 0.5);
            int b=(int) (priv->var_b*255.0 + 0.5);
            if (r < 0) r=0;
            else if (r > 255)r=255;
            if (g < 0) g=0;
            else if (g > 255)g=255;
            if (b < 0) b=0;
            else if (b > 255)b=255;
            t[512]=r;
            t[256]=g;
            t[0]=b;
            t++;
        }
        priv->tab_valid=1;
    }

    unsigned char *fb=(unsigned char *)pixels;
    int l=w*h;
    while (l--)
    {
        fb[0]=priv->tab[fb[0]];
        fb[1]=priv->tab[(int)fb[1]+256];
        fb[2]=priv->tab[(int)fb[2]+512];
        fb+=4;
    }

    return 0;
}

