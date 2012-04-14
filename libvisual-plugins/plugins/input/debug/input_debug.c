/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2005 Dennis Smit <dsmit@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_debug.c,v 1.3 2005/12/22 21:50:10 synap Exp $
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
#include <math.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define OUTPUT_RATE       44100
#define OUTPUT_SAMPLES    4096
#define DEFAULT_FREQUENCY (OUTPUT_RATE/25)
#define DEFAULT_AMPLITUDE 1.0

typedef struct {
	float frequency;
	float ampltitude;

	float angle;
	float angle_step;
} DebugPriv;

static int inp_debug_init (VisPluginData *plugin);
static int inp_debug_cleanup (VisPluginData *plugin);
static int inp_debug_events (VisPluginData *plugin, VisEventQueue *events);
static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio);

static void change_param (VisPluginData *plugin, VisParamEntry *param);
static void setup_wave (DebugPriv *priv);

const VisPluginInfo *get_plugin_info (void)
{
	static VisInputPlugin input = {
		.upload = inp_debug_upload
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_INPUT,
		.plugname = "debug",
		.name     = "debug",
		.author   = "Vitaly V. Bursov <vitalyvb@urk.net>",
		.version  = "0.2",
		.about    = N_("debug input plugin"),
		.help     = N_("this will generate a sine wave for debugging purposes"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = inp_debug_init,
		.cleanup  = inp_debug_cleanup,
		.events   = inp_debug_events,
		.plugin   = VISUAL_OBJECT (&input)
	};

	return &info;
}

static int inp_debug_init (VisPluginData *plugin)
{
	DebugPriv *priv;
	VisParamEntry *param;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_FLOAT ("frequency",  DEFAULT_FREQUENCY),
		VISUAL_PARAM_LIST_ENTRY_FLOAT ("ampltitude", DEFAULT_AMPLITUDE),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (DebugPriv, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->frequency	 = DEFAULT_FREQUENCY;
	priv->ampltitude = DEFAULT_AMPLITUDE;
	setup_wave (priv);

	visual_param_container_add_many (paramcontainer, params);

	param = visual_param_container_get (paramcontainer, "frequency");
	visual_param_entry_min_set_float (param, 0.0);
	visual_param_entry_max_set_float (param, 22000.0);

	param = visual_param_container_get (paramcontainer, "ampltitude");
	visual_param_entry_min_set_float (param, 0.0);
	visual_param_entry_max_set_float (param, 1.0);

	return 0;
}

static int inp_debug_cleanup (VisPluginData *plugin)
{
	return 0;
}

static void setup_wave (DebugPriv *priv)
{
	priv->angle = 0.0;
	priv->angle_step = (2 * VISUAL_MATH_PI * priv->frequency) / OUTPUT_RATE;
}

static void change_param (VisPluginData *plugin, VisParamEntry *param)
{
	/* FIXME: Implement */
}

static int inp_debug_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM: {
				VisParamEntry *param = ev.event.param.param;
				change_param (plugin, param);
			}
			default:; /* discard */
		}
	}

	return 0;
}

static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio)
{
	/* FIXME: Wave is incrementally calculated and will very gradually
	   increase/decrease in frequency due to error accumulation */

	DebugPriv *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	int16_t data[OUTPUT_SAMPLES];
	int i;

	for(i = 0; i < VISUAL_TABLESIZE(data); i++) {
		data[i] = (int16_t) (65535/2 * priv->ampltitude * sin (priv->angle));

		priv->angle += priv->angle_step;
		if (priv->angle >= 2 * VISUAL_MATH_PI) {
			priv->angle -= 2 * VISUAL_MATH_PI;
		}
	}

	VisBuffer buffer;
	visual_buffer_init (&buffer, data, VISUAL_TABLESIZE (data), NULL);

	visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
			VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

	return 0;
}
