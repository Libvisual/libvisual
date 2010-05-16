/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_audio.c,v 1.43 2006/02/17 22:00:17 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "lv_common.h"
#include "lv_math.h"
#include "lv_audio.h"

static int audio_dtor (VisObject *object);
static int audio_samplepool_dtor (VisObject *object);
static int audio_samplepool_channel_dtor (VisObject *object);
static int audio_sample_dtor (VisObject *object);

static int audio_band_total (VisAudio *audio, int begin, int end);
static int audio_band_energy (VisAudio *audio, int band, int length);

/* Format transform functions */
static int transform_format_buffer_from_float (VisBuffer *dest, VisBuffer *src, int size, int sign);
static int transform_format_buffer_to_float (VisBuffer *dest, VisBuffer *src, int size, int sign);
static int transform_format_buffer (VisBuffer *dest, VisBuffer *src, int dsize, int ssize, int dsigned, int ssigned);

/* Ringbuffer data provider functions */
static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);
static void sample_destroy_func (VisRingBufferEntry *entry);
static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);

/*  functions */
static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate);


static int audio_dtor (VisObject *object)
{
	VisAudio *audio = VISUAL_AUDIO (object);

	if (audio->samplepool != NULL)
		visual_object_unref (VISUAL_OBJECT (audio->samplepool));

	audio->samplepool = NULL;

	return VISUAL_OK;
}

static int audio_samplepool_dtor (VisObject *object)
{
	VisAudioSamplePool *samplepool = VISUAL_AUDIO_SAMPLEPOOL (object);

	if (samplepool->channels != NULL)
		visual_object_unref (VISUAL_OBJECT (samplepool->channels));

	samplepool->channels = NULL;

	return VISUAL_OK;
}

static int audio_samplepool_channel_dtor (VisObject *object)
{
	VisAudioSamplePoolChannel *channel = VISUAL_AUDIO_SAMPLEPOOL_CHANNEL (object);

	if (channel->samples != NULL)
		visual_object_unref (VISUAL_OBJECT (channel->samples));

	if (channel->channelid != NULL)
		visual_mem_free (channel->channelid);

	channel->samples = NULL;
	channel->channelid= NULL;

	return VISUAL_OK;
}

static int audio_sample_dtor (VisObject *object)
{
	VisAudioSample *sample = VISUAL_AUDIO_SAMPLE (object);

	if (sample->buffer != NULL)
		visual_object_unref (VISUAL_OBJECT (sample->buffer));

	if (sample->processed != NULL)
		visual_object_unref (VISUAL_OBJECT (sample->processed));

	sample->buffer = NULL;
	sample->processed = NULL;

	return VISUAL_OK;
}


static int audio_band_total (VisAudio *audio, int begin, int end)
{
	int bpmtotal = 0;
	int i;

//	for (i = begin; i < end; i++)
//		bpmtotal += audio->freq[2][i];

	if (bpmtotal > 0)
		return bpmtotal / (end - begin);
	else
		return 0;
}

static int audio_band_energy (VisAudio *audio, int band, int length)
{
	int energytotal = 0;
	int i;

//	for (i = 0; i < length; i++)
//		energytotal += audio->bpmhistory[i][band];

	if (energytotal > 0)
		return energytotal / length;
	else
		return 0;
}

/**
 * @defgroup VisAudio VisAudio
 * @{
 */

/**
 * Creates a new VisAudio structure.
 *
 * @return A newly allocated VisAudio, or NULL on failure.
 */
VisAudio *visual_audio_new ()
{
	VisAudio *audio;

	audio = visual_mem_new0 (VisAudio, 1);

	visual_audio_init (audio);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (audio), TRUE);
	visual_object_ref (VISUAL_OBJECT (audio));

	return audio;
}

/**
 * Initializes a VisAudio, this should not be used to reset a VisAudio.
 * The resulting initialized VisAudio is a valid VisObject even if it was not allocated.
 * Keep in mind that VisAudio structures that were created by visual_audio_new() should not
 * be passed to visual_audio_init().
 *
 * @see visual_audio_new
 *
 * @param audio Pointer to the VisAudio which needs to be initialized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_AUDIO_NULL on failure.
 */
int visual_audio_init (VisAudio *audio)
{
	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (audio));
	visual_object_set_dtor (VISUAL_OBJECT (audio), audio_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (audio), FALSE);

	/* Reset the VisAudio data */
	audio->samplepool = visual_audio_samplepool_new ();

	return VISUAL_OK;
}

/**
 * This function analyzes the VisAudio, the Fourier frequency magic gets done here, also
 * the audio energy is calculated and some other magic to provide the developer more
 * information about the current sample and the stream.
 *
 * For every sample that is being retrieved this needs to be called. However keep in mind
 * that the VisBin runs it automaticly.
 *
 * @param audio Pointer to a VisAudio that needs to be analyzed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_AUDIO_NULL on failure.
 */
int visual_audio_analyze (VisAudio *audio)
{
        float temp_out[256];
	float temp_audio[2][512];
	short pcm[3][1024];
	double scale;
	int i, j, y;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);

	/* Load the pcm data */
#if 0
	for (i = 0; i < 512; i++) {
		audio->pcm[0][i] = audio->plugpcm[0][i];
		audio->pcm[1][i] = audio->plugpcm[1][i];
		audio->pcm[2][i] = (audio->plugpcm[0][i] + audio->plugpcm[1][i]) >> 1;
	}
#endif
	/* -------------------------------- audio pool testing */
	{
/*		VisBuffer *buffer = visual_buffer_new_allocate (sizeof (int16_t) * 512, visual_buffer_destroyer_free);
		VisTime timestamp;
		VisAudioSample *sample;

		visual_mem_copy (visual_buffer_get_data (buffer), audio->plugpcm[2], sizeof (int16_t) * 512);

		visual_time_get (&timestamp);

		sample = visual_audio_sample_new (buffer, &timestamp,
				VISUAL_AUDIO_SAMPLE_RATE_44100,
				VISUAL_AUDIO_SAMPLE_FORMAT_S8);

		visual_audio_samplepool_add (audio->samplepool, sample, "front");
*/
		VisAudioSamplePoolChannel *channel;
		VisBuffer buffer;

		visual_audio_samplepool_flush_old (audio->samplepool);

		channel = visual_audio_samplepool_get_channel (audio->samplepool, VISUAL_AUDIO_CHANNEL_LEFT);

		if (channel != 0) {
			visual_buffer_init (&buffer, pcm[0], 1024, NULL);

//			printf ("--channel debug: %s: %d\n", channel->channelid, visual_ringbuffer_get_size (channel->samples));
			visual_ringbuffer_get_data (channel->samples, &buffer, 1024);

			visual_object_unref (VISUAL_OBJECT (&buffer));
		}

		channel = visual_audio_samplepool_get_channel (audio->samplepool, VISUAL_AUDIO_CHANNEL_RIGHT);

		if (channel != 0) {
			visual_buffer_init (&buffer, pcm[1], 1024, NULL);

//			printf ("--channel debug: %s: %d\n", channel->channelid, visual_ringbuffer_get_size (channel->samples));
			visual_ringbuffer_get_data (channel->samples, &buffer, 1024);

			visual_object_unref (VISUAL_OBJECT (&buffer));
		}

	}
	/* /-------------------------------- audio pool testing */

//	for (i = 0; i < 512; i++) {
//		audio->pcm[2][i] = (audio->pcm[0][i] + audio->pcm[1][i]) >> 1;
//	}

#if 0
	/* Convert int16_t audio to float audio, (won't be needed when the rest of the new audio
	 * core lands). */
	for (i = 0; i < 512; i++) {
		temp_audio[0][i] = audio->pcm[0][i];
		temp_audio[1][i] = audio->pcm[1][i];
	}
	/* Fourier analyze the pcm data */
	visual_fourier_perform (audio->fourier, temp_audio[0], temp_out);

	for (i = 0; i < 256; i++)
		audio->freq[0][i] = temp_out[i] * 50;

	visual_fourier_perform (audio->fourier, temp_audio[1], temp_out);

	for (i = 0; i < 256; i++)
		audio->freq[1][i] = temp_out[i] * 50;

	/* Average channel */
	for (i = 0; i < 256; i++)
		audio->freq[2][i] = (audio->freq[0][i] + audio->freq[1][i]) >> 1;

	/* Normalized frequency analyzer */
	/** @todo FIXME Not sure if this is totally correct */
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 256; j++) {
			/* (Height / log (256)) */
			scale = 256 / log (256);

			y = audio->freq[i][j];
			y = log (y) * scale;

			if (y < 0)
				y = 0;

			audio->freqnorm[i][j] = y;
		}
	}

#endif
#if 0
	/* BPM stuff, used for the audio energy only right now */
	for (i = 1023; i > 0; i--) {
		visual_mem_copy (&audio->bpmhistory[i], &audio->bpmhistory[i - 1], 6 * sizeof (short int));
		visual_mem_copy (&audio->bpmdata[i], &audio->bpmdata[i - 1], 6 * sizeof (short int));
	}

	/* Calculate the audio energy */
	audio->energy = 0;

	for (i = 0; i < 6; i++)	{
		audio->bpmhistory[0][i] = audio_band_total (audio, i * 2, (i * 2) + 3);
		audio->bpmenergy[i] = audio_band_energy (audio, i, 10);

		audio->bpmdata[0][i] = audio->bpmhistory[0][i] - audio->bpmenergy[i];

		audio->energy += audio_band_energy(audio, i, 50);
	}

	audio->energy >>= 7;

	if (audio->energy > 100)
		audio->energy = 100;
#endif

	return VISUAL_OK;
}

int visual_audio_get_sample (VisAudio *audio, VisBuffer *buffer, char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

	channel = visual_audio_samplepool_get_channel (audio->samplepool, channelid);

	if (channel == NULL) {
		visual_buffer_fill (buffer, 0);

		return -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL;
	}

	visual_ringbuffer_get_data_from_end (channel->samples, buffer, visual_buffer_get_size (buffer));

	return VISUAL_OK;
}

/* FIXME too much code duplication with get_sample_mixed, reduce */
int visual_audio_get_sample_mixed_simple (VisAudio *audio, VisBuffer *buffer, int channels, ...)
{
	VisAudioSamplePoolChannel *channel;
	VisBuffer temp;
	char **chanids;
	va_list ap;
	int i;
	int first = TRUE;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_init_allocate (&temp, visual_buffer_get_size (buffer), visual_buffer_destroyer_free);

	chanids = visual_mem_malloc (channels * sizeof (char *));

	va_start (ap, channels);

	/* Retrieving mixing data from valist */
	for (i = 0; i < channels; i++)
		chanids[i] = va_arg (ap, char *);

	visual_buffer_fill (buffer, 0);

	/* The mixing loop */
	for (i = 0; i < channels; i++) {
		if (visual_audio_get_sample (audio, &temp, chanids[i]) == VISUAL_OK) {
			channel = visual_audio_samplepool_get_channel (audio->samplepool, chanids[i]);

			if (first == TRUE) {
				visual_audio_sample_buffer_mix (buffer, &temp, FALSE, channel->factor);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, &temp, TRUE, channel->factor);
			}
		}
	}

	va_end (ap);

	visual_object_unref (VISUAL_OBJECT (&temp));

	visual_mem_free (chanids);

	return VISUAL_OK;
}

int visual_audio_get_sample_mixed (VisAudio *audio, VisBuffer *buffer, int divide, int channels, ...)
{
	VisBuffer temp;
	char **chanids;
	double *chanmuls;
	va_list ap;
	int i;
	int first = TRUE;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_init_allocate (&temp, visual_buffer_get_size (buffer), visual_buffer_destroyer_free);

	chanids = visual_mem_malloc (channels * sizeof (char *));
	chanmuls = visual_mem_malloc (channels * sizeof (double));

	va_start (ap, channels);

	/* Retrieving mixing data from valist */
	for (i = 0; i < channels; i++)
		chanids[i] = va_arg (ap, char *);

	for (i = 0; i < channels; i++)
		chanmuls[i] = va_arg (ap, double);

	visual_buffer_fill (buffer, 0);

	/* The mixing loop */
	for (i = 0; i < channels; i++) {
		if (visual_audio_get_sample (audio, &temp, chanids[i]) == VISUAL_OK) {
			if (first == TRUE) {
				visual_audio_sample_buffer_mix (buffer, &temp, FALSE, chanmuls[i]);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, &temp, divide, chanmuls[i]);
			}
		}
	}

	va_end (ap);

	visual_object_unref (VISUAL_OBJECT (&temp));

	visual_mem_free (chanids);
	visual_mem_free (chanmuls);

	return VISUAL_OK;
}

int visual_audio_get_sample_mixed_category (VisAudio *audio, VisBuffer *buffer, char *category, int divide)
{
	VisListEntry *le = NULL;
	VisAudioSamplePool *samplepool;
	VisAudioSamplePoolChannel *channel;
	VisBuffer temp;
	int first = TRUE;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (category != NULL, -VISUAL_ERROR_NULL);

	visual_buffer_init_allocate (&temp, visual_buffer_get_size (buffer), visual_buffer_destroyer_free);

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		if (strstr (channel->channelid, category) != NULL) {
			if (visual_audio_get_sample (audio, &temp, channel->channelid) == VISUAL_OK) {
				if (first == TRUE) {
					visual_audio_sample_buffer_mix (buffer, &temp, FALSE, 1.0);

					first = FALSE;
				} else {
					visual_audio_sample_buffer_mix (buffer, &temp, divide, 1.0);
				}
			}
		}
	}

	visual_object_unref (VISUAL_OBJECT (&temp));

	return VISUAL_OK;
}

int visual_audio_get_sample_mixed_all (VisAudio *audio, VisBuffer *buffer, int divide)
{
	VisListEntry *le = NULL;
	VisAudioSamplePool *samplepool;
	VisAudioSamplePoolChannel *channel;
	VisBuffer temp;
	int first = TRUE;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

	visual_buffer_init_allocate (&temp, visual_buffer_get_size (buffer), visual_buffer_destroyer_free);

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		if (visual_audio_get_sample (audio, &temp, channel->channelid) == VISUAL_OK) {
			if (first == TRUE) {
				visual_audio_sample_buffer_mix (buffer, &temp, FALSE, 1.0);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, &temp, divide, 1.0);
			}
		}
	}

	visual_object_unref (VISUAL_OBJECT (&temp));

	return VISUAL_OK;
}

int visual_audio_get_spectrum (VisAudio *audio, VisBuffer *buffer, int samplelen, char *channelid, int normalised)
{
	VisBuffer sample;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_init_allocate (&sample, samplelen, visual_buffer_destroyer_free);

	if (visual_audio_get_sample (audio, &sample, channelid) == VISUAL_OK)
		visual_audio_get_spectrum_for_sample (buffer, &sample, normalised);
	else
		visual_buffer_fill (buffer, 0);

	visual_object_unref (VISUAL_OBJECT (&sample));

	return VISUAL_OK;
}

int visual_audio_get_spectrum_multiplied (VisAudio *audio, VisBuffer *buffer, int samplelen, char *channelid, int normalised, float multiplier)
{
	int ret;
	float *data;
	int datasize;
	int i;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

	ret = visual_audio_get_spectrum (audio, buffer, samplelen, channelid, normalised);

	data = visual_buffer_get_data (buffer);
	datasize = visual_buffer_get_size (buffer) / sizeof (float);

	visual_math_vectorized_multiplier_floats_const_float (data, data, datasize, multiplier);

	return ret;
}

int visual_audio_get_spectrum_for_sample (VisBuffer *buffer, VisBuffer *sample, int normalised)
{
	VisDFT dft;

	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_dft_init (&dft, visual_buffer_get_size (buffer) / sizeof (float),
			visual_buffer_get_size (sample) / sizeof (float));

	/* Fourier analyze the pcm data */
	visual_dft_perform (&dft, visual_buffer_get_data (buffer), visual_buffer_get_data (sample));

	if (normalised == TRUE)
		visual_audio_normalise_spectrum (buffer);

	visual_object_unref (VISUAL_OBJECT (&dft));

	return VISUAL_OK;
}

int visual_audio_get_spectrum_for_sample_multiplied (VisBuffer *buffer, VisBuffer *sample, int normalised, float multiplier)
{
	int ret;
	float *data;
	int datasize;
	int i;

	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_BUFFER_NULL);

	ret = visual_audio_get_spectrum_for_sample (buffer, sample, normalised);

	data = visual_buffer_get_data (buffer);
	datasize = visual_buffer_get_size (buffer) / sizeof (float);

	visual_math_vectorized_multiplier_floats_const_float (data, data, datasize, multiplier);

	return ret;
}

int visual_audio_normalise_spectrum (VisBuffer *buffer)
{
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_dft_log_scale_standard (visual_buffer_get_data (buffer),
			visual_buffer_get_data (buffer),
			visual_buffer_get_size (buffer) / sizeof (float));

	return VISUAL_OK;
}

VisAudioSamplePool *visual_audio_samplepool_new ()
{
	VisAudioSamplePool *samplepool;

	samplepool = visual_mem_new0 (VisAudioSamplePool, 1);

	visual_audio_samplepool_init (samplepool);

	/* Do the VisObject initialization */
        visual_object_set_allocated (VISUAL_OBJECT (samplepool), TRUE);
	visual_object_ref (VISUAL_OBJECT (samplepool));

	return samplepool;
}

int visual_audio_samplepool_init (VisAudioSamplePool *samplepool)
{
	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (samplepool));
	visual_object_set_dtor (VISUAL_OBJECT (samplepool), audio_samplepool_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (samplepool), FALSE);

	/* Reset the VisAudioSamplePool structure */
	samplepool->channels = visual_list_new (visual_object_collection_destroyer);

	return VISUAL_OK;
}

int visual_audio_samplepool_add (VisAudioSamplePool *samplepool, VisAudioSample *sample, char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_log_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_NULL);

	channel = visual_audio_samplepool_get_channel (samplepool, channelid);

	/* Channel not there yet, make it */
	if (channel == NULL) {
		channel = visual_audio_samplepool_channel_new (channelid);

		visual_audio_samplepool_add_channel (samplepool, channel);
	}

	visual_audio_samplepool_channel_add (channel, sample);

	return VISUAL_OK;
}

int visual_audio_samplepool_add_channel (VisAudioSamplePool *samplepool, VisAudioSamplePoolChannel *channel)
{
	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	visual_list_add (samplepool->channels, channel);

	return VISUAL_OK;
}

VisAudioSamplePoolChannel *visual_audio_samplepool_get_channel (VisAudioSamplePool *samplepool, char *channelid)
{
	VisListEntry *le = NULL;
	VisAudioSamplePoolChannel *channel;

	visual_log_return_val_if_fail (samplepool != NULL, NULL);
	visual_log_return_val_if_fail (channelid != NULL, NULL);

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		if (strcmp (channel->channelid, channelid) == 0)
			return channel;
	}

	return NULL;
}

int visual_audio_samplepool_flush_old (VisAudioSamplePool *samplepool)
{
	VisListEntry *le = NULL;
	VisAudioSamplePoolChannel *channel;

	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		visual_audio_samplepool_channel_flush_old (channel);
	}

	return VISUAL_OK;
}

int visual_audio_samplepool_input (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleRateType rate,
		VisAudioSampleFormatType format,
		VisAudioSampleChannelType channeltype)
{
	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (channeltype == VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO)
		input_interleaved_stereo (samplepool, buffer, format, rate);

	return VISUAL_OK;
}

int visual_audio_samplepool_input_channel (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleRateType rate,
		VisAudioSampleFormatType format,
		char *channelid)
{
	VisAudioSample *sample;
	VisBuffer *pcmbuf;
	VisTime timestamp;

	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	pcmbuf = visual_buffer_new ();
	visual_buffer_clone (pcmbuf, buffer);

	visual_time_get (&timestamp);

	visual_buffer_set_destroyer (pcmbuf, visual_buffer_destroyer_free);

	sample = visual_audio_sample_new (pcmbuf, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, channelid);

	return VISUAL_OK;
}

VisAudioSamplePoolChannel *visual_audio_samplepool_channel_new (char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	channel = visual_mem_new0 (VisAudioSamplePoolChannel, 1);

	visual_audio_samplepool_channel_init (channel, channelid);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (channel), TRUE);
	visual_object_ref (VISUAL_OBJECT (channel));

	return channel;
}

int visual_audio_samplepool_channel_init (VisAudioSamplePoolChannel *channel, char *channelid)
{
	visual_log_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (channel));
	visual_object_set_dtor (VISUAL_OBJECT (channel), audio_samplepool_channel_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (channel), FALSE);

	/* Reset the VisAudioSamplePoolChannel data */
	channel->samples = visual_ringbuffer_new ();

	visual_time_set (&channel->samples_timeout, 1, 0); /* FIXME not safe against time screws */
	channel->channelid = strdup (channelid);
	channel->factor = 1.0;

	return VISUAL_OK;
}

int visual_audio_samplepool_channel_add (VisAudioSamplePoolChannel *channel, VisAudioSample *sample)
{
	visual_log_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	visual_ringbuffer_add_function (channel->samples,
			sample_data_func,
			sample_destroy_func,
			sample_size_func, sample);

	return VISUAL_OK;
}

int visual_audio_samplepool_channel_flush_old (VisAudioSamplePoolChannel *channel)
{
	VisList *list;
	VisListEntry *le = NULL;
	VisRingBufferEntry *rentry;
	VisAudioSample *sample;

	visual_log_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	list = visual_ringbuffer_get_list (channel->samples);

	while ((rentry = visual_list_next (list, &le)) != NULL) {
		VisTime diff;
		VisTime curtime;

		sample = visual_ringbuffer_entry_get_functiondata (rentry);

		visual_time_get (&curtime);

		visual_time_difference (&diff, &sample->timestamp, &curtime);

		if (visual_time_past (&diff, &channel->samples_timeout) == TRUE) {
			visual_list_destroy (list, &le);

			if (le == NULL)
				break;
		}
	}

	return VISUAL_OK;
}

int visual_audio_sample_buffer_mix (VisBuffer *dest, VisBuffer *src, int divide, float multiplier)
{
	float *dbuf;
	float *sbuf;
	int scnt;
	int i;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_log_return_val_if_fail (visual_buffer_get_size (dest) == visual_buffer_get_size (src),
			-VISUAL_ERROR_BUFFER_OUT_OF_BOUNDS);

	dbuf = visual_buffer_get_data (dest);
	sbuf = visual_buffer_get_data (src);

	scnt = visual_buffer_get_size (dest) / sizeof (float);

	/* FIXME make simd version of these */
	if (divide == FALSE) {
		if (multiplier == 1.0) {
			for (i = 0; i < scnt; i++)
				dbuf[i] += sbuf[i];
		} else {
			for (i = 0; i < scnt; i++)
				dbuf[i] += (sbuf[i] * multiplier);
		}
	} else {
		if (multiplier == 1.0) {
			for (i = 0; i < scnt; i++)
				dbuf[i] = (dbuf[i] + sbuf[i]) * 0.5;
		} else {
			for (i = 0; i < scnt; i++)
				dbuf[i] = (dbuf[i] + (sbuf[i] * multiplier)) * 0.5;
		}
	}

	return VISUAL_OK;
}

int visual_audio_sample_buffer_mix_many (VisBuffer *dest, int divide, int channels, ...)
{
	VisBuffer **buffers;
	double *chanmuls;
	va_list ap;
	int i;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);

	buffers = visual_mem_malloc (channels * sizeof (VisBuffer *));
	chanmuls = visual_mem_malloc (channels * sizeof (double));

	va_start (ap, channels);

	/* Retrieving mixing data from valist */
	for (i = 0; i < channels; i++)
		buffers[i] = va_arg (ap, VisBuffer *);

	for (i = 0; i < channels; i++)
		chanmuls[i] = va_arg (ap, double);

	visual_buffer_fill (dest, 0);
	visual_audio_sample_buffer_mix (dest, buffers[0], FALSE, chanmuls[0]);

	/* The mixing loop */
	for (i = 1; i < channels; i++)
		visual_audio_sample_buffer_mix (dest, buffers[0], divide, chanmuls[i]);

	va_end (ap);

	visual_mem_free (buffers);
	visual_mem_free (chanmuls);

	return VISUAL_OK;
}

VisAudioSample *visual_audio_sample_new (VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	VisAudioSample *sample;

	sample = visual_mem_new0 (VisAudioSample, 1);

	visual_audio_sample_init (sample, buffer, timestamp, format, rate);

	/* Do the VisObject initialization */
        visual_object_set_allocated (VISUAL_OBJECT (sample), TRUE);
	visual_object_ref (VISUAL_OBJECT (sample));

	return sample;
}

int visual_audio_sample_init (VisAudioSample *sample, VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (sample));
	visual_object_set_dtor (VISUAL_OBJECT (sample), audio_sample_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (sample), FALSE);

	/* Reset the VisAudioSamplePool structure */
	visual_time_copy (&sample->timestamp, timestamp);
	sample->rate = rate;
	sample->format = format;
	sample->buffer = buffer;
	sample->processed = NULL;

	return VISUAL_OK;
}

int visual_audio_sample_has_internal (VisAudioSample *sample)
{
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	if (sample->processed != NULL)
		return TRUE;

	return FALSE;
}

int visual_audio_sample_transform_format (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleFormatType format)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	if (dest->buffer != NULL)
		visual_object_unref (VISUAL_OBJECT (dest->buffer));

	dest->buffer = visual_buffer_new_allocate (
			visual_audio_sample_rate_get_length (dest->rate) *
			visual_audio_sample_format_get_size (format),
			visual_buffer_destroyer_free);

	dest->format = format;

	if (dest->format == VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT) {

		transform_format_buffer_from_float (dest->buffer, src->buffer,
				visual_audio_sample_format_get_size (src->format),
				visual_audio_sample_format_is_signed (src->format));

	} else if (src->format == VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT) {

		transform_format_buffer_to_float (dest->buffer, src->buffer,
				visual_audio_sample_format_get_size (dest->format),
				visual_audio_sample_format_is_signed (dest->format));

	} else if (dest->format == VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT && src->format ==
			VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT) {

				visual_buffer_put (dest->buffer, src->buffer, 0);
	} else {

		transform_format_buffer (dest->buffer, src->buffer,
				visual_audio_sample_format_get_size (dest->format),
				visual_audio_sample_format_get_size (src->format),
				visual_audio_sample_format_is_signed (dest->format),
				visual_audio_sample_format_is_signed (src->format));
	}

	return VISUAL_OK;
}

int visual_audio_sample_transform_rate (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleRateType rate)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	/* FIXME error on dest format != src format */

	if (dest->buffer != NULL)
		visual_object_unref (VISUAL_OBJECT (dest->buffer));


	dest->buffer = visual_buffer_new_allocate (
			visual_audio_sample_rate_get_length (rate) *
			visual_audio_sample_format_get_size (src->format),
			visual_buffer_destroyer_free);



	return VISUAL_OK;
}

int visual_audio_sample_rate_get_length (VisAudioSampleRateType rate)
{
	static int ratelengthtable[] = {
		[VISUAL_AUDIO_SAMPLE_RATE_NONE]		= 0,
		[VISUAL_AUDIO_SAMPLE_RATE_8000]		= 8000,
		[VISUAL_AUDIO_SAMPLE_RATE_11250]	= 11250,
		[VISUAL_AUDIO_SAMPLE_RATE_22500]	= 22500,
		[VISUAL_AUDIO_SAMPLE_RATE_32000]	= 32000,
		[VISUAL_AUDIO_SAMPLE_RATE_44100]	= 44100,
		[VISUAL_AUDIO_SAMPLE_RATE_48000]	= 48000,
		[VISUAL_AUDIO_SAMPLE_RATE_96000]	= 96000
	};

	if (rate >= VISUAL_AUDIO_SAMPLE_RATE_LAST)
		return -1; /* FIXME decent error, also for format_get_size */

	return ratelengthtable[rate];
}

int visual_audio_sample_format_get_size (VisAudioSampleFormatType format)
{
	static int formatsizetable[] = {
		[VISUAL_AUDIO_SAMPLE_FORMAT_NONE]	= 0,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U8]		= 1,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S8]		= 1,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U16]	= 2,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S16]	= 2,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U32]	= 4,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S32]	= 4,
		[VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT]	= 4
	};

	if (format >= VISUAL_AUDIO_SAMPLE_FORMAT_LAST)
		return -1;

	return formatsizetable[format];
}

int visual_audio_sample_format_is_signed (VisAudioSampleFormatType format)
{
	static int formatsignedtable[] = {
		[VISUAL_AUDIO_SAMPLE_FORMAT_NONE]	= FALSE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U8]		= FALSE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S8]		= TRUE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U16]	= FALSE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S16]	= TRUE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_U32]	= FALSE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_S32]	= TRUE,
		[VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT]	= TRUE
	};

	if (format >= VISUAL_AUDIO_SAMPLE_FORMAT_LAST)
		return -1;

	return formatsignedtable[format];
}

static int byte_max_numeric (int bytes)
{
	int result = 256;
	int i;

	if (bytes == 0)
		return 0;

	for (i = 1; i < bytes; i++)
		result *= 256;

	return result;
}

/* FIXME use lv_math acceleration here! */
#define FORMAT_BUFFER_FROM_FLOAT(a,b)										\
	{													\
		if (sign) {											\
			a *dbuf = visual_buffer_get_data (src);							\
			for (i = 0; i < entries; i++) {								\
				dbuf[i] = sbuf[i] * signedcorr;							\
			}											\
		} else {											\
			b *dbuf = visual_buffer_get_data (src);							\
			for (i = 0; i < entries; i++) {								\
				dbuf[i] = (sbuf[i] * signedcorr) + signedcorr;					\
			}											\
		}												\
	}

static int transform_format_buffer_from_float (VisBuffer *dest, VisBuffer *src, int size, int sign)
{
	float *sbuf = visual_buffer_get_data (src);
	int entries = visual_buffer_get_size (dest) / size;
	int signedcorr;
	int i;

	signedcorr += byte_max_numeric (size) / 2;

	if (size == 1)
		FORMAT_BUFFER_FROM_FLOAT(int8_t, uint8_t)
	else if (size == 2)
		FORMAT_BUFFER_FROM_FLOAT(int16_t, uint16_t)
	else if (size == 4)
		FORMAT_BUFFER_FROM_FLOAT(int32_t, uint32_t)

	return VISUAL_OK;
}

#define FORMAT_BUFFER_TO_FLOAT(a,b)										\
	{													\
		float multiplier = 1.0 / signedcorr;								\
		if (sign) {											\
			a *sbuf = visual_buffer_get_data (src);							\
			for (i = 0; i < entries; i++) {								\
				dbuf[i] = sbuf[i] * multiplier;							\
			}											\
		} else {											\
			b *sbuf = visual_buffer_get_data (src);							\
			for (i = 0; i < entries; i++) {								\
				dbuf[i] = (sbuf[i] - signedcorr) * multiplier;					\
			}											\
		}												\
	}

static int transform_format_buffer_to_float (VisBuffer *dest, VisBuffer *src, int size, int sign)
{
	float *dbuf = visual_buffer_get_data (dest);
	int entries = visual_buffer_get_size (dest) /
		visual_audio_sample_format_get_size (VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT);
	int signedcorr;
	int i;

	signedcorr = byte_max_numeric (size) / 2;

	if (size == 1)
		FORMAT_BUFFER_TO_FLOAT(int8_t, uint8_t)
	else if (size == 2)
		FORMAT_BUFFER_TO_FLOAT(int16_t, uint16_t)
	else if (size == 4)
		FORMAT_BUFFER_TO_FLOAT(int32_t, uint32_t)

	return VISUAL_OK;
}

#define FORMAT_BUFFER(a,b,c,d)											\
		{												\
			if (signedcorr == 0) {									\
				visual_buffer_put (dest, src, 0);						\
			} else {										\
				if (signedcorr < 0) {								\
					a *dbuf = visual_buffer_get_data (dest);				\
					b *sbuf = visual_buffer_get_data (src);					\
					for (i = 0; i < entries; i++) {						\
						dbuf[i] = sbuf[i] + signedcorr;					\
					}									\
				} else {									\
					c *dbuf = visual_buffer_get_data (dest);				\
					d *sbuf = visual_buffer_get_data (src);					\
					for (i = 0; i < entries; i++) {						\
						dbuf[i] = sbuf[i] + signedcorr;					\
					}									\
				}										\
			}											\
		}

#define FORMAT_BUFFER_INCREASE(a,b,c,d)										\
		{												\
			if (signedcorr < 0) {									\
				a *dbuf = visual_buffer_get_data (dest);					\
				b *sbuf = visual_buffer_get_data (src);						\
				for (i = 0; i < entries; i++) {							\
					dbuf[i] = (sbuf[i] << shifter) + signedcorr;				\
				}										\
			} else {										\
				c *dbuf = visual_buffer_get_data (dest);					\
				d *sbuf = visual_buffer_get_data (src);						\
				for (i = 0; i < entries; i++) {							\
					dbuf[i] = (sbuf[i] << shifter) + signedcorr;				\
				}										\
			}											\
		}

#define FORMAT_BUFFER_DECREASE(a,b,c,d)										\
		{												\
			if (signedcorr < 0) {									\
				a *dbuf = visual_buffer_get_data (dest);					\
				b *sbuf = visual_buffer_get_data (src);						\
				for (i = 0; i < entries; i++) {							\
					dbuf[i] = (sbuf[i] >> shifter) + signedcorr;				\
				}										\
			} else {										\
				c *dbuf = visual_buffer_get_data (dest);					\
				d *sbuf = visual_buffer_get_data (src);						\
				for (i = 0; i < entries; i++) {							\
					dbuf[i] = (sbuf[i] >> shifter) + signedcorr;				\
				}										\
			}											\
		}

static int transform_format_buffer (VisBuffer *dest, VisBuffer *src, int dsize, int ssize, int dsigned, int ssigned)
{
	int signedcorr = 0;
	int entries = visual_buffer_get_size (dest) / dsize;
	int shifter = 0;
	int i;

	if (dsigned == TRUE && ssigned == FALSE)
		signedcorr -= byte_max_numeric (ssize) / 2;
	else if (dsigned == FALSE && ssigned == TRUE)
		signedcorr += byte_max_numeric (dsize) / 2;

	if (dsize > ssize)
		shifter = dsize - ssize;
	else if (dsize < ssize)
		shifter = ssize - dsize;

	/* FIXME simd versions of every conversion */
	if (dsize == 1 && ssize == 1)		/* 8 to 8 */
		FORMAT_BUFFER(int8_t, uint8_t, uint8_t, int8_t)
	else if (dsize == 2 && ssize == 1)	/* 8 to 16 */
		FORMAT_BUFFER_INCREASE(int16_t, int8_t, uint16_t, uint8_t)
	else if (dsize == 4 && ssize == 1)	/* 8 to 32 */
		FORMAT_BUFFER_INCREASE(int32_t, int8_t, uint32_t, uint8_t)
	else if (dsize == 2 && ssize == 2)	/* 16 to 16 */
		FORMAT_BUFFER(int16_t, uint16_t, uint16_t, int16_t)
	else if (dsize == 4 && ssize == 2)	/* 32 to 16 */
		FORMAT_BUFFER_INCREASE(int32_t, int16_t, uint32_t, uint16_t)
	else if (dsize == 4 && ssize == 4)	/* 32 to 32 */
		FORMAT_BUFFER(int32_t, uint32_t, uint32_t, int32_t)
	else if (dsize == 1 && ssize == 2)	/* 16 to 8 */
		FORMAT_BUFFER_DECREASE(int8_t, int16_t, uint8_t, uint16_t)
	else if (dsize == 2 && ssize == 4)	/* 32 to 16 */
		FORMAT_BUFFER_DECREASE(int16_t, int32_t, uint16_t, uint32_t)
	else if (dsize == 1 && ssize == 4)	/* 32 to 8 */
		FORMAT_BUFFER_DECREASE(int8_t, int32_t, uint8_t, uint32_t)

	return VISUAL_OK;
}

static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	/* We have internal format ready */
	if (sample->processed != NULL) {
		visual_object_ref (VISUAL_OBJECT (sample->processed));

		return sample->processed;
	}

	sample->processed = visual_buffer_new_allocate (
			(visual_buffer_get_size (sample->buffer) /
			 visual_audio_sample_format_get_size (sample->format)) * sizeof (float),
			visual_buffer_destroyer_free);

	transform_format_buffer_to_float (sample->processed, sample->buffer,
			visual_audio_sample_format_get_size (sample->format),
			visual_audio_sample_format_is_signed (sample->format));

	visual_object_ref (VISUAL_OBJECT (sample->processed));

	return sample->processed;
}

static void sample_destroy_func (VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	visual_object_unref (VISUAL_OBJECT (sample));
}

static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	return (visual_buffer_get_size (sample->buffer) /
		visual_audio_sample_format_get_size (sample->format)) * sizeof (float);
}

/*  functions */
#define STEREO_INTERLEAVED(x)											\
		{												\
			chan1 = visual_buffer_new_allocate (sizeof (x) * (visual_buffer_get_size (buffer) / 2),	\
				visual_buffer_destroyer_free);							\
			chan2 = visual_buffer_new_allocate (sizeof (x) * (visual_buffer_get_size (buffer) / 2),	\
					visual_buffer_destroyer_free);						\
			x *pcm = visual_buffer_get_data (buffer);						\
			x *chan1buf = visual_buffer_get_data (chan1);						\
			x *chan2buf = visual_buffer_get_data (chan2);						\
			for (i = 0; i < visual_buffer_get_size (buffer); i += 2) {				\
				chan1buf[i >> 1] = pcm[i];							\
				chan2buf[i >> 1] = pcm[i + 1];							\
			}											\
		}

static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	VisBuffer *chan1 = NULL;
	VisBuffer *chan2 = NULL;
	VisAudioSample *sample;
	VisTime timestamp;
	int i;

	visual_time_get (&timestamp);

	if (format == VISUAL_AUDIO_SAMPLE_FORMAT_U8)
		STEREO_INTERLEAVED(uint8_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_S8)
		STEREO_INTERLEAVED(int8_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_U16)
		STEREO_INTERLEAVED(uint16_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_S16)
		STEREO_INTERLEAVED(int16_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_U32)
		STEREO_INTERLEAVED(uint32_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_S32)
		STEREO_INTERLEAVED(int32_t)
	else if (format == VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT)
		STEREO_INTERLEAVED(float)
	else
		return -1;

	visual_log_return_val_if_fail (chan1 != NULL, -1);
	visual_log_return_val_if_fail (chan2 != NULL, -1);

	visual_buffer_set_destroyer (chan1, visual_buffer_destroyer_free);
	visual_buffer_set_destroyer (chan2, visual_buffer_destroyer_free);

	sample = visual_audio_sample_new (chan1, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_LEFT);

	sample = visual_audio_sample_new (chan2, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_RIGHT);

	return VISUAL_OK;
}

/**
 * @}
 */

