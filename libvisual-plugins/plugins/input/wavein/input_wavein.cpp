/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2012 Chong Kai Xiong <kaixiong@codeleft.sg>
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

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define PCM_BUFFER_SIZE 1024

struct WaveInPrivate {
    HWAVEIN device_handle;
    bool    loaded;

    WAVEHDR buffer_header;
    int16_t buffers[2][PCM_BUFFER_SIZE];

    bool    buffer_ready[2];
    int     active_buffer;
    HANDLE  mutex;
};

namespace {

  int  inp_wavein_init    (VisPluginData *plugin);
  void inp_wavein_cleanup (VisPluginData *plugin);
  int  inp_wavein_events  (VisPluginData *plugin, VisEventQueue* events);
  int  inp_wavein_upload  (VisPluginData *plugin, VisAudio* audio);

} // anonymous namespace

VisPluginInfo const* get_plugin_info ()
{
    static VisInputPlugin input = {
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
        &input
    };

    return &info;
}

namespace {

  void log_wavein_error (std::string const& message, MMRESULT error)
  {
      char error_text[MAXERRORLENGTH];
      waveInGetErrorText (error, error_text, MAXERRORLENGTH);

      visual_log (VISUAL_LOG_ERROR, "%s: %s", message.c_str (), error_text);
  }

  void prepare_buffer (WaveInPrivate* priv, unsigned int buffer_id)
  {
      priv->buffer_ready[buffer_id] = false;

      priv->buffer_header.lpData = reinterpret_cast<LPSTR> (priv->buffers[buffer_id]);
      priv->buffer_header.dwBufferLength = PCM_BUFFER_SIZE * sizeof (int16_t);

      waveInPrepareHeader (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));
      waveInAddBuffer (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));
  }

  void CALLBACK handle_wavein_message (HWAVEIN device, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2)
  {
      DWORD thread_id = DWORD (instance);

      // Forward the message to our processing thread
      PostThreadMessage (thread_id, msg, WPARAM (device), LPARAM (param1));
  }

  DWORD WINAPI process_buffers (LPVOID param)
  {
      WaveInPrivate* priv = reinterpret_cast<WaveInPrivate*> (param);

      MSG message;

      while (true) {
          if (PeekMessage (&message, HWND (-1), 0, 0, PM_REMOVE)) {
              switch (message.message) {
                  case WIM_OPEN:
                      // Nothing to do
                      break;

                  case WIM_CLOSE:
                      // Exit if device is closed. We use return instead of ExitThread() so
                      // that C++ destructors can run
                      return 0;

                  case WIM_DATA:
                      WaitForSingleObject (priv->mutex, INFINITE);

                      waveInUnprepareHeader (priv->device_handle, &priv->buffer_header, sizeof (WAVEHDR));
                      priv->buffer_ready[priv->active_buffer] = true;

                      priv->active_buffer ^= 0x1;
                      prepare_buffer (priv, priv->active_buffer);

                      ReleaseMutex (priv->mutex);

                      break;
              }
          }

          SwitchToThread ();
      }
  }

  bool check_available_devices ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Checking the number of input devices available");

      UINT num_devices = waveInGetNumDevs ();
      if (num_devices == 0) {
          return false;
      }

      WAVEINCAPS device_caps;

      for (unsigned int device_id = 0; device_id < num_devices; device_id++) {
          visual_log (VISUAL_LOG_DEBUG, "Querying device #%u", device_id);

          MMRESULT result = waveInGetDevCaps (UINT_PTR (device_id), &device_caps, sizeof (WAVEINCAPS));
          if (result != MMSYSERR_NOERROR) {
              log_wavein_error ("Failed to query device capabilities", result);
              continue;
          }

          visual_log (VISUAL_LOG_DEBUG, "Name: %s", device_caps.szPname);
          visual_log (VISUAL_LOG_DEBUG, "Supported format flags: 0x%08x", (unsigned int) device_caps.dwFormats);
          visual_log (VISUAL_LOG_DEBUG, "Number of channels: %d", device_caps.wChannels);
      }

      return true;
  }

  int inp_wavein_init (VisPluginData* plugin)
  {
#if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

      if (!check_available_devices ()) {
          visual_log (VISUAL_LOG_ERROR, "No input device can be found!");
          return FALSE;
      }

      auto priv = visual_mem_new0 (WaveInPrivate, 1);
      visual_plugin_set_private (plugin, priv);

      visual_log (VISUAL_LOG_DEBUG, "Querying audio formats supported by input device");

      WAVEFORMATEX format = {
          WAVE_FORMAT_PCM, // Audio format
          2,               // Number of channels
          44100,           // Samples per second
          44100*2*2,       // Bytes per second (samples per second * block alignment
          2*2,             // Block alignment (channels * bits per sample / 8)
          16,              // Bits per sample
          0                // Size of extra format information
      };

      // Check if the input device supports our audio format

      MMRESULT result = waveInOpen (&priv->device_handle, WAVE_MAPPER, &format, 0, 0, WAVE_FORMAT_QUERY);

      if (result == WAVERR_BADFORMAT) {
          log_wavein_error ("Required audio format is not supported by device", result);
          return FALSE;
      }

      // Create background processing thread

      visual_log (VISUAL_LOG_DEBUG, "Creating processing thread");

      DWORD thread_id;
      HANDLE thread = CreateThread (0, 0, process_buffers, priv, CREATE_SUSPENDED, &thread_id);

      // Open the input device

      visual_log (VISUAL_LOG_DEBUG, "Opening input device");

      result = waveInOpen (&priv->device_handle, WAVE_MAPPER, &format, DWORD_PTR (&handle_wavein_message),
                           DWORD_PTR (thread_id), CALLBACK_FUNCTION);

      if (result != MMSYSERR_NOERROR) {
          // FIXME: Do we need to destroy the thread?
          log_wavein_error ("Failed to open capture device", result);
          return FALSE;
      }

      priv->loaded = true;

      priv->mutex = CreateMutex (NULL, FALSE, NULL);

      // Initialize buffer statuses

      priv->buffer_ready[0] = false;
      priv->buffer_ready[1] = false;

      // Register the first buffer with the API for capture

      priv->active_buffer = 0;
      prepare_buffer (priv, priv->active_buffer);

      // Start capturing samples

      visual_log (VISUAL_LOG_DEBUG, "Starting audio capture");

      ResumeThread (thread);

      waveInStart (priv->device_handle);

      return TRUE;
  }

  void inp_wavein_cleanup (VisPluginData* plugin)
  {
      auto priv = static_cast<WaveInPrivate*> (visual_plugin_get_private (plugin));

      if (priv->loaded) {
          visual_log (VISUAL_LOG_DEBUG, "Stopping audio capturing and clearing all pending buffers");

          MMRESULT result = waveInReset (priv->device_handle);
          if (result != MMSYSERR_NOERROR) {
              log_wavein_error ("Failed to clear pending buffers", result);
          }

          visual_log (VISUAL_LOG_DEBUG, "Closing capture device");

          result = waveInClose (priv->device_handle);
          if (result != MMSYSERR_NOERROR) {
              log_wavein_error ("Failed to close capture device", result);
          }

          CloseHandle (priv->mutex);
      }

      visual_mem_free (priv);
  }

  int inp_wavein_events (VisPluginData* plugin, VisEventQueue* events)
  {
      VisEvent ev;

      while (visual_event_queue_poll (events, &ev)) {
          switch (ev.type) {
              default:; // discard
          }
      }

      return TRUE;
  }

  int inp_wavein_upload (VisPluginData* plugin, VisAudio* audio)
  {
      auto priv = static_cast<WaveInPrivate*> (visual_plugin_get_private (plugin));

      int buffer_to_read;

      while (true) {
          WaitForSingleObject (priv->mutex, INFINITE);

          buffer_to_read = priv->active_buffer ^ 0x1;

          if (priv->buffer_ready[buffer_to_read]) {
              LV::BufferPtr buffer = LV::Buffer::wrap (priv->buffers[buffer_to_read], PCM_BUFFER_SIZE*2*sizeof(int16_t), false);

              audio->input (buffer,
                            VISUAL_AUDIO_SAMPLE_RATE_44100,
                            VISUAL_AUDIO_SAMPLE_FORMAT_S16,
                            VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

              priv->buffer_ready[buffer_to_read] = false;

              ReleaseMutex (priv->mutex);

              break;
          }

          ReleaseMutex (priv->mutex);
      }

      return TRUE;
  }

} // anonymous namespace

