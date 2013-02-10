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
#include "gettext.h"
#include <stdlib.h>
#include <string.h>
#include <pulse/simple.h>
#include <pulse/error.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define SAMPLES 1024
#define BUFFERS 2

pa_sample_spec sample_spec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};

typedef struct {
    pa_simple *simple;
    int16_t pcm_data[SAMPLES*2];
} pulseaudio_priv_t;

static int  inp_pulseaudio_init    (VisPluginData *plugin);
static void inp_pulseaudio_cleanup (VisPluginData *plugin);
static int  inp_pulseaudio_upload  (VisPluginData *plugin, VisAudio *audio);
static int  inp_pulseaudio_events  (VisPluginData *plugin, VisEventQueue *events);

const VisPluginInfo *get_plugin_info( void ) {
    static VisInputPlugin input = {
        .upload = inp_pulseaudio_upload
    };

    static VisPluginInfo info = {
        .type     = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "pulseaudio",
        .name     = "Pulseaudio input plugin",
        .author   = "Scott Sibley <scott@starlon.net>",
        .version  = "1.0",
        .about    = "Use input data from pulseaudio",
        .help     = "",
        .license  = VISUAL_PLUGIN_LICENSE_GPL,

        .init     = inp_pulseaudio_init,
        .cleanup  = inp_pulseaudio_cleanup,
        .events   = inp_pulseaudio_events,
        .plugin   = &input
    };

    return &info;
}

static int inp_pulseaudio_init( VisPluginData *plugin ) {

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    pulseaudio_priv_t *priv = visual_mem_new0(pulseaudio_priv_t, 1);
    visual_plugin_set_private(plugin, priv);

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_string ("device",
                                                         N_("Device name"),
                                                         "",
                                                         NULL),
                                NULL);

    int error;

    priv->simple = pa_simple_new(
        NULL,
        "lv-pulseaudio",
        PA_STREAM_RECORD,
        NULL,
        "record",
        &sample_spec, NULL, NULL, &error);

    if( priv->simple == NULL ) {
        visual_log(VISUAL_LOG_CRITICAL, "pa_simple_new() failed: %s", pa_strerror(error));
        return FALSE;
    }

    return TRUE;
}

static void inp_pulseaudio_cleanup( VisPluginData *plugin )
{
    pulseaudio_priv_t *priv = visual_plugin_get_private(plugin);

    pa_simple_free(priv->simple);

    visual_mem_free (priv);
}

static int inp_pulseaudio_events (VisPluginData *plugin, VisEventQueue *events)
{
    pulseaudio_priv_t *priv = visual_plugin_get_private (plugin);
    VisEvent ev;
    VisParam *param;
    const char *tmp;
    int error;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_has_name (param, "device")) {
                    tmp = visual_param_get_value_string (param);

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
                        return FALSE;
                    }

                }
                break;

            default:
                break;

        }
    }

    return TRUE;
}
int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio )
{
    pulseaudio_priv_t *priv = visual_plugin_get_private(plugin);

    int error;

    if (pa_simple_read(priv->simple, priv->pcm_data, sizeof(priv->pcm_data), &error) < 0) {
        visual_log(VISUAL_LOG_CRITICAL, "pa_simple_read() failed: %s", pa_strerror(error));
        return FALSE;
    }

    VisBuffer *visbuffer = visual_buffer_new_wrap_data (priv->pcm_data, sizeof(priv->pcm_data), FALSE);

    visual_audio_input(audio, visbuffer,
                       VISUAL_AUDIO_SAMPLE_RATE_44100,
                       VISUAL_AUDIO_SAMPLE_FORMAT_S16,
                       VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    visual_buffer_unref (visbuffer);

    return TRUE;
}

