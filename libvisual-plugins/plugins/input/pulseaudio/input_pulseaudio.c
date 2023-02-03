/*    This file is part of Xmms2-libvisual.
 *
 *    Xmms2-libvisual is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Xmms2-libvisual is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Xmms2-libvisual.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libvisual/libvisual.h>
#include <pulse/mainloop.h>
#include <pulse/stream.h>
#include <pulse/thread-mainloop.h>
#include <assert.h>
#include <stdatomic.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define SAMPLE_RATE 44100
#define SAMPLE_RATE_TYPE_LV VISUAL_AUDIO_SAMPLE_RATE_44100
#define CHANNELS 2
#define CHANNELS_TYPE_LV VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO
#define SAMPLE_TYPE int16_t
#define SAMPLE_FORMAT_PA PA_SAMPLE_S16LE
#define SAMPLE_FORMAT_LV VISUAL_AUDIO_SAMPLE_FORMAT_S16

#define FRAMES 512
#define CHUNK_SIZE_BYTES (FRAMES * CHANNELS * sizeof(SAMPLE_TYPE))
#define CHUNKS (2 * SAMPLE_RATE / FRAMES + 1)  // i.e. 2+ seconds of audio

pa_sample_spec sample_spec = {
    .format = SAMPLE_FORMAT_PA,
    .rate = SAMPLE_RATE,
    .channels = CHANNELS
};

typedef struct {
    pa_threaded_mainloop * mainloop;
    pa_context * context;
    pa_stream * input_stream;

    SAMPLE_TYPE pcm_data[CHUNKS][FRAMES * CHANNELS];  // ringbuffer of chunks
    atomic_uint_fast64_t chunks_written;
    uint64_t chunk_write_offset_bytes;
    uint64_t chunks_read;
} pulseaudio_priv_t;

static int  inp_pulseaudio_init    (VisPluginData *plugin);
static void inp_pulseaudio_cleanup (VisPluginData *plugin);
static int  inp_pulseaudio_upload  (VisPluginData *plugin, VisAudio *audio);
static void on_input_stream_data   (pa_stream *p, size_t nbytes, void *userdata);

const VisPluginInfo *get_plugin_info( void ) {
    static VisInputPlugin input = {
        .upload = inp_pulseaudio_upload
    };

    static VisPluginInfo info = {
        .type     = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "pulseaudio",
        .name     = "Pulseaudio input plugin",
        .author   = "Scott Sibley <scott@starlon.net>"
                    ", "
                    "Sebastian Pipping <sebastian@pipping.org>",
        .version  = "2.0",
        .about    = "Use input data from pulseaudio",
        .help     = "",
        .license  = VISUAL_PLUGIN_LICENSE_GPL, // v3 or later, see header

        .init     = inp_pulseaudio_init,
        .cleanup  = inp_pulseaudio_cleanup,
        .plugin   = &input
    };

    return &info;
}

static int inp_pulseaudio_init( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv = visual_mem_new0(pulseaudio_priv_t, 1);
    visual_plugin_set_private(plugin, priv);

    priv->mainloop = pa_threaded_mainloop_new();
    visual_return_val_if_fail(priv->mainloop != NULL, FALSE);

    pa_mainloop_api *const mainloop_api = pa_threaded_mainloop_get_api(priv->mainloop);
    visual_return_val_if_fail(mainloop_api != NULL, FALSE);

    priv->context = pa_context_new(mainloop_api, "lv-pulseaudio");
    visual_return_val_if_fail(priv->context != NULL, FALSE);

    const int connect_res =
            pa_context_connect(priv->context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    visual_return_val_if_fail(connect_res == 0, FALSE);

    // NOTE: Starting the main loop prior to pa_context_connect would get us into
    //       this situation:
    //       Assertion 'c->callback' failed at
    //       ../pulseaudio-16.1/src/pulsecore/socket-client.c:126, function
    //       do_call(). Aborting.
    //       https://gitlab.freedesktop.org/pulseaudio/pulseaudio/-/issues/991
    const int mainloop_start_ret = pa_threaded_mainloop_start(priv->mainloop);
    visual_return_val_if_fail(mainloop_start_ret == 0, FALSE);
    while (pa_context_get_state(priv->context) < PA_CONTEXT_READY) {
        visual_usleep(1000);
    }

    priv->input_stream = pa_stream_new(priv->context, "Recording", &sample_spec, NULL);
    visual_return_val_if_fail(priv->input_stream != NULL, FALSE);
    pa_stream_set_read_callback(priv->input_stream, on_input_stream_data, priv);

    const pa_buffer_attr input_buffer_attr = {
        .fragsize = CHUNK_SIZE_BYTES,
        .maxlength = CHUNK_SIZE_BYTES,
    };
    const int input_connect_res = pa_stream_connect_record(
            priv->input_stream, NULL, &input_buffer_attr, PA_STREAM_ADJUST_LATENCY);
    visual_return_val_if_fail(input_connect_res == 0, FALSE);

    return TRUE;
}

static void inp_pulseaudio_cleanup( VisPluginData *plugin )
{
    pulseaudio_priv_t *priv = visual_plugin_get_private(plugin);
    visual_return_if_fail(priv != NULL);

    pa_stream_disconnect(priv->input_stream);
    pa_stream_unref(priv->input_stream);
    pa_context_disconnect(priv->context);
    pa_context_unref(priv->context);
    pa_threaded_mainloop_stop(priv->mainloop);
    pa_threaded_mainloop_free(priv->mainloop);

    visual_mem_free (priv);
}

static int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio )
{
    pulseaudio_priv_t *priv = visual_plugin_get_private(plugin);
    visual_return_val_if_fail(priv != NULL, FALSE);

    // `priv->chunks_written` is monotonically increasing in another thread in parallel,
    // so we make a snapshot to work with a single consistent value below.
    // Also, plain reads to 64bit are not atomic on 32bit platforms, so we add protection.
    // This is attomic `priv->chunks_written = frozen_chunks_written`.
    uint64_t frozen_chunks_written = atomic_load_explicit(&priv->chunks_written, memory_order_acquire);

    assert(priv->chunks_read <= frozen_chunks_written);
    if (priv->chunks_read == frozen_chunks_written) {
        return TRUE;
    }

    // The writing head keeps moving "in parallel" in another thread
    // and without locking.  So if the reader get too far behind, the writer
    // overtakes the reader, in theory.  It is not likely to happen because
    // (1) the reader drains all available bytes at once, (2) the buffer
    // is large enough to protect against temporary jittering, and (3)
    // because `.upload` is guaranteed to be called at least at rendering
    // FPS frequency.
    const uint64_t tolerable_behind_by = CHUNKS / 2;
    uint64_t behind_by = frozen_chunks_written - priv->chunks_read;
    if (behind_by > tolerable_behind_by) {
        priv->chunks_read = frozen_chunks_written - tolerable_behind_by;
        behind_by = tolerable_behind_by;
    }
    const uint64_t target_chunks_read = priv->chunks_read + behind_by;

    while (priv->chunks_read < target_chunks_read) {
        void *const data = priv->pcm_data[priv->chunks_read % CHUNKS];
        VisBuffer * const visbuffer = visual_buffer_new_wrap_data (data, CHUNK_SIZE_BYTES, FALSE);
        visual_return_val_if_fail(visbuffer != NULL, FALSE);
        visual_audio_input(audio, visbuffer, SAMPLE_RATE_TYPE_LV,
                           SAMPLE_FORMAT_LV, CHANNELS_TYPE_LV);
        priv->chunks_read++;
        visual_buffer_unref(visbuffer);
    }

    return TRUE;
}

static void on_input_stream_data(pa_stream *p, size_t nbytes, void *userdata) {
    pulseaudio_priv_t * const priv = (pulseaudio_priv_t *)userdata;
    visual_return_if_fail(priv != NULL);

    const void *source = NULL;
    const int peek_res = pa_stream_peek(p, &source, &nbytes);
    visual_return_if_fail(peek_res == 0);
    visual_return_if_fail(source != 0);

    const int drop_res = pa_stream_drop(p);
    visual_return_if_fail(drop_res == 0);

    // Copy all readable bytes from `source` to the right place in `priv->pcm_data`.
    while (nbytes > 0) {
        void *const target = (void *)priv->pcm_data[priv->chunks_written % CHUNKS] + priv->chunk_write_offset_bytes;
        size_t round_nbytes = nbytes;

        // Would a full write overflow the current chunk?
        if (priv->chunk_write_offset_bytes + round_nbytes > CHUNK_SIZE_BYTES) {
            // Cut down to fit the chunk
            round_nbytes = CHUNK_SIZE_BYTES - priv->chunk_write_offset_bytes;
        }

        visual_mem_copy(target, source, round_nbytes);

        // Figure out write offset location for the next round
        if (priv->chunk_write_offset_bytes + round_nbytes < CHUNK_SIZE_BYTES) {
            // Same chunk but further behind
            priv->chunk_write_offset_bytes += round_nbytes;
        } else {
            // Start of the next chunk
            priv->chunk_write_offset_bytes = 0;

            // This is atomic `priv->chunks_written++`
            uint64_t new_chunks_written = priv->chunks_written + 1;
            atomic_store_explicit(&priv->chunks_written, new_chunks_written, memory_order_release);
        }

        nbytes -= round_nbytes;
    }
}
