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
#include "actor_blursk.h"
#include "blursk.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

const VisPluginInfo *get_plugin_info (void);

static int act_blursk_init (VisPluginData *plugin);
static int act_blursk_cleanup (VisPluginData *plugin);
static int act_blursk_requisition (VisPluginData *plugin, int *width, int *height);
static int act_blursk_resize (VisPluginData *plugin, int width, int height);
static int act_blursk_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_blursk_palette (VisPluginData *plugin);
static int act_blursk_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR


const VisPluginInfo *get_plugin_info (void)
{
        static VisActorPlugin actor = {
                .requisition = act_blursk_requisition,
                .palette = act_blursk_palette,
                .render = act_blursk_render,
                .vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
        };

        static VisPluginInfo info = {
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

                .plugin = VISUAL_OBJECT (&actor)
        };

        return &info;
}

static int act_blursk_init (VisPluginData *plugin) {
    BlurskPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    VisParamEntry *param;

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_COLOR   ("color", 0x00, 0xff, 0xff),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("color_style", "Rainbow"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("signal_color", "Normal signal"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("contour_lines", FALSE),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("hue_on_beats", FALSE),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("background", "Black bkgnd"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_style", "Random"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("transition_speed", "Medium switch"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_when", "Full blur"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("blur_stencil", "No stencil"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("fade_speed", "Medium fade"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("slow_motion", FALSE),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("signal_style", "Stereo spectrum"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("plot_style", "Line"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("thick_on_beats", TRUE),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("flash_style", "No flash"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("overall_effect", "Normal effect"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("floaters", "No floaters"),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("cpu_speed", "Fast CPU"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beat_sensitivity", 4),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("config_string", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("show_info", "Aever show info"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("info_timeout", 4),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("show_timestamp", TRUE),
        VISUAL_PARAM_LIST_END
    };

    /* init plugin */
    priv = visual_mem_new0 (BlurskPrivate, 1);
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->plugin = plugin;

    priv->rcontext = visual_plugin_get_random_context (plugin);

    priv->pal = visual_palette_new (256);

    visual_param_container_add_many (paramcontainer, params);

    param = visual_param_container_get(paramcontainer, "color");
    visual_param_entry_set_annotation(param, "Color");

    param = visual_param_container_get(paramcontainer, "color_style");
    visual_param_entry_set_annotation(param, "Color style");

    param = visual_param_container_get(paramcontainer, "signal_color");
    visual_param_entry_set_annotation(param, "Signal color");

    param = visual_param_container_get(paramcontainer, "contour_lines");
    visual_param_entry_set_annotation(param, "Contour lines");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 1);

    param = visual_param_container_get(paramcontainer, "hue_on_beats");
    visual_param_entry_set_annotation(param, "Huge on beats");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 1);

    param = visual_param_container_get(paramcontainer, "background");
    visual_param_entry_set_annotation(param, "Background");

    param = visual_param_container_get(paramcontainer, "blur_style");
    visual_param_entry_set_annotation(param, "Blur style");

    param = visual_param_container_get(paramcontainer, "blur_stencil");
    visual_param_entry_set_annotation(param, "Blur stencil");

    param = visual_param_container_get(paramcontainer, "fade_speed");
    visual_param_entry_set_annotation(param, "Fade speed");

    param = visual_param_container_get(paramcontainer, "slow_motion");
    visual_param_entry_set_annotation(param, "Slow motion");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 1);

    param = visual_param_container_get(paramcontainer, "signal_style");
    visual_param_entry_set_annotation(param, "Signal style");

    param = visual_param_container_get(paramcontainer, "plot_style");
    visual_param_entry_set_annotation(param, "Plot style");

    param = visual_param_container_get(paramcontainer, "thick_on_beats");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 1);
    visual_param_entry_set_annotation(param, "Thick on beats");

    param = visual_param_container_get(paramcontainer, "flash_style");
    visual_param_entry_set_annotation(param, "Flash style");

    param = visual_param_container_get(paramcontainer, "overall_effect");
    visual_param_entry_set_annotation(param, "Overall effect");

    param = visual_param_container_get(paramcontainer, "floaters");
    visual_param_entry_set_annotation(param, "Floaters");

    param = visual_param_container_get(paramcontainer, "cpu_speed");
    visual_param_entry_set_annotation(param, "Cpu speed");

    param = visual_param_container_get(paramcontainer, "beat_sensitivity");
    visual_param_entry_set_annotation(param, "Beat sensitivity");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 10);

    param = visual_param_container_get(paramcontainer, "config_string");
    visual_param_entry_set_annotation(param, "Config string");
    param = visual_param_container_get(paramcontainer, "show_info");
    visual_param_entry_set_annotation(param, "Show info");

    param = visual_param_container_get(paramcontainer, "info_timeout");
    visual_param_entry_set_annotation(param, "Info timeout");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, INT_MAX);

    param = visual_param_container_get(paramcontainer, "show_timestamp");
    visual_param_entry_set_annotation(param, "Show timestamp");
    visual_param_entry_min_set_integer(param, 0);
    visual_param_entry_max_set_integer(param, 1);

    priv->pcmbuf = visual_buffer_new_allocate (512 * sizeof (float), visual_buffer_destroyer_free);

    config_default(&config);

    __blursk_init(priv);
    return 0;
}

static int act_blursk_cleanup (VisPluginData *plugin) {
    return 0;
    BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    __blursk_cleanup (priv);

    visual_palette_free (priv->pal);

    visual_object_unref (VISUAL_OBJECT (priv->pcmbuf));

    visual_mem_free (priv);

    return 0;
}

static int act_blursk_requisition (VisPluginData *plugin, int *width, int *height) {
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

static int act_blursk_resize (VisPluginData *plugin, int width, int height)
{
        BlurskPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

        priv->width = width;
        priv->height = height;

        config.height = height;
        config.width = width;

        return 0;
}

static int act_blursk_events (VisPluginData *plugin, VisEventQueue *events) {
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
                act_blursk_resize (plugin, ev.event.resize.width, ev.event.resize.height);
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

static VisPalette *act_blursk_palette (VisPluginData *plugin) {
    BlurskPrivate *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    return priv->pal;
}

static int act_blursk_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio) {
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


