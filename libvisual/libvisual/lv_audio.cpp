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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <vector>
#include <iostream>

static void audio_samplepool_dtor (VisObject *object);
static void audio_samplepool_channel_dtor (VisObject *object);
static void audio_sample_dtor (VisObject *object);

/* Ringbuffer data provider functions */
static VisBuffer *sample_data_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);
static void sample_destroy_func (VisRingBufferEntry *entry);
static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);

/*	functions */
static int input_interleaved_stereo (VisAudioSamplePool *samplepool,
                                     VisBuffer *buffer,
                                     VisAudioSampleFormatType format,
                                     VisAudioSampleRateType rate);

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
	VisAudio *audio = visual_mem_new0 (VisAudio, 1);
	audio->samplepool = visual_audio_samplepool_new ();

	return audio;
}

void visual_audio_free (VisAudio* audio)
{
	if (audio->samplepool)
		visual_object_unref (VISUAL_OBJECT (audio->samplepool));
}

int visual_audio_get_sample (VisAudio *audio, VisBuffer *buffer, const char *channelid)
{
	visual_return_val_if_fail (audio != NULL, false);
	visual_return_val_if_fail (buffer != NULL, false);
	visual_return_val_if_fail (channelid != NULL, false);

	VisAudioSamplePoolChannel *channel = visual_audio_samplepool_get_channel (audio->samplepool, channelid);

	if (!channel) {
		buffer->fill (0);
		return false;
	}

	visual_ringbuffer_get_data_from_end (channel->samples, buffer, buffer->get_size ());

	return true;
}

// FIXME: too much code duplication with get_sample_mixed
void visual_audio_get_sample_mixed_simple (VisAudio *audio, VisBuffer *buffer, unsigned int channels, ...)
{
	visual_return_if_fail (audio  != NULL);
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (channels > 0);

	std::vector<std::string> chanids (channels);

	va_list ap;

	va_start (ap, channels);

	for (unsigned int i = 0; i < channels; i++)
		chanids[i] = va_arg (ap, char *);

	va_end (ap);

	LV::Buffer temp (buffer->get_size ());
	buffer->fill (0);

	bool first = true;

	// The mixing loop
	for (unsigned int i = 0; i < channels; i++) {
		if (visual_audio_get_sample (audio, &temp, chanids[i].c_str ())) {

			VisAudioSamplePoolChannel* channel = visual_audio_samplepool_get_channel (audio->samplepool, chanids[i].c_str ());

			if (first) {
				visual_audio_sample_buffer_mix (buffer, &temp, false, channel->factor);
				first = false;
			} else {
				visual_audio_sample_buffer_mix (buffer, &temp, true, channel->factor);
			}
		}
	}
}

void visual_audio_get_sample_mixed (VisAudio *audio, VisBuffer *buffer, int divide, unsigned int channels, ...)
{
	visual_return_if_fail (audio  != NULL);
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (channels > 0);

	visual_log (VISUAL_LOG_INFO, "Channels: %u", channels);

	std::vector<std::string> chanids  (channels);
	std::vector<double>		 chanmuls (channels);

	va_list ap;

	va_start (ap, channels);

	for (unsigned int i = 0; i < channels; i++) {
		chanids[i] = va_arg (ap, char *);
	}

	for (unsigned int i = 0; i < channels; i++)
		chanmuls[i] = va_arg (ap, double);

	va_end (ap);

	LV::Buffer temp (buffer->get_size ());

	buffer->fill (0);

	bool first = true;

	// The mixing loop
	for (unsigned int i = 0; i < channels; i++) {
		if (visual_audio_get_sample (audio, &temp, chanids[i].c_str ())) {
			if (first) {
				visual_audio_sample_buffer_mix (buffer, &temp, false, chanmuls[i]);
				first = false;
			} else {
				visual_audio_sample_buffer_mix (buffer, &temp, divide, chanmuls[i]);
			}
		}
	}
}

void visual_audio_get_spectrum (VisAudio *audio, VisBuffer *buffer, visual_size_t samplelen, const char *channelid, int normalised)
{
	visual_return_if_fail (audio != NULL);
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (channelid != NULL);

	LV::Buffer sample (samplelen);

	if (visual_audio_get_sample (audio, &sample, channelid))
		visual_audio_get_spectrum_for_sample (buffer, &sample, normalised);
	else
		buffer->fill (0);
}

void visual_audio_get_spectrum_multiplied (VisAudio *audio, VisBuffer *buffer, visual_size_t samplelen, const char *channelid, int normalised, float multiplier)
{
	visual_return_if_fail (audio != NULL);
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (channelid != NULL);

	visual_audio_get_spectrum (audio, buffer, samplelen, channelid, normalised);

	float* data = static_cast<float*> (buffer->get_data ());
	std::size_t datasize = buffer->get_size () / sizeof (float);

	visual_math_simd_mul_floats_float (data, data, datasize, multiplier);
}

void visual_audio_get_spectrum_for_sample (VisBuffer *buffer, VisBuffer *sample, int normalised)
{
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (sample != NULL);

	LV::DFT dft (buffer->get_size () / sizeof (float),
				 sample->get_size () / sizeof (float));

	// Fourier analyze the pcm data
	dft.perform (static_cast<float*> (buffer->get_data ()),
				 static_cast<float*> (sample->get_data ()));

	if (normalised)
		visual_audio_normalise_spectrum (buffer);
}

void visual_audio_get_spectrum_for_sample_multiplied (VisBuffer *buffer, VisBuffer *sample, int normalised, float multiplier)
{
	visual_return_if_fail (buffer != NULL);
	visual_return_if_fail (sample != NULL);

	visual_audio_get_spectrum_for_sample (buffer, sample, normalised);

	float* data = static_cast<float*> (buffer->get_data ());
	std::size_t datasize = buffer->get_size () / sizeof (float);

	visual_math_simd_mul_floats_float (data, data, datasize, multiplier);
}

void visual_audio_normalise_spectrum (VisBuffer *buffer)
{
	visual_return_if_fail (buffer != NULL);

	LV::DFT::log_scale_standard (static_cast<float*> (buffer->get_data ()),
								 static_cast<float*> (buffer->get_data ()),
								 buffer->get_size () / sizeof (float));
}

VisAudioSamplePool *visual_audio_samplepool_new ()
{
	VisAudioSamplePool *samplepool;

	samplepool = visual_mem_new0 (VisAudioSamplePool, 1);

	visual_object_init (VISUAL_OBJECT (samplepool), audio_samplepool_dtor);

	// Reset the VisAudioSamplePool structure
	samplepool->channels = visual_list_new (visual_object_collection_destroyer);

	return samplepool;
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
	visual_return_val_if_fail (samplepool != NULL, NULL);
	visual_return_val_if_fail (channelid != NULL, NULL);

	VisListEntry *le = NULL;
	VisAudioSamplePoolChannel *channel;

	while ((channel = static_cast<VisAudioSamplePoolChannel*> (visual_list_next (samplepool->channels, &le))) != NULL) {
		if (std::strcmp (channel->channelid, channelid) == 0)
			return channel;
	}

	return NULL;
}

void visual_audio_samplepool_flush_old (VisAudioSamplePool *samplepool)
{
	visual_return_if_fail (samplepool != NULL);

	VisListEntry *le = NULL;
	VisAudioSamplePoolChannel *channel;

	while ((channel = static_cast<VisAudioSamplePoolChannel*> (visual_list_next (samplepool->channels, &le))) != NULL) {
		visual_audio_samplepool_channel_flush_old (channel);
	}
}

void visual_audio_samplepool_input (VisAudioSamplePool *samplepool,
                                    VisBuffer *buffer,
                                    VisAudioSampleRateType rate,
                                    VisAudioSampleFormatType format,
                                    VisAudioSampleChannelType channeltype)
{
	visual_return_if_fail (samplepool != NULL);
	visual_return_if_fail (buffer != NULL);

	if (channeltype == VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO)
		input_interleaved_stereo (samplepool, buffer, format, rate);
}

void visual_audio_samplepool_input_channel (VisAudioSamplePool *samplepool, VisBuffer *buffer,
                                            VisAudioSampleRateType rate,
                                            VisAudioSampleFormatType format,
                                            const char *channelid)
{
	visual_return_if_fail (samplepool != NULL);
	visual_return_if_fail (buffer != NULL);

	VisBuffer *pcmbuf = visual_buffer_clone (buffer);

	LV::Time timestamp = LV::Time::now ();

	VisAudioSample *sample = visual_audio_sample_new (pcmbuf, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, channelid);
}

VisAudioSamplePoolChannel *visual_audio_samplepool_channel_new (const char *channelid)
{
	VisAudioSamplePoolChannel *channel;

	channel = visual_mem_new0 (VisAudioSamplePoolChannel, 1);

	// Do the VisObject initialization
	visual_object_init (VISUAL_OBJECT (channel), audio_samplepool_channel_dtor);

	/* Reset the VisAudioSamplePoolChannel data */
	channel->samples = visual_ringbuffer_new ();
	channel->samples_timeout = visual_time_new_with_values (1, 0); /* FIXME not safe against time skews */

	channel->channelid = visual_strdup (channelid);
	channel->factor = 1.0;

	return channel;
}

void visual_audio_samplepool_channel_add (VisAudioSamplePoolChannel *channel, VisAudioSample *sample)
{
	visual_return_if_fail (channel != NULL);
	visual_return_if_fail (sample != NULL);

	visual_ringbuffer_add_function (channel->samples,
			sample_data_func,
			sample_destroy_func,
			sample_size_func, sample);
}

void visual_audio_samplepool_channel_flush_old (VisAudioSamplePoolChannel *channel)
{
	visual_return_if_fail (channel != NULL);

	VisList* list = visual_ringbuffer_get_list (channel->samples);

	VisListEntry *le = NULL;
	VisRingBufferEntry *rentry;

	while ((rentry = static_cast<VisRingBufferEntry*> (visual_list_next (list, &le))) != NULL) {

		VisAudioSample *sample = static_cast<VisAudioSample*> (visual_ringbuffer_entry_get_functiondata (rentry));

		LV::Time diff = LV::Time::now () - *sample->timestamp;

		if (diff > *channel->samples_timeout) {
			visual_list_destroy (list, &le);

			if (le == NULL) {
				break;
			}
		}
	}
}

void visual_audio_sample_buffer_mix (VisBuffer *dest, VisBuffer *src, int divide, float multiplier)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src != NULL);
	visual_return_if_fail (dest->get_size () == src->get_size ());

	float* dbuf = static_cast<float*> (dest->get_data ());
	float const* sbuf = static_cast<float const*> (src->get_data ());

	std::size_t scnt = visual_buffer_get_size (dest) / sizeof (float);

	/* FIXME make simd version of these */
	if (!divide) {
		if (multiplier == 1.0) {
			for (unsigned int i = 0; i < scnt; i++)
				dbuf[i] += sbuf[i];
		} else {
			for (unsigned int i = 0; i < scnt; i++)
				dbuf[i] += (sbuf[i] * multiplier);
		}
	} else {
		if (multiplier == 1.0) {
			for (unsigned int i = 0; i < scnt; i++)
				dbuf[i] = (dbuf[i] + sbuf[i]) * 0.5;
		} else {
			for (unsigned int i = 0; i < scnt; i++)
				dbuf[i] = (dbuf[i] + (sbuf[i] * multiplier)) * 0.5;
		}
	}
}

void visual_audio_sample_buffer_mix_many (VisBuffer *dest, int divide, unsigned int channels, ...)
{
	visual_return_if_fail (dest != NULL);

	std::vector<LV::Buffer*> buffers  (channels);
	std::vector<double>		 chanmuls (channels);

	va_list ap;

	va_start (ap, channels);

	/* Retrieving mixing data from valist */
	for (unsigned int i = 0; i < channels; i++)
		buffers.push_back (va_arg (ap, VisBuffer *));

	for (unsigned int i = 0; i < channels; i++)
		chanmuls.push_back (va_arg (ap, double));

	va_end (ap);

	dest->fill (0);
	visual_audio_sample_buffer_mix (dest, buffers[0], false, chanmuls[0]);

	/* The mixing loop */
	for (unsigned int i = 1; i < channels; i++)
		visual_audio_sample_buffer_mix (dest, buffers[0], divide, chanmuls[i]);
}

VisAudioSample *visual_audio_sample_new (VisBuffer *buffer,
										 VisTime *timestamp,
										 VisAudioSampleFormatType format,
										 VisAudioSampleRateType rate)
{
	VisAudioSample *sample = visual_mem_new0 (VisAudioSample, 1);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (sample), audio_sample_dtor);

	/* Reset the VisAudioSamplePool structure */
	sample->timestamp = visual_time_clone (timestamp);
	sample->rate = rate;
	sample->format = format;
	sample->buffer = buffer;
	sample->processed = NULL;

	return sample;
}

int visual_audio_sample_has_internal (VisAudioSample *sample)
{
	visual_return_val_if_fail (sample != NULL, -VISUAL_ERROR_AUDIO_SAMPLE_NULL);

	if (sample->processed)
		return true;

	return false;
}

void visual_audio_sample_transform_format (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleFormatType format)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src	!= NULL);

	if (dest->buffer)
		visual_buffer_free (dest->buffer);

	dest->buffer = new LV::Buffer (
			visual_audio_sample_rate_get_length (dest->rate) *
			visual_audio_sample_format_get_size (format));
	dest->format = format;

	visual_audio_sample_convert (dest->buffer, dest->format, src->buffer, src->format);
}

void visual_audio_sample_transform_rate (VisAudioSample *dest, VisAudioSample *src, VisAudioSampleRateType rate)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src != NULL);

	if (dest->buffer)
		visual_buffer_free (dest->buffer);

	dest->buffer = new LV::Buffer (
			visual_audio_sample_rate_get_length (rate) *
			visual_audio_sample_format_get_size (src->format));
}

visual_size_t visual_audio_sample_rate_get_length (VisAudioSampleRateType rate)
{
	static visual_size_t ratelengthtable[] = {
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

visual_size_t visual_audio_sample_format_get_size (VisAudioSampleFormatType format)
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
	VisAudioSample *sample = static_cast<VisAudioSample*> (entry->functiondata);

	/* We have internal format ready */
	if (sample->processed) {
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
	VisAudioSample *sample = static_cast<VisAudioSample*> (entry->functiondata);

	visual_object_unref (VISUAL_OBJECT (sample));
}

static int sample_size_func (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	VisAudioSample *sample = static_cast<VisAudioSample*> (entry->functiondata);

	return (sample->buffer->get_size () /
		visual_audio_sample_format_get_size (sample->format)) * sizeof (float);
}

static int input_interleaved_stereo (VisAudioSamplePool *samplepool, VisBuffer *buffer,
		VisAudioSampleFormatType format,
		VisAudioSampleRateType rate)
{
	std::size_t sample_size = visual_audio_sample_format_get_size (format);

	LV::Buffer *chan1 = new LV::Buffer (sample_size * buffer->get_size () / 2);
	LV::Buffer *chan2 = new LV::Buffer (sample_size * buffer->get_size () / 2);

	visual_audio_sample_deinterleave_stereo (chan1, chan2, buffer, format);

	LV::Time timestamp = LV::Time::now ();

	VisAudioSample *sample;

	sample = visual_audio_sample_new (chan1, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_LEFT);

	sample = visual_audio_sample_new (chan2, &timestamp, format, rate);
	visual_audio_samplepool_add (samplepool, sample, VISUAL_AUDIO_CHANNEL_RIGHT);

	return VISUAL_OK;
}
