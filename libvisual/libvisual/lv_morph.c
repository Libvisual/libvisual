#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <lvconfig.h>
#include "lv_log.h"
#include "lv_morph.h"
#include "lv_log.h"
#include "lv_mem.h"

extern VisList *__lv_plugins_morph;

static VisMorphPlugin *get_morph_plugin (VisMorph *morph);

static VisMorphPlugin *get_morph_plugin (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, NULL);
	visual_log_return_val_if_fail (morph->plugin != NULL, NULL);

	morphplugin = morph->plugin->plugin.morphplugin;

	return morphplugin;
}

/**
 * @defgroup VisMorph VisMorph
 * @{
 */

/**
 * Gives the encapsulated LVPlugin from a VisMorph.
 *
 * @param morph Pointer of a VisMorph of which the LVPlugin needs to be returned.
 *
 * @return LVPlugin that is encapsulated in the VisMorph, possibly NULL.
 */
LVPlugin *visual_morph_get_plugin (VisMorph *morph)
{
	        return morph->plugin;
}

/**
 * Gives a list of morph plugins in the current plugin registry.
 *
 * @return a VisList containing the morph plugins in the plugin registry.
 */
VisList *visual_morph_get_list ()
{
	return __lv_plugins_morph;
}

/**
 * Gives the next morph plugin based on the name of a plugin.
 *
 * @see visual_morph_get_prev_by_name
 *
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
char *visual_morph_get_next_by_name (char *name)
{
	return visual_plugin_get_next_by_name (visual_morph_get_list (), name);
}

/**
 * Gives the previous morph plugin based on the name of a plugin.
 *
 * @see visual_morph_get_next_by_name
 *
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
char *visual_morph_get_prev_by_name (char *name)
{
	return visual_plugin_get_prev_by_name (visual_morph_get_list (), name);
}

/**
 * Checks if the morph plugin is in the registry, based on it's name.
 *
 * @param name The name of the plugin that needs to be checked.
 *
 * @return TRUE if found, else FALSE.
 */
int visual_morph_valid_by_name (char *name)
{
	if (visual_plugin_find (visual_morph_get_list (), name) == NULL)
		return FALSE;
	else
		return TRUE;
}

/**
 * Creates a new VisMorph from name, the plugin will be loaded but won't be realized.
 *
 * @param morphname
 * 	The name of the plugin to load, or NULL to simply allocate a new
 * 	morph.
 * 
 * @return A newly allocated VisMorph, optionally containing a loaded plugin. Or NULL on error.
 */  
VisMorph *visual_morph_new (char *morphname)
{
	VisMorph *morph;
	VisPluginRef *ref;

	if (__lv_plugins_morph == NULL && morphname != NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "the plugin list is NULL");
		return NULL;
	}

	morph = visual_mem_new0 (VisMorph, 1);

	visual_palette_allocate_colors (&morph->morphpal, 256);

	if (morphname == NULL)
		return morph;

	ref = visual_plugin_find (__lv_plugins_morph, morphname);

	morph->plugin = visual_plugin_load (ref);

	return morph;
}

/**
 * Realize the VisMorph. This also calls the plugin init function.
 *
 * @param morph Pointer to a VisMorph that needs to be realized.
 *
 * @return 0 on succes -1 on error.
 */
int visual_morph_realize (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, -1);
	visual_log_return_val_if_fail (morph->plugin != NULL, -1);

	visual_plugin_realize (morph->plugin);

	return 0;
}

/**
 * Destroy the VisMorph. This unloads the plugin when it's loaded, and also frees the morph itself including
 * all it's members.
 * 
 * @param morph Pointer to a VisMorph that needs to be destroyed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_morph_destroy (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, -1);

	if (morph->plugin != NULL)
		visual_plugin_unload (morph->plugin);

	return visual_morph_free (morph);
}

/**
 * Free the VisMorph. This frees the VisMorph data structure, but does not destroy the plugin within. If this is desired
 * use visual_morph_destroy.
 *
 * @see visual_morph_destroy
 *
 * @param morph Pointer to a VisMorph that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_morph_free (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, -1);

	visual_palette_free_colors (&morph->morphpal);

	visual_mem_free (morph);

	return 0;
}

/**
 * Gives the by the plugin natively supported depths
 *
 * @param morph Pointer to a VisMorph of which the supported depth of it's
 * 	  encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_CONTEXT_* values which can be checked against using AND on succes, -1 on error
 */
int visual_morph_get_supported_depth (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, -1);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given morph does not reference any plugin");
		return -1;
	}

	return morphplugin->depth;
}

/**
 * Used to connect the target display, or a buffer it's VisVideo to the VisMorph plugin.
 *
 * @see visual_video_new
 *
 * @param morph Pointer to a VisMorph to which the VisVideo needs to be set.
 * @param video Pointer to a VisVideo which contains information about the target display and the pointer
 * 	  to it's screenbuffer.
 *
 * @return 0 on succes -1 on error.
 */
int visual_morph_set_video (VisMorph *morph, VisVideo *video)
{
	visual_log_return_val_if_fail (morph != NULL, -1);
	visual_log_return_val_if_fail (video != NULL, -1);

	morph->dest = video;

	return 0;
}

/**
 * Used to set the rate of the VisMmorph. The rate ranges from 0 to 1
 * and the content of the result depends on the morph plugin being used.
 *
 * @param morph Pointer to a VisMorph to which the rate needs to be set.
 * @param rate Value that sets the rate of the current morph. The rate 
 * 	  contains the amount that is currently being morphed and needs to be
 * 	  manually adjust. The morph system doesn't increase the rate itself.
 *
 * @return 0 on succes -1 on error.
 */
int visual_morph_set_rate (VisMorph *morph, float rate)
{
	visual_log_return_val_if_fail (morph != NULL, -1);

	morph->rate = rate;

	return 0;
}

/**
 * Some morph plugins can give a custom palette while morphing two 8 bits plugins.
 *
 * @param morph Pointer to a VisMorph of which the palette needs to be retrieved.
 *
 * @return The pointer to the custom palette on succes or NULL on error.
 */
VisPalette *visual_morph_get_palette (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, NULL);

	return &morph->morphpal;
}

/**
 * This is called to run the VisMorph. It will put the result in the buffer that is previously
 * set by visual_morph_set_video and also when the morph is being runned in 8 bits mode
 * it will automaticly interpolate between the two palettes if the plugin doesn't have
 * a method for adjusting the palette.
 *
 * Note that all the VisVideo structures being used need to be clones.
 *
 * @param morph Pointer to a VisMorph that needs to be runned.
 * @param audio Pointer to a VisAudio which a morph could use for extra effects
 * @param src1 Pointer to a VisVideo that acts as the first source for the morph.
 * @param src2 Pointer to a VisVideo that acts as the second source for the morph.
 *
 * @return 0 on succes -1 on error,
 */ 
int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2)
{
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, -1);
	visual_log_return_val_if_fail (audio != NULL, -1);
	visual_log_return_val_if_fail (src1 != NULL, -1);
	visual_log_return_val_if_fail (src2 != NULL, -1);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given morph does not reference any plugin");
		return -1;
	}

	if (morphplugin->palette != NULL)
		morphplugin->palette (morphplugin, morph->rate, audio, &morph->morphpal, src1, src2);
	else
		visual_palette_blend (&morph->morphpal, src1->pal, src2->pal, morph->rate);

	morphplugin->apply (morphplugin, morph->rate, audio, morph->dest, src1, src2);

	return 0;
}

/**
 * @}
 */

