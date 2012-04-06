/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Vitaly V. Bursov <vitalyvb@urk,net>
 *
 * Authors: Vitaly V. Bursov <vitalyvb@urk,net>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_alsa.c,v 1.23 2006/02/13 20:36:11 synap Exp $
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

#include <libvisual/libvisual.h>
#include <windows.h>
#include <cstdlib>

#define PCM_BUFFER_SIZE 4096

VISUAL_PLUGIN_API_VERSION_VALIDATOR;
    
VISUAL_C_LINKAGE VisPluginInfo const* get_plugin_info (int* count);

struct WaveInPrivate {
    HWAVEIN device_handle;
    bool    loaded;

    WAVEHDR buffer_header;
    char    buffers[2][PCM_BUFFER_SIZE];
    bool    buffer_read[2];

    int     active_buffer;
    HANDLE  mutex;
};

namespace {

  int inp_wavein_init    (VisPluginData *plugin);
  int inp_wavein_cleanup (VisPluginData *plugin);
  int inp_wavein_events  (VisPluginData *plugin, VisEventQueue* events);
  int inp_wavein_upload  (VisPluginData *plugin, VisAudio* audio);

  int const inp_wavein_var_samplerate = 44100;
  int const inp_wavein_var_channels   = 2;

} // anonymous namespace

VISUAL_C_LINKAGE VisPluginInfo const* get_plugin_info (int* count)
{
    static VisInputPlugin input = {
        { 0 },
        inp_wavein_upload
    };

    static VisPluginInfo info = {
        { 0 },
        VISUAL_PLUGIN_TYPE_INPUT,

        "wavein",
        "Wave/In input",
        "Chong Kai Xiong <kaixiong@codeleft.sg>",
        "0.1",
        N_("Wave/In capture plugin"),
        N_("Use this plugin to capture PCM data on Windows through the Wave/In API"),
        VISUAL_PLUGIN_LICENSE_LGPL,

        inp_wavein_init,
        inp_wavein_cleanup,
        inp_wavein_events,

        0,
        VISUAL_OBJECT (&input)
    };

    *count = 1;

    return &info;
}

namespace {

  std::string get_wavein_error_string (MMRESULT error)
  {
      char text[MAXERRORLENGTH];
      waveInGetErrorText (error, text, MAXERRORLENGTH);
      return text;
  }

  void inp_wavein_handle_message (HWAVEIN device, UINT msg, DWORD_PTR instance, DWORD param1, DWORD param2)
  {
      WaveInPrivate* priv = reinterpret_cast<WaveInPrivate*> (instance);

      switch (msg) {
          case WIM_OPEN:
              break;

          case WIM_CLOSE:
              break;

          case WIM_DATA:
              waveInUnprepareHeader (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));

              // Immediately switch to another so we can start reading
              // from the newly filled buffer in inp_wavein_upload()

              int next_buffer = priv->active_buffer ^ 0x1;

              priv->buffer_header.lpData = priv->buffers[next_buffer];
              priv->buffer_header.dwBufferLength = PCM_BUFFER_SIZE;
              waveInPrepareHeader (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));
              waveInAddBuffer (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));

              WaitForSingleObject (priv->mutex, INFINITE);

              priv->buffer_read[priv->active_buffer] = false;
              priv->active_buffer = next_buffer;

              ReleaseMutex (priv->mutex);

              break;
      }
  }

  int inp_wavein_init (VisPluginData* plugin)
  {
      visual_return_val_if_fail (plugin != NULL, -1);

      WaveInPrivate* priv = visual_mem_new0 (WaveInPrivate, 1);
      visual_object_set_private (VISUAL_OBJECT (plugin), priv);

      visual_log (VISUAL_LOG_DEBUG, "Opening default Wave/In device");

      WAVEFORMATEX format = {
          WAVE_FORMAT_PCM,
          2,
          44100,
          44100*2*2,
          2*2,
          16,
          0
      };

      HWAVEIN handle;
      MMRESULT result = waveInOpen (&handle, WAVE_MAPPER, &format, DWORD_PTR (&inp_wavein_handle_message), DWORD_PTR (priv), CALLBACK_FUNCTION);

      if (result != MMSYSERR_NOERROR) {
          visual_log (VISUAL_LOG_ERROR, "Failed to open capture device");
          return -1;
      }

      priv->device_handle = handle;
      priv->loaded = true;
      priv->mutex  = CreateMutex (NULL, FALSE, NULL);
      priv->active_buffer = 0;

      visual_mem_set (priv->buffers[0], 0, PCM_BUFFER_SIZE);
      visual_mem_set (priv->buffers[1], 0, PCM_BUFFER_SIZE);

      priv->buffer_header.lpData = priv->buffers[0];
      priv->buffer_header.dwBufferLength = PCM_BUFFER_SIZE;
      waveInPrepareHeader (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));
      waveInAddBuffer (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));

      visual_log (VISUAL_LOG_DEBUG, "Starting audio capture");

      waveInStart (handle);
      
      return 0;
  }

  int inp_wavein_cleanup (VisPluginData* plugin)
  {
      visual_return_val_if_fail (plugin != NULL, -1);

      WaveInPrivate* priv = static_cast<WaveInPrivate*> (visual_object_get_private (VISUAL_OBJECT (plugin)));
      visual_return_val_if_fail (priv != NULL, -1);

      int retval = 0;

      if (priv->loaded) {
	  visual_log (VISUAL_LOG_DEBUG, "Stopping audio capturing and clearing all pending buffers");

          MMRESULT result = waveInReset (priv->device_handle);
          if (result != MMSYSERR_NOERROR) {
              visual_log (VISUAL_LOG_ERROR, "Failed to clear pending buffers");
              retval = -1;
          }

	  visual_log (VISUAL_LOG_DEBUG, "Closing capture device");

          result = waveInClose (priv->device_handle);
          if (result != MMSYSERR_NOERROR) {
              visual_log (VISUAL_LOG_ERROR, "Failed to close capture device");
              retval = -1;
          }

          CloseHandle (priv->mutex);
      }

      visual_mem_free (priv);

      return retval;
  }

  int inp_wavein_events (VisPluginData* plugin, VisEventQueue* events)
  {
      VisEvent ev;

      while (visual_event_queue_poll (events, &ev)) {
          switch (ev.type) {
              default:; // discard
          }
      }

      return 0;
  }

  int inp_wavein_upload (VisPluginData* plugin, VisAudio* audio)
  {
      visual_return_val_if_fail (plugin != NULL, -1);
      visual_return_val_if_fail (audio  != NULL, -1);

      WaveInPrivate* priv = static_cast<WaveInPrivate*> (visual_object_get_private (VISUAL_OBJECT (plugin)));
      visual_return_val_if_fail (priv != NULL, -1);

      int buffer_to_read;

      while (true) {
          WaitForSingleObject (priv->mutex, INFINITE);

	  buffer_to_read = priv->active_buffer ^ 0x1;

          if (!priv->buffer_read[buffer_to_read]) {
              priv->buffer_read[buffer_to_read] = true;
              ReleaseMutex (priv->mutex);
              break;
          }

          ReleaseMutex (priv->mutex);
      }

      VisBuffer buffer;
      visual_buffer_init (&buffer, priv->buffers[buffer_to_read], PCM_BUFFER_SIZE, NULL);

      visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
                                     VISUAL_AUDIO_SAMPLE_FORMAT_U16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

      return 0;
  }

} // anonymous namespace
