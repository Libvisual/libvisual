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

#include <libvisual/lv_object.h>
#include <libvisual/lv_audio.h>
#include <libvisual/lv_plugin.h>

/**
 * @defgroup VisInput VisInput
 * @{
 */

#define VISUAL_INPUT(obj)               (VISUAL_CHECK_CAST ((obj), VisInput))
#define VISUAL_INPUT_PLUGIN(obj)            (VISUAL_CHECK_CAST ((obj), VisInputPlugin))

/**
 * Type defination that should be used in plugins to set the plugin type for an input  plugin.
 */

typedef struct _VisInput VisInput;
typedef struct _VisInputPlugin VisInputPlugin;

/**
 * Function type and signature for input custom PCM data callbacks.
 *
 * @see visual_input_set_callback()
 *
 * @param input     Input object
 * @param audio     Audio object to upload data to
 * @param user_data Data set in visual_input_set_callback()
 */
typedef int (*VisInputUploadCallbackFunc)(VisInput *input, VisAudio *audio, void *user_data);

/**
 * Function type and siganture of VisInput upload() method.
 *
 * @param plugin Plugin object
 * @param audio  Audio object to upload data to
 *
 * @return 0 on success, -1 on error.
 */
typedef int (*VisPluginInputUploadFunc)(VisPluginData *plugin, VisAudio *audio);

/**
 * Input class.
 */
struct _VisInput;

/**
 * Input plugin class.
 */
struct _VisInputPlugin {
    VisObject                object;    /**< Parent class. */
    VisPluginInputUploadFunc upload;    /**< Sample upload function */
};

LV_BEGIN_DECLS

/**
 * Returns the plugin object that backs an Input.
 *
 * @param input Input object
 *
 * @return Plugin object, or NULL if none
 */
LV_API VisPluginData *visual_input_get_plugin (VisInput *input);

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

/**
 * Creates a new Input with a plugin of a given name.
 *
 * @see visual_input_realize()
 *
 * @param name Name of plugin to load, or NULL to allocate an empty object
 *
 * @return A new Input, or NULL on failure.
 */
LV_API VisInput *visual_input_new (const char *name);

/**
 * Realizes an Input.
 *
 * @param input Input object
 *
 * @return VISUAL_OK on success
 */
LV_API int visual_input_realize (VisInput *input);

/**
 * Sets a PCM data callback for this Input.
 *
 * Used for adding a custom upload function.
 *
 * @note Setting a callback will bypass the plugin upload() method.
 *
 * @param input     Input object
 * @param callback  Callback
 * @param user_data Pointer passed to callback during invocations
 *
 * @return VISUAL_OK on success
 */
LV_API int visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *user_data);

LV_API VisAudio *visual_input_get_audio (VisInput *audio);

/**
 * Runs the Input.
 *
 * This function will call the plugin upload() method to retrieve audio samples. If a custom callback is set
 * via visual_input_set_callback(), the callback will be used instead.
 *
 * @param input Input object
 *
 * @return VISUAL_OK on success
 */
LV_API int visual_input_run (VisInput *input);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_INPUT_H */
