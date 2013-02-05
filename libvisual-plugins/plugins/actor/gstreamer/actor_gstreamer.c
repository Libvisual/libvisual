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

static void handle_sink_data   (GstElement *sink, GstBuffer *buffer, GstPad *pad, GStreamerPrivate *data);
static void handle_bus_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv);

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

    char *launch_str = g_strdup_printf ("filesrc location=%s ! decodebin ! ffmpegcolorspace ! "
                                        "videoscale ! capsfilter name=capsfilter ! "
                                        "fakesink name=sink signal-handoffs=true sync=true",
                                        "test.mpg");

    GError *error = NULL;
    priv->pipeline = gst_parse_launch (launch_str, &error);
    g_free (launch_str);

    if (!priv->pipeline) {
        visual_log (VISUAL_LOG_ERROR, "Failed to create pipeline: %s", error->message);
        g_error_free (error);
        return FALSE;
    }

    priv->capsfilter = gst_bin_get_by_name (GST_BIN (priv->pipeline), "capsfilter");
    GstCaps *caps = gst_caps_new_simple ("video/x-raw-rgb",
                                         "depth" , G_TYPE_INT, 24,
                                         "bpp"   , G_TYPE_INT, 24,
                                         "width" , G_TYPE_INT, 320,
                                         NULL);
    g_object_set (priv->capsfilter, "caps", caps, NULL);
    gst_caps_unref (caps);

    priv->buffer = NULL;
#if GLIB_VERSION_CUR_STABLE >= GLIB_VERSION_2_32
    priv->mutex = g_slice_new0 (GMutex);
    g_mutex_init (priv->mutex);
#else
    priv->mutex = g_mutex_new ();
#endif

    priv->sink = gst_bin_get_by_name (GST_BIN (priv->pipeline), "sink");
    g_signal_connect (priv->sink, "handoff", G_CALLBACK (handle_sink_data), priv);

    gst_element_set_state (priv->pipeline, GST_STATE_PAUSED);

    GstStateChangeReturn status = gst_element_get_state (priv->pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    if (status != GST_STATE_CHANGE_SUCCESS) {
        visual_log (VISUAL_LOG_ERROR, "Failed to ready pipeline");
        return FALSE;
    }

    GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (priv->pipeline));
    g_signal_connect (bus, "message", G_CALLBACK (handle_bus_message), priv);
    gst_object_unref (bus);

    priv->glib_main_loop = g_main_loop_new (NULL, FALSE);

    return TRUE;
}

static void act_gstreamer_cleanup (VisPluginData *plugin)
{
    GStreamerPrivate *priv = visual_plugin_get_private (plugin);

    if (priv->pipeline) {
        g_signal_handlers_disconnect_by_func (priv->sink, "handoff", handle_sink_data);

        GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (priv->pipeline));
        g_signal_handlers_disconnect_by_func (bus, handle_bus_message, priv);
        gst_object_unref (bus);

        g_main_loop_unref (priv->glib_main_loop);

        gst_object_unref (priv->sink);
        gst_object_unref (priv->capsfilter);

        gst_element_set_state (priv->pipeline, GST_STATE_NULL);
        gst_object_unref (priv->pipeline);

        gst_buffer_unref (priv->buffer);

#if GLIB_VERSION_CUR_STABLE >= GLIB_VERSION_2_32
        g_mutex_clear (priv->mutex);
        g_slice_free (GMutex, priv->mutex);
#else
        g_mutex_free (priv->mutex);
#endif
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

    GstCaps *caps = gst_caps_new_simple ("video/x-raw-rgb",
                                         "width" , G_TYPE_INT, width,
                                         "height", G_TYPE_INT, height,
                                         "depth" , G_TYPE_INT, 24,
                                         "bpp"   , G_TYPE_INT, 24,
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
        int buffer_size = visual_video_get_pitch (video) * visual_video_get_height (video);

        /* Copy buffer to video only if dimensions match, ignoring
         * buffers received from GStreamer before their corresponding
         * resizes were completed. */
        if (GST_BUFFER_SIZE (priv->buffer) == buffer_size) {
            VisVideo *source = visual_video_new_wrap_buffer (GST_BUFFER_DATA (priv->buffer),
                                                             FALSE,
                                                             visual_video_get_width  (video),
                                                             visual_video_get_height (video),
                                                             VISUAL_VIDEO_DEPTH_24BIT,
                                                             0);

            visual_video_flip_pixel_bytes (video, source);

            visual_video_unref (source);
        }

        gst_buffer_unref (priv->buffer);

        priv->buffer = NULL;
    }
    g_mutex_unlock (priv->mutex);
}

static void handle_sink_data (GstElement *sink, GstBuffer *buffer, GstPad *pad, GStreamerPrivate* priv)
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

static void handle_bus_message (GstBus *bus, GstMessage *message, GStreamerPrivate *priv)
{
    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_EOS:
            visual_log (VISUAL_LOG_INFO, "End of stream!");
            break;

        case GST_MESSAGE_ERROR: {
            GError *error = NULL;
            char   *msg   = NULL;
            gst_message_parse_error (message, &error, &msg);
            visual_log (VISUAL_LOG_ERROR, "GStreamer error: %s", msg);
            g_error_free (error);
            g_free (msg);
            break;
        }

        default:
            visual_log (VISUAL_LOG_INFO, "Got bus message of type %s", GST_MESSAGE_TYPE_NAME (message));
            break;
    }
}
