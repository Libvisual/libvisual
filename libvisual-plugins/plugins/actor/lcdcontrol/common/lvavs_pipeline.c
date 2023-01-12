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
#include <limits.h>


#ifdef _OPENMP
#include <omp.h>
#endif

#include <libvisual/libvisual.h>

#include "lvavs_preset.h"
#include "lvavs_pipeline.h"
#include "avs_common.h"

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
        ;//FIXME visual_object_unref (VISUAL_OBJECT (container->members));

    container->members = NULL;

    lvavs_pipeline_element_dtor (object);

    return TRUE;
}

/* LVAVS Preset */
LVAVSPipeline *lvavs_pipeline_new ()
{
    LVAVSPipeline *pipeline;
    VisColor *col = visual_color_black();
    int i,j;
    

    pipeline = visual_mem_new0 (LVAVSPipeline, 1);

    pipeline->dummy_vid = visual_video_new_with_buffer(0, 0, 1);

    pipeline->last_vid = visual_video_new_with_buffer(0, 0, 1);
    
    for(i = 0; i < sizeof(pipeline->buffers) / sizeof(VisVideo); i++) {
        pipeline->buffers[i] = visual_video_new_with_buffer(0, 0, 1);
    }
    for (j=0;j<256;j++)
        for (i=0;i<256;i++)
            pipeline->blendtable[i][j] = (unsigned char)((i / 255.0) * (float)j);

    /* Do the VisObject initialization */
    visual_object_set_allocated (VISUAL_OBJECT (pipeline), TRUE);
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
    int size = BEAT_MAX_SIZE/2;

    float data[2][2][size];

    VisBuffer *tmp = visual_buffer_new_allocate (sizeof(float) * size);

    /* Left audio */
    if(visual_audio_get_sample(audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT)) {
        VisBuffer *pcmbuf1 = visual_buffer_new_wrap_data (data[0][0], sizeof(float) * size, 0);
        visual_audio_sample_buffer_mix (pcmbuf1, tmp, TRUE, 1.0);
        visual_buffer_unref(pcmbuf1);
    }

    VisBuffer *spmbuf1 = visual_buffer_new_wrap_data (data[1][0], sizeof(float) * size, 0);
    visual_audio_get_spectrum_for_sample (spmbuf1, tmp, TRUE);
    visual_buffer_unref (spmbuf1);

    /* Right audio */

    if(visual_audio_get_sample (audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT)) {
        VisBuffer *pcmbuf2 = visual_buffer_new_wrap_data (data[0][1], sizeof(float) * size, 0);
        visual_audio_sample_buffer_mix (pcmbuf2, tmp, TRUE, 1.0);
        visual_buffer_unref(pcmbuf2);
    }

    VisBuffer *spmbuf2 = visual_buffer_new_wrap_data (data[1][1], sizeof(float) * size, 0);
    visual_audio_get_spectrum_for_sample (spmbuf2, tmp, TRUE);
    visual_buffer_unref(spmbuf2);

    visual_buffer_unref(tmp);

#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif

    for(int i = size - 1; i >= 0; i--) {
        pipeline->audiodata[0][0][i] = (data[0][0][i] + 1) / 2.0;
        pipeline->audiodata[1][0][i] = (data[1][0][i] + 1) / 2.0;
        pipeline->audiodata[0][1][i] = (data[0][1][i] + 1) / 2.0;
        pipeline->audiodata[1][1][i] = (data[1][1][i] + 1) / 2.0;
    }

/*
    float beatdata[BEAT_MAX_SIZE];
    char visdata[BEAT_MAX_SIZE];

    memcpy(beatdata, data[1][0], size * sizeof(float));
    memcpy(beatdata + size, data[1][1], size * sizeof(float));
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif

    for(i = BEAT_MAX_SIZE - 1; i >= 0; i--) {
        visdata[i] = (beatdata[i] + 1) / 2.0 * CHAR_MAX;
    }

    pipeline->isBeat = visual_audio_is_beat_with_data(audio, VISUAL_BEAT_ALGORITHM_PEAK, visdata, BEAT_MAX_SIZE/2);
*/

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
                    visual_object_set_private(VISUAL_OBJECT(element->data.actor->plugin), pipeline);

                } else if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_MORPH) == 0) {

                    element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_MORPH);
                    element->data.morph = visual_morph_new (pelem->element_name);
                    visual_object_set_private(VISUAL_OBJECT(element->data.morph->plugin), pipeline);

                } else if (strcmp (ref->info->type, VISUAL_PLUGIN_TYPE_TRANSFORM) == 0) {

                    element = lvavs_pipeline_element_new (LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM);
                    element->data.transform = visual_transform_new (pelem->element_name);
                    visual_object_set_private(VISUAL_OBJECT(element->data.transform->plugin), pipeline);

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

        LVAVS_PIPELINE_ELEMENT(cont)->params = LVAVS_PIPELINE_ELEMENT(container)->params;
        LVAVS_PIPELINE_ELEMENT(cont)->pipeline = LVAVS_PIPELINE_ELEMENT(container)->pipeline;

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

static int blendin(int mode) {return ((mode>>8)&31);}
static void set_blendin(int v, int *mode) { *mode&=~(31<<8); *mode|=(v&31)<<8; }
static int blendout(int  mode) { return ((mode>>16)&31)^1; }
static void set_blendout(int v, int *mode) { *mode&=~(31<<16); *mode|=((v^1)&31)<<16; }

static int render_now(LVAVSPipelineContainer *container, VisVideo *video, VisAudio *audio, int s)
{
    LVAVSPipeline *pipeline = LVAVS_PIPELINE_ELEMENT(container)->pipeline;

    int i;
    int count = visual_list_count(container->members);
    for(i = 0; i < count; i++) {
        LVAVSPipelineElement *element = visual_list_get(container->members, i);
        VisVideo *tmpvid;

        if(s) {
            pipeline->framebuffer = visual_video_get_pixels(pipeline->dummy_vid);
            pipeline->fbout = visual_video_get_pixels(video);
        } else {
            pipeline->fbout = visual_video_get_pixels(pipeline->dummy_vid);
            pipeline->framebuffer = visual_video_get_pixels(video);
        }

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

        if(pipeline->swap&1) {
            s^=1;
            pipeline->swap = 0;
        }

    }

    return s;
}
int pipeline_container_run (LVAVSPipelineContainer *container, VisVideo *video, VisAudio *audio)
{
    int i, s = 0;
    VisListEntry *le = NULL;
    LVAVSPipelineElement *element;
    VisBuffer pcmbuf1;
    VisBuffer pcmbuf2;
    VisBuffer spmbuf1;
    VisBuffer spmbuf2;
    VisBuffer tmp;
    int *fbout;
    int *framebuffer;
    LVAVSPipeline *pipeline = LVAVS_PIPELINE_ELEMENT(container)->pipeline;
    int w = video->width, h = video->height;

    if(video->width != pipeline->dummy_vid->width || video->height != pipeline->dummy_vid->height || video->depth != pipeline->dummy_vid->depth) {

        if(pipeline->dummy_vid)
            visual_object_unref(VISUAL_OBJECT(pipeline->dummy_vid));

        if(pipeline->last_vid)
            visual_object_unref(VISUAL_OBJECT(pipeline->last_vid));
    
        pipeline->dummy_vid = visual_video_scale_depth_new(video, video->width, video->height, video->depth, VISUAL_VIDEO_COMPOSITE_TYPE_SRC);

        pipeline->last_vid = visual_video_scale_depth_new(video, video->width, video->height, video->depth, VISUAL_VIDEO_COMPOSITE_TYPE_SRC);
        
        for(i = 0; i < 16; i++) {
            VisVideo *vid = pipeline->buffers[i];
            if(vid)
                visual_object_unref(VISUAL_OBJECT(vid));
            vid = visual_video_scale_depth_new(video, video->width, video->height, video->depth, VISUAL_VIDEO_COMPOSITE_TYPE_NONE);
            pipeline->buffers[i] = vid;
        }
    }

    visual_video_blit_overlay(video, pipeline->last_vid, 0, 0, 0.5);
      
    fbout = visual_video_get_pixels(video);
    framebuffer = visual_video_get_pixels(pipeline->dummy_vid);

    int is_preinit = pipeline->isBeat;//(pipeline->isBeat&0x80000000);

/*    if(pipeline->isBeat && beat_render)
    fake_enabled = beat_render_frames;
*/
    s = render_now(container, video, audio, s);

    if(!is_preinit)
    {
        int x = video->width * video->height;
        int *tfb=framebuffer;
        int *o = fbout;
        int use_blendin=blendin(pipeline->blendmode);
        if(use_blendin == 10 && pipeline->use_inblendval >= 255)
            use_blendin=1;

        switch (use_blendin)
        {
            case 1:
                visual_mem_copy(o, tfb, w*h*sizeof(int));
            break;
            case 2:
                mmx_avgblend_block(o,tfb,x);
            break;
            case 3:
                while(x--)
                {
                    *o=BLEND_MAX(*o, *tfb++);
                    o++;
                }
            break;
            case 4:
            //mmx_addblend_block(pipeline->blendtable, o, tfb, x);
            break;
            case 5:
                while(x--)
                {
                    *o=BLEND_SUB(*o,*tfb++);
                    o++;
                }
            break;
            case 6:
                while(x--)
                {
                    *o=BLEND_SUB(*tfb++, *o);
                    o++;
                }
            break;
            case 7:
            {
                int y=h/2;
                while(x-- > 0)
                {
                    visual_mem_copy(o,tfb,w*sizeof(int));
                    tfb+=w*2;
                    o+=w*2;
                }
            break;
            }
            case 8:
            {
                int r = 0;
                int y = h;
                while(y-- > 0)
                {
                    int *out, *in;
                    int x=w/2;
                    out=o+r;
                    in=tfb+r;
                    r^=1;
                    while(x-- > 0)
                    {
                        *out=*in;
                        out+=2;
                        in+=2;
                    }
                    o+=w;
                    tfb+=w;
                }
            break;
            }
            case 9:
                while(x--)
                {
                    *o=*o^*tfb++;
                    o++;
                }
            break;
            case 10:
                mmx_adjblend_block(pipeline->blendtable,o,tfb,o,x,pipeline->use_inblendval);
            break;
            case 11:
                mmx_mulblend_block(pipeline->blendtable, o,tfb,x);
            break;
            case 13:
                while(x--)
                {
                    *o=BLEND_MIN(*o,*tfb++);
                    o++;
                }
            break;
            case 12:
    /*
                                    {
                                            int *buf=(int*)getGlobalBuffer(w,h,bufferin,0);
                                            if (!buf) break;
                                            while (x--)
                                            {
                                                    *o=BLEND_ADJ(*tfb++,*o, depthof(*buf, ininvert));
                                                    o++;
                                                    buf++;
                                            }
                                    }
    */
            break;
            default:
            break;
        }
    }
    int x;
    int line_blend_mode_save=pipeline->blendmode;
    //if(!is_preinit) pipeline->blendmode = 0;

    s = render_now(container, video, audio, s);
    //if(!is_preinit) pipeline->blendmode = line_blend_mode_save;

    if(!is_preinit)
    {
        if(s) visual_mem_copy(framebuffer, fbout, w*h*sizeof(int));

        int *tfb=s?fbout:framebuffer;
        int *o=framebuffer;
        x=w*h;
        int use_blendout=blendout(pipeline->blendmode);
        int use_outblendval = 100;
        if(use_blendout == 10 && use_outblendval >= 255)
            use_blendout=1;
        switch(use_blendout)
        {
            case 1:
                visual_mem_copy(o,tfb,x*sizeof(int));
            break;
            case 2:
                mmx_avgblend_block(o,tfb,x);
            break;
            case 3:
                while(x--)
                {
                    *o=BLEND_MAX(*o, *tfb++);
                    o++;
                }
            break;
            case 4:
                mmx_addblend_block(o, tfb, x);
            break;
            case 5:
                while(x--)
                {
                    *o = BLEND_SUB(*o, *tfb++);
                    o++;
                }
            break;
            case 6:
                while(x--)
                {
                    *o=BLEND_SUB(*tfb++, *o);
                    o++;
                }
            break;
            case 7:
            {
                int y=h/2;
                while(y-- > 0)
                {
                    visual_mem_copy(o, tfb, w*sizeof(int));
                    tfb+=w*2;
                    o+=w*2;
                }
            }
            break;
            case 8:
            {
                int r = 0;
                int y = h;
                while(y-- > 0)
                {
                    int *out, *in;
                    int x=w/2;
                    out=o+r;
                    in=tfb+r;
                    r^=1;
                    while(x-- > 0)
                    {
                        *out=*in;
                        out+=2;
                        in+=2;
                    }
                    o+=w;
                    tfb+=2;
                }
            }
            case 9:
                while(x--)
                {
                    *o=*o^*tfb++;
                    o++;
                }
            break;
            case 10:
                mmx_adjblend_block(pipeline->blendtable,o, tfb, o, x, use_outblendval);
            break;
            case 11:
                mmx_mulblend_block(pipeline->blendtable, o, tfb, x);
            break;
            case 13:
                while(x--)
                {
                    *o=BLEND_MIN(*o, *tfb++);
                    o++;
                }
            break;
            case 12:
            {
                //uint32_t *buf = buffer[bufferout]
            }
            break;
            default:
            break;
        }
    } 

    // Save state for next frame.
    visual_video_blit_overlay(pipeline->last_vid, video, 0, 0, 0);
    return VISUAL_OK;
}

