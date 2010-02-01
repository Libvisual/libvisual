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

/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

// video delay
// copyright tom holden, 2002
// mail: cfp@myrealbox.com

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

typedef struct {
    AVSGlobalProxy *proxy;

    // params
    int enabled, usebeats;
    uint32_t delay;

    // Others
    AvsLPVOID buffer;
    AvsLPVOID inoutpos;
    uint64_t buffersize;
    uint64_t virtualbuffersize;
    uint64_t oldvirtualbuffersize;
    uint64_t framesincebeat;
    uint64_t framedelay;
    uint64_t framemem;
    uint64_t oldframemem;


} VideodelayPrivate;

int lv_videodelay_init (VisPluginData *plugin);
int lv_videodelay_cleanup (VisPluginData *plugin);
int lv_videodelay_events (VisPluginData *plugin, VisEventQueue *events);
int lv_videodelay_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_videodelay_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_videodelay_palette,
        .video = lv_videodelay_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_videodelay",
        .name = "Libvisual AVS Transform: videodelay element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: videodelay element",
        .help = "This is the videodelay element for the libvisual AVS system",

        .init = lv_videodelay_init,
        .cleanup = lv_videodelay_cleanup,
        .events = lv_videodelay_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_videodelay_init (VisPluginData *plugin)
{
    VideodelayPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Videodelay"),
        VISUAL_PARAM_LIST_INTEGER("usebeats", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Use beats"),
        VISUAL_PARAM_LIST_INTEGER("delay", 0, VISUAL_PARAM_LIMIT_NONE, "Delay"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (VideodelayPrivate, 1);

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

int lv_videodelay_cleanup (VisPluginData *plugin)
{
    VideodelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_videodelay_events (VisPluginData *plugin, VisEventQueue *events)
{
    VideodelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;
    int usebeats = 0;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "usebeats")) {
                    usebeats = priv->usebeats = visual_param_entry_get_integer(param);
                } else if(visual_param_entry_is(param, "delay"))
                    priv->delay = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    if(usebeats)
    {
        if(priv->delay > 16)
            priv->delay = 16;
    }
    else 
    {
        if (priv->delay > 200)
            priv->delay = 200;
    }

    return 0;
}

int lv_videodelay_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

int lv_videodelay_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    VideodelayPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t *fbout = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    uint8_t *framebuffer = visual_mem_malloc(w * h * video->pitch);
    visual_mem_copy(framebuffer, fbout, w * h * video->pitch);

    if (isBeat&0x80000000) return 0;

    priv->framemem = w*h*4;
    if (priv->usebeats)
    {
        if (isBeat)
        {
            priv->framedelay = priv->framessincebeat*priv->delay; //changed
            if (priv->framedelay > 400) priv->framedelay = 400; //new
            priv->framessincebeat = 0;
        }
        priv->framessincebeat++;
    }
    if (priv->enabled && priv->framedelay!=0)
    {
        priv->virtualbuffersize = priv->framedelay*priv->framemem;
        if (priv->framemem == priv->oldframemem)
        {
            if (priv->virtualbuffersize != priv->oldvirtualbuffersize)
            {
                if (priv->virtualbuffersize > priv->oldvirtualbuffersize)
                {
                    if (priv->virtualbuffersize > priv->buffersize)
                    {
                        // allocate new memory
                        if (visual_mem_free(buffer) != VISUAL_OK) return 0;
                        if (priv->usebeats)
                        {
                            priv->buffersize = 2*priv->virtualbuffersize;
                            if (priv->buffersize > priv->framemem*400) priv->buffersize = priv->framemem*400;   //new
                            priv->buffer.fb = visual_mem_malloc0(buffersize);
                            if (priv->buffer.fb == NULL)
                            {
                                priv->buffersize = priv->virtualbuffersize;
                                priv->buffer.fb = visual_mem_malloc0(priv->buffersize);
                            }
                        }
                        else
                        {
                            priv->buffersize = priv->virtualbuffersize;
                            priv->buffer.fb = visual_mem_malloc0(priv->buffersize);
                        }
                        priv->inoutpos.fb = priv->buffer.fb;
                        if (priv->buffer.fb == NULL)
                        {
                            priv->framedelay = 0;
                            priv->framessincebeat = 0;
                            return 0;
                        }
                    }
                    else
                    {
                        unsigned long size = (((unsigned long)priv->buffer.fb)+priv->oldvirtualbuffersize) - ((unsigned long)priv->inoutpos.fb);
                        unsigned long l = ((unsigned long)priv->buffer.fb)+priv->virtualbuffersize;
                        unsigned long d =  l - size;
                        memmove((void *)d, priv->inoutpos.fb, size);
                        for (l = (unsigned long)priv->inoutpos.fb; l < d; l += priv->framemem) 
                            memcopy((void *)l,(void *)d,priv->framemem);
                    }
                }
                else
                {   // virtualbuffersize < oldvirtualbuffersize
                    unsigned long presegsize = ((unsigned long)priv->inoutpos.fb)-((unsigned long)priv->buffer.fb)+priv->framemem;
                    if (priv->presegsize > priv->virtualbuffersize)
                    {
                        memmove(priv->buffer.fb,(void *)(((unsigned long)priv->buffer.fb)+presegsize-priv->virtualbuffersize),priv->virtualbuffersize);
                        priv->inoutpos.fb = (void *)(((unsigned long)priv->buffer.fb)+priv->virtualbuffersize-priv->framemem);
                    }
                    else if (priv->presegsize < priv->virtualbuffersize) 
                        memmove((void *)(((unsigned long)priv->inoutpos.fb)+priv->framemem),(void *)(((unsigned long)priv->buffer.fb)+priv->oldvirtualbuffersize+presegsize-priv->virtualbuffersize),priv->virtualbuffersize-presegsize);
                }
                priv->oldvirtualbuffersize = priv->virtualbuffersize;
            }
        }
        else
        {
            // allocate new memory
            if (!visual_mem_free(buffer) != VISUAL_OK) return 0;
            if (priv->usebeats)
            {
                priv->buffersize = 2*priv->virtualbuffersize;
                priv->buffer.fb = visual_mem_malloc0(buffersize);
                if (priv->buffer.fb == NULL)
                {
                    priv->buffersize = priv->virtualbuffersize;
                    priv->buffer.fb = visual_mem_malloc0(buffersize);
                }
            }
            else
            {
                priv->buffersize = priv->virtualbuffersize;
                priv->buffer.fb = visual_mem_malloc0(buffersize);
            }
            priv->inoutpos.fb = priv->buffer.fb;
            if (priv->buffer == NULL)
            {
                priv->framedelay = 0;
                priv->framessincebeat = 0;
                return 0;
            }
            priv->oldvirtualbuffersize = priv->virtualbuffersize;
        }
        priv->oldframemem = priv->framemem;
        memcopy(fbout,priv->inoutpos.fb,priv->framemem);
        memcopy(priv->inoutpos.fb,framebuffer,priv->framemem);
        priv->inoutpos.fb = (void *)(((unsigned long)priv->inoutpos.fb)+priv->framemem);
    }

    visual_mem_free(framebuffer);
    return 0;
}

