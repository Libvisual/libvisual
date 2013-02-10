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
#include "actor_plazma.h"
#include "plazma.h"
#include <libvisual/libvisual.h>
#include <limits.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_plazma_init        (VisPluginData *plugin);
static void        act_plazma_cleanup     (VisPluginData *plugin);
static void        act_plazma_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_plazma_resize      (VisPluginData *plugin, int width, int height);
static int         act_plazma_events      (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_plazma_palette     (VisPluginData *plugin);
static void        act_plazma_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_plazma_requisition,
		.palette     = act_plazma_palette,
		.render      = act_plazma_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "plazma",
		.name     = "Plazma plugin",
		.author   = N_("Original by: Pascal Brochart <p.brochart@libertysurf.fr>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.0.1",
		.about    = N_("Libvisual Plazma visual plugin"),
		.help     = N_("This is the libvisual port of the xmms Plazma plugin"),
		.license  = VISUAL_PLUGIN_LICENSE_GPL,

		.init     = act_plazma_init,
		.cleanup  = act_plazma_cleanup,
		.events   = act_plazma_events,
		.plugin   = &actor
	};

	return &info;
}

static int act_plazma_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	PlazmaPrivate *priv = visual_mem_new0 (PlazmaPrivate, 1);
	visual_plugin_set_private (plugin, priv);

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_integer ("bass_sensitivity",
                                                          N_("Bass sensitivity"),
                                                          0,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_bool    ("plasma_effect",
                                                          N_("Plasma effect"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("3d_effect_option",
                                                          N_("3D effect option"),
                                                          FALSE,
                                                          NULL),
                                visual_param_new_bool    ("lines",
                                                          N_("Lines"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("spectrum",
                                                          N_("Spectrum"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("3d_effect",
                                                          N_("3D effect"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_float   ("rotation_speed",
                                                          N_("Rotation speed"),
                                                          0.4,
                                                          NULL),
                                NULL);

	priv->colors = visual_palette_new (256);

	priv->val_maxi =		127;
	priv->chcol0 =			36;
	priv->state =			1368;
	priv->old_state =		1368;

	return TRUE;
}

static void act_plazma_cleanup (VisPluginData *plugin)
{
	PlazmaPrivate *priv = visual_plugin_get_private (plugin);

	_plazma_cleanup (priv);

	visual_palette_free (priv->colors);

	visual_mem_free (priv);
}

static void act_plazma_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;
}

static void act_plazma_resize (VisPluginData *plugin, int width, int height)
{
	PlazmaPrivate *priv = visual_plugin_get_private (plugin);

	priv->width = width;
	priv->height = height;

	priv->tablex = width * 2;
	priv->tabley = height * 2;

	_plazma_cleanup (priv);
	_plazma_init (priv);
}

static int act_plazma_events (VisPluginData *plugin, VisEventQueue *events)
{
	PlazmaPrivate *priv = visual_plugin_get_private (plugin);
	VisEvent ev;
	VisParam *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_plazma_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_has_name (param, "bass_sensitivity")) {
					priv->bass_sensibility = visual_param_get_value_integer (param);

				} else if (visual_param_has_name (param, "plasma_effect")) {
					priv->effect = visual_param_get_value_bool (param);
					_plazma_change_effect (priv);

				} else if (visual_param_has_name (param, "3d_effect_option")) {
					priv->options = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "lines")) {
					priv->lines = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "spectrum")) {
					priv->spectrum = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "3d_effect")) {
					priv->use_3d = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "rotation_speed")) {
					priv->rot_tourni = visual_param_get_value_float (param);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_plazma_palette (VisPluginData *plugin)
{
	PlazmaPrivate *priv = visual_plugin_get_private (plugin);

	return priv->colors;
}

static void act_plazma_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PlazmaPrivate *priv = visual_plugin_get_private (plugin);
	VisBuffer *pcmback;
	VisBuffer *fbuf;
	int i;

	pcmback = visual_buffer_new_wrap_data (priv->pcm_buffer, sizeof (float) * 1024, FALSE);
	visual_audio_get_sample_mixed_simple (audio, pcmback, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);


	fbuf = visual_buffer_new_wrap_data (priv->render_buffer, sizeof (float) * 256, FALSE);
	visual_audio_get_spectrum_for_sample (fbuf, pcmback, TRUE);

	visual_buffer_unref (pcmback);
	visual_buffer_unref (fbuf);

	/* Analyse spectrum data */
	priv->bass = 0;
	for (i = 0; i < 6; i++)
		priv->bass += (priv->render_buffer[i]);

	priv->old_state = priv->state;

	if (priv->bass_sensibility >= 0)
		priv->state += (priv->bass) + 1 + (priv->bass_sensibility / 2);
	if (priv->bass_sensibility < 0)   {
		priv->state += (priv->bass) + 1 - (abs (priv->bass_sensibility / 2));
		if (priv->state < (priv->old_state + 1))
			priv->state = priv->old_state + 1;
	}

	priv->video = video;
	priv->pixel = visual_video_get_pixels (video);

	_plazma_run (priv);
}

