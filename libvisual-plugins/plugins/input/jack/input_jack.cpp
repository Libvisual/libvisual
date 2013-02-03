/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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
#include <vector>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define JACK_SERVER_NAME NULL

namespace {

  typedef std::vector<int16_t> Buffer;

  struct JackPrivate {
      jack_client_t* client;
      jack_port_t*   input_port;
      bool           shutdown;
      jack_nframes_t buffer_size;
      jack_nframes_t sample_rate;
      Buffer         buffer;
  };

  int  process_callback     (jack_nframes_t nframes, void* arg);
  void shutdown_callback    (void* arg);
  int  buffer_size_callback (jack_nframes_t nframes, void* arg);
  int  sample_rate_callback (jack_nframes_t nframes, void* arg);

  int  inp_jack_init    (VisPluginData* plugin);
  void inp_jack_cleanup (VisPluginData* plugin);
  int  inp_jack_upload  (VisPluginData* plugin, VisAudio* audio);

}

VisPluginInfo const* get_plugin_info ()
{
    static VisInputPlugin input = {
        inp_jack_upload
    };

    static VisPluginInfo info;

    info.type     = VISUAL_PLUGIN_TYPE_INPUT;
    info.plugname = "jack";
    info.name     = "JACK input";
    info.author   = "Dennis Smit <ds@nerds-incorporated.org>";
    info.version  = "0.1";
    info.about    = N_("Jackit capture plugin");
    info.help     =  N_("Use this plugin to capture PCM data from jackd");
    info.license  = VISUAL_PLUGIN_LICENSE_LGPL;

    info.init    = inp_jack_init;
    info.cleanup = inp_jack_cleanup;
    info.plugin  = &input;

    return &info;
}

namespace {

  int inp_jack_init (VisPluginData* plugin)
  {
#if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

      JackPrivate* priv = new JackPrivate;
      visual_plugin_set_private (plugin, priv);

      jack_options_t options = JackNullOption;
      jack_status_t  status;

      // Open client connection to JACK, automatically starting the
      // server if needed
      priv->client = jack_client_open ("Libvisual", options, &status, JACK_SERVER_NAME);
      if (!priv->client) {
          visual_log (VISUAL_LOG_ERROR, "Cannot connect to JACK server: status = 0x%2.0x", status);
          return FALSE;
      }

      // Check if server was started
      if (status & JackServerStarted) {
          visual_log (VISUAL_LOG_INFO, "JACK server started");
      }

      // Initialize audio settings
      priv->buffer_size = jack_get_buffer_size (priv->client);
      priv->sample_rate = jack_get_sample_rate (priv->client);
      priv->buffer.resize (priv->buffer_size * 2);
      priv->shutdown = false;

      // Setup callbacks
      jack_set_process_callback (priv->client, process_callback, priv);
      jack_on_shutdown (priv->client, shutdown_callback, priv);
      jack_set_sample_rate_callback (priv->client, sample_rate_callback, priv);
      jack_set_buffer_size_callback (priv->client, buffer_size_callback, priv);

      // Create an input port to receive data on
      priv->input_port = jack_port_register (priv->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
      if (!priv->input_port) {
          visual_log (VISUAL_LOG_ERROR, "No more JACK input port available");
          return FALSE;
      }

      // Activate this client. From here on JACK will start invoking
      // our callbacks
      if (jack_activate (priv->client) == 1) {
          visual_log (VISUAL_LOG_ERROR, "Cannot activate client");
          return FALSE;
      }

      // Look for physical capture ports we can try and connect to
      const char **ports = jack_get_ports (priv->client, NULL, NULL, JackPortIsPhysical | JackPortIsOutput);
      if (!ports) {
          visual_log (VISUAL_LOG_ERROR, "Cannot find any physical capture ports");
          return FALSE;
      }

      visual_log (VISUAL_LOG_INFO, "Available ports:");
      for (char const** port = ports; *port; port++) {
          visual_log (VISUAL_LOG_INFO, "%s", *port);
      }

      // Receive our input from the first capture port
      if (jack_connect (priv->client, ports[0], jack_port_name (priv->input_port))) {
          visual_log (VISUAL_LOG_ERROR, "Cannot connect input port");
          free (ports);
          return FALSE;
      }

      free (ports);

      return TRUE;
  }

  void inp_jack_cleanup (VisPluginData* plugin)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (visual_plugin_get_private (plugin));

      if (priv->client) {
          jack_client_close (priv->client);
      }

      delete priv;
  }

  int inp_jack_upload (VisPluginData* plugin, VisAudio* audio)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (visual_plugin_get_private (plugin));

      if (priv->shutdown) {
          visual_log (VISUAL_LOG_ERROR, "JACK server seems to have shutdown");
          return FALSE;
      }

      LV::BufferPtr buffer = LV::Buffer::wrap (priv->buffer.data(), priv->buffer.size() * sizeof(int16_t), false);
      audio->input (buffer,
                    VISUAL_AUDIO_SAMPLE_RATE_44100,
                    VISUAL_AUDIO_SAMPLE_FORMAT_S16,
                    VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

      return TRUE;
  }

  int buffer_size_callback (jack_nframes_t nframes, void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      // Buffer size changed, adjust buffer accordingly
      priv->buffer_size = nframes;
      priv->buffer.resize (priv->buffer_size * 2);

      return 0;
  }

  int sample_rate_callback (jack_nframes_t nframes, void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      // FIXME: Update sample rate setting in inp_jack_upload()
      priv->sample_rate = nframes;

      return 0;
  }

  int process_callback (jack_nframes_t nframes, void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      jack_default_audio_sample_t* in =
          static_cast<jack_default_audio_sample_t*> (jack_port_get_buffer (priv->input_port, nframes));

      // FIXME: As we're only receiving audio data from a single input
      // port, we need to duplicate it across both left and right
      // channels. VisAudio does not accept mono inputs yet.

      // FIXME: Float inputs do not appear to work at the moment
      // either, so we convert the data to signed 16-bit here as well.

      for (unsigned int i = 0; i < nframes; i++) {
          priv->buffer[i << 1] = priv->buffer[(i << 1) + 1] = int16_t (in[i] * 32767);
      }

      return 0;
  }

  void shutdown_callback (void* arg)
  {
      JackPrivate* priv = static_cast<JackPrivate*> (arg);

      priv->shutdown = true;
  }

}
