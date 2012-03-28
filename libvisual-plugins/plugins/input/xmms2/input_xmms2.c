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
#include <xmmsclient/xmmsclient.h>
#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE 1024

const VisPluginInfo *get_plugin_info(int *count);

typedef struct {
    xmmsc_connection_t *connection;
    int vis;
    VisSongInfo *songinfo;
    int current_id;
} xmms2_priv_t;

static int inp_xmms2_init( VisPluginData *plugin );
static int inp_xmms2_cleanup( VisPluginData *plugin );
static int inp_xmms2_upload( VisPluginData *plugin, VisAudio *audio );
static int inp_xmms2_events(VisPluginData *plugin, VisEventQueue *events);

static int result_get_string(xmmsc_result_t *res, const char *key, const char **buf);
static int result_get_int(xmmsc_result_t *res, const char *key, int *num);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info( int *count ) {
    static VisInputPlugin input[] = {{
        .upload = inp_xmms2_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,
        .plugname = "xmms2",
        .name = "Xmms2 input plugin",
        .author = "Scott Sibley <scott@starlon.net>",
        .version = "$Revision$",
        .about = "Use data exported from Xmms2",
        .help = "This plugin requires an xmms2 daemon to be running",
        .license = VISUAL_PLUGIN_LICENSE_GPL,

        .init = inp_xmms2_init,
        .cleanup = inp_xmms2_cleanup,
        .events = inp_xmms2_events,

        .plugin = VISUAL_OBJECT(&input[0])
    }};

    *count = sizeof(info) / sizeof(*info);

    return info;
}

static int inp_xmms2_init( VisPluginData *plugin ) {
    xmms2_priv_t *priv;
    xmmsv_t *val;
    const char *err_buf;
    VisParamContainer *paramcontainer = visual_plugin_get_params(plugin);

    VisParamEntry *param = visual_param_entry_new("songinfo");
    param->type = VISUAL_PARAM_ENTRY_TYPE_OBJECT;

    visual_param_container_add(paramcontainer, param);

    priv = visual_mem_malloc0(sizeof(xmms2_priv_t));

    visual_object_set_private(VISUAL_OBJECT(plugin), priv);

    priv->songinfo = NULL;

    priv->connection = xmmsc_init("libvisual-xmms2-plugin");

    if(!priv->connection) {
        visual_log(VISUAL_LOG_ERROR, "Unable to allocate xmms2 connection");
        return -VISUAL_ERROR_GENERAL;
    }

    if(!xmmsc_connect(priv->connection, getenv("XMMS_PATH"))) {
        visual_log(VISUAL_LOG_ERROR, "Connection failed. Is xmms2d running?");
        return -VISUAL_ERROR_GENERAL;
    }

    xmmsc_result_t *res = xmmsc_visualization_version(priv->connection);

    xmmsc_result_wait(res);

    val = xmmsc_result_get_value(res);

    if(xmmsc_result_iserror(res) && xmmsv_get_error(val, &err_buf)) {
        visual_log(VISUAL_LOG_ERROR, "%s", err_buf);
        return -VISUAL_ERROR_GENERAL;
    } else {
        int32_t version;
        xmmsv_t *v = xmmsc_result_get_value(res);
        if(xmmsv_is_error(v) && xmmsv_get_error(v, &err_buf)) {
            visual_log(VISUAL_LOG_ERROR, "%s", err_buf);
            return -VISUAL_ERROR_GENERAL;
        }

        xmmsv_get_int(v, &version);

        if(version < 1) {
            visual_log(VISUAL_LOG_ERROR,
                "The xmms2 server only supports formats up to version %d (needed is %d)", version, 1);
                return -VISUAL_ERROR_GENERAL;
        }
    }

    xmmsc_result_unref(res);

    res = xmmsc_visualization_init(priv->connection);
    xmmsc_result_wait(res);

    val = xmmsc_result_get_value(res);

    if(xmmsc_result_iserror(res) && xmmsv_get_error(val, &err_buf)) {
        visual_log(VISUAL_LOG_ERROR, "%s", err_buf);
        return -VISUAL_ERROR_GENERAL;
    }

    priv->vis = xmmsc_visualization_init_handle( res);

    xmmsv_t *configdict = xmmsv_new_dict();

    xmmsv_dict_set(configdict, "type", xmmsv_new_string("pcm"));
    xmmsv_dict_set(configdict, "stereo", xmmsv_new_string("1"));

    xmmsc_result_unref(res);

    res = xmmsc_visualization_properties_set(priv->connection, priv->vis, configdict);
    xmmsc_result_wait(res);

    val = xmmsc_result_get_value(res);

    if( xmmsc_result_iserror( res ), xmmsv_get_error(val, &err_buf)) {
        visual_log(VISUAL_LOG_ERROR, "%s", err_buf);
        return -VISUAL_ERROR_GENERAL;
    }
    xmmsc_result_unref(res);

    while(!xmmsc_visualization_started(priv->connection, priv->vis)) {
        res = xmmsc_visualization_start(priv->connection, priv->vis);
        if(xmmsc_visualization_errored(priv->connection, priv->vis)) {
            visual_log(VISUAL_LOG_ERROR,
                "Couldn't start visualization transfer: %s", xmmsc_get_last_error(priv->connection));
            return -VISUAL_ERROR_GENERAL;
        }
        if(res) {
            xmmsc_result_wait(res);
            xmmsc_visualization_start_handle(priv->connection, res);
            xmmsc_result_unref(res);
        }
    }

    return VISUAL_OK;
}

static int inp_xmms2_cleanup( VisPluginData *plugin ) {
    xmms2_priv_t *priv = NULL;

    visual_return_val_if_fail( plugin != NULL, VISUAL_ERROR_GENERAL);
    priv = visual_object_get_private(VISUAL_OBJECT( plugin));
    visual_return_val_if_fail( priv != NULL, VISUAL_ERROR_GENERAL);

    xmmsc_visualization_shutdown(priv->connection, priv->vis);

    visual_mem_free (priv);
    return VISUAL_OK;
}

/* Extract a string from an xmms2 dict */
static int result_get_string(xmmsc_result_t *res, const char *key, const char **buf)
{
    xmmsv_t *val = xmmsc_result_get_value(res);
    xmmsv_t *tmp;
    if(xmmsv_get_type(val) != XMMSV_TYPE_DICT) {
        return false;
    }
    if(!xmmsv_dict_get(val, key, &tmp)) {
        return false;
    }
    if(xmmsv_get_type(tmp) == XMMSV_TYPE_DICT) {
        xmmsv_dict_iter_t *it;
        xmmsv_get_dict_iter(tmp, &it);
        const char *k;
        xmmsv_t *v;
        if(!xmmsv_dict_iter_pair(it, &k, &v))
            return false;
        xmmsv_get_string(v, buf);
        return true;
    }
    return false;
}

/* Extract an integer from an xmms2 dict */
static int result_get_int(xmmsc_result_t *res, const char *key, int *num)
{
    xmmsv_t *val = xmmsc_result_get_value(res);
    xmmsv_t *tmp;
    if(xmmsv_get_type(val) != XMMSV_TYPE_DICT) {
        return false;
    }
    if(!xmmsv_dict_get(val, key, &tmp)) {
        return false;
    }
    if(xmmsv_get_type(tmp) == XMMSV_TYPE_DICT) {
        xmmsv_dict_iter_t *it;
        xmmsv_get_dict_iter(tmp, &it);
        const char *k;
        xmmsv_t *v;
        if(!xmmsv_dict_iter_pair(it, &k, &v))
            return false;
        xmmsv_get_int(v, num);
        return true;
    }
    return true;
}

static int inp_xmms2_upload( VisPluginData *plugin, VisAudio *audio )
{
    xmms2_priv_t *priv = NULL;
    short pcm_data[PCM_BUF_SIZE];
    xmmsc_result_t *res;
    xmmsv_t *val;
    VisBuffer buffer;
    VisSongInfo *songinfo;
    int id, time;
    const char *dictbuf;
    int dictnum;

    visual_return_val_if_fail( audio != NULL, -VISUAL_ERROR_GENERAL);
    visual_return_val_if_fail( plugin != NULL, -VISUAL_ERROR_GENERAL);

    priv = visual_object_get_private(VISUAL_OBJECT(plugin));

    visual_return_val_if_fail( priv != NULL, -VISUAL_ERROR_GENERAL);

    visual_plugin_events_pump(plugin);

    songinfo = priv->songinfo;

    if(songinfo != NULL)
    {
        /* Get ID of the currently playing song */
        res = xmmsc_playback_current_id(priv->connection);

        xmmsc_result_wait(res);

        val = xmmsc_result_get_value(res);

        visual_return_val_if_fail(xmmsv_get_int(val, &id) > 0, -VISUAL_ERROR_GENERAL);

	if(id != priv->current_id)
        {
            priv->current_id = id;

            xmmsc_result_unref(res);

            /* Get media info about the current song and fill the VisSongInfo */
            res = xmmsc_medialib_get_info(priv->connection, id);

            xmmsc_result_wait(res);

            visual_songinfo_set_type(songinfo, VISUAL_SONGINFO_TYPE_ADVANCED);

            if(result_get_string(res, "title", &dictbuf)) {
                visual_songinfo_set_song(songinfo, (char *)dictbuf);
            } else {
                visual_songinfo_set_song(songinfo, "(null)");
            }
            if(result_get_int(res, "duration", &dictnum)) {
                visual_songinfo_set_length(songinfo, dictnum);
            } else {
                visual_songinfo_set_length(songinfo, -1);
            }

            if(result_get_string(res, "album", &dictbuf)) {
                visual_songinfo_set_album(songinfo, (char *)dictbuf);
            } else {
                visual_songinfo_set_album(songinfo, "(null)");
            }

            if(result_get_string(res, "artist", &dictbuf)) {
                visual_songinfo_set_artist(songinfo, (char *)dictbuf);
            } else {
                visual_songinfo_set_artist(songinfo, "(null)");
            }

            xmmsc_result_unref(res);

            /* Get current playtime */
            res = xmmsc_signal_playback_playtime(priv->connection);

            xmmsc_result_wait(res);

            val = xmmsc_result_get_value(res);
            if(xmmsv_get_int(val, &time)) {
                visual_songinfo_set_elapsed(songinfo, time);
            } else {
                visual_songinfo_set_elapsed(songinfo, -1);
            }

            visual_beat_change_song(audio->beat);
    	}
        xmmsc_result_unref(res);

    }

    /* Provide the VisAudio with pcm data */
    if(xmmsc_visualization_chunk_get(priv->connection, priv->vis, pcm_data, 0, 0)) {
        visual_buffer_init(&buffer, pcm_data, PCM_BUF_SIZE, NULL);
        visual_audio_samplepool_input(audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
            VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
    }
    return 0;
}

static int inp_xmms2_events(VisPluginData *plugin, VisEventQueue *events) {
    xmms2_priv_t *priv = visual_object_get_private (VISUAL_OBJECT(plugin));
    VisEvent ev;
    VisParamEntry *param;

    while(visual_event_queue_poll(events, &ev))
    {
        switch(ev.type)
        {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;
                if(visual_param_entry_get_type(param) == VISUAL_PARAM_ENTRY_TYPE_OBJECT)
                {
                    priv->songinfo = VISUAL_SONGINFO(visual_param_entry_get_object(param));
                }
                break;
            default:
                break;

        }
    }
    return 0;
}
