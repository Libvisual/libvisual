/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "avs_parse.h"

typedef struct {
	AVSTree		*wtree;		/* The winamp AVS tree */
	int		 wavs;		/* TRUE if winamp AVS, FALSE if native libvisual AVS */

	LVAVSPreset	*lvtree;	/* The LV AVS tree */
	LVAVSPipeline	*pipeline;	/* The LV AVS Render pipeline */
} AVSPrivate;

int act_avs_init (VisPluginData *plugin);
int act_avs_cleanup (VisPluginData *plugin);
int act_avs_requisition (VisPluginData *plugin, int *width, int *height);
int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_avs_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_avs_palette (VisPluginData *plugin);
int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_avs_requisition,
		.palette = act_avs_palette,
		.render = act_avs_render,
		.depth = VISUAL_VIDEO_DEPTH_32BIT,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs",
		.name = "Libvisual advanced visual studio plugin",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The libvisual advanced visual studio plugin",
		.help = "",

		.init = act_avs_init,
		.cleanup = act_avs_cleanup,
		.events = act_avs_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_avs_init (VisPluginData *plugin)
{
	AVSPrivate *priv;

	priv = visual_mem_new0 (AVSPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);
	
	return 0;
}

int act_avs_cleanup (VisPluginData *plugin)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static const VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING ("filename", NULL),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("winamp avs", 1),
		VISUAL_PARAM_LIST_END
	};

	visual_param_container_add_many (paramcontainer, params);

	visual_mem_free (priv);

	return 0;
}

int act_avs_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	
	visual_video_set_dimension (video, width, height);

	lvavs_pipeline_negotiate (priv->avs_pipeline, video);

	return 0;
}

int act_avs_events (VisPluginData *plugin, VisEventQueue *events)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_avs_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				if (visual_param_entry_is (param, "filename")) {
					char *filename = visual_param_entry_get_string (param);
					
					if (priv->wtree != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->wtree));

					if (priv->lvtree != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->lvtree));

					if (priv->avs_pipeline != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->avs_pipeline));

					priv->wtree = NULL;
					
					if (filename != NULL) {
						if (priv->wavs == TRUE) {
							priv->wtree = avs_tree_new_from_preset (filename);
							priv->lvtree = lvavs_preset_new_from_wavs (priv->wtree);
						} else {
							priv->lvtree = lvavs_preset_new_from_preset (filename);
						}
					} else {
						LVAVSPreset *preset;
						preset = lvavs_preset_new ();
						preset->main = lvavs_preset_container_new ();

						visual_list_add (preset->main->members,
								lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN,
									"oinksie"));

						priv->lvtree = preset;
					}

					/* Neat, now make the render pipeline */
					priv->avs_pipeline = lvavs_pipeline_new_from_preset (priv->lvtree);

					lvavs_pipeline_realize (priv->avs_pipeline);

					/* Negotiate complete pipeline for current size */
				}

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				if (visual_param_entry_is (param, "winamp avs")) {
					priv->wavs = visual_param_entry_get_integer (param);
						
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_avs_palette (VisPluginData *plugin)
{
	return NULL;
}

int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	lvavs_pipeline_run (priv->avs_pipeline, video, audio);

	return 0;
}

