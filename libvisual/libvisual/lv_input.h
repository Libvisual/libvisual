/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_input.h,v 1.17 2006/01/22 13:23:37 synap Exp $
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
 * Callback function that is set using visual_input_set_callback should use this signature.
 *
 * @see visual_input_set_callback
 *
 * @arg input Pointer to the VisInput structure.
 *
 * @arg audio Pointer to the VisAudio containing all the audio information, and in which
 * data needs to be set within the callback.
 *
 * @arg priv Private field to be used by the client. The library will never touch this.
 */
typedef int (*VisInputUploadCallbackFunc)(VisInput *input, VisAudio *audio, void *priv);

/* Input plugin methods */

/**
 * An input plugin needs this signature for the sample upload function. The sample upload function
 * is used to retrieve sample information when a input is being used to retrieve the
 * audio sample.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg audio Pointer to the VisAudio in which the new sample data is set.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginInputUploadFunc)(VisPluginData *plugin, VisAudio *audio);

/**
 * The VisInput structure encapsulates the input plugin and provides
 * abstract interfaces to the input. The VisInput system provides
 * PCM data to the visualisation elements of libvisual. This can be done
 * through both plugins and callback functions.
 *
 * Members in the structure shouldn't be accessed directly but instead
 * it's adviced to use the methods provided.
 *
 * @see visual_input_new
 */
struct _VisInput {
    VisObject                    object;    /**< The VisObject data. */

    VisPluginData               *plugin;    /**< Pointer to the plugin itself. */
    VisAudio                    *audio;     /**< Pointer to the VisAudio structure
                                               * that contains the audio analyse
                                               * results. @see visual_audio_analyse */
    VisInputUploadCallbackFunc   callback;  /**< Callback function when a callback
                                              * is used instead of a plugin. */
    VisSongInfo                 *songinfo;
};

/**
 * The VisInputPlugin structure is the main data structure
 * for the input plugin.
 *
 * The input plugin is used to retrieve PCM samples from
 * certain sources.
 */
struct _VisInputPlugin {
    VisObject            object;    /**< The VisObject data. */
    VisPluginInputUploadFunc     upload;    /**< The sample upload function. This is the main function
                              * of the plugin which uploads sample data into
                              * libvisual. */
};

LV_BEGIN_DECLS

/**
 * Gives the encapsulated VisPluginData from a VisInput.
 *
 * @param input Pointer of a VisInput of which the VisPluginData needs to be returned.
 *
 * @return VisPluginData that is encapsulated in the VisInput, possibly NULL.
 */
LV_API VisPluginData *visual_input_get_plugin (VisInput *input);

/**
 * Gives a list of input plugins in the current plugin registry.
 *
 * @return An VisList of VisPluginRef's containing the input plugins in the plugin registry.
 */
LV_API VisList *visual_input_get_list (void);

/**
 * Gives the next input plugin based on the name of a plugin.
 *
 * @see visual_input_get_prev_by_name
 *
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
LV_API const char *visual_input_get_next_by_name (const char *name);

/**
 * Gives the previous input plugin based on the name of a plugin.
 *
 * @see visual_input_get_next_by_name
 *
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
LV_API const char *visual_input_get_prev_by_name (const char *name);

/**
 * Creates a new VisInput from name, the plugin will be loaded but won't be realized.
 *
 * @param inputname
 *  The name of the plugin to load, or NULL to simply allocate a new
 *  input.
 *
 * @return A newly allocated VisInput, optionally containing a loaded plugin. Or NULL on failure.
 */
LV_API VisInput *visual_input_new (const char *inputname);

/**
 * Initializes a VisInput, this will set the allocated flag for the object to FALSE. Should not
 * be used to reset a VisInput, or on a VisInput created by visual_input_new().
 *
 * @see visual_input_new
 *
 * @param input Pointer to the VisInput that is initialized.
 * @param inputname
 *  The name of the plugin to load, or NULL to simply initialize a new input.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_INPUT_NULL or -VISUAL_ERROR_PLUGIN_NO_LIST on failure.
 */
LV_API int visual_input_init (VisInput *input, const char *inputname);

/**
 * Realize the VisInput. This also calls the plugin init function.
 *
 * @param input Pointer to a VisInput that needs to be realized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_INPUT_NULL or error values returned by
 *  visual_plugin_realize () on failure.
 */
LV_API int visual_input_realize (VisInput *input);


/**
 * Sets a callback function for VisInput. Callback functions can be used instead of plugins. Using
 * a callback function you can implement an in app PCM data upload function which is like the
 * upload callback that is used for input plugins.
 *
 * @param input Pointer to a VisInput that to which a callback needs to be set.
 * @param callback The in app callback function that should be used instead of a plugin.
 * @param priv A private that can be read within the callback function.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_INPUT_NULL on failure.
 */
LV_API int visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *priv);

/**
 * This is called to run a VisInput. This function will call the plugin to upload it's samples and run it
 * through the visual_audio_analyze function. If a callback is set it will use the callback instead of
 * the plugin.
 *
 * @param input A pointer to a VisInput that needs to be runned.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_INPUT_NULL or -VISUAL_ERROR_INPUT_PLUGIN_NULL on failure.
 */
LV_API int visual_input_run (VisInput *input);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_INPUT_H */
