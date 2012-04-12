/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_gstreamer.c,v 1.9 2006/01/27 20:19:16 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <libvisual/libvisual.h>

#include <gst/gst.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (void);

#define BARS 16

typedef struct {
	VisVideo *old_video;
	GstPipeline *pipe;
} GstreamerPrivate;


static void have_data (GstElement *sink, GstBuffer *buffer, gpointer data);

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
	GstreamerPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (GstreamerPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	return 0;
}

static int act_gstreamer_cleanup (VisPluginData *plugin)
{
	GstreamerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

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
	GstreamerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	static int playing = 0;

	if (playing == 0) {
		char pipe[1024];

		gst_init (NULL, NULL);
/*
		snprintf(pipe, 1024, "filesrc location=%s ! decodebin ! ffmpegcolorspace ! "
				"videoscale ! video/x-raw-rgb,bpp=32,depth=32,width=%d,height=%d,"
				"red_mask=0xff000000,green_mask=0x00ff0000,blue_mask=0x0000ff00 !"
				"fakesink name=sink sync=true", "test.mpg", video->width, video->height);

*/
		snprintf(pipe, 1024, "filesrc location=%s ! decodebin ! ffmpegcolorspace ! "
				"video/x-raw-rgb,bpp=24,depth=24 ! "
				"fakesink name=sink signal-handoffs=true", "test.mpg");

		GError *err = NULL;

		priv->pipe = GST_PIPELINE_CAST(gst_parse_launch (pipe, &err));

		if (err) {
			visual_log (VISUAL_LOG_ERROR, "Failed to create pipeline", err->message);
			return;
		}

		gst_element_set_state (GST_ELEMENT (priv->pipe), GST_STATE_PLAYING);

		g_signal_connect (gst_bin_get_by_name_recurse_up (GST_BIN (priv->pipe), "sink"),
				"handoff", G_CALLBACK (have_data), video);

		playing = 1;
	}

//	g_signal_handlers_disconnect_by_func (gst_bin_get_by_name_recurse_up (GST_BIN (priv->pipe), "sink"),
//			G_CALLBACK (have_data), priv->old_video);


	gst_bin_iterate (GST_BIN (priv->pipe));

	priv->old_video = video;

	return 0;
}

static void have_data (GstElement *sink, GstBuffer *buffer, gpointer data)
{
	VisVideo *video = data;
	uint32_t *dest = visual_video_get_pixels (video);
	uint32_t *src = (uint32_t *) GST_BUFFER_DATA (buffer);

	visual_mem_copy (dest, src, GST_BUFFER_SIZE (buffer));
}

