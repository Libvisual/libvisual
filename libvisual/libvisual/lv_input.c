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

	inplugin = input->plugin->plugin.inputplugin;

	return inplugin;
}

/**
 * @defgroup VisInput VisInput
 * @{
 */

/**
 * Gives the encapsulated LVPlugin from a VisInput.
 *
 * @param input Pointer of a VisInput of which the LVPlugin needs to be returned.
 *
 * @return LVPlugin that is encapsulated in the VisInput, possibly NULL.
 */
LVPlugin *visual_input_get_plugin (VisInput *input)
{
	        return input->plugin;
}

/**
 * Gives a list of input plugins in the current plugin registry.
 *
 * @return An VisList containing the input plugins in the plugin registry.
 */
VisList *visual_input_get_list ()
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
char *visual_input_get_next_by_name (char *name)
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
char *visual_input_get_prev_by_name (char *name)
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
int visual_input_valid_by_name (char *name)
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
 * @return A newly allocated VisInput, optionally containing a loaded plugin. Or NULL on error.
 */
VisInput *visual_input_new (char *inputname)
{
	VisInput *input;
	VisPluginRef *ref;

	if (__lv_plugins_input == NULL && inputname != NULL)
		return NULL;

	input = malloc (sizeof (VisInput));
	memset (input, 0, sizeof (VisInput));

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
 * @return 0 on succes -1 on error.
 */
int visual_input_realize (VisInput *input)
{
	if (input == NULL)
		return -1;

	if (input->plugin != NULL && input->callback == NULL)
		visual_plugin_realize (input->plugin);

	return 0;
}

/**
 *  Destroy the VisInput. This unloads the plugin when it's loaded, and also frees the input itself including
 *  all it's members.
 *
 *  @param input Pointer to a VisInput that needs to be destroyed.
 *
 *  @return 0 on succes -1 on error.
 */
int visual_input_destroy (VisInput *input)
{
	if (input == NULL)
		return -1;

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
 * @return 0 on succes -1 on error.
 */
int visual_input_free (VisInput *input)
{
	if (input == NULL)
		return -1;

	if (input->audio != NULL)
		visual_audio_free (input->audio);
	
	free (input);
	
	return 0;
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
 * @return 0 on succes -1 on error.
 */
int visual_input_set_callback (VisInput *input, input_upload_callback_func_t callback, void *priv)
{
	if (input == NULL)
		return -1;

	input->callback = callback;
	input->priv = priv;

	return 0;
}

/**
 * This is called to run a VisInput. This function will call the plugin to upload it's samples and run it
 * through the visual_audio_analyze function. If a callback is set it will use the callback instead of
 * the plugin.
 *
 * @param input A pointer to a VisInput that needs to be runned.
 *
 * @return 0 on succes -1 on error.
 */
int visual_input_run (VisInput *input)
{
	VisInputPlugin *inplugin;
	
	if (input == NULL)
		return -1;

	if (input->callback == NULL) {
		inplugin = get_input_plugin (input);	

		inplugin->upload (inplugin, input->audio);
	} else
		input->callback (input, input->audio, input->priv);

	visual_audio_analyze (input->audio);

	return 0;
}

/**
 * @}
 */

