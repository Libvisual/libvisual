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
#include "lvavs_preset.h"
#include "lvavs_pipeline.h"

typedef struct {
	AVSTree		*wtree;		/* The winamp AVS tree */
	int		 wavs;		/* TRUE if winamp AVS, FALSE if native libvisual AVS */

	LVAVSPreset	*lvtree;	/* The LV AVS tree */
	LVAVSPipeline	*pipeline;	/* The LV AVS Render pipeline */

	int		 needsnego;	/* Pipeline out of sync, needs reneg ? */
} AVSPrivate;

int act_avs_init (VisPluginData *plugin);
int act_avs_cleanup (VisPluginData *plugin);
int act_avs_requisition (VisPluginData *plugin, int *width, int *height);
int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_avs_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_avs_palette (VisPluginData *plugin);
int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_avs_requisition,
		.palette = act_avs_palette,
		.render = act_avs_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT,
	}};

	static const VisPluginInfo info[] = {{
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

	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
//		VISUAL_PARAM_LIST_ENTRY_STRING ("filename", "/usr/src/libvisual-bromo/libvisual-avs/testpresets/sscope_movement2.avs"),
		VISUAL_PARAM_LIST_ENTRY_STRING ("filename", NULL),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("winamp avs", 1),
		VISUAL_PARAM_LIST_END
	};

	visual_param_container_add_many (paramcontainer, params);

	priv = visual_mem_new0 (AVSPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	return 0;
}

int act_avs_cleanup (VisPluginData *plugin)
{
	AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

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

	if (priv->pipeline != NULL)
		lvavs_pipeline_negotiate (priv->pipeline, video);

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
				act_avs_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "filename")) {
					char *filename = visual_param_entry_get_string (param);
					
					if (priv->wtree != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->wtree));

					if (priv->lvtree != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->lvtree));

					if (priv->pipeline != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->pipeline));

					priv->wtree = NULL;

					priv->wavs = TRUE;
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
									"avs_superscope"));

						visual_list_add (preset->main->members,
								lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN,
									"avs_movement"));

//						visual_list_add (preset->main->members,
//								lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN,
//									"avs_onetone"));

						priv->lvtree = preset;
					}

					/* Neat, now make the render pipeline */
					priv->pipeline = lvavs_pipeline_new_from_preset (priv->lvtree);

					lvavs_pipeline_realize (priv->pipeline);

					priv->needsnego = TRUE;
				}

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

	if (priv->needsnego == TRUE) {
		lvavs_pipeline_negotiate (priv->pipeline, video);

		priv->needsnego = FALSE;
	}

	/* Clear screen bit is on, clear screen every frame (This is from winamp AVS main section) */
#if 0
	if (visual_param_entry_get_integer (visual_param_container_get (LVAVS_PRESET_ELEMENT (priv->lvtree->main)->pcont,
					"clear screen")) == 1) {
		memset ((uint8_t *) visual_video_get_pixels (video), 0, visual_video_get_size (video));
	}
#endif
	lvavs_pipeline_run (priv->pipeline, video, audio);

	return 0;
}

