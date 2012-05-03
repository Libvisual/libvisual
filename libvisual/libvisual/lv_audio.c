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

#include "config.h"
#include "lv_audio.h"
#include "lv_common.h"
#include "lv_fourier.h"
#include "lv_math.h"
#include "lv_util.h"
#include "private/lv_audio_convert.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

static void audio_dtor (VisObject *object);
static void audio_samplepool_dtor (VisObject *object);
static void audio_samplepool_channel_dtor (VisObject *object);
static void audio_sample_dtor (VisObject *object);

/* Ringbuffer data provider functions */
static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);
static void sample_destroy_func (VisRingBufferEntry *entry);
static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);

/*  functions */
static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate);


static void audio_dtor (VisObject *object)
{
	VisAudio *audio = VISUAL_AUDIO (object);

	if (audio->samplepool)
		visual_object_unref (VISUAL_OBJECT (audio->samplepool));
}

static void audio_samplepool_dtor (VisObject *object)
{
	VisAudioSamplePool *samplepool = VISUAL_AUDIO_SAMPLEPOOL (object);

	if (samplepool->channels)
		visual_object_unref (VISUAL_OBJECT (samplepool->channels));
}

static void audio_samplepool_channel_dtor (VisObject *object)
{
	VisAudioSamplePoolChannel *channel = VISUAL_AUDIO_SAMPLEPOOL_CHANNEL (object);

	visual_time_free (channel->samples_timeout);

	if (channel->samples)
		visual_object_unref (VISUAL_OBJECT (channel->samples));

	if (channel->channelid)
		visual_mem_free (channel->channelid);
}

static void audio_sample_dtor (VisObject *object)
{
	VisAudioSample *sample = VISUAL_AUDIO_SAMPLE (object);

	visual_time_free (sample->timestamp);

	if (sample->buffer)
		visual_buffer_unref (sample->buffer);

	if (sample->processed)
		visual_buffer_unref (sample->processed);
}

VisAudio *visual_audio_new ()
{
	VisAudio *audio;

	audio = visual_mem_new0 (VisAudio, 1);
	visual_audio_init (audio);

	return audio;
}

int visual_audio_init (VisAudio *audio)
{
	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (audio), audio_dtor);

	/* Reset the VisAudio data */
	audio->samplepool = visual_audio_samplepool_new ();

	return VISUAL_OK;
}

int visual_audio_get_sample (VisAudio *audio, VisBuffer *buffer, const char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

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
	VisBuffer *temp;
	char **chanids;
	va_list ap;
	int i;
	int first = TRUE;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	temp = visual_buffer_new_allocate (visual_buffer_get_size (buffer));

	chanids = visual_mem_malloc (channels * sizeof (char *));

	va_start (ap, channels);

	/* Retrieving mixing data from valist */
	for (i = 0; i < channels; i++)
		chanids[i] = va_arg (ap, char *);

	visual_buffer_fill (buffer, 0);

	/* The mixing loop */
	for (i = 0; i < channels; i++) {
		if (visual_audio_get_sample (audio, temp, chanids[i]) == VISUAL_OK) {
			channel = visual_audio_samplepool_get_channel (audio->samplepool, chanids[i]);

			if (first) {
				visual_audio_sample_buffer_mix (buffer, temp, FALSE, channel->factor);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, temp, TRUE, channel->factor);
			}
		}
	}

	va_end (ap);

	visual_buffer_free (temp);

	visual_mem_free (chanids);

	return VISUAL_OK;
}

int visual_audio_get_sample_mixed (VisAudio *audio, VisBuffer *buffer, int divide, int channels, ...)
{
	VisBuffer *temp;
	char **chanids;
	double *chanmuls;
	va_list ap;
	int i;
	int first = TRUE;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	temp = visual_buffer_new_allocate (visual_buffer_get_size (buffer));

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
		if (visual_audio_get_sample (audio, temp, chanids[i]) == VISUAL_OK) {
			if (first) {
				visual_audio_sample_buffer_mix (buffer, temp, FALSE, chanmuls[i]);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, temp, divide, chanmuls[i]);
			}
		}
	}

	va_end (ap);

	visual_buffer_free (temp);

	visual_mem_free (chanids);
	visual_mem_free (chanmuls);

	return VISUAL_OK;
}

// FIXME: This function is entirely broken from the looks of it.
int visual_audio_get_sample_mixed_category (VisAudio *audio, VisBuffer *buffer, const char *category, int divide)
{
	VisListEntry *le = NULL;
	VisAudioSamplePool *samplepool;
	VisAudioSamplePoolChannel *channel;
	VisBuffer *temp;
	int first = TRUE;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_return_val_if_fail (category != NULL, -VISUAL_ERROR_NULL);

	temp = visual_buffer_new_allocate (visual_buffer_get_size (buffer));

    // this function's broken. This line was added to get rid of a compile warning. Not sure what samplepool's supposed
    // to be, because it appears the below code expects an already existing VisAudioSamplePool. Dunno.
	samplepool = visual_audio_samplepool_new ();

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		if (strstr (channel->channelid, category) != NULL) {
			if (visual_audio_get_sample (audio, temp, channel->channelid) == VISUAL_OK) {
				if (first) {
					visual_audio_sample_buffer_mix (buffer, temp, FALSE, 1.0);

					first = FALSE;
				} else {
					visual_audio_sample_buffer_mix (buffer, temp, divide, 1.0);
				}
			}
		}
	}

	visual_buffer_free (temp);

	return VISUAL_OK;
}

// FIXME: This function is entirely broken from the looks of it.
int visual_audio_get_sample_mixed_all (VisAudio *audio, VisBuffer *buffer, int divide)
{
	VisListEntry *le = NULL;
	VisAudioSamplePool *samplepool;
	VisAudioSamplePoolChannel *channel;
	VisBuffer *temp;
	int first = TRUE;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

	temp = visual_buffer_new_allocate (visual_buffer_get_size (buffer));

	samplepool = visual_audio_samplepool_new (); // this function's broken. This line was added to get rid of a compile warning. Not sure what samplepool's supposed to be, because it appears the below code expects an already existing VisAudioSamplePool. Dunno.

	while ((channel = visual_list_next (samplepool->channels, &le)) != NULL) {
		if (visual_audio_get_sample (audio, temp, channel->channelid) == VISUAL_OK) {
			if (first) {
				visual_audio_sample_buffer_mix (buffer, temp, FALSE, 1.0);

				first = FALSE;
			} else {
				visual_audio_sample_buffer_mix (buffer, temp, divide, 1.0);
			}
		}
	}

	visual_buffer_free (temp);

	return VISUAL_OK;
}

int visual_audio_get_spectrum (VisAudio *audio, VisBuffer *buffer, int samplelen, const char *channelid, int normalised)
{
	VisBuffer *sample;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

	sample = visual_buffer_new_allocate (samplelen);

	if (visual_audio_get_sample (audio, sample, channelid) == VISUAL_OK)
		visual_audio_get_spectrum_for_sample (buffer, sample, normalised);
	else
		visual_buffer_fill (buffer, 0);

	visual_buffer_free (sample);

	return VISUAL_OK;
}

int visual_audio_get_spectrum_multiplied (VisAudio *audio, VisBuffer *buffer, int samplelen, const char *channelid, int normalised, float multiplier)
{
	int ret;
	float *data;
	int datasize;

	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_BUFFER_NULL);

	ret = visual_audio_get_spectrum (audio, buffer, samplelen, channelid, normalised);

	data = visual_buffer_get_data (buffer);
	datasize = visual_buffer_get_size (buffer) / sizeof (float);

	visual_math_simd_mul_floats_float (data, data, datasize, multiplier);

	return ret;
}

int visual_audio_get_spectrum_for_sample (VisBuffer *buffer, VisBuffer *sample, int normalised)
{
	VisDFT *dft;

	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_BUFFER_NULL);

	dft = visual_dft_new (visual_buffer_get_size (buffer) / sizeof (float),
			visual_buffer_get_size (sample) / sizeof (float));

	/* Fourier analyze the pcm data */
	visual_dft_perform (dft, visual_buffer_get_data (buffer), visual_buffer_get_data (sample));

	if (normalised)
		visual_audio_normalise_spectrum (buffer);

	visual_dft_free (dft);

	return VISUAL_OK;
}

int visual_audio_get_spectrum_for_sample_multiplied (VisBuffer *buffer, VisBuffer *sample, int normalised, float multiplier)
{
	int ret;
	float *data;
	int datasize;

	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_BUFFER_NULL);

	ret = visual_audio_get_spectrum_for_sample (buffer, sample, normalised);

	data = visual_buffer_get_data (buffer);
	datasize = visual_buffer_get_size (buffer) / sizeof (float);

	visual_math_simd_mul_floats_float (data, data, datasize, multiplier);

	return ret;
}

int visual_audio_normalise_spectrum (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

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

	return samplepool;
}

int visual_audio_samplepool_init (VisAudioSamplePool *samplepool)
{
	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

	visual_object_init (VISUAL_OBJECT (samplepool), audio_samplepool_dtor);

	/* Reset the VisAudioSamplePool structure */
	samplepool->channels = visual_list_new (visual_object_collection_destroyer);

	return VISUAL_OK;
}

int visual_audio_samplepool_add (VisAudioSamplePool *samplepool, VisAudioSample *sample, const char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_return_val_if_fail (channelid != NULL, -VISUAL_ERROR_NULL);

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
	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	visual_list_add (samplepool->channels, channel);

	return VISUAL_OK;
}

VisAudioSamplePoolChannel *visual_audio_samplepool_get_channel (VisAudioSamplePool *samplepool, const char *channelid)
{
	VisListEntry *le = NULL;
	VisAudioSamplePoolChannel *channel;

	visual_return_val_if_fail (samplepool != NULL, NULL);
	visual_return_val_if_fail (channelid != NULL, NULL);

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

	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);

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
	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (channeltype == VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO)
		input_interleaved_stereo (samplepool, buffer, format, rate);

	return VISUAL_OK;
}

int visual_audio_samplepool_input_channel (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleRateType rate,
		VisAudioSampleFormatType format,
		const char *channelid)
{
	VisAudioSample *sample;
	VisBuffer *pcmbuf;
	VisTime *timestamp;

	visual_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	pcmbuf = visual_buffer_clone (buffer);

	timestamp = visual_time_new_now ();

	sample = visual_audio_sample_new (pcmbuf, timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, channelid);

	visual_time_free (timestamp);

	return VISUAL_OK;
}

VisAudioSamplePoolChannel *visual_audio_samplepool_channel_new (const char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	channel = visual_mem_new0 (VisAudioSamplePoolChannel, 1);

	visual_audio_samplepool_channel_init (channel, channelid);

	return channel;
}

int visual_audio_samplepool_channel_init (VisAudioSamplePoolChannel *channel, const char *channelid)
{
	visual_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (channel), audio_samplepool_channel_dtor);

	/* Reset the VisAudioSamplePoolChannel data */
	channel->samples = visual_ringbuffer_new ();

	channel->samples_timeout = visual_time_new_with_values (1, 0); /* FIXME not safe against time skews */

	channel->channelid = visual_strdup (channelid);
	channel->factor = 1.0;

	return VISUAL_OK;
}

int visual_audio_samplepool_channel_add (VisAudioSamplePoolChannel *channel, VisAudioSample *sample)
{
	visual_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

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
	VisTime *diff;
	VisTime *curtime;

	visual_return_val_if_fail (channel != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL);

	curtime = visual_time_new ();
	diff = visual_time_new ();

	list = visual_ringbuffer_get_list (channel->samples);

	while ((rentry = visual_list_next (list, &le)) != NULL) {

		sample = visual_ringbuffer_entry_get_functiondata (rentry);
		visual_time_get_now (curtime);

		visual_time_diff (diff, curtime, sample->timestamp);

		if (visual_time_is_past (diff, channel->samples_timeout)) {
			visual_list_destroy (list, &le);

			if (le == NULL) {
				break;
			}
		}
	}

	visual_time_free (diff);
	visual_time_free (curtime);

	return VISUAL_OK;
}

int visual_audio_sample_buffer_mix (VisBuffer *dest, VisBuffer *src, int divide, float multiplier)
{
	float *dbuf;
	float *sbuf;
	int scnt;
	int i;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (visual_buffer_get_size (dest) == visual_buffer_get_size (src),
			-VISUAL_ERROR_BUFFER_OUT_OF_BOUNDS);

	dbuf = visual_buffer_get_data (dest);
	sbuf = visual_buffer_get_data (src);

	scnt = visual_buffer_get_size (dest) / sizeof (float);

	/* FIXME make simd version of these */
	if (!divide) {
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

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);

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

	return sample;
}

int visual_audio_sample_init (VisAudioSample *sample, VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (sample), audio_sample_dtor);

	/* Reset the VisAudioSamplePool structure */
	sample->timestamp = visual_time_clone (timestamp);
	sample->rate = rate;
	sample->format = format;
	sample->buffer = buffer;
	sample->processed = NULL;

	return VISUAL_OK;
}

int visual_audio_sample_has_internal (VisAudioSample *sample)
{
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	if (sample->processed != NULL)
		return TRUE;

	return FALSE;
}

int visual_audio_sample_transform_format (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleFormatType format)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	if (dest->buffer)
		visual_buffer_free (dest->buffer);

	dest->buffer = visual_buffer_new_allocate (
			visual_audio_sample_rate_get_length (dest->rate) *
			visual_audio_sample_format_get_size (format));
	dest->format = format;

	visual_audio_sample_convert (dest->buffer, dest->format, src->buffer, src->format);

	return VISUAL_OK;
}

int visual_audio_sample_transform_rate (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleRateType rate)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	/* FIXME error on dest format != src format */

	if (dest->buffer)
		visual_buffer_free (dest->buffer);

	dest->buffer = visual_buffer_new_allocate (
			visual_audio_sample_rate_get_length (rate) *
			visual_audio_sample_format_get_size (src->format));

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

static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	/* We have internal format ready */
	if (sample->processed != NULL) {
		visual_buffer_ref (sample->processed);

		return sample->processed;
	}

	sample->processed = visual_buffer_new_allocate (
			(visual_buffer_get_size (sample->buffer) /
			 visual_audio_sample_format_get_size (sample->format)) * sizeof (float));

	visual_audio_sample_convert (sample->processed, VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT,
	                             sample->buffer, sample->format);

	visual_buffer_ref (sample->processed);

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

static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	VisBuffer *chan1 = NULL;
	VisBuffer *chan2 = NULL;
	int sample_size;
	VisAudioSample *sample;
	VisTime *timestamp;

	sample_size = visual_audio_sample_format_get_size (format);

	chan1 = visual_buffer_new_allocate (sample_size * (visual_buffer_get_size (buffer) / 2));
	chan2 = visual_buffer_new_allocate (sample_size * (visual_buffer_get_size (buffer) / 2));

	visual_audio_sample_deinterleave_stereo (chan1, chan2, buffer, format);

	visual_return_val_if_fail (chan1 != NULL, -1);
	visual_return_val_if_fail (chan2 != NULL, -1);

	timestamp = visual_time_new_now ();

	sample = visual_audio_sample_new (chan1, timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_LEFT);

	sample = visual_audio_sample_new (chan2, timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_time_free (timestamp);

	return VISUAL_OK;
}
