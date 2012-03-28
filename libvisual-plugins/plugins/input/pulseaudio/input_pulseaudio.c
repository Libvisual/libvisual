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
#include <string.h>
#include <libvisual/libvisual.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define PCM_BUF_SIZE 1024

    
pa_sample_spec sample_spec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};

typedef struct {
    pa_simple *simple;
} pulseaudio_priv_t;

int inp_pulseaudio_init( VisPluginData *plugin );
int inp_pulseaudio_cleanup( VisPluginData *plugin );
int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio );
int inp_pulseaudio_events (VisPluginData *plugin, VisEventQueue *events);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info( int *count );
const VisPluginInfo *get_plugin_info( int *count ) {
    static VisInputPlugin input[] = {{
        .upload = inp_pulseaudio_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,
        .plugname = "pulseaudio",
        .name = "Pulseaudio input plugin",
        .author = "Scott Sibley <scott@starlon.net>",
        .version = "1.0",
        .about = "Use input data from pulseaudio",
        .help = "",
        .license = VISUAL_PLUGIN_LICENSE_GPL,

        .init = inp_pulseaudio_init,
        .cleanup = inp_pulseaudio_cleanup,
        .events = inp_pulseaudio_events,

        .plugin = VISUAL_OBJECT(&input[0])
    }};

    *count = sizeof(info) / sizeof(*info);

    return info;
}

int inp_pulseaudio_init( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv;
    int error;

    VisParamContainer *paramcontainer = visual_plugin_get_params(plugin);

    priv = visual_mem_new0(pulseaudio_priv_t, 1);

    visual_object_set_private(VISUAL_OBJECT(plugin), priv);

    memset(priv, 0, sizeof(pulseaudio_priv_t));


    priv->simple = pa_simple_new(
        NULL, 
        "lv-pulseaudio", 
        PA_STREAM_RECORD, 
        NULL, 
        "record", 
        &sample_spec, NULL, NULL, &error);

    if( priv->simple == NULL ) {
        visual_log(VISUAL_LOG_CRITICAL, "pa_simple_new() failed: %s", pa_strerror(error));
        return -VISUAL_ERROR_GENERAL;
    }

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_STRING ("device", ""),
        VISUAL_PARAM_LIST_END
    };

    visual_param_container_add_many (paramcontainer, params);

    return VISUAL_OK;
}

int inp_pulseaudio_cleanup( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv = NULL;

    visual_return_val_if_fail( plugin != NULL, VISUAL_ERROR_GENERAL);

    priv = visual_object_get_private(VISUAL_OBJECT( plugin));

    visual_return_val_if_fail( priv != NULL, VISUAL_ERROR_GENERAL);

    pa_simple_free(priv->simple);

    visual_mem_free (priv);
    return VISUAL_OK;
}

int inp_pulseaudio_events (VisPluginData *plugin, VisEventQueue *events)
{
    pulseaudio_priv_t *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisEvent ev;
    VisParamEntry *param;
    char *tmp;
    int error;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "device")) {
                    tmp = visual_param_entry_get_string (param);
                    
                    if(priv->simple != NULL)
                        pa_simple_free(priv->simple);

                    priv->simple = pa_simple_new(
                        NULL, 
                        "lv-pulseaudio", 
                        PA_STREAM_RECORD, 
                        tmp, 
                        "record", 
                        &sample_spec, NULL, NULL, &error);

                    if( priv->simple == NULL ) {
                        visual_log(VISUAL_LOG_CRITICAL, "pa_simple_new() failed: %s", pa_strerror(error));
                        return -VISUAL_ERROR_GENERAL;
                    }

                }
                break;

            default:
                break;

        }
    }

    return 0;
}
int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio )
{
    pulseaudio_priv_t *priv = NULL;
    short pcm_data[PCM_BUF_SIZE];
    VisBuffer buffer;
    int error;

    memset(pcm_data, 0, PCM_BUF_SIZE * sizeof(short));

    visual_return_val_if_fail( audio != NULL, -VISUAL_ERROR_GENERAL);
    visual_return_val_if_fail( plugin != NULL, -VISUAL_ERROR_GENERAL);

    priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    visual_return_val_if_fail( priv != NULL, -VISUAL_ERROR_GENERAL);

    if(pa_simple_read(priv->simple, pcm_data, PCM_BUF_SIZE, &error) < 0) {
        visual_log(VISUAL_LOG_CRITICAL, "pa_simple_read() failed: %s", pa_strerror(error));
        return -VISUAL_ERROR_GENERAL;
    }

    visual_buffer_init(&buffer, pcm_data, PCM_BUF_SIZE, NULL);
    visual_audio_samplepool_input(audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
   
    return 0;
}

