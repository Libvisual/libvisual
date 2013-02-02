#include "test.h"
#include <libvisual/libvisual.h>
#include <vector>
#include <cstring>
#include <cstdint>
#include <limits>

const unsigned int sample_count = 256;

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);

    // Fill input

    auto input_buffer = LV::Buffer::create (sample_count * 2 * sizeof (int16_t));
    auto input_data = static_cast<int16_t*> (input_buffer->get_data ());

    for (unsigned int i = 0; i < sample_count*2; i++) {
        input_data[i] = i;
    }

    // Upload

    LV::Audio audio;
    audio.input (input_buffer, VISUAL_AUDIO_SAMPLE_RATE_44100, VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    // Check correctness of stereo deinterleaving and format conversion

    auto output_buffer = LV::Buffer::create (sample_count * sizeof (float));
    auto output_data = static_cast<float*> (output_buffer->get_data ());

    auto int_max = std::numeric_limits<int16_t>::max () + 1;

    audio.get_sample (output_buffer, VISUAL_AUDIO_CHANNEL_LEFT);
    for (unsigned int i = 0; i < sample_count; i++) {
        LV_TEST_ASSERT (output_data[i] == float (i*2) / int_max);
    }

    audio.get_sample (output_buffer, VISUAL_AUDIO_CHANNEL_RIGHT);
    for (unsigned int i = 0; i < sample_count; i++) {
        LV_TEST_ASSERT (output_data[i] == float (i*2+1) / int_max);
    }

    // Check correctness of stereo-to-mono downmixing

    audio.get_sample_mixed_simple (output_buffer, 2, VISUAL_AUDIO_CHANNEL_LEFT, VISUAL_AUDIO_CHANNEL_RIGHT);
    for (unsigned int i = 0; i < sample_count; i++) {
        LV_TEST_ASSERT (output_data[i] == float (i*2+0.5) / int_max);
    }

    LV::System::destroy ();

    return EXIT_SUCCESS;
}
