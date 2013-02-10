/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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

#include "config.h"
#include "gettext.h"

#include "feedback.h"
#include "plotter.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_jakdaw_init        (VisPluginData *plugin);
static void        act_jakdaw_cleanup     (VisPluginData *plugin);
static void        act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_jakdaw_resize      (VisPluginData *plugin, int width, int height);
static int         act_jakdaw_events      (VisPluginData *plugin, VisEventQueue *events);
static void        act_jakdaw_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *act_jakdaw_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_jakdaw_requisition,
		.palette     = act_jakdaw_palette,
		.render      = act_jakdaw_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "jakdaw",
		.name     = "Jakdaw plugin",
		.author   = N_("Original by: Christopher Wilson <Jakdaw@usa.net>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.0.1",
		.about    = N_("jakdaw visual plugin"),
		.help     = N_("This is the libvisual port of the xmms Jakdaw plugin"),
		.license  = VISUAL_PLUGIN_LICENSE_GPL,

		.init     = act_jakdaw_init,
		.cleanup  = act_jakdaw_cleanup,
		.events   = act_jakdaw_events,
		.plugin   = &actor
	};

	return &info;
}

static int act_jakdaw_init (VisPluginData *plugin)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    JakdawPrivate *priv = visual_mem_new0 (JakdawPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_integer ("zoom_mode", N_("Zoom mode"),
                                                          FEEDBACK_ZOOMRIPPLE,
                                                          NULL),
                                visual_param_new_integer ("plotter_trigger", N_("Plotter trigger"),
                                                          PLOTTER_COLOUR_MUSICTRIG,
                                                          NULL),
                                visual_param_new_integer ("plotter type", N_("Plotter type"),
                                                          PLOTTER_SCOPE_LINES,
                                                          NULL),
                                NULL);

    /*

    static VisParamEntry zoomparamchoices[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Zoom ripple",     FEEDBACK_ZOOMRIPPLE),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Blur only",       FEEDBACK_BLURONLY),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Zoom rotate",     FEEDBACK_ZOOMROTATE),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Scroll",      FEEDBACK_SCROLL),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Into screen",     FEEDBACK_INTOSCREEN),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Ripple",      FEEDBACK_NEWRIPPLE),
        VISUAL_PARAM_LIST_END
    };

    static VisParamEntry colorparamchoices[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Solid",       PLOTTER_COLOUR_SOLID),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Random",      PLOTTER_COLOUR_RANDOM),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("On music",        PLOTTER_COLOUR_MUSICTRIG),
        VISUAL_PARAM_LIST_END
    };

    static VisParamEntry scopeparamchoices[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Lines",       PLOTTER_SCOPE_LINES),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Dots",        PLOTTER_SCOPE_DOTS),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Solid",       PLOTTER_SCOPE_SOLID),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("Nothing",     PLOTTER_SCOPE_NOTHING),
        VISUAL_PARAM_LIST_END
    };
    */

    priv->rcontext = visual_plugin_get_random_context (plugin);

    priv->decay_rate = 1;

    priv->zoom_ripplesize = 32;
    priv->zoom_ripplefact = 0.1;
    priv->zoom_zoomfact = 0.9;

    priv->plotter_amplitude = 0.5;

    /* FIXME make param of this one as well */
    priv->plotter_scopecolor = 0xff00ff;

    priv->pcmbuf = visual_buffer_new_allocate (512 * sizeof (float));
    priv->freqbuf = visual_buffer_new_allocate (256 * sizeof (float));

    return TRUE;
}

static void act_jakdaw_cleanup (VisPluginData *plugin)
{
	JakdawPrivate *priv = visual_plugin_get_private (plugin);

	_jakdaw_feedback_close (priv);

	visual_buffer_unref (priv->pcmbuf);
	visual_buffer_unref (priv->freqbuf);

	visual_mem_free (priv);
}

static void act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height)
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
}

static void act_jakdaw_resize (VisPluginData *plugin, int width, int height)
{
	JakdawPrivate *priv = visual_plugin_get_private (plugin);

	priv->xres = width;
	priv->yres = height;

	_jakdaw_feedback_reset (priv, width, height);
}

static int act_jakdaw_events (VisPluginData *plugin, VisEventQueue *events)
{
    JakdawPrivate *priv = visual_plugin_get_private (plugin);
    VisEvent ev;
    VisParam *param;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                act_jakdaw_resize (plugin, ev.event.resize.width, ev.event.resize.height);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                visual_log (VISUAL_LOG_DEBUG, "Param changed: %s", visual_param_get_name (param));

                if (visual_param_has_name (param, "zoom mode")) {
                    visual_log (VISUAL_LOG_DEBUG, "New value for the zoom mode param: %d",
                                visual_param_get_value_integer (param));

                    priv->zoom_mode = visual_param_get_value_integer (param);

                    _jakdaw_feedback_reset (priv, priv->xres, priv->yres);
                }
                else if (visual_param_has_name (param, "plotter trigger")) {
                    visual_log (VISUAL_LOG_DEBUG, "New value for the plotter trigger param: %d",
                                visual_param_get_value_integer (param));

                    priv->plotter_colortype = visual_param_get_value_integer (param);

                }
                else if (visual_param_has_name (param, "plotter type")) {
                    visual_log (VISUAL_LOG_DEBUG, "New value for the plotter type param: %d",
                                visual_param_get_value_integer (param));

                    priv->plotter_scopetype = visual_param_get_value_integer (param);

                    _jakdaw_feedback_reset (priv, priv->xres, priv->yres);
                }

                break;

            default: /* to avoid warnings */
                break;
        }
    }

    return TRUE;
}

static VisPalette *act_jakdaw_palette (VisPluginData *plugin)
{
	return NULL;
}

static void act_jakdaw_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	JakdawPrivate *priv = visual_plugin_get_private (plugin);
	uint32_t *vscr = visual_video_get_pixels (video);

	visual_audio_get_sample_mixed_simple (audio, priv->pcmbuf, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (priv->freqbuf, priv->pcmbuf, TRUE);

	_jakdaw_feedback_render (priv, vscr);
	_jakdaw_plotter_draw (priv,
			visual_buffer_get_data (priv->pcmbuf),
			visual_buffer_get_data (priv->freqbuf), vscr);
}

