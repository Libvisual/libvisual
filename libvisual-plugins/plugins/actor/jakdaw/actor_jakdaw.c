/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_jakdaw.c,v 1.26 2006/01/27 20:19:16 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include "feedback.h"
#include "plotter.h"

#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int act_jakdaw_init (VisPluginData *plugin);
static int act_jakdaw_cleanup (VisPluginData *plugin);
static int act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height);
static int act_jakdaw_resize (VisPluginData *plugin, int width, int height);
static int act_jakdaw_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_jakdaw_palette (VisPluginData *plugin);
static int act_jakdaw_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_jakdaw_requisition,
		.palette = act_jakdaw_palette,
		.render = act_jakdaw_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "jakdaw",
		.name = "Jakdaw plugin",
		.author = N_("Original by: Christopher Wilson <Jakdaw@usa.net>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.0.1",
		.about = N_("jakdaw visual plugin"),
		.help = N_("This is the libvisual port of the xmms Jakdaw plugin"),
		.license = VISUAL_PLUGIN_LICENSE_GPL,

		.init = act_jakdaw_init,
		.cleanup = act_jakdaw_cleanup,
		.events = act_jakdaw_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	return &info;
}

static int act_jakdaw_init (VisPluginData *plugin)
{
	JakdawPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("zoom mode",		FEEDBACK_ZOOMRIPPLE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("plotter trigger",	PLOTTER_COLOUR_MUSICTRIG),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("plotter type",	PLOTTER_SCOPE_LINES),
		VISUAL_PARAM_LIST_END
	};

	/*

	static VisParamEntry zoomparamchoices[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Zoom ripple",		FEEDBACK_ZOOMRIPPLE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Blur only",		FEEDBACK_BLURONLY),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Zoom rotate",		FEEDBACK_ZOOMROTATE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Scroll",		FEEDBACK_SCROLL),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Into screen",		FEEDBACK_INTOSCREEN),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Ripple",		FEEDBACK_NEWRIPPLE),
		VISUAL_PARAM_LIST_END
	};

	static VisParamEntry colorparamchoices[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Solid",		PLOTTER_COLOUR_SOLID),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Random",		PLOTTER_COLOUR_RANDOM),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("On music",		PLOTTER_COLOUR_MUSICTRIG),
		VISUAL_PARAM_LIST_END
	};

	static VisParamEntry scopeparamchoices[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Lines",		PLOTTER_SCOPE_LINES),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Dots",		PLOTTER_SCOPE_DOTS),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Solid",		PLOTTER_SCOPE_SOLID),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("Nothing",		PLOTTER_SCOPE_NOTHING),
		VISUAL_PARAM_LIST_END
	};
	*/

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (JakdawPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->rcontext = visual_plugin_get_random_context (plugin);

	priv->decay_rate = 1;

	priv->zoom_ripplesize = 32;
	priv->zoom_ripplefact = 0.1;
	priv->zoom_zoomfact = 0.9;

	priv->plotter_amplitude = 0.5;

	/* FIXME make param of this one as well */
	priv->plotter_scopecolor = 0xff00ff;

	visual_param_container_add_many (paramcontainer, params);

	priv->pcmbuf = visual_buffer_new_allocate (512 * sizeof (float), visual_buffer_destroyer_free);
	priv->freqbuf = visual_buffer_new_allocate (256 * sizeof (float), visual_buffer_destroyer_free);

	return 0;
}

static int act_jakdaw_cleanup (VisPluginData *plugin)
{
	JakdawPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	_jakdaw_feedback_close (priv);

	visual_object_unref (VISUAL_OBJECT (priv->pcmbuf));
	visual_object_unref (VISUAL_OBJECT (priv->freqbuf));

	visual_mem_free (priv);

	return 0;
}

static int act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

static int act_jakdaw_resize (VisPluginData *plugin, int width, int height)
{
	JakdawPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	priv->xres = width;
	priv->yres = height;

	_jakdaw_feedback_reset (priv, width, height);

	return 0;
}

static int act_jakdaw_events (VisPluginData *plugin, VisEventQueue *events)
{
	JakdawPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_jakdaw_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				visual_log (VISUAL_LOG_DEBUG, "Param changed: %s", param->name);

				if (visual_param_entry_is (param, "zoom mode")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the zoom mode param: %d",
							param->numeric.integer);

					priv->zoom_mode = visual_param_entry_get_integer (param);

					_jakdaw_feedback_reset (priv, priv->xres, priv->yres);
				}
				else if (visual_param_entry_is (param, "plotter trigger")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the plotter trigger param: %d",
							param->numeric.integer);

					priv->plotter_colortype = visual_param_entry_get_integer (param);

				}
				else if (visual_param_entry_is (param, "plotter type")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the plotter type param: %d",
							param->numeric.integer);

					priv->plotter_scopetype = visual_param_entry_get_integer (param);

					_jakdaw_feedback_reset (priv, priv->xres, priv->yres);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *act_jakdaw_palette (VisPluginData *plugin)
{
	return NULL;
}

static int act_jakdaw_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	JakdawPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint32_t *vscr = visual_video_get_pixels (video);

	visual_audio_get_sample_mixed_simple (audio, priv->pcmbuf, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (priv->freqbuf, priv->pcmbuf, TRUE);

	_jakdaw_feedback_render (priv, vscr);
	_jakdaw_plotter_draw (priv,
			visual_buffer_get_data (priv->pcmbuf),
			visual_buffer_get_data (priv->freqbuf), vscr);

	return 0;
}

