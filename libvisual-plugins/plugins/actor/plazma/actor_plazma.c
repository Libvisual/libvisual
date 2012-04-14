/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_plazma.c,v 1.21 2006-09-20 19:28:29 synap Exp $
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

#include <libvisual/libvisual.h>

#include "plazma.h"
#include "actor_plazma.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int act_plazma_init (VisPluginData *plugin);
static int act_plazma_cleanup (VisPluginData *plugin);
static int act_plazma_requisition (VisPluginData *plugin, int *width, int *height);
static int act_plazma_resize (VisPluginData *plugin, int width, int height);
static int act_plazma_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_plazma_palette (VisPluginData *plugin);
static int act_plazma_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_plazma_requisition,
		.palette = act_plazma_palette,
		.render = act_plazma_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "plazma",
		.name = "Plazma plugin",
		.author = ("Original by: Pascal Brochart <p.brochart@libertysurf.fr>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.0.1",
		.about = ("Libvisual Plazma visual plugin"),
		.help = ("This is the libvisual port of the xmms Plazma plugin"),
		.license = VISUAL_PLUGIN_LICENSE_GPL,

		.init = act_plazma_init,
		.cleanup = act_plazma_cleanup,
		.events = act_plazma_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	return &info;
}

static int act_plazma_init (VisPluginData *plugin)
{
	PlazmaPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("bass sensitivity",	0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("plazma effect",	TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("3d effect option",	FALSE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("lines",		TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("spectrum",		TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("3d effect",		TRUE),
		VISUAL_PARAM_LIST_ENTRY_FLOAT	("rotation speed",	0.4),
		VISUAL_PARAM_LIST_END
	};

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (PlazmaPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->colors = visual_palette_new (256);

	visual_param_container_add_many (paramcontainer, params);

	priv->val_maxi =		127;
	priv->chcol0 =			36;
	priv->state =			1368;
	priv->old_state =		1368;

	return 0;
}

static int act_plazma_cleanup (VisPluginData *plugin)
{
	PlazmaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	_plazma_cleanup (priv);

	visual_palette_free (priv->colors);

	visual_mem_free (priv);

	return 0;
}

static int act_plazma_requisition (VisPluginData *plugin, int *width, int *height)
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

	return 0;
}

static int act_plazma_resize (VisPluginData *plugin, int width, int height)
{
	PlazmaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	priv->width = width;
	priv->height = height;

	priv->tablex = width * 2;
	priv->tabley = height * 2;

	_plazma_cleanup (priv);
	_plazma_init (priv);

	return 0;
}

static int act_plazma_events (VisPluginData *plugin, VisEventQueue *events)
{
	PlazmaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_plazma_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "bass sensitivity")) {
					priv->bass_sensibility = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "plasma effect")) {
					priv->effect = visual_param_entry_get_integer (param);
					_plazma_change_effect (priv);

				} else if (visual_param_entry_is (param, "3d effect option")) {
					priv->options = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "lines")) {
					priv->lines = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "spectrum")) {
					priv->spectrum = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "3d effect")) {
					priv->use_3d = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "rotation speed")) {
					priv->rot_tourni = visual_param_entry_get_float (param);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *act_plazma_palette (VisPluginData *plugin)
{
	PlazmaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return priv->colors;
}

static int act_plazma_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PlazmaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer pcmback;
	VisBuffer fbuf;
	int i;

	visual_buffer_set_data_pair (&pcmback, priv->pcm_buffer, sizeof (float) * 1024);
	visual_audio_get_sample_mixed (audio, &pcmback, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

	visual_buffer_set_data_pair (&fbuf, priv->render_buffer, sizeof (float) * 256);
	visual_audio_get_spectrum_for_sample (&fbuf, &pcmback, TRUE);

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

	return 0;
}

