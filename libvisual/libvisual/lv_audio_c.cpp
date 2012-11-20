#include "config.h"
#include "lv_audio.h"
#include "lv_common.h"
#include <cstdarg>

VisAudio *visual_audio_new (void)
{
    return new LV::Audio;
}

void visual_audio_free (VisAudio *self)
{
    delete self;
}

void visual_audio_input (VisAudio                  *self,
                         VisBuffer                 *buffer,
                         VisAudioSampleRateType     rate,
                         VisAudioSampleFormatType   format,
                         VisAudioSampleChannelType  channeltype)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    self->input (LV::BufferPtr (buffer), rate, format, channeltype);
}

void visual_audio_input_channel (VisAudio                 *self,
                                 VisBuffer                *buffer,
                                 VisAudioSampleRateType    rate,
                                 VisAudioSampleFormatType  format,
                                 const char               *channelid)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    self->input (LV::BufferPtr (buffer), rate, format, channelid);
}

int visual_audio_get_sample (VisAudio *self, VisBuffer *buffer, const char *channel_name)
{
    visual_return_val_if_fail (self   != nullptr, FALSE);
    visual_return_val_if_fail (buffer != nullptr, FALSE);

    return self->get_sample (LV::BufferPtr (buffer), channel_name);
}

void visual_audio_get_sample_mixed_simple (VisAudio *self, VisBuffer *buffer, unsigned int channels, ...)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    va_list args;

    va_start (args, channels);
    self->get_sample_mixed_simple (LV::BufferPtr (buffer), channels, args);
    va_end (args);
}

void visual_audio_get_sample_mixed (VisAudio *self, VisBuffer *buffer, int divide, unsigned int channels, ...)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    va_list args;

    va_start (args, channels);
    self->get_sample_mixed (LV::BufferPtr (buffer), divide, channels, args);
    va_end (args);
}

void visual_audio_get_spectrum (VisAudio *self, VisBuffer *buffer, int samplelen, const char *channel_name, int normalised)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    self->get_spectrum (LV::BufferPtr (buffer), samplelen, channel_name, normalised);
}

void visual_audio_get_spectrum_multiplied (VisAudio *self, VisBuffer *buffer, int samplelen, const char *channel_name, int normalised, float multiplier)
{
    visual_return_if_fail (self   != nullptr);
    visual_return_if_fail (buffer != nullptr);

    self->get_spectrum (LV::BufferPtr (buffer), samplelen, channel_name, normalised, multiplier);
}

void visual_audio_get_spectrum_for_sample (VisBuffer *buffer, VisBuffer *sample, int normalised)
{
    visual_return_if_fail (buffer != nullptr);
    visual_return_if_fail (sample != nullptr);

    LV::Audio::get_spectrum_for_sample (LV::BufferPtr (buffer), LV::BufferPtr (sample), normalised);
}

void visual_audio_get_spectrum_for_sample_multiplied (VisBuffer *buffer, VisBuffer *sample, int normalised, float multiplier)
{
    visual_return_if_fail (buffer != nullptr);
    visual_return_if_fail (sample != nullptr);

    LV::Audio::get_spectrum_for_sample (LV::BufferPtr (buffer), LV::BufferPtr (sample), normalised, multiplier);
}

void visual_audio_normalise_spectrum (VisBuffer *buffer)
{
    visual_return_if_fail (buffer != nullptr);

    LV::Audio::normalise_spectrum (LV::BufferPtr (buffer));
}
