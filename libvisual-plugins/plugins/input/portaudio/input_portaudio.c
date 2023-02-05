/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2023 Sebastian Pipping <sebastian@pipping.org>
 *
 * Authors: Sebastian Pipping <sebastian@pipping.org>
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

#include <stdlib.h>  // malloc

#include <libvisual/libvisual.h>
#include "gettext.h"
#include <portaudio.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define SAMPLE_RATE 44100
#define SAMPLE_RATE_TYPE_LV VISUAL_AUDIO_SAMPLE_RATE_44100
#define CHANNELS 2
#define CHANNELS_TYPE VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO
#define SAMPLE_FORMAT_BITS 16
#define SAMPLE_FORMAT_PA paInt16
#define SAMPLE_FORMAT_LV VISUAL_AUDIO_SAMPLE_FORMAT_S16

#define FRAMES 1024

typedef struct {
	PaStream *stream;
	void * buffer;
} PortAudioPrivate;

static int inp_portaudio_init (VisPluginData *plugin);
static int inp_portaudio_cleanup (VisPluginData *plugin);
static int inp_portaudio_upload (VisPluginData *plugin, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisInputPlugin input[] = {{
		.upload = inp_portaudio_upload
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "portaudio",
		.name = "portaudio",
		.author = "Sebastian Pipping <sebastian@pipping.org>",
		.version = "1.0",
		.about = N_("PortAudio capture plugin"),
		.help = N_("Use this plugin to capture PCM data from the PortAudio record device"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = inp_portaudio_init,
		.cleanup = inp_portaudio_cleanup,

		.plugin = VISUAL_OBJECT (&input[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int inp_portaudio_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	PortAudioPrivate * const priv = visual_mem_new0 (PortAudioPrivate, 1);
	visual_log_return_val_if_fail(priv != NULL, -1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	// Init PortAudio
	const PaError init_error = Pa_Initialize();
	visual_log_return_val_if_fail (init_error == paNoError, -2);

	// Select input device
	const PaDeviceIndex input_device = Pa_GetDefaultInputDevice ();
	visual_log_return_val_if_fail (input_device != paNoDevice, -3);

	// Open stream
	const double latency_seconds = 1.0 / SAMPLE_RATE * FRAMES;
	const PaStreamParameters input_parameters = {
			.device = input_device,
			.channelCount = CHANNELS,
			.sampleFormat = SAMPLE_FORMAT_PA,
			.suggestedLatency = latency_seconds,
			.hostApiSpecificStreamInfo = NULL};
	const PaError open_error =
			Pa_OpenStream (&priv->stream, &input_parameters, NULL,
						   SAMPLE_RATE, FRAMES, paClipOff, NULL, NULL);
	if (open_error != paNoError) {
		visual_log (VISUAL_LOG_CRITICAL,
				"PortAudio: Could not open input stream, error %d \"%s\".",
				open_error, Pa_GetErrorText (open_error));
		return -3;
	}

	// Allocate buffer
	const visual_size_t buffer_size_bytes = FRAMES * CHANNELS * (SAMPLE_FORMAT_BITS / 8);
	priv->buffer = malloc (buffer_size_bytes);
	visual_log_return_val_if_fail (priv->buffer != NULL, -4);

	// Start stream
	const PaError input_start_error = Pa_StartStream (priv->stream);
	visual_log_return_val_if_fail (input_start_error == paNoError, -5);

	return 0;
}

int inp_portaudio_cleanup (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	PortAudioPrivate * const priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail (priv != NULL, -2);

	free (priv->buffer);
	priv->buffer = NULL;

	Pa_StopStream (priv->stream);
	Pa_CloseStream (priv->stream);
	priv->stream = NULL;

	Pa_Terminate ();

	visual_mem_free (priv);

	return 0;
}

int inp_portaudio_upload (VisPluginData *plugin, VisAudio *audio)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);
	visual_log_return_val_if_fail (audio != NULL, -2);

	PortAudioPrivate * const priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail (priv != NULL, -3);
	visual_log_return_val_if_fail (priv->stream != NULL, -4);

	for (;;) {
		long frames_to_read = Pa_GetStreamReadAvailable (priv->stream);
		if (frames_to_read < 1) {
			return 0;
		}
		if (frames_to_read > FRAMES) {
			frames_to_read = FRAMES;  // because that's all that priv->buffer has space for
		}

		const PaError read_error = Pa_ReadStream(priv->stream, priv->buffer, frames_to_read);
		if (read_error != paNoError) {
			return 0;
		}

		const visual_size_t bytes_to_write = frames_to_read * CHANNELS * (SAMPLE_FORMAT_BITS / 8);

		VisBuffer buffer;
		visual_buffer_init(&buffer, priv->buffer, bytes_to_write, NULL);

		visual_audio_samplepool_input(audio->samplepool, &buffer, SAMPLE_RATE_TYPE_LV,
									  SAMPLE_FORMAT_LV, CHANNELS_TYPE);
	}

	// we never get here
}
