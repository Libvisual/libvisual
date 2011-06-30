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

    // params
    int mode;
    int activebuffer;

} MultidelayPrivate;

int lv_multidelay_init (VisPluginData *plugin);
int lv_multidelay_cleanup (VisPluginData *plugin);
int lv_multidelay_events (VisPluginData *plugin, VisEventQueue *events);
int lv_multidelay_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_multidelay_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_multidelay_palette,
        .video = lv_multidelay_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_multidelay",
        .name = "Libvisual AVS Transform: multidelay element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: multidelay element",
        .help = "This is the multidelay element for the libvisual AVS system",

        .init = lv_multidelay_init,
        .cleanup = lv_multidelay_cleanup,
        .events = lv_multidelay_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_multidelay_init (VisPluginData *plugin)
{
    MultidelayPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("mode", 0, VISUAL_PARAM_LIMIT_NONE, "Mode"),
        VISUAL_PARAM_LIST_INTEGER("activebuffer", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 6), "Active Buffer"),
        VISUAL_PARAM_LIST_INTEGER("usebeats0", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Usebeats 0"),
        VISUAL_PARAM_LIST_INTEGER("usebeats1", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Usebeats 1"),
        VISUAL_PARAM_LIST_INTEGER("usebeats2", 0, VISUAL_PARAM_LIMIT_BOOLAEN, "Usebeats 2"),
        VISUAL_PARAM_LIST_INTEGER("usebeats3", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Usebeats 3"),
        VISUAL_PARAM_LIST_INTEGER("usebeats4", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Usebeats 4"),
        VISUAL_PARAM_LIST_INTEGER("usebeats5", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Usebeats 5"),
        VISUAL_PARAM_LIST_INTEGER("delay0", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 0"),
        VISUAL_PARAM_LIST_INTEGER("delay1", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 1"),
        VISUAL_PARAM_LIST_INTEGER("delay2", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 2"),
        VISUAL_PARAM_LIST_INTEGER("delay3", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 3"),
        VISUAL_PARAM_LIST_INTEGER("delay4", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 4"),
        VISUAL_PARAM_LIST_INTEGER("delay5", 0, VISUAL_PARAM_LIMIT_NONE, "Delay 5"),

        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (MultidelayPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    AvsMultidelayGlobals *multidelay = proxy->multidelay;
    multidelay->numinstances++;
    priv->creationid = multidelay->numinstances;
    return 0;
}

int lv_multidelay_cleanup (VisPluginData *plugin)
{
    MultidelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int trans_load_runnable(MultidelayPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(MultidelayPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_multidelay_events (VisPluginData *plugin, VisEventQueue *events)
{
    MultidelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsMultidelayGlobals *multidelay = priv->multidelay;
    VisParamEntry *param;
    VisEvent ev;
    int need_delay;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "mode"))
                    priv->mode = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "activebuffer"))
                    priv->activebuffer = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats0"))
                    multidelay->usebeats[0] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats1"))
                    multidelay->usebeats[1] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats2"))
                    multidelay->usebeats[2] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats3"))
                    multidelay->usebeats[3] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats4"))
                    multidelay->usebeats[4] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats5"))
                    multidelay->usebeats[5] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "delay0")) {
                    multidelay->delay[0] = visual_param_entry_get_integer(param);
                } else if(visual_param_entry_is(param, "delay1")) {
                    multidelay->delay[1] = visual_param_entry_get_integer(param);
                    need_delay = 1;
                } else if(visual_param_entry_is(param, "delay2")) {
                    multidelay->delay[2] = visual_param_entry_get_integer(param);
                    need_delay = 1;
                } else if(visual_param_entry_is(param, "delay3")) {
                    multidelay->delay[3] = visual_param_entry_get_integer(param);
                    need_delay = 1;
                } else if(visual_param_entry_is(param, "delay4")) {
                    multidelay->delay[4] = visual_param_entry_get_integer(param);
                    need_delay = 1;
                } else if(visual_param_entry_is(param, "delay5")) {
                    multidelay->delay[5] = visual_param_entry_get_integer(param);
                    need_delay = 1;
                }
                break;

            default:
                break;
        }
    }

    if(need_delay) 
    {
        int i;
        for(i = 0; i < 6; i++)
        {
            multidelay->framedelay[i] = (multidelay->usebeats[i]?multidelay->framesperbeat:multidelay->delay[i])+1;
        }
    }
    return 0;
}

int lv_multidelay_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_multidelay_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    MultidelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsMultidelayGlobals *multidelay = priv->proxy->multidelay;
    uint8_t *framebuffer = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    int *fbin=framebuffer;

  if (isBeat&0x80000000) return 0;

    if (priv->renderid == multidelay->numinstances) priv->renderid = 0;
    priv->renderid++;
    if (priv->renderid == 1)
    {
        multidelay->framemem = w*h*4;
        if (isBeat)
        {
            multidelay->framesperbeat = multidelay->framessincebeat;
            for (int i=0;i<6;i++) if (multidelay->usebeats[i]) multidelay->framedelay[i] = multidelay->framesperbeat+1;
            multidelay->framessincebeat = 0;
        }
        multidelay->framessincebeat++;
        for (int i=0;i<6;i++)
        {
            if (multidelay->framedelay[i]>1)
            {
                multidelay->virtualbuffersize[i] = multidelay->framedelay[i]*multidelay->framemem;
                if (framemem == oldframemem)
                {
                    if (multidelay->virtualbuffersize[i] != multidelay->oldvirtualbuffersize[i])
                    {
                        if (multidelay->virtualbuffersize[i] > multidelay->oldvirtualbuffersize[i])
                        {
                            if (multidelay->virtualbuffersize[i] > multidelay->buffersize[i])
                            {
                                // allocate new memory
                                if (visual_mem_free(multidelay->buffer[i].fb) != VISUAL_OK) return 0;
                                if (multidelay->usebeats[i])
                                {
                                    multidelay->buffersize[i] = 2*multidelay->virtualbuffersize[i];
                                    multidelay->buffer[i].fb = visual_mem_malloc0(buffersize[i]);
                                    if (multidelay->buffer[i].fb == NULL)
                                    {
                                        multidelay->buffersize[i] = multidelay->virtualbuffersize[i];
                                        multidelay->buffer[i].fb = visual_mem_malloc0(multidelay->buffersize[i]);
                                    }
                                }
                                else
                                {
                                    multidelay->buffersize[i] = multidelay->virtualbuffersize[i];
                                    multidelay->buffer[i].fb = visual_mem_malloc0(multidelay->buffersize[i]);
                                }
                                multidelay->outpos[i].fb = multidelay->buffer[i];
                                multidelay->inpos[i] = multidelay->buffer[i]+multidelay->virtualbuffersize[i]-multidelay->framemem;
                                if (multidelay->buffer[i] == NULL)
                                {
                                    multidelay->framedelay[i] = 0;
                                    if (multidelay->usebeats[i])
                                    {
                                        multidelay->framesperbeat = 0;
                                        multidelay->framessincebeat = 0;
                                        multidelay->framedelay[i] = 0;
                                        multidelay->delay[i] = 0;
                                    }
                                }
                            }
                            else
                            {
                                unsigned long size = (((unsigned long)multidelay->buffer[i].fb)+multidelay->oldvirtualbuffersize[i]) - ((unsigned long)multidelay->outpos[i].fb);
                                unsigned long l = ((unsigned long)multidelay->buffer[i].fb)+multidelay->virtualbuffersize[i];
                                unsigned long d =  l - size;
                                memmove((void *)&d, multidelay->outpos[i].fb, size);
                                for (l = (unsigned long)multidelay->outpos[i].fb; l < d; l += multidelay->framemem) 
                                    memcopy((void *)&l, (void *)&d, multidelay->framemem);
                            }
                        }
                        else
                        {   // virtualbuffersize < oldvirtualbuffersize
                            unsigned long presegsize = ((unsigned long)multidelay->outpos[i])-((unsigned long)multidelay->buffer[i].fb);
                            if (multidelay->presegsize > multidelay->virtualbuffersize[i])
                            {
                                memmove(multidelay->buffer[i].fb, (void *)(((unsigned long)multidelay->buffer[i].fb)+presegsize-multidelay->virtualbuffersize[i]), multidelay->virtualbuffersize[i]);
                                inpos[i] = (void *)(((unsigned long)multidelay->buffer[i].fb)+multidelay->virtualbuffersize[i]-multidelay->framemem);
                                outpos[i].fb = buffer[i].fb;
                            }
                            else if (presegsize < virtualbuffersize[i]) 
                            memcpy(outpos[i].fb, (void *)(((unsigned long)buffer[i].fb)+oldvirtualbuffersize[i]+presegsize-virtualbuffersize[i]), virtualbuffersize[i]-presegsize);
                        }
                        oldvirtualbuffersize[i] = virtualbuffersize[i];
                    }
                }
                else
                {
                    // allocate new memory
                    if(visual_mem_free(buffer[i].fb) != VISUAL_OK) return 0;
                    if (usebeats[i])
                    {
                        buffersize[i] = 2*virtualbuffersize[i];
                        buffer[i].fb = visual_mem_malloc0(buffersize[i]);
                        if (buffer[i].fb == NULL)
                        {
                            buffersize[i] = virtualbuffersize[i];
                            buffer[i].fb = visual_mem_malloc0(buffersize[i]);
                        }
                    }
                    else
                    {
                        buffersize[i] = virtualbuffersize[i];
                        buffer[i].fb = visual_mem_malloc0(buffersize[i]);
                    }
                    outpos[i] = buffer[i].fb;
                    inpos[i].fb = (void *)(((unsigned long)buffer[i].fb)+virtualbuffersize[i]-framemem);
                    if (buffer[i].fb == NULL)
                    {
                        framedelay[i] = 0;
                        if (usebeats[i])
                        {
                            framesperbeat = 0;
                            framessincebeat = 0;
                            framedelay[i] = 0;
                            delay[i] = 0;
                        }
                    }
                    oldvirtualbuffersize[i] = virtualbuffersize[i];
                }
                oldframemem = framemem;
            }
        }
    }
    if (mode != 0 && framedelay[activebuffer]>1)
    {
        if (mode == 2) 
            memcpy(framebuffer, outpos[activebuffer].fb, framemem);
        else 
            memcpy(inpos[activebuffer].fb, framebuffer, framemem);
    }
    if (renderid == numinstances) for (int i=0;i<6;i++)
    {
        inpos[i].fb = (void *)(((unsigned long)inpos[i].fb)+framemem);
        outpos[i].fb = (void *)(((unsigned long)outpos[i].fb)+framemem);
        if ((unsigned long)inpos[i].fb>=((unsigned long)buffer[i].fb)+virtualbuffersize[i]) 
            inpos[i].fb = buffer[i].fb;
        if ((unsigned long)outpos[i].fb>=((unsigned long)buffer[i].fb)+virtualbuffersize[i]) 
            outpos[i].fb = buffer[i].fb;
    }

    return 0;
}

