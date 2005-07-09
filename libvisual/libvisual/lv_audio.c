/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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
#include <string.h>
#include <math.h>

#include "lv_common.h"
#include "lv_audio.h"

static int audio_dtor (VisObject *object);
static int audio_samplepool_dtor (VisObject *object);
static int audio_samplepool_channel_dtor (VisObject *object);
static int audio_sample_dtor (VisObject *object);

static int audio_band_total (VisAudio *audio, int begin, int end);
static int audio_band_energy (VisAudio *audio, int band, int length);

/* Ringbuffer data provider functions */
static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);
static void sample_destroy_func (VisRingBufferEntry *entry);
static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);

/* Input functions */
static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleInputFormatType formattype,
		VisAudioSampleInputFreqType freqtype);


static int audio_dtor (VisObject *object)
{
	VisAudio *audio = VISUAL_AUDIO (object);

	if (audio->fft != NULL)
		visual_object_unref (VISUAL_OBJECT (audio->fft));

	if (audio->samplepool != NULL)
		visual_object_unref (VISUAL_OBJECT (audio->samplepool));

	audio->fft = NULL;
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

	for (i = begin; i < end; i++)
		bpmtotal += audio->freq[2][i];

	if (bpmtotal > 0)
		return bpmtotal / (end - begin);
	else
		return 0;
}

static int audio_band_energy (VisAudio *audio, int band, int length)
{
	int energytotal = 0;
	int i;

	for (i = 0; i < length; i++)
		energytotal += audio->bpmhistory[i][band];

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
	audio->fft = NULL;
	audio->samplepool = visual_audio_samplepool_new ();

	return VISUAL_OK;
}

/**
 * This function analyzes the VisAudio, the FFT frequency magic gets done here, also
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
	double scale;
	int i, j, y;

	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);

	/* Load the pcm data */
	for (i = 0; i < 512; i++) {
		audio->pcm[0][i] = audio->plugpcm[0][i];
		audio->pcm[1][i] = audio->plugpcm[1][i];
		audio->pcm[2][i] = (audio->plugpcm[0][i] + audio->plugpcm[1][i]) >> 1;
	}

	/* -------------------------------- audio pool testing */
	{
		VisBuffer *buffer = visual_buffer_new_allocate (sizeof (int16_t) * 512, visual_buffer_destroyer_free);
		VisTime timestamp;
		VisAudioSample *sample;
		VisAudioSamplePoolChannel *channel;

		visual_mem_copy (visual_buffer_get_data (buffer), audio->plugpcm[2], sizeof (int16_t) * 512);

		visual_time_get (&timestamp);

		sample = visual_audio_sample_new (buffer, &timestamp,
				VISUAL_AUDIO_SAMPLE_INPUT_FREQ_44100,
				VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_S8);

		visual_audio_samplepool_add (audio->samplepool, sample, "front");

		channel = visual_audio_samplepool_get_channel (audio->samplepool, "front");

		visual_audio_samplepool_flush_old (audio->samplepool);

		printf ("--channel debug %s: %d\n", channel->channelid, visual_ringbuffer_get_size (channel->samples));
	}
	/* /-------------------------------- audio pool testing */

	/* Initialize fft if not yet initialized */
	if (audio->fft == NULL)
		audio->fft = visual_fft_new (512, 256);

	/* Convert int16_t audio to float audio, (won't be needed when the rest of the new audio
	 * core lands). */
	for (i = 0; i < 512; i++) {
		temp_audio[0][i] = audio->pcm[0][i];
		temp_audio[1][i] = audio->pcm[1][i];
	}

	/* FFT analyze the pcm data */
	visual_fft_perform (audio->fft, temp_audio[0], temp_out);

	for (i = 0; i < 256; i++)
		audio->freq[0][i] = temp_out[i] * 50;

	visual_fft_perform (audio->fft, temp_audio[1], temp_out);

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
	samplepool->channels = visual_list_new (visual_object_list_destroyer);

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
		VisAudioSampleInputFreqType freqtype,
		VisAudioSampleInputFormatType formattype,
		VisAudioSampleInputChannelType channeltype)
{
	visual_log_return_val_if_fail (samplepool != NULL, -VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL);
	visual_log_return_val_if_fail (buffer!= NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (channeltype == VISUAL_AUDIO_SAMPLE_INPUT_CHANNEL_STEREO)
		input_interleaved_stereo (samplepool, buffer, formattype, freqtype);

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
	visual_time_set (&channel->samples_timeout, 1, 0); /* FIXME not save against time screws */
	channel->channelid = strdup (channelid);

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

		/* FIXME: check if we have enough data */
/*
		printf ("Jow %d %d | %d %d | %d %d\n",
				curtime.tv_sec, curtime.tv_usec,
				sample->timestamp.tv_sec, sample->timestamp.tv_usec,
				diff.tv_sec, diff.tv_usec);
*/
		/* add buffer time length to the time, so we always have N amount of seconds of audio. */

		if (visual_time_past (&diff, &channel->samples_timeout) == TRUE)
			visual_list_destroy (list, &le);
	}

	return VISUAL_OK;
}

VisAudioSample *visual_audio_sample_new (VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleInputFormatType formattype,
		VisAudioSampleInputFreqType freqtype)
{
	VisAudioSample *sample;

	sample = visual_mem_new0 (VisAudioSample, 1);

	visual_audio_sample_init (sample, buffer, timestamp, formattype, freqtype);

	/* Do the VisObject initialization */
        visual_object_set_allocated (VISUAL_OBJECT (sample), TRUE);
	visual_object_ref (VISUAL_OBJECT (sample));

	return sample;
}

int visual_audio_sample_init (VisAudioSample *sample, VisBuffer *buffer, VisTime *timestamp,
		VisAudioSampleInputFormatType formattype,
		VisAudioSampleInputFreqType freqtype)
{
	visual_log_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (sample));
	visual_object_set_dtor (VISUAL_OBJECT (sample), audio_sample_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (sample), FALSE);

	/* Reset the VisAudioSamplePool structure */
	visual_time_copy (&sample->timestamp, timestamp);
	sample->freq = freqtype;
	sample->format = formattype;
	sample->buffer = buffer;
	sample->processed = NULL;

	return VISUAL_OK;
}

static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	/* FIXME transform to end format here, and cache this */

	visual_object_ref (VISUAL_OBJECT (sample->buffer));

	return sample->buffer;
}

static void sample_destroy_func (VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	visual_object_unref (VISUAL_OBJECT (sample));
}

static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = entry->functiondata;

	/* FIXME return the size without the buffer */

	return visual_buffer_get_size (sample->buffer);
}

/* Input functions */
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
		VisAudioSampleInputFormatType formattype,
		VisAudioSampleInputFreqType freqtype)
{
	VisBuffer *chan1 = NULL;
	VisBuffer *chan2 = NULL;
	VisAudioSample *sample;
	VisTime timestamp;
	int i;

	if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_U8)
		STEREO_INTERLEAVED(uint8_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_S8)
		STEREO_INTERLEAVED(int8_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_U16)
		STEREO_INTERLEAVED(uint16_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_S16)
		STEREO_INTERLEAVED(int16_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_U32)
		STEREO_INTERLEAVED(uint32_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_S32)
		STEREO_INTERLEAVED(int32_t)
	else if (formattype == VISUAL_AUDIO_SAMPLE_INPUT_FORMAT_FLOAT)
		STEREO_INTERLEAVED(float)
	else
		return -1;

	visual_log_return_val_if_fail (chan1 != NULL, -1);
	visual_log_return_val_if_fail (chan2 != NULL, -1);

	visual_time_get (&timestamp);

	sample = visual_audio_sample_new (chan1, &timestamp, formattype, freqtype);
	visual_audio_samplepool_add (samplepool, sample, "front left 1");

	sample = visual_audio_sample_new (chan2, &timestamp, formattype, freqtype);
	visual_audio_samplepool_add (samplepool, sample, "front right 1");

	return VISUAL_OK;
}

/**
 * @}
 */

