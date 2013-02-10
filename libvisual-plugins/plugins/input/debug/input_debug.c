/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2005 Dennis Smit <dsmit@nerds-incorporated.org>
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
#include <math.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define OUTPUT_RATE       44100
#define OUTPUT_SAMPLES    1024
#define DEFAULT_FREQUENCY (OUTPUT_RATE / 500.0)
#define DEFAULT_AMPLITUDE 1.0

#define UPLOAD_PERIOD_USECS  (((uint64_t) OUTPUT_SAMPLES * VISUAL_USECS_PER_SEC) / OUTPUT_RATE)

typedef struct {
	float frequency;
	float ampltitude;

	float angle;
	float angle_step;

	VisTime *last_upload_time;
} DebugPriv;

static int  inp_debug_init    (VisPluginData *plugin);
static void inp_debug_cleanup (VisPluginData *plugin);
static int  inp_debug_events  (VisPluginData *plugin, VisEventQueue *events);
static int  inp_debug_upload  (VisPluginData *plugin, VisAudio *audio);

static void change_param (VisPluginData *plugin, VisParam *param);
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
		.url      = "http://libvisual.org",
		.about    = N_("debug input plugin"),
		.help     = N_("this will generate a sine wave for debugging purposes"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = inp_debug_init,
		.cleanup  = inp_debug_cleanup,
		.events   = inp_debug_events,
		.plugin   = &input
	};

	return &info;
}

static int inp_debug_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	DebugPriv *priv = visual_mem_new0 (DebugPriv, 1);
	visual_plugin_set_private (plugin, priv);

	VisParamList *params = visual_plugin_get_params (plugin);
	visual_param_list_add_many (params,
                                visual_param_new_float ("frequency",
                                                        N_("Frequency of sine wave"),
                                                        DEFAULT_FREQUENCY,
                                                        visual_param_in_range_float (0.0f, 22000.0f)),
                                visual_param_new_float ("ampltitude",
                                                        N_("Ampltitude of sine wave"),
                                                        DEFAULT_AMPLITUDE,
                                                        visual_param_in_range_float (0.0f, 1.0f)),
                                NULL);

	priv->frequency	 = DEFAULT_FREQUENCY;
	priv->ampltitude = DEFAULT_AMPLITUDE;

	priv->last_upload_time = NULL;

	setup_wave (priv);

	return TRUE;
}

static void inp_debug_cleanup (VisPluginData *plugin)
{
	DebugPriv *priv = visual_plugin_get_private (plugin);

	visual_time_free (priv->last_upload_time);
	visual_mem_free (priv);
}

static void setup_wave (DebugPriv *priv)
{
	priv->angle = 0.0;
	priv->angle_step = (2.0 * VISUAL_MATH_PI * priv->frequency) / OUTPUT_RATE;
}

static void change_param (VisPluginData *plugin, VisParam *param)
{
	/* FIXME: Implement */
}

static int inp_debug_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM: {
				VisParam *param = ev.event.param.param;
				change_param (plugin, param);
			}
			default:; /* discard */
		}
	}

	return TRUE;
}

static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio)
{
	/* FIXME: Wave is incrementally calculated and will very gradually
	   increase/decrease in frequency due to error accumulation */

	DebugPriv *priv = visual_plugin_get_private (plugin);

	/* Sleep for the appropriate amount of time to simulate blocking
	 * due to buffer underruns */

	VisTime *current_time = visual_time_new_now ();

	if (!priv->last_upload_time) {
		priv->last_upload_time = visual_time_clone (current_time);
	}

	VisTime *diff_time = visual_time_new ();
	visual_time_diff (diff_time, current_time, priv->last_upload_time);

	int64_t sleep_time = (int64_t) UPLOAD_PERIOD_USECS - (int64_t) visual_time_to_usecs (diff_time);
	if (sleep_time > 0) {
		visual_usleep (sleep_time);
	}

	visual_time_free (diff_time);
	visual_time_free (priv->last_upload_time);

	priv->last_upload_time = current_time;

	/* Generate and upload samples */

	float data[OUTPUT_SAMPLES*2];
	int i;

	for(i = 0; i < OUTPUT_SAMPLES*2; i += 2) {
		data[i] = data[i+1] = priv->ampltitude * sin (priv->angle);

		priv->angle += priv->angle_step;
		while (priv->angle >= 2 * VISUAL_MATH_PI) {
			priv->angle -= 2 * VISUAL_MATH_PI;
		}
	}

	VisBuffer *buffer = visual_buffer_new_wrap_data (data, sizeof (data), FALSE);

	visual_audio_input (audio, buffer,
	                    VISUAL_AUDIO_SAMPLE_RATE_44100,
	                    VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
	                    VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

	visual_buffer_unref (buffer);

    return TRUE;
}
