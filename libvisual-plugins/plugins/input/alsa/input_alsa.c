/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Vitaly V. Bursov <vitalyvb@urk,net>
 *
 * Authors: Vitaly V. Bursov <vitalyvb@urk,net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_alsa.c,v 1.23 2006/02/13 20:36:11 synap Exp $
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

#include <math.h>

#include <libvisual/libvisual.h>

#include <alsa/version.h>
#if (SND_LIB_MAJOR == 0 && SND_LIB_MINOR == 9)
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#endif
#include <sys/types.h> // Workaround ALSA regression
#include <alsa/asoundlib.h>

#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define PCM_BUF_SIZE 1024

typedef struct {
	snd_pcm_t *chandle;
	int loaded;
} alsaPrivate;

static int  inp_alsa_init    (VisPluginData *plugin);
static void inp_alsa_cleanup (VisPluginData *plugin);
static int  inp_alsa_upload  (VisPluginData *plugin, VisAudio *audio);

static const char *inp_alsa_var_cdevice    = "default";
static const int   inp_alsa_var_samplerate = 44100;
static const int   inp_alsa_var_frames_target = 256;
static const int   inp_alsa_var_channels   = 2;

const VisPluginInfo *get_plugin_info (void)
{
	static VisInputPlugin input = {
		.upload = inp_alsa_upload
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "alsa",
		.name     = "alsa",
		.author   = "Vitaly V. Bursov <vitalyvb@urk.net>",
		.version  = "0.1",
		.about    = N_("ALSA capture plugin"),
		.help     = N_("Use this plugin to capture PCM data from the ALSA record device"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init    = inp_alsa_init,
		.cleanup = inp_alsa_cleanup,
		.plugin  = &input
	};

	return &info;
}

int inp_alsa_init (VisPluginData *plugin)
{
	snd_pcm_hw_params_t *hwparams = NULL;
	alsaPrivate *priv;
	unsigned int rate = inp_alsa_var_samplerate;
	unsigned int exact_rate;
	unsigned int tmp;
	int dir = 0;
	int err;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	priv = visual_mem_new0 (alsaPrivate, 1);
	visual_plugin_set_private (plugin, priv);

	if ((err = snd_pcm_open(&priv->chandle, inp_alsa_var_cdevice,
			SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0) {
		visual_log(VISUAL_LOG_ERROR,
			    "Record open error: %s", snd_strerror(err));
		return FALSE;
	}

	snd_pcm_hw_params_malloc(&hwparams);
	visual_return_val_if_fail(hwparams != NULL, FALSE);

	if (snd_pcm_hw_params_any(priv->chandle, hwparams) < 0) {
		visual_log(VISUAL_LOG_ERROR,
			   "Cannot configure this PCM device");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	if (snd_pcm_hw_params_set_access(priv->chandle, hwparams,
					 SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		visual_log(VISUAL_LOG_ERROR, "Error setting access");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

#if VISUAL_LITTLE_ENDIAN == 1
	if (snd_pcm_hw_params_set_format(priv->chandle, hwparams,
					 SND_PCM_FORMAT_S16_LE) < 0) {
#else
	if (snd_pcm_hw_params_set_format(priv->chandle, hwparams,
					 SND_PCM_FORMAT_S16_BE) < 0) {
#endif
		visual_log(VISUAL_LOG_ERROR, "Error setting format");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	exact_rate = rate;

	if (snd_pcm_hw_params_set_rate_near(priv->chandle, hwparams,
					    &exact_rate, &dir) < 0) {
		visual_log(VISUAL_LOG_ERROR, "Error setting rate");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}
	if (exact_rate != rate) {
		visual_log(VISUAL_LOG_INFO,
			   "The rate %d Hz is not supported by your " \
			   "hardware.\n" \
			   "==> Using %d Hz instead", rate, exact_rate);
	}

	if (snd_pcm_hw_params_set_channels(priv->chandle, hwparams,
					   inp_alsa_var_channels) < 0) {
		visual_log(VISUAL_LOG_ERROR, "Error setting channels");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	tmp = (int)ceil((1000.0 * 1000.0 * inp_alsa_var_frames_target) / inp_alsa_var_samplerate);  // in micro seconds
	if (snd_pcm_hw_params_set_period_time_near(priv->chandle, hwparams, &tmp, &dir) < 0){
		visual_log(VISUAL_LOG_ERROR, "Error setting period time");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	tmp = (int)ceil((1000.0 * 1000.0 * inp_alsa_var_frames_target) / inp_alsa_var_samplerate);  // in micro seconds
	if (snd_pcm_hw_params_set_buffer_time_near(priv->chandle, hwparams, &tmp, &dir) < 0){
		visual_log(VISUAL_LOG_ERROR, "Error setting buffer time");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}


	if (snd_pcm_hw_params(priv->chandle, hwparams) < 0) {
		visual_log(VISUAL_LOG_ERROR, "Error setting HW params");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	if (snd_pcm_prepare(priv->chandle) < 0) {
		visual_log(VISUAL_LOG_ERROR, "Failed to prepare interface");
		snd_pcm_hw_params_free(hwparams);
		return FALSE;
	}

	snd_pcm_hw_params_free(hwparams);

	priv->loaded = TRUE;

	return TRUE;
}

void inp_alsa_cleanup (VisPluginData *plugin)
{
	alsaPrivate *priv = visual_plugin_get_private (plugin);

	if (priv->loaded) {
		snd_pcm_close(priv->chandle);
    }

	visual_mem_free (priv);
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
	alsaPrivate *priv = visual_plugin_get_private (plugin);

	int16_t data[PCM_BUF_SIZE];
	const snd_pcm_uframes_t frames_wanted = sizeof(data) / sizeof(data[0]) / inp_alsa_var_channels;
	snd_pcm_sframes_t rcnt;

	do {
		rcnt = snd_pcm_readi(priv->chandle, data, frames_wanted);

		if (rcnt > 0) {
			VisBuffer *buffer;

			buffer = visual_buffer_new_wrap_data (data, rcnt * sizeof(data[0]) * inp_alsa_var_channels, FALSE);

			visual_audio_input (audio, buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
					VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

			visual_buffer_unref (buffer);
		}

		if (rcnt < 0) {
			if (rcnt == -EPIPE) {
				visual_log(VISUAL_LOG_WARNING, "ALSA: Buffer Overrun");

				if (snd_pcm_prepare(priv->chandle) < 0) {
					visual_log(VISUAL_LOG_ERROR,
							"Failed to prepare interface");
					return FALSE;
				}
			}
		}
	} while (rcnt > 0);

	return TRUE;
}
