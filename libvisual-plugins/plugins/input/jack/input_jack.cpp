/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_jack.c,v 1.14 2006/01/22 13:25:27 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"
#include <jack/jack.h>
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define PCM_BUF_SIZE     1024
#define JACK_SERVER_NAME NULL

namespace {

  struct JackPrivate {
      jack_client_t* client;
      jack_port_t*   input_port;
      bool           shutdown;
      VisBuffer*     buffer;
      int16_t        fakebuf[PCM_BUF_SIZE];
  };

  int  process_callback  (jack_nframes_t nframes, void* arg);
  void shutdown_callback (void* arg);

  int inp_jack_init    (VisPluginData* plugin);
  int inp_jack_cleanup (VisPluginData* plugin);
  int inp_jack_upload  (VisPluginData* plugin, VisAudio* audio);

}

VisPluginInfo const* get_plugin_info ()
{
    static VisInputPlugin input = {
        { 0 },
        inp_jack_upload
    };

    static VisPluginInfo info = {
        { 0 },
        VISUAL_PLUGIN_TYPE_INPUT,

        "jack",
        "JACK input",
        "Dennis Smit <ds@nerds-incorporated.org>",
        "0.1",
        N_("Jackit capture plugin"),
        N_("Use this plugin to capture PCM data from jackd"),
        VISUAL_PLUGIN_LICENSE_LGPL,

        inp_jack_init,
        inp_jack_cleanup,
        NULL,

        0,
        VISUAL_OBJECT (&input)
    };

    return &info;
}

namespace {

  int inp_jack_init (VisPluginData* plugin)
  {
#if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

      visual_return_val_if_fail (plugin != NULL, -1);

      JackPrivate* priv = new JackPrivate;
      visual_return_val_if_fail (priv != NULL, -1);
      visual_object_set_private (VISUAL_OBJECT (plugin), priv);

      jack_options_t options = JackNullOption;
      jack_status_t  status;

      priv->client = jack_client_open ("Libvisual", options, &status, JACK_SERVER_NAME);
      if (!priv->client) {
          visual_log (VISUAL_LOG_ERROR, "Cannot connect to JACK server: status = 0x%2.0x", status);
          return -1;
      }

      if (status & JackServerStarted) {
          visual_log (VISUAL_LOG_INFO, "JACK server started");
      }

      priv->shutdown = false;

      jack_set_process_callback (priv->client, process_callback, priv);
      jack_on_shutdown (priv->client, shutdown_callback, priv);

      priv->input_port = jack_port_register (priv->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
      if (!priv->input_port) {
          visual_log (VISUAL_LOG_ERROR, "No more JACK input port available");
          return -1;
      }

      if (jack_activate (priv->client) == 1) {
          visual_log (VISUAL_LOG_ERROR, "Cannot activate client");
          return -1;
      }

      const char **ports = jack_get_ports (priv->client, NULL, NULL, JackPortIsPhysical | JackPortIsOutput);
      if (!ports) {
          visual_log (VISUAL_LOG_ERROR, "Cannot find any physical capture ports");
          return -1;
      }

      if (jack_connect (priv->client, ports[0], jack_port_name (priv->input_port))) {
          visual_log (VISUAL_LOG_ERROR, "Cannot connect input port");
          free (ports);
          return -1;
      }

      free (ports);

      return 0;
  }

  int inp_jack_cleanup (VisPluginData* plugin)
  {
      visual_return_val_if_fail (plugin != NULL, -1);

      JackPrivate* priv = static_cast<JackPrivate*> (visual_object_get_private (VISUAL_OBJECT (plugin)));
      visual_return_val_if_fail (priv != NULL, -1);

      if (priv->client)
          jack_client_close (priv->client);

      delete priv;

      return 0;
  }

  int inp_jack_upload (VisPluginData* plugin, VisAudio* audio)
  {
      visual_return_val_if_fail (audio  != NULL, -1);
      visual_return_val_if_fail (plugin != NULL, -1);

      JackPrivate* priv = static_cast<JackPrivate*> (visual_object_get_private (VISUAL_OBJECT (plugin)));
      visual_return_val_if_fail (priv != NULL, -1);

      if (priv->shutdown) {
          visual_log (VISUAL_LOG_ERROR, "JACK server seems to have shutdown");
          return -1;
      }

      // VisBuffer buffer;
      // visual_buffer_init (&buffer, data, rcnt, NULL);
      // visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
      //                                 VISUAL_AUDIO_SAMPLE_FORMAT_FLOAT, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

      for (unsigned int i = 0; i < PCM_BUF_SIZE && i < 1024; i += 2) {
          audio->plugpcm[0][i >> 1] = priv->fakebuf[i];
          audio->plugpcm[1][i >> 1] = priv->fakebuf[i + 1];
      }

      return 0;
  }

  int process_callback (jack_nframes_t nframes, void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      jack_default_audio_sample_t* in =
          static_cast<jack_default_audio_sample_t*> (jack_port_get_buffer (priv->input_port, nframes));

      visual_mem_set (&priv->fakebuf, 0, sizeof (priv->fakebuf));

      for (unsigned int i = 0; i < nframes && i < 1024; i++)
          priv->fakebuf[i] = in[i] * 32767;

      return 0;
  }

  void shutdown_callback (void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      priv->shutdown = true;
  }

}
