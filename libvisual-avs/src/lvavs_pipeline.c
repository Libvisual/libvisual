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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "lvavs_preset.h"
#include "lvavs_pipeline.h"

/* Prototypes */
static int lvavs_pipeline_dtor (VisObject *object);
static int lvavs_pipeline_element_dtor (VisObject *object);
static int lvavs_pipeline_container_dtor (VisObject *object);

int pipeline_from_preset (LVAVSPipelineContainer *container, LVAVSPresetContainer *presetcont);
int pipeline_container_realize (LVAVSPipelineContainer *container);
int pipeline_container_negotiate (LVAVSPipelineContainer *container, VisVideo *video);
int pipeline_container_run (LVAVSPipelineContainer *container, VisVideo *video, VisAudio *audio);

/* Object destructors */
static int lvavs_pipeline_dtor (VisObject *object)
{
	LVAVSPipeline *pipeline = LVAVS_PIPELINE (object);

	if (pipeline->renderstate != NULL)
		visual_object_unref (VISUAL_OBJECT (pipeline->renderstate));

	if (pipeline->container != NULL)
		visual_object_unref (VISUAL_OBJECT (pipeline->container));

	pipeline->renderstate = NULL;
	pipeline->container = NULL;

	return VISUAL_OK;
}

static int lvavs_pipeline_element_dtor (VisObject *object)
{
	LVAVSPipelineElement *element = LVAVS_PIPELINE_ELEMENT (object);

	if (element->pipeline != NULL)
		visual_object_unref (VISUAL_OBJECT (element->pipeline));

	if (element->params != NULL)
		visual_object_unref (VISUAL_OBJECT (element->params));
	
	switch (element->type) {
		case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:
		case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:
		case LVAVS_PIPELINE_ELEMENT_TYPE_MORPH:
		case LVAVS_PIPELINE_ELEMENT_TYPE_RENDERSTATE:
		case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:

			visual_object_unref (VISUAL_OBJECT (element->data.actor));

			break;

		default:
			visual_log (VISUAL_LOG_WARNING, "Tried to destroy invalid LVAVSPipelineElement type");

			break;
	}

	element->pipeline = NULL;
	element->params = NULL;
	element->data.actor = NULL;
	
	return VISUAL_OK;
}

static int lvavs_pipeline_container_dtor (VisObject *object)
{
	LVAVSPipelineContainer *container = LVAVS_PIPELINE_CONTAINER (object);

	if (container->members != NULL)
		visual_object_unref (VISUAL_OBJECT (container->members));

	container->members = NULL;

	lvavs_element_dtor (object);

	return VISUAL_OK;
}


/* LVAVS Preset */
LVAVSPipeline *lvavs_pipeline_new ()
{
	LVAVSPipeline *pipeline;

	pipeline = visual_mem_new0 (LVAVSPipeline, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (pipeline), TRUE, lvavs_pipeline_dtor);

	return pipeline;
}

LVAVSPipelineElement *lvavs_pipeline_element_new (LVAVSPipelineElementType type)
{
	LVAVSPipelineElement *element;

	element = visual_mem_new0 (LVAVSPipelineElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (element), TRUE, lvavs_pipeline_element_dtor);

	element->type = type;

	return element;
}

LVAVSPipelineContainer *lvavs_pipeline_container_new ()
{
	LVAVSPipelineContainer *container;

	container = visual_mem_new0 (LVAVSPipelineContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (container), TRUE, lvavs_pipeline_container_dtor);

	LVAVS_PIPELINE_ELEMENT (container)->type = LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER;
	
	container->members = visual_list_new (visual_object_list_destroyer);

	return container;
}

LVAVSPipeline *lvavs_pipeline_new_from_preset (LVAVSPreset *preset)
{
	LVAVSPipeline *pipeline;

	pipeline = lvavs_pipeline_new ();

	pipeline->container = lvavs_pipeline_container_new ();
	LVAVS_PIPELINE_ELEMENT (pipeline->container)->pipeline = pipeline;
	
	pipeline_from_preset (pipeline->container, preset->main);

	return pipeline;
}

int lvavs_pipeline_realize (LVAVSPipeline *pipeline)
{
	pipeline_container_realize (LVAVS_PIPELINE_CONTAINER (pipeline->container));

	return VISUAL_OK;
}

/* The pipeline is currently depth unaware, and always runs in 32bits mode.
 * This will change when we introduce VisPipeline.
 */
int lvavs_pipeline_negotiate (LVAVSPipeline *pipeline, VisVideo *video)
{
	pipeline_container_negotiate (LVAVS_PIPELINE_CONTAINER (pipeline->container), video);

	return VISUAL_OK;
}

int lvavs_pipeline_run (LVAVSPipeline *pipeline, VisVideo *video, VisAudio *audio)
{
	pipeline_container_run (LVAVS_PIPELINE_CONTAINER (pipeline->container), video, audio);

	return VISUAL_OK;
}

/* Internal functions */
int pipeline_from_preset (LVAVSPipelineContainer *container, LVAVSPresetContainer *presetcont)
{
	VisListEntry *le = NULL;
	LVAVSPresetElement *pelem;
	LVAVSPipelineElement *element;
	LVAVSPipelineContainer *cont;
	VisPluginRef *ref;

	while ((pelem = visual_list_next (presetcont->members, &le)) != NULL) {

		switch (pelem->type) {
			case LVAVS_PRESET_ELEMENT_TYPE_PLUGIN:
				ref = visual_plugin_find (visual_plugin_get_registry (), pelem->element_name);

				if (ref == NULL) {
					visual_log (VISUAL_LOG_CRITICAL, "Requested plugin %s not in registry", pelem->element_name);

					break;
				}

				if (ref->info == NULL) {
					visual_log (VISUAL_LOG_CRITICAL, "Could not get VisPluginInfo for %s", pelem->element_name);

					break;
				}

				if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_ACTOR) == 0) {

					element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR);
					element->data.actor = visual_actor_new (pelem->element_name);

				} else if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_MORPH) == 0) {
					
					element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_MORPH);
					element->data.morph = visual_morph_new (pelem->element_name);
	
				} else {

					visual_Log (VISUAL_LOG_CRITICAL, "Unsupported plugin type for LVAVSPipelineElement %s",
							ref->info->type);

					break;
				}

				if (pelem->pcont != NULL) {
					element->params = visual_param_container_new ();
					visual_param_container_copy (element->params, pelem->pcont);
				}

				element->pipeline = LVAVS_PIPELINE_ELEMENT (container)->pipeline;
				
				visual_list_add (container->members, element);
				
				break;

			case LVAVS_PRESET_ELEMENT_TYPE_CONTAINER:
				cont = lvavs_pipeline_container_new ();
				
				visual_list_add (container->members, cont);
				
				pipeline_from_preset (cont, LVAVS_PRESET_CONTAINER (pelem));
				
				break;

			case LVAVS_PRESET_ELEMENT_TYPE_RENDERMODE:

				break;

			case LVAVS_PRESET_ELEMENT_TYPE_COMMENT:

				break;

			default:
				visual_log (VISUAL_LOG_CRITICAL, "Invalid LVAVSPresetElementType in LVAVSPresetElement");

				break;
		}
	}

	return VISUAL_OK;
}

int pipeline_container_realize (LVAVSPipelineContainer *container)
{
	VisListEntry *le = NULL;
	LVAVSPipelineElement *element;
	
	while ((element = visual_list_next (container->members, &le)) != NULL) {

		switch (element->type) {
			case LVAVS_PIPELINE_ELEMENT_TYPE_NULL:

				break;
				
			case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:

				visual_actor_realize (element->data.actor);
				visual_param_container_copy_match (visual_plugin_get_params (
							visual_actor_get_plugin (element->data.actor)),
							element->params);
				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:
			
				break;
			
			case LVAVS_PIPELINE_ELEMENT_TYPE_MORPH:
				
				visual_morph_realize (element->data.morph);
				
				break;
			
			case LVAVS_PIPELINE_ELEMENT_TYPE_RENDERSTATE:
				
				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:
				
				pipeline_container_realize (LVAVS_PIPELINE_CONTAINER (element));

				break;

			default:
				visual_log (VISUAL_LOG_CRITICAL, "Invalid LVAVSPipelineElementType");

				break;
		}
	}
}

int pipeline_container_negotiate (LVAVSPipelineContainer *container, VisVideo *video)
{
	VisListEntry *le = NULL;
	LVAVSPipelineElement *element;
	
	while ((element = visual_list_next (container->members, &le)) != NULL) {

		switch (element->type) {
			case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:
				visual_actor_set_video (element->data.actor, video);
				visual_actor_video_negotiate (element->data.actor, VISUAL_VIDEO_DEPTH_NONE, FALSE, FALSE);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:
				
				pipeline_container_negotiate (LVAVS_PIPELINE_CONTAINER (element), video);

				break;

			default:

				break;
		}
	}

	return VISUAL_OK;
}

int pipeline_container_run (LVAVSPipelineContainer *container, VisVideo *video, VisAudio *audio)
{
	VisListEntry *le = NULL;
	LVAVSPipelineElement *element;
	
	while ((element = visual_list_next (container->members, &le)) != NULL) {

		switch (element->type) {
			case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:

				visual_actor_set_video (element->data.actor, video);
				visual_actor_run (element->data.actor, audio);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:
				
				pipeline_container_run (LVAVS_PIPELINE_CONTAINER (element), video, audio);

				break;

			default:

				break;
		}
	}

	return VISUAL_OK;
}

