/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_INPUT_H
#define _LV_INPUT_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_plugin.h>

/**
 * @defgroup VisInput VisInput
 * @{
 */

#ifdef __cplusplus

#include <libvisual/lv_intrusive_ptr.hpp>
#include <functional>
#include <memory>
#include <string_view>

namespace LV {

  class Input;

  typedef LV::IntrusivePtr<Input> InputPtr;

  class LV_API Input
  {
  public:

      /**
       * Determines if an input plugin by the given name is available.
       *
       * @param name Name of input plugin to check for
       *
       * @return True if an input plugin by that name is available, else false
       */
      static bool available (std::string_view name);

      /**
       * Creates a new Input with a plugin of a given name.
       *
       * @param name Name of plugin to load
       *
       * @return A new Input, or nullptr on failure.
       */
      static InputPtr load (std::string_view name);

      ~Input ();

      /**
       * Returns the plugin object.
       *
       * @return Plugin object
       */
      VisPluginData* get_plugin ();

      /**
       * Realizes this Input.
       *
       * @return true on success, false otherwise
       */
      bool realize ();

      /**
       * Sets a PCM data callback.
       *
       * Used for adding a custom upload function.
       *
       * @note Setting a callback will bypass the plugin upload() method.
       *
       * @param callback  Callback
       */
      void set_callback (std::function<bool (Audio&)> const& callback);

      Audio const& get_audio ();

      /**
       * Runs this Input.
       *
       * This function will call the plugin upload() method to
       * retrieve audio samples. If a custom callback is set via
       * set_callback(), the callback will be used instead.
       *
       * @return true on success, false otherwise
       */
      bool run ();

  private:

      friend void intrusive_ptr_add_ref (Input const* input);
      friend void intrusive_ptr_release (Input const* input);

      class Impl;
      const std::unique_ptr<Impl> m_impl;

      mutable unsigned int m_ref_count;

      explicit Input (std::string_view name);
  };

  inline void intrusive_ptr_add_ref (Input const* input)
  {
      input->m_ref_count++;
  }

  inline void intrusive_ptr_release (Input const* input)
  {
      if (--input->m_ref_count == 0) {
          delete input;
      }
  }

} // LV namespace

typedef LV::Input VisInput;

#else

typedef struct _VisInput VisInput;
struct _VisInput;

#endif /* __cplusplus */

/**
 * Input plugin type.
 */
typedef struct _VisInputPlugin VisInputPlugin;

/**
 * Function signature and type of input custom PCM data callbacks.
 *
 * @see visual_input_set_callback()
 *
 * @param input     Input object
 * @param audio     Audio object to upload data to
 * @param user_data Data set in visual_input_set_callback()
 */
typedef int (*VisInputUploadCallbackFunc)(VisInput *input, VisAudio *audio, void *user_data);

/**
 * Function signature and type of the Input upload() method.
 *
 * The upload() method is called to produce audio samples for rendering by Actors.
 *
 * @param plugin Plugin object
 * @param audio  Audio object to upload data to
 *
 * @return 0 on success, -1 on error.
 */
typedef int (*VisPluginInputUploadFunc)(VisPluginData *plugin, VisAudio *audio);

/**
 * Input plugin class.
 */
struct _VisInputPlugin {
    VisPluginInputUploadFunc upload;    /**< Sample upload function */
};

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisInput *visual_input_new (const char *name);

LV_API void visual_input_ref     (VisInput *input);
LV_API void visual_input_unref   (VisInput *input);
LV_API int  visual_input_realize (VisInput *input);
LV_API int  visual_input_run     (VisInput *input);

LV_API VisPluginData *visual_input_get_plugin  (VisInput *input);
LV_API VisAudio      *visual_input_get_audio    (VisInput *audio);
LV_API void           visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *user_data);

/**
 * Returns the name of the next available input plugin.
 *
 * @see visual_input_get_prev_by_name()
 *
 * @param name Name of current plugin, or NULL to get the first
 *
 * @return Name of next available plugin
 */
LV_API const char *visual_input_get_next_by_name (const char *name);

/**
 * Returns the name of the previous available input plugin.
 *
 * @see visual_input_get_next_by_name()
 *
 * @param name Name of current plugin, or NULL to get the last
 *
 * @return Name of previous available plugin
 */
LV_API const char *visual_input_get_prev_by_name (const char *name);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_INPUT_H */
