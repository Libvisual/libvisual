/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lvavs_pipeline.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
int pipeline_container_propagate_event (LVAVSPipelineContainer *container, VisEvent *event);
int pipeline_container_run (LVAVSPipelineContainer *container, VisVideo *video, VisAudio *audio);

/* Object destructors */
static int lvavs_pipeline_dtor (VisObject *object)
{
	LVAVSPipeline *pipeline = LVAVS_PIPELINE (object);

	if (pipeline->renderstate != NULL)
		visual_object_unref (VISUAL_OBJECT (pipeline->renderstate));

	if (pipeline->container != NULL)
		visual_object_unref (VISUAL_OBJECT (pipeline->container));

    //if (pipeline->proxy != NULL)
    //    visual_object_unref (VISUAL_OBJECT (pipeline->proxy));

	pipeline->renderstate = NULL;
	pipeline->container = NULL;

	return TRUE;
}

static int lvavs_pipeline_element_dtor (VisObject *object)
{
	LVAVSPipelineElement *element = LVAVS_PIPELINE_ELEMENT (object);

	if (element->params != NULL)
		visual_object_unref (VISUAL_OBJECT (element->params));

	switch (element->type) {
		case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:
			visual_object_unref (VISUAL_OBJECT (element->data.actor));
			element->data.actor = NULL;

			break;

		case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:
			visual_object_unref (VISUAL_OBJECT (element->data.transform));
			element->data.transform = NULL;

			break;


		case LVAVS_PIPELINE_ELEMENT_TYPE_MORPH:
			visual_object_unref (VISUAL_OBJECT (element->data.morph));
			element->data.morph = NULL;

			break;

		case LVAVS_PIPELINE_ELEMENT_TYPE_RENDERSTATE:
		case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:

			break;

		default:
			visual_log (VISUAL_LOG_WARNING, "Tried to destroy invalid LVAVSPipelineElement type");

			break;
	}

	element->pipeline = NULL;
	element->params = NULL;

	return TRUE;
}

static int lvavs_pipeline_container_dtor (VisObject *object)
{
	LVAVSPipelineContainer *container = LVAVS_PIPELINE_CONTAINER (object);

	if (container->members != NULL)
		visual_object_unref (VISUAL_OBJECT (container->members));

	container->members = NULL;

	lvavs_pipeline_element_dtor (object);

	return TRUE;
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

	container->members = visual_list_new (visual_object_collection_destroyer);

	return container;
}

LVAVSPipeline *lvavs_pipeline_new_from_preset (LVAVSPreset *preset)
{
	LVAVSPipeline *pipeline;

	pipeline = lvavs_pipeline_new ();

    pipeline->proxy = avs_global_proxy_new();

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

int lvavs_pipeline_propagate_event (LVAVSPipeline *pipeline, VisEvent *event)
{
	return pipeline_container_propagate_event (pipeline->container, event);
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
    LVAVSPipeline *pipeline = LVAVS_PIPELINE_ELEMENT(container)->pipeline; 

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

				/* FIXME fix libvisual type lookup and use the functions here */
				if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_ACTOR) == 0) {

					element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR);
					element->data.actor = visual_actor_new (pelem->element_name);
                    visual_object_set_private(VISUAL_OBJECT(element->data.actor->plugin), pipeline->proxy);

				} else if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_MORPH) == 0) {

					element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_MORPH);
					element->data.morph = visual_morph_new (pelem->element_name);
                    visual_object_set_private(VISUAL_OBJECT(element->data.morph->plugin), pipeline->proxy);

				} else if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_TRANSFORM) == 0) {

					element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM);
					element->data.transform = visual_transform_new (pelem->element_name);
                    visual_object_set_private(VISUAL_OBJECT(element->data.transform->plugin), pipeline->proxy);

				} else {
                    printf("uknown type '%s' '%s'\n", ref->info->type, ref->info->name);
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

			case LVAVS_PRESET_ELEMENT_TYPE_RENDERSTATE:

				break;

			case LVAVS_PRESET_ELEMENT_TYPE_COMMENT:

				break;

            case LVAVS_PRESET_ELEMENT_TYPE_BPM:

                break;

            case LVAVS_PRESET_ELEMENT_TYPE_STACK:

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
							visual_actor_get_plugin (element->data.actor)), element->params);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:

				visual_transform_realize (element->data.transform);
				visual_param_container_copy_match (visual_plugin_get_params (
							visual_transform_get_plugin (element->data.transform)), element->params);

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
    return 0;
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

			case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:
				visual_transform_set_video (element->data.transform, video);
				visual_transform_video_negotiate (element->data.transform);

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

int pipeline_container_propagate_event (LVAVSPipelineContainer *container, VisEvent *event)
{
	VisListEntry *le = NULL;
	VisEventQueue *pluginqueue;
	LVAVSPipelineElement *element;

	while ((element = visual_list_next (container->members, &le)) != NULL) {

		switch (element->type) {
			case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:

				pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (element->data.actor));

				visual_object_ref (VISUAL_OBJECT (event));
				visual_event_queue_add (pluginqueue, event);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:

				pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin (element->data.actor));

				visual_object_ref (VISUAL_OBJECT (event));
				visual_event_queue_add (pluginqueue, event);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER:

				pipeline_container_propagate_event (LVAVS_PIPELINE_CONTAINER (element), event);

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
    VisBuffer pcmbuf1;
    VisBuffer pcmbuf2;
    VisBuffer spmbuf1;
    VisBuffer spmbuf2;
    VisBuffer tmp;
    float data[2][2][1024];
    
    
    visual_buffer_init_allocate(&tmp, sizeof(float) * 1024, visual_buffer_destroyer_free);

    /* Left audio */
    visual_buffer_set_data_pair(&pcmbuf1, data[0][0], sizeof(float) * 1024);

    if(visual_audio_get_sample(audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT) == VISUAL_OK)

        visual_audio_sample_buffer_mix(&pcmbuf1, &tmp, TRUE, 1.0);

    visual_buffer_set_data_pair(&spmbuf1, &data[1][0], sizeof(float) * 1024);

    visual_audio_get_spectrum_for_sample (&spmbuf1, &tmp, TRUE);

    /* Right audio */
    visual_buffer_set_data_pair(&pcmbuf2, data[0][1], sizeof(float) * 1024);

    if(visual_audio_get_sample(audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT) == VISUAL_OK)

        visual_audio_sample_buffer_mix(&pcmbuf2, &tmp, TRUE, 1.0);

    visual_buffer_set_data_pair(&spmbuf2, data[1][1], sizeof(float) * 1024);

    visual_audio_get_spectrum_for_sample(&spmbuf2, &tmp, TRUE);

    visual_object_unref(VISUAL_OBJECT(&tmp));

    printf("pipeline_container_run\n");
	while ((element = visual_list_next (container->members, &le)) != NULL) {

        AvsGlobalProxy *proxy = element->pipeline->proxy;
        int i;
        float *beatdata = visual_mem_malloc0(2048 * sizeof(float));
        
        for(i = 0; i < 1024; i++) {
            proxy->audiodata[0][0][i] = data[0][0][i];
            proxy->audiodata[1][0][i] = data[1][0][i];
            proxy->audiodata[0][1][i] = data[0][1][i];
            proxy->audiodata[1][1][i] = data[1][1][i];
        }
        
        beatdata = data[0][0];
        beatdata += 1024;
        beatdata = data[0][1];
        beatdata -= 1024;

        proxy->isBeat = visual_audio_is_beat_with_data(audio, VISUAL_BEAT_ALGORITHM_ADV, beatdata, 2408);

		switch (element->type) {
			case LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR:

				visual_actor_set_video (element->data.actor, video);
				visual_actor_run (element->data.actor, audio);

				break;

			case LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM:

				visual_transform_set_video (element->data.transform, video);
				visual_transform_run (element->data.transform, audio);

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

