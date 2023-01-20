/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2012      Libvisual Team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <gst/gst.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
    GstElement *pipeline;
    GMainLoop  *glib_main_loop;
    GstBuffer  *buffer;
    GstElement *capsfilter;
    GstElement *sink;
    GMutex     *mutex;
} GStreamerPrivate;

static int         act_gstreamer_init        (VisPluginData *plugin);
static void        act_gstreamer_cleanup     (VisPluginData *plugin);
static void        act_gstreamer_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_gstreamer_resize      (VisPluginData *plugin, int width, int height);
static int         act_gstreamer_events      (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_gstreamer_palette     (VisPluginData *plugin);
static void        act_gstreamer_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void handle_sink_handoff (GstElement *sink, GstBuffer *buffer, GstPad *pad, GStreamerPrivate *data);
static void handle_bus_error_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv);
static void handle_bus_eos_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv);

const VisPluginInfo *get_plugin_info (void)
{
    static VisActorPlugin actor = {
        .requisition = act_gstreamer_requisition,
        .palette     = act_gstreamer_palette,
        .render      = act_gstreamer_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_24BIT
    };

    static VisPluginInfo info = {
        .type     = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "gstreamer",
        .name     = "libvisual gstreamer",
        .author   = N_("Dennis Smit  <synap@yourbase.nl>"),
        .version  = "0.1",
        .about    = N_("Libvisual gstreamer plugin"),
        .help     = N_("Capable of using gstreamer to play video, or do effects using the gstreamer pipeline"),
        .license  = VISUAL_PLUGIN_LICENSE_LGPL,

        .init     = act_gstreamer_init,
        .cleanup  = act_gstreamer_cleanup,
        .events   = act_gstreamer_events,
        .plugin   = &actor
    };

    return &info;
}

static int act_gstreamer_init (VisPluginData *plugin)
{
    GStreamerPrivate *priv;

#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    priv = visual_mem_new0 (GStreamerPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    gst_init (NULL, NULL);

    // Regarding the pipeline below:
    // - Element "filesrc" reads a video file from disk.
    // - Element "decodebin" auto-magically constructs a decoding pipeline
    //   using available decoders and demuxers via auto-plugging.
    // - Element "videoconvert" does conversion between video formats
    //   from given to wanted.
    // - Element "videoscale" does scaling from given to wanted resolution.
    // - Element "capsfilter" limits the output to the width/height/format we want.
    // - Element "fakesink" allows accessing the rendered raw pixels from C code.
    // - "name=capsfilter" and "name=sink" are needed to access these elements
    //   from within C code further down.
    // - "signal-handoffs=true" makes fakesink let use know it started using
    //   a different buffer.
    // - "sync=true" makes fakesink respect the target framerate of the video
    //   input to give the appearance of original playback speed
    //   (in contrast to serving frames as fast as possible).
    char *launch_str = g_strdup_printf ("filesrc location=%s ! decodebin ! videoconvert ! "
                                        "videoscale ! capsfilter name=capsfilter ! "
                                        "fakesink name=sink signal-handoffs=true sync=true",
                                        "test.mpg");

    GError *error = NULL;
    priv->pipeline = gst_parse_launch (launch_str, &error);

    if (!priv->pipeline) {
        visual_log (VISUAL_LOG_ERROR, "Failed to create pipeline: %s", error->message);
        g_error_free (error);
        g_free (launch_str);
        return FALSE;
    }

    priv->capsfilter = gst_bin_get_by_name (GST_BIN (priv->pipeline), "capsfilter");
    GstCaps *caps = gst_caps_new_simple ("video/x-raw",
                                         "width", G_TYPE_INT, 320,
                                         "height", G_TYPE_INT, 240,
                                         "format", G_TYPE_STRING, "BGR",
                                         NULL);
    g_object_set (priv->capsfilter, "caps", caps, NULL);
    gst_caps_unref (caps);

    priv->buffer = NULL;
    priv->mutex = g_slice_new0 (GMutex);
    g_mutex_init (priv->mutex);

    priv->sink = gst_bin_get_by_name (GST_BIN (priv->pipeline), "sink");
    g_signal_connect (priv->sink, "handoff", G_CALLBACK (handle_sink_handoff), priv);

    gst_element_set_state (priv->pipeline, GST_STATE_PAUSED);

    GstStateChangeReturn status = gst_element_get_state (priv->pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    if (status != GST_STATE_CHANGE_SUCCESS) {
        visual_log (VISUAL_LOG_ERROR, "Failed to ready pipeline: %s", launch_str);
        g_free (launch_str);
        return FALSE;
    }
    g_free (launch_str);

    GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (priv->pipeline));
    g_signal_connect (bus, "message::error", G_CALLBACK (handle_bus_error_message), priv);
    g_signal_connect (bus, "message::eos", G_CALLBACK (handle_bus_eos_message), priv);
    gst_object_unref (bus);

    priv->glib_main_loop = g_main_loop_new (NULL, FALSE);

    return TRUE;
}

static void act_gstreamer_cleanup (VisPluginData *plugin)
{
    GStreamerPrivate *priv = visual_plugin_get_private (plugin);

    if (priv->pipeline) {
        g_signal_handlers_disconnect_by_func (priv->sink, "handoff", handle_sink_handoff);

        GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (priv->pipeline));
        g_signal_handlers_disconnect_by_func (bus, handle_bus_error_message, priv);
        g_signal_handlers_disconnect_by_func (bus, handle_bus_eos_message, priv);
        gst_object_unref (bus);

        g_main_loop_unref (priv->glib_main_loop);

        gst_object_unref (priv->sink);
        gst_object_unref (priv->capsfilter);

        gst_element_set_state (priv->pipeline, GST_STATE_NULL);
        gst_object_unref (priv->pipeline);

        if (priv->buffer) {
            gst_buffer_unref (priv->buffer);
        }

        g_mutex_clear (priv->mutex);
        g_slice_free (GMutex, priv->mutex);
    }

    visual_mem_free (priv);
}

static void act_gstreamer_requisition (VisPluginData *plugin, int *width, int *height)
{
    GStreamerPrivate *priv = visual_plugin_get_private (plugin);

    GstCaps *caps = NULL;
    g_object_get (priv->capsfilter, "caps", &caps, NULL);

    GstStructure *structure = gst_caps_get_structure (caps, 0);
    gst_structure_get (structure,
                       "width", width,
                       "height", height,
                       NULL);
    gst_caps_unref (caps);
}


static void act_gstreamer_resize (VisPluginData *plugin, int width, int height)
{
    GStreamerPrivate *priv = visual_plugin_get_private (plugin);

    GstCaps *caps = gst_caps_new_simple ("video/x-raw",
                                         "width" , G_TYPE_INT, width,
                                         "height", G_TYPE_INT, height,
                                         "format", G_TYPE_STRING, "BGR",
                                         NULL);
    g_object_set (priv->capsfilter, "caps", caps, NULL);
    gst_caps_unref (caps);
}

static int act_gstreamer_events (VisPluginData *plugin, VisEventQueue *events)
{
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                act_gstreamer_resize (plugin, ev.event.resize.width, ev.event.resize.height);
                break;

            default:
                break;
        }
    }

    return TRUE;
}

static VisPalette *act_gstreamer_palette (VisPluginData *plugin)
{
    return NULL;
}

static void act_gstreamer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    GStreamerPrivate *priv = visual_plugin_get_private (plugin);

    GstState state, pending_state;
    GstStateChangeReturn status;

    gst_element_get_state (priv->pipeline, &state, &pending_state, 0);
    if (state != GST_STATE_PLAYING && pending_state != GST_STATE_PLAYING) {
        /* Set pipeline to playing if not already done so */
        gst_element_set_state (priv->pipeline, GST_STATE_PLAYING);

        /* Check pipeline state, make sure it's already playing, or going to be set playing */
        status = gst_element_get_state (priv->pipeline, &state, &pending_state, 10 * GST_MSECOND);

        switch (status) {
            case GST_STATE_CHANGE_ASYNC:
                visual_log (VISUAL_LOG_INFO, "Waiting for pipeline to get ready (Current state: %s)",
                            gst_element_state_get_name (state));
                return;

            case GST_STATE_CHANGE_FAILURE:
                visual_log (VISUAL_LOG_INFO, "Failed to animate pipeline");
                return;

            default: /* nothing */;
        }
    }

    /* Process any messages on the bus */
    g_main_context_iteration (g_main_loop_get_context (priv->glib_main_loop), FALSE);

    /* Draw if we have a buffer */
    g_mutex_lock (priv->mutex);
    if (priv->buffer) {
        // NOTE: GStreamer format "BGR" does not have any padding
        const int expected_row_pitch = visual_video_get_width (video) * (24 / 8);
        const int expected_buffer_size = expected_row_pitch * visual_video_get_height (video);

        /* Copy buffer to video only if dimensions match, ignoring
         * buffers received from GStreamer before their corresponding
         * resizes were completed. */
        if (gst_buffer_get_size (priv->buffer) == expected_buffer_size) {
            GstMapInfo info;
            if (gst_buffer_map(priv->buffer, &info, GST_MAP_READ)) {
                VisVideo * const source = visual_video_new_wrap_buffer (info.data,
                                                                       FALSE,
                                                                       visual_video_get_width  (video),
                                                                       visual_video_get_height (video),
                                                                       VISUAL_VIDEO_DEPTH_24BIT,
                                                                       0 /* i.e. auto-calculated */);

                visual_video_convert_depth (video, source);

                visual_video_unref (source);

                gst_buffer_unmap(priv->buffer, &info);
            }
        }

        gst_buffer_unref (priv->buffer);

        priv->buffer = NULL;
    }
    g_mutex_unlock (priv->mutex);
}

static void handle_sink_handoff (GstElement *sink, GstBuffer *buffer, GstPad *pad, GStreamerPrivate* priv)
{
    g_mutex_lock (priv->mutex);

    /* Keep the most recently received buffer alive for rendering */
    if (priv->buffer) {
        gst_buffer_unref (priv->buffer);
    }

    priv->buffer = buffer;
    gst_buffer_ref (priv->buffer);

    g_mutex_unlock (priv->mutex);
}

static void handle_bus_error_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv)
{
    GError *error = NULL;
    char   *msg   = NULL;
    gst_message_parse_error (message, &error, &msg);
    visual_log (VISUAL_LOG_ERROR, "GStreamer error: %s", msg);
    g_error_free (error);
    g_free (msg);
}

static void handle_bus_eos_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv)
{
    visual_log (VISUAL_LOG_INFO, "End of stream!");
}
