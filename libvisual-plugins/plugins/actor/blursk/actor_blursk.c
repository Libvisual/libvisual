/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of Blurks-libvisual.
 *
 * Blurks-libvisual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blurks-libvisual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blurks-libvisual.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libvisual/libvisual.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>



#include "actor_blursk.h"
#include "blursk.h"

int act_blursk_init (VisPluginData *plugin);
int act_blursk_cleanup (VisPluginData *plugin);
int act_blursk_requisition (VisPluginData *plugin, int *width, int *height);
int act_blursk_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_blursk_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_blursk_palette (VisPluginData *plugin);
int act_blursk_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR


const VisPluginInfo *get_plugin_info (int *count)
{
        static VisActorPlugin actor[] = {{
                .requisition = act_blursk_requisition,
                .palette = act_blursk_palette,
                .render = act_blursk_render,
                .vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
        }};

        static VisPluginInfo info[] = {{
                .type = VISUAL_PLUGIN_TYPE_ACTOR,

                .plugname = "blursk",
                .name = "blursk plugin",
                .author = "Read AUTHORS",
                .version = "0.0.1",
                .about = "blursk visual plugin",
                .help = "This is the libvisual port of blursk xmms visualization",
                .license = VISUAL_PLUGIN_LICENSE_GPL,

                .init = act_blursk_init,
                .cleanup = act_blursk_cleanup,
                .events = act_blursk_events,

                .plugin = VISUAL_OBJECT (&actor[0])
        }};

        *count = sizeof (info) / sizeof (*info);

        return info;
}







int act_blursk_init (VisPluginData *plugin) {
    BlurskPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_COLOR   ("color", 0x00, 0xff, 0xff, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("color_style", "Rainbow", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("signal_color", "Normal signal", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("contour_lines", FALSE, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("hue_on_beats", FALSE, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("background", "Black bkgnd", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_style", "Random", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("transition_speed", "Medium switch", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_when", "Full blur", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_stencil", "No stencil", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("fade_speed", "Medium fade", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("slow_motion", FALSE, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("signal_style", "Stereo spectrum", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("plot_style", "Line", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("thick_on_beats", TRUE, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("flash_style", "No flash", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("overall_effect", "Normal effect", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("floaters", "No floaters", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("cpu_speed", "Fast CPU", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beat_sensitivity", 4, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("config_string", "", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("show_info", "Never show info", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("info_timeout", 4, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("show_timestamp", FALSE, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_END
    };

    /* init plugin */
    priv = visual_mem_new0 (BlurskPrivate, 1);
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->plugin = plugin;
    
    priv->rcontext = visual_plugin_get_random_context (plugin);

    visual_palette_allocate_colors (&priv->pal, 256);

    visual_param_container_add_many_proxy (paramcontainer, params);
        
    priv->pcmbuf = visual_buffer_new_allocate (512 * sizeof (float), visual_buffer_destroyer_free);

    config_default(&config);

    __blursk_init(priv);
    return 0;
}

int act_blursk_cleanup (VisPluginData *plugin) {
    return 0;
    BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    __blursk_cleanup (priv);

    visual_palette_free_colors (&priv->pal);

    visual_object_unref (VISUAL_OBJECT (priv->pcmbuf));

    visual_mem_free (priv);

    return 0;
}

int act_blursk_requisition (VisPluginData *plugin, int *width, int *height) {
        int reqw, reqh;

        reqw = *width;
        reqh = *height;

        while (reqw % 4)
                reqw--;

        while (reqh % 2)
                reqh--;

        if (reqw < 32)
                reqw = 32;

        if (reqh < 32)
                reqh = 32;

        *width = reqw;
        *height = reqh;

        return 0;
}

int act_blursk_dimension (VisPluginData *plugin, VisVideo *video, int width, int height) 
{
        BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

        visual_video_set_dimension (video, width, height);

        priv->video = video;

        priv->width = width;
        priv->height = height;

        video->pitch = width;

        config.height = height;
        config.width = width;

        return 0;
}

int act_blursk_events (VisPluginData *plugin, VisEventQueue *events) {
    BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT(plugin));
    VisEvent ev;
    VisParamEntry *param;
    VisSongInfo *newsong;
    
    int size_update = 0;

    /* reset regen-colmap-flag */
    priv->update_colmap = 0;
    
    while(visual_event_queue_poll(events, &ev)) 
    {
        switch(ev.type) 
        {
            
            case VISUAL_EVENT_RESIZE:
                act_blursk_dimension (plugin, ev.event.resize.video,
                    ev.event.resize.width, ev.event.resize.height);
                size_update = 1;
                break;

            
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;
                /* change config parameter */
                config_change_param(priv, param);          
                break;

            case VISUAL_EVENT_NEWSONG:
                newsong = ev.event.newsong.songinfo;
                /* pass along the song info to blursk's core */
                blursk_event_newsong(newsong);
                break;
            default:
                break;
        }
    }
    
    /* re-generate config string */
    //if(priv->update_config_string)
    //    config_string_genstring(priv);
    
    /* regenerate palette */
    if(priv->update_colmap)
        color_genmap(priv, FALSE);
    
    /* resize plugin */
    if(size_update)
        img_resize(priv, config.width, config.height);
    

    return 0;
}

VisPalette *act_blursk_palette (VisPluginData *plugin) {
    BlurskPrivate *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    return &priv->pal;
}

int act_blursk_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio) {
        int16_t tpcm[512];
        float *pcm;
    
        BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

        priv->video = video;

        visual_audio_get_sample_mixed_simple (audio, priv->pcmbuf, 2,
                        VISUAL_AUDIO_CHANNEL_LEFT,
                        VISUAL_AUDIO_CHANNEL_RIGHT,
                        1.0,
                        1.0);
    
    
        pcm = visual_buffer_get_data(priv->pcmbuf);
    
        int i;
        for(i = 0; i < sizeof(tpcm)/sizeof(int16_t); i++)
            tpcm[i] = (int16_t) (pcm[i]*32767);
    
        __blursk_render_pcm (priv, tpcm);

        visual_mem_copy (visual_video_get_pixels (video), priv->rgb_buf, visual_video_get_size (video));


    return 0;
}


