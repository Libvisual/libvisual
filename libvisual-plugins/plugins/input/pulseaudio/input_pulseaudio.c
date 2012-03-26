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
#include <pulse/pulseaudio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

static pa_sample_spec sample_spec = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};

struct thread_data
{
    char *pa_ctx_name;
    char *sink_input_index;
    int end;
};

typedef struct {
    void *pcm_data;
    VisMutex *mutex;
    struct thread_data td;
    pthread_t thread_id;
    int length;
} pulseaudio_priv_t;


const VisPluginInfo *get_plugin_info(int *count);

void get_monitor_source_index_callback(pa_context *context,const
pa_sink_info *i,int eol,void *userdata);
void get_sink_index_callback(pa_context *context,const
pa_sink_input_info *i,int eol,void *userdata);
void pa_state_callback(pa_context *c, void *userdata);
void read_callback(pa_stream *stream,size_t length,void *userdata);
void quit(pa_mainloop_api *pa_mlapi,int ret);
void record_thread(void *data);
uint32_t get_monitor_source_index(pa_context *context,pa_mainloop
*pa_ml,uint32_t sink_input_index);

static int inp_pulseaudio_init( VisPluginData *plugin );
static int inp_pulseaudio_cleanup( VisPluginData *plugin );
static int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio );

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
        .version = "1.0",
        .about = "Use pcm input from pulseaudio",
        .help = "This plugin requires you to call XinitThreads()",
        .license = VISUAL_PLUGIN_LICENSE_GPL,

        .init = inp_pulseaudio_init,
        .cleanup = inp_pulseaudio_cleanup,

        .plugin = VISUAL_OBJECT(&input[0])
    }};

    *count = sizeof(info) / sizeof(*info);

    return info;
}

static int inp_pulseaudio_init( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv;

    priv = visual_mem_new0(pulseaudio_priv_t, 1);

    visual_object_set_private(VISUAL_OBJECT(plugin), priv);

    memset(priv, 0, sizeof(pulseaudio_priv_t));

    priv->mutex = visual_mutex_new();

    priv->td.pa_ctx_name="lv-pulseaudio";
    priv->td.sink_input_index="1";

    pthread_create(&priv->thread_id,NULL,(void *)record_thread,&priv->td);

    return VISUAL_OK;
}

static int inp_pulseaudio_cleanup( VisPluginData *plugin ) {
    pulseaudio_priv_t *priv = NULL;

    visual_log_return_val_if_fail( plugin != NULL, VISUAL_ERROR_GENERAL);
    priv = visual_object_get_private(VISUAL_OBJECT( plugin));
    visual_log_return_val_if_fail( priv != NULL, VISUAL_ERROR_GENERAL);

    priv->td.end = 1;

    visual_mutex_lock(priv->mutex);

    visual_mutex_free(priv->mutex);

    visual_mem_free (priv);
   
    return VISUAL_OK;
}

static int inp_pulseaudio_upload( VisPluginData *plugin, VisAudio *audio )
{
    pulseaudio_priv_t *priv = NULL;
    VisBuffer buffer;

    visual_log_return_val_if_fail( audio != NULL, -VISUAL_ERROR_GENERAL);
    visual_log_return_val_if_fail( plugin != NULL, -VISUAL_ERROR_GENERAL);

    priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    visual_log_return_val_if_fail( priv != NULL, -VISUAL_ERROR_GENERAL);

    if(priv->pcm_data == NULL)
        return 0;

    visual_mutex_lock(priv->mutex);

    visual_buffer_init(&buffer, priv->pcm_data, priv->length, NULL);
    visual_audio_samplepool_input(audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    visual_mutex_unlock(priv->mutex);
   
    return 0;
}


void record_thread(void *data){
    pa_stream *stream;
    pa_mainloop *pa_ml;
    pa_mainloop_api *pa_mlapi;
    pa_context *context;
    pa_buffer_attr attr;
    int ret=1;
    char msi[16];

    struct thread_data *td=(struct thread_data*)data;

    memset(&attr,0,sizeof(attr));
    attr.fragsize=(uint32_t)-1;
    attr.maxlength=(uint32_t)-1;

    pa_ml=pa_mainloop_new();
    pa_mlapi=pa_mainloop_get_api(pa_ml);
    context=pa_context_new(pa_mlapi,(*td).pa_ctx_name);
    pa_context_connect(context,NULL,0,NULL);

    if(!(stream=pa_stream_new(context,(*td).pa_ctx_name,&sample_spec,NULL))){
        visual_log(VISUAL_LOG_ERROR, "Failed to create monitoring stream");
        goto quit;
    }

    pa_stream_set_monitor_stream(stream,atoi((*td).sink_input_index));
    pa_stream_set_read_callback(stream,read_callback,data);

    uint32_t monitor_source_index=get_monitor_source_index(context,pa_ml,atoi((*td).sink_input_index));
    snprintf(msi,sizeof(msi),"%u",monitor_source_index);

    if(pa_stream_connect_record(stream,msi,&attr,(pa_stream_flags_t)(PA_STREAM_DONT_MOVE|PA_STREAM_PEAK_DETECT))<0){
        visual_log(VISUAL_LOG_ERROR, "Failed to connect monitoring stream");
        pa_stream_unref(stream);
        goto quit;
    }

    if(td->end)
        quit(pa_mlapi,0);

    if(pa_mainloop_run(pa_ml,&ret)<0){
        visual_log(VISUAL_LOG_ERROR, "Main loop failed");
        goto quit;
    }

quit:
    if(stream)
        pa_stream_unref(stream);

    if(context)
        pa_context_unref(context);

    if(pa_ml){
        pa_signal_done();
        pa_mainloop_free(pa_ml);
    }
}

uint32_t get_monitor_source_index(pa_context *context,pa_mainloop
*pa_ml,uint32_t sink_input_index){
    pa_operation *pa_op;
    uint32_t sink_index;
    uint32_t monitor_source_index;
    int state = 0;
    int pa_ready = 0;


    pa_context_set_state_callback(context, pa_state_callback, &pa_ready);

    for (;;) {
        if (pa_ready == 0) {
            pa_mainloop_iterate(pa_ml, 1, NULL);
            continue;
        }

        if (pa_ready == 2) {
            pa_context_disconnect(context);
            pa_context_unref(context);
            pa_mainloop_free(pa_ml);
            return -1;
        }

        switch (state) {
            case 0:
                pa_op = pa_context_get_sink_input_info(context,
                        sink_input_index,
                        get_sink_index_callback,
                        &sink_index
                        );

                visual_log(VISUAL_LOG_DEBUG, "Getting sink index...");
                state++;
            break;

            case 1:
                if (pa_operation_get_state(pa_op) == PA_OPERATION_DONE) {
            pa_operation_unref(pa_op);

                    pa_op = pa_context_get_sink_info_by_index(context,
                        sink_index,
                        get_monitor_source_index_callback,
                        &monitor_source_index
                        );

                    visual_log(VISUAL_LOG_DEBUG, "Getting monitor source index...");
                    state++;
                }
            break;

            case 2:
                if (pa_operation_get_state(pa_op) == PA_OPERATION_DONE) {
                    pa_operation_unref(pa_op);

                    return monitor_source_index;
                }
            break;

            default:
                visual_log(VISUAL_LOG_DEBUG, "In state %d", state);
                return -1;
        }

        pa_mainloop_iterate(pa_ml, 1, NULL);
    }
}

void get_monitor_source_index_callback(pa_context *context,const
pa_sink_info *i,int eol,void *userdata){
    uint32_t *monitor_source_index=userdata;

    if (eol > 0) {
        visual_log(VISUAL_LOG_DEBUG, "Get monitor source index complete... (index=%d)",*(monitor_source_index));
        return;
    }

    *(monitor_source_index)=i->monitor_source;
}

void get_sink_index_callback(pa_context *context,const
pa_sink_input_info *i,int eol,void *userdata){
    uint32_t *sink_index=userdata;

    if (eol > 0) {
        visual_log(VISUAL_LOG_DEBUG, "Get sink index complete (index=%d)",*(sink_index));
        return;
    }

    *(sink_index)=i->sink;
}

void pa_state_callback(pa_context *c, void *userdata) {
        pa_context_state_t state;
        int *pa_ready = userdata;

        state = pa_context_get_state(c);
        switch  (state) {
                case PA_CONTEXT_UNCONNECTED:
                case PA_CONTEXT_CONNECTING:
                case PA_CONTEXT_AUTHORIZING:
                case PA_CONTEXT_SETTING_NAME:
                default:
                        break;
                case PA_CONTEXT_FAILED:
                case PA_CONTEXT_TERMINATED:
                        *pa_ready = 2;
                        break;
                case PA_CONTEXT_READY:
                        *pa_ready = 1;
                        break;
        }
}

void read_callback(pa_stream *stream,size_t length,void *userdata){
    const void *data;
    pulseaudio_priv_t *priv = userdata;

    if(pa_stream_peek(stream,&data,&length)<0){
        visual_log(VISUAL_LOG_ERROR, "Failed to read data from stream");
        return;
    }

    visual_mutex_lock(priv->mutex);

    if(priv->length != length)
    {
        if(priv->pcm_data != NULL)
            free(priv->pcm_data);

        priv->pcm_data = malloc(length);
    }

    memcpy(priv->pcm_data, data, length);

    priv->length = length;

    visual_mutex_unlock(priv->mutex);

    pa_stream_drop(stream);
}

void quit(pa_mainloop_api *pa_mlapi,int ret){
    assert(pa_mlapi);
    pa_mlapi->quit(pa_mlapi,ret);
}

