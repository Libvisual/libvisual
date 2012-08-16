/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
} GStreamerPrivate;


static void have_data (GstElement *sink, GstBuffer *buffer, GStreamerPrivate *data);

static int act_gstreamer_init (VisPluginData *plugin);
static int act_gstreamer_cleanup (VisPluginData *plugin);
static int act_gstreamer_requisition (VisPluginData *plugin, int *width, int *height);
static int act_gstreamer_resize (VisPluginData *plugin, int width, int height);
static int act_gstreamer_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_gstreamer_palette (VisPluginData *plugin);
static int act_gstreamer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
    static VisActorPlugin actor = {
        .requisition = act_gstreamer_requisition,
        .palette = act_gstreamer_palette,
        .render = act_gstreamer_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_24BIT
    };

    static VisPluginInfo info = {
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "gstreamer",
        .name = "libvisual gstreamer",
        .author = N_("Dennis Smit  <synap@yourbase.nl>"),
        .version = "0.1",
        .about = N_("Libvisual gstreamer plugin"),
        .help = N_("Capable of using gstreamer to play video, or do effects using the gstreamer pipeline"),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = act_gstreamer_init,
        .cleanup = act_gstreamer_cleanup,
        .events = act_gstreamer_events,

        .plugin = VISUAL_OBJECT (&actor)
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
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->glib_main_loop = g_main_loop_new (NULL, FALSE);

    gst_init (NULL, NULL);

    char pipe[1024];
    snprintf(pipe, 1024,
             "filesrc location=%s ! decodebin ! ffmpegcolorspace ! "
             "video/x-raw-rgb,bpp=24,depth=24 ! "
             "fakesink name=sink signal-handoffs=true", "test.mpg");

    GError *err = NULL;
    priv->pipeline = gst_parse_launch (pipe, &err);
    priv->buffer   = NULL;

    if (err) {
        visual_log (VISUAL_LOG_ERROR, "Failed to create pipeline: %s", err->message);
        g_error_free (err);
        return -1;
    }

    GstElement *sink = gst_bin_get_by_name (GST_BIN (priv->pipeline), "sink");
    g_signal_connect (sink, "handoff", G_CALLBACK (have_data), priv);
    gst_object_unref (sink);

    return 0;
}

static int act_gstreamer_cleanup (VisPluginData *plugin)
{
    GStreamerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    gst_object_unref (priv->pipeline);
    g_main_loop_unref (priv->glib_main_loop);

    visual_mem_free (priv);

    return 0;
}

static int act_gstreamer_requisition (VisPluginData *plugin, int *width, int *height)
{

    return 0;
}

static int act_gstreamer_resize (VisPluginData *plugin, int width, int height)
{
    return 0;
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

    return 0;
}

static VisPalette *act_gstreamer_palette (VisPluginData *plugin)
{
    return NULL;
}

static int act_gstreamer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    GStreamerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    GstState state, pending_state;
    GstStateChangeReturn status;

    /* Set pipeline state to PLAYING if it is only just initialized */
    gst_element_get_state (priv->pipeline, &state, &pending_state, GST_CLOCK_TIME_NONE);
    if (state == GST_STATE_NULL && pending_state != GST_STATE_PLAYING) {
        gst_element_set_state (priv->pipeline, GST_STATE_PLAYING);
    }

    /* Check pipeline state, make sure it's already playing, or going to be set playing */
    status = gst_element_get_state (priv->pipeline, &state, &pending_state, 10 * GST_MSECOND);
    if (state != GST_STATE_PLAYING) {
        if (pending_state == GST_STATE_PLAYING) {
            if (status == GST_STATE_CHANGE_ASYNC) {
                visual_log (VISUAL_LOG_INFO, "Waiting for pipeline to get ready (Current state: %s)", gst_element_state_get_name (state));
                return 0;
            } else if (status == GST_STATE_CHANGE_FAILURE) {
                visual_log (VISUAL_LOG_INFO, "Failed to animate pipeline");
                return -1;
            }
        } else {
            visual_log (VISUAL_LOG_ERROR, "Pipeline in indeterminate state");
            return -1;
        }
    }

    /* Process any messages on the bus */
    g_main_context_iteration (g_main_loop_get_context (priv->glib_main_loop), FALSE);

    /* Draw if we have a buffer */
    if (priv->buffer) {
        /* FIXME: Extract the last frame from the GStreamer buffer into the target video buffer */
        /* FIXME: This contains a buffer overrun if the buffer has multiple frames */
        /* FIXME: This contains a race if have_data() is called asynchronously.. */

        memcpy (visual_video_get_pixels (video),
                GST_BUFFER_DATA (priv->buffer),
                GST_BUFFER_SIZE (priv->buffer));

        gst_buffer_unref (priv->buffer);

        priv->buffer = NULL;
    }

    return 0;
}

static void have_data (GstElement *sink, GstBuffer *buffer, GStreamerPrivate* priv)
{
    /* Keep the most recently received buffer alive for rendering */

    if (priv->buffer) {
        gst_buffer_unref (priv->buffer);
    }

    priv->buffer = buffer;
    gst_buffer_ref (priv->buffer);
}
