#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_list.h"
#include "lv_input.h"

extern VisList *__lv_plugins_input;

static VisInputPlugin *get_input_plugin (VisInput *input);
                                                                                                                                               
static VisInputPlugin *get_input_plugin (VisInput *input)
{
	VisInputPlugin *inplugin;

	visual_log_return_val_if_fail (input != NULL, NULL);
	visual_log_return_val_if_fail (input->plugin != NULL, NULL);

	inplugin = input->plugin->info->plugin;

	return inplugin;
}

/**
 * @defgroup VisInput VisInput
 * @{
 */

/**
 * Gives the encapsulated VisPluginData from a VisInput.
 *
 * @param input Pointer of a VisInput of which the VisPluginData needs to be returned.
 *
 * @return VisPluginData that is encapsulated in the VisInput, possibly NULL.
 */
VisPluginData *visual_input_get_plugin (VisInput *input)
{
	        return input->plugin;
}

/**
 * Gives a list of input plugins in the current plugin registry.
 *
 * @return An VisList of VisPluginRef's containing the input plugins in the plugin registry.
 */
const VisList *visual_input_get_list ()
{
	return __lv_plugins_input;
}

/**
 * Gives the next input plugin based on the name of a plugin.
 *
 * @see visual_input_get_prev_by_name
 *
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
const char *visual_input_get_next_by_name (const char *name)
{
	return visual_plugin_get_next_by_name (visual_input_get_list (), name);
}

/**
 * Gives the previous input plugin based on the name of a plugin.
 *
 * @see visual_input_get_next_by_name
 *
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
const char *visual_input_get_prev_by_name (const char *name)
{
	return visual_plugin_get_prev_by_name (visual_input_get_list (), name);
}


/**
 * Checks if the input plugin is in the registry, based on it's name.
 *
 * @param name The name of the plugin that needs to be checked.
 *
 * @return TRUE if found, else FALSE.
 */
int visual_input_valid_by_name (const char *name)
{
	if (visual_plugin_find (visual_input_get_list (), name) == NULL)
		return FALSE;
	else
		return TRUE;
}

/**
 * Creates a new VisInput from name, the plugin will be loaded but won't be realized.
 *
 * @param inputname
 * 	The name of the plugin to load, or NULL to simply allocate a new
 * 	input.
 *
 * @return A newly allocated VisInput, optionally containing a loaded plugin. Or NULL on failure.
 */
VisInput *visual_input_new (const char *inputname)
{
	VisInput *input;
	VisPluginRef *ref;

//	visual_log_return_val_if_fail (__lv_plugins_input != NULL && inputname == NULL, NULL);

	if (__lv_plugins_input == NULL && inputname != NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "the plugin list is NULL");
		return NULL;
	}
	
	input = visual_mem_new0 (VisInput, 1);
	
	input->audio = visual_audio_new ();
	
	if (inputname == NULL)
		return input;
	
	ref = visual_plugin_find (__lv_plugins_input, inputname);
	
	input->plugin = visual_plugin_load (ref);

	return input;
}

/**
 * Realize the VisInput. This also calls the plugin init function.
 * 
 * @param input Pointer to a VisInput that needs to be realized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_INPUT_NULL or error values returned by
 *	visual_plugin_realize () on failure.
 */
int visual_input_realize (VisInput *input)
{
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

	if (input->plugin != NULL && input->callback == NULL)
		return visual_plugin_realize (input->plugin);

	return VISUAL_OK;
}

/**
 * Destroy the VisInput. This unloads the plugin when it's loaded, and also frees the input itself including
 * all it's members.
 *
 * @param input Pointer to a VisInput that needs to be destroyed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_INPUT_NULL or error values returned by visual_input_free ()
 *	on failure.
 */
int visual_input_destroy (VisInput *input)
{
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

	if (input->plugin != NULL)
		visual_plugin_unload (input->plugin);

	return visual_input_free (input);
}

/**
 * Free the VisInput. This frees the VisInput data structure, but does not destroy the plugin within. If this is desired
 * use visual_input_destroy.
 *
 * @see visual_input_destroy
 *
 * @param input Pointer to a VisInput that needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_INPUT_NULL or error values returned by visual_mem_free () on failure.
 */
int visual_input_free (VisInput *input)
{
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

	if (input->audio != NULL)
		visual_audio_free (input->audio);
	
	return visual_mem_free (input);
}

/**
 * Sets a callback function for VisInput. Callback functions can be used instead of plugins. Using
 * a callback function you can implement an in app PCM data upload function which is like the
 * upload callback that is used for input plugins.
 *
 * @param input Pointer to a VisInput that to which a callback needs to be set.
 * @param callback The in app callback function that should be used instead of a plugin.
 * @param priv A private that can be read within the callback function.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_INPUT_NULL on failure.
 */
int visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *priv)
{
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

	input->callback = callback;
	input->priv = priv;

	return VISUAL_OK;
}

/**
 * This is called to run a VisInput. This function will call the plugin to upload it's samples and run it
 * through the visual_audio_analyze function. If a callback is set it will use the callback instead of
 * the plugin.
 *
 * @param input A pointer to a VisInput that needs to be runned.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_INPUT_NULL or -VISUAL_ERROR_INPUT_PLUGIN_NULL on failure.
 */
int visual_input_run (VisInput *input)
{
	VisInputPlugin *inplugin;

	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_INPUT_NULL);

	if (input->callback == NULL) {
		inplugin = get_input_plugin (input);

		if (inplugin == NULL) {
			visual_log (VISUAL_LOG_CRITICAL, "The input plugin is not loaded correctly.");
		
			return -VISUAL_ERROR_INPUT_PLUGIN_NULL;
		}
		
		inplugin->upload (input->plugin, input->audio);
	} else
		input->callback (input, input->audio, input->priv);

	visual_audio_analyze (input->audio);

	return VISUAL_OK;
}

/**
 * @}
 */

