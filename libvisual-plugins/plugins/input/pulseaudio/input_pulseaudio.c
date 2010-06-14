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

#include <stdlib.h>
#include <libvisual/libvisual.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#define PCM_BUF_SIZE 1024

typedef struct {
    pa_simple *simple;
    int error;
} pulseaudio_priv_t;

int inp_pulseaudio_init( VisPluginData *plugin );
int inp_pulseaudio_cleanup( VisPluginData *plugin );
int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio );

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info( int *count ) {
    static VisInputPlugin input[] = {{
        .upload = inp_pulseaudio_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,
        .plugname = "pulseaudio",
        .name = "Pulseaudio input plugin",
        .author = "Scott Sibley <scott@starlon.net>",
        .version = "$Revision$",
        .about = "Use data exported from pulseaudio",
        .help = "",
        .license = VISUAL_PLUGIN_LICENSE_GPL,

        .init = inp_pulseaudio_init,
        .cleanup = inp_pulseaudio_cleanup,

        .plugin = VISUAL_OBJECT(&input[0])
    }};

    *count = sizeof(info) / sizeof(*info);

    return info;
}

int inp_pulseaudio_init( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv;
    pa_sample_spec sample_spec;

    priv = visual_mem_new0(pulseaudio_priv_t, 1);

    visual_object_set_private(VISUAL_OBJECT(plugin), priv);

    sample_spec.format = PA_SAMPLE_S16LE;
    sample_spec.rate = 44100;
    sample_spec.channels = 2;

    priv->simple = pa_simple_new(NULL, "lv-pulseaudio", PA_STREAM_RECORD, NULL, "Libvisual pulseaudio plugin", &sample_spec, NULL, NULL, &priv->error);

    return VISUAL_OK;
}

int inp_pulseaudio_cleanup( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv = NULL;

    visual_log_return_val_if_fail( plugin != NULL, VISUAL_ERROR_GENERAL);
    priv = visual_object_get_private(VISUAL_OBJECT( plugin));
    visual_log_return_val_if_fail( priv != NULL, VISUAL_ERROR_GENERAL);

    pa_simple_free(priv->simple);

    visual_mem_free (priv);
    return VISUAL_OK;
}

int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio )
{
    pulseaudio_priv_t *priv = NULL;
    short pcm_data[PCM_BUF_SIZE];
    VisBuffer buffer;

    visual_log_return_val_if_fail( audio != NULL, -VISUAL_ERROR_GENERAL);
    visual_log_return_val_if_fail( plugin != NULL, -VISUAL_ERROR_GENERAL);

    priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    visual_log_return_val_if_fail( priv != NULL, -VISUAL_ERROR_GENERAL);

    /* Provide the VisAudio with pcm data */
    if(pa_simple_read(priv->simple, pcm_data, PCM_BUF_SIZE, NULL)) {
        visual_buffer_init(&buffer, pcm_data, PCM_BUF_SIZE, NULL);
        visual_audio_samplepool_input(audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
            VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
    }
    return 0;
}

