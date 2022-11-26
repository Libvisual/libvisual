/*
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

#include "config.h"
#include "gettext.h"
#include "actor_blursk.h"
#include "blursk.h"
#include <libvisual/libvisual.h>
#include <limits.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_blursk_init        (VisPluginData *plugin);
static void        act_blursk_cleanup     (VisPluginData *plugin);
static void        act_blursk_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_blursk_resize      (VisPluginData *plugin, int width, int height);
static int         act_blursk_events      (VisPluginData *plugin, VisEventQueue *events);
static void        act_blursk_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *act_blursk_palette     (VisPluginData *plugin);

BlurskConfig config;

const VisPluginInfo *get_plugin_info (void)
{
        static VisActorPlugin actor = {
                .requisition = act_blursk_requisition,
                .palette     = act_blursk_palette,
                .render      = act_blursk_render,
                .vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
        };

        static VisPluginInfo info = {
                .type     = VISUAL_PLUGIN_TYPE_ACTOR,

                .plugname = "blursk",
                .name     = "Blursk plugin",
                .author   = "Read AUTHORS",
                .version  = "0.0.1",
                .about    = N_("blursk visual plugin"),
                .help     = N_("This is the libvisual port of blursk xmms visualization"),
                .license  = VISUAL_PLUGIN_LICENSE_GPL,

                .init     = act_blursk_init,
                .cleanup  = act_blursk_cleanup,
                .events   = act_blursk_events,
                .plugin   = &actor
        };

        return &info;
}

static int act_blursk_init (VisPluginData *plugin) {
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_color_rgb ("color",
                                                            N_("Color"),
                                                            0x00, 0xff, 0xff,
                                                            NULL),
                                visual_param_new_string  ("color_style",
                                                          N_("Color style"),
                                                          "Rainbow",
                                                          NULL),
                                visual_param_new_string  ("signal_color",
                                                          N_("Signal color"),
                                                          "Normal signal",
                                                          NULL),
                                visual_param_new_bool    ("contour_lines",
                                                          N_("Contour lines"),
                                                          FALSE,
                                                          NULL),
                                visual_param_new_bool    ("hue_on_beats",
                                                          N_("Hue change on beats"),
                                                          FALSE,
                                                          NULL),
                                visual_param_new_string  ("background",
                                                          N_("Background"),
                                                          "Black bkgnd",
                                                          NULL),
                                visual_param_new_string  ("blur_style",
                                                          N_("Blur style"),
                                                          "Random",
                                                          NULL),
                                visual_param_new_string  ("transition_speed",
                                                          N_("Transition speed"),
                                                          "Medium switch",
                                                          NULL),
                                visual_param_new_string  ("blur_when",
                                                          N_("When to blur"),
                                                          "Full blur",
                                                          NULL),
                                visual_param_new_string  ("blur_stencil",
                                                          N_("Blur stencil"),
                                                          "No stencil",
                                                          NULL),
                                visual_param_new_string  ("fade_speed",
                                                          N_("Fade speed"),
                                                          "Medium fade",
                                                          NULL),
                                visual_param_new_bool    ("slow_motion",
                                                          N_("Slow motion"),
                                                          FALSE,
                                                          NULL),
                                visual_param_new_string  ("signal_style",
                                                          N_("Signal type"),
                                                          "Stereo spectrum",
                                                          NULL),
                                visual_param_new_string  ("plot_style",
                                                          N_("Plot style"),
                                                          "Line",
                                                          NULL),
                                visual_param_new_bool    ("thick_on_beats",
                                                          N_("Thick on beats"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_string  ("flash_style",
                                                          N_("Flash style"),
                                                          "No flash",
                                                          NULL),
                                visual_param_new_string  ("overall_effect",
                                                          N_("Overall effect"),
                                                          "Normal effect",
                                                          NULL),
                                visual_param_new_string  ("floaters",
                                                          N_("Floaters"),
                                                          "No floaters",
                                                          NULL),
                                visual_param_new_string  ("cpu_speed",
                                                          N_("CPU speed"),
                                                          "Fast CPU",
                                                          NULL),
                                visual_param_new_integer ("beat_sensitivity",
                                                          N_("Beat sensitivity"),
                                                          4,
                                                          visual_param_in_range_integer (0, 10)),
                                visual_param_new_string  ("config_string",
                                                          N_("Config string"),
                                                          "",
                                                          NULL),
                                visual_param_new_string  ("show_info",
                                                          N_("Show info"),
                                                          "Never show info",
                                                          NULL),
                                visual_param_new_integer ("info_timeout",
                                                          N_("Info timeout"),
                                                          4,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_bool    ("show_timestamp",
                                                          N_("Show timestamp"),
                                                          TRUE,
                                                          NULL),
                                NULL);

    /* init plugin */
    BlurskPrivate *priv = visual_mem_new0 (BlurskPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    priv->plugin   = plugin;
    priv->rcontext = visual_plugin_get_random_context (plugin);
    priv->pal      = visual_palette_new (256);
    priv->pcmbuf   = visual_buffer_new_allocate (512 * sizeof (float));

    config_default (&config);

    __blursk_init (priv);

    return TRUE;
}

static void act_blursk_cleanup (VisPluginData *plugin)
{
    BlurskPrivate *priv = visual_plugin_get_private (plugin);

    __blursk_cleanup (priv);

    visual_palette_free (priv->pal);

    visual_buffer_unref (priv->pcmbuf);

    visual_mem_free (priv);
}

static void act_blursk_requisition (VisPluginData *plugin, int *width, int *height)
{
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
}

static void act_blursk_resize (VisPluginData *plugin, int width, int height)
{
        BlurskPrivate *priv = visual_plugin_get_private (plugin);

        priv->width = width;
        priv->height = height;

        config.height = height;
        config.width = width;
}

static int act_blursk_events (VisPluginData *plugin, VisEventQueue *events)
{
    BlurskPrivate *priv = visual_plugin_get_private (plugin);
    VisEvent ev;
    VisParam *param;
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

    return TRUE;
}

static VisPalette *act_blursk_palette (VisPluginData *plugin)
{
    BlurskPrivate *priv = visual_plugin_get_private (plugin);

    return priv->pal;
}

static void act_blursk_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    BlurskPrivate *priv = visual_plugin_get_private (plugin);

    int16_t tpcm[512];
    float *pcm;

        priv->video = video;

        visual_audio_get_sample_mixed_simple (audio, priv->pcmbuf, 2,
                        VISUAL_AUDIO_CHANNEL_LEFT,
                        VISUAL_AUDIO_CHANNEL_RIGHT);

        pcm = visual_buffer_get_data(priv->pcmbuf);

        int i;
        for(i = 0; i < sizeof(tpcm)/sizeof(int16_t); i++)
            tpcm[i] = (int16_t) (pcm[i]*32767);

        __blursk_render_pcm (priv, tpcm);

        visual_mem_copy (visual_video_get_pixels (video), priv->rgb_buf, visual_video_get_size (video));
}
