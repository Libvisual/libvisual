/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_morph.c,v 1.31 2006/01/27 20:18:26 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include <lvconfig.h>
#include "lv_log.h"
#include "lv_morph.h"
#include "lv_log.h"
#include "lv_mem.h"

extern VisList *__lv_plugins_morph;

static int morph_dtor (VisObject *object);

static VisMorphPlugin *get_morph_plugin (VisMorph *morph);

static int morph_dtor (VisObject *object)
{
	VisMorph *morph = VISUAL_MORPH (object);

	if (morph->plugin != NULL)
		visual_plugin_unload (morph->plugin);

	visual_palette_free_colors (&morph->morphpal);

	morph->plugin = NULL;

	return VISUAL_OK;
}

static VisMorphPlugin *get_morph_plugin (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, NULL);
	visual_log_return_val_if_fail (morph->plugin != NULL, NULL);

	morphplugin = VISUAL_MORPH_PLUGIN (morph->plugin->info->plugin);

	return morphplugin;
}

/**
 * @defgroup VisMorph VisMorph
 * @{
 */

/**
 * Gives the encapsulated VisPluginData from a VisMorph.
 *
 * @param morph Pointer of a VisMorph of which the VisPluginData needs to be returned.
 *
 * @return VisPluginData that is encapsulated in the VisMorph, possibly NULL.
 */
VisPluginData *visual_morph_get_plugin (VisMorph *morph)
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
const char *visual_morph_get_next_by_name (const char *name)
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
const char *visual_morph_get_prev_by_name (const char *name)
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
int visual_morph_valid_by_name (const char *name)
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
 * @return A newly allocated VisMorph, optionally containing a loaded plugin. Or NULL on failure.
 */
VisMorph *visual_morph_new (const char *morphname)
{
	VisMorph *morph;

	morph = visual_mem_new0 (VisMorph, 1);

	visual_morph_init (morph, morphname);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (morph), TRUE);
	visual_object_ref (VISUAL_OBJECT (morph));

	return morph;
}

/**
 * Initializes a VisMorph, this will set the allocated flag for the object to FALSE. Should not
 * be used to reset a VisMorph, or on a VisMorph created by visual_morph_new().
 *
 * @see visual_morph_new
 *
 * @param morph Pointer to the VisMorph that is initialized.
 * @param morphname
 *	The name of the plugin to load, or NULL to simply initialize a new morph.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_PLUGIN_NO_LIST on failure.
 */
int visual_morph_init (VisMorph *morph, const char *morphname)
{
	VisPluginRef *ref;

	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	if (__lv_plugins_morph == NULL && morphname != NULL) {
		visual_log (VISUAL_LOG_CRITICAL, _("the plugin list is NULL"));

		return -VISUAL_ERROR_PLUGIN_NO_LIST;
	}

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (morph));
	visual_object_set_dtor (VISUAL_OBJECT (morph), morph_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (morph), FALSE);
	
	/* Reset the VisMorph data */
	morph->plugin = NULL;
	morph->dest = NULL;
	visual_palette_init (&morph->morphpal);
	visual_time_init (&morph->morphtime);
	visual_timer_init (&morph->timer);
	visual_morph_set_rate (morph, 0);
	visual_morph_set_steps (morph, 0);
	morph->stepsdone = 0;

	visual_palette_allocate_colors (&morph->morphpal, 256);

	visual_morph_set_mode (morph, VISUAL_MORPH_MODE_SET);

	if (morphname == NULL)
		return VISUAL_OK;

	ref = visual_plugin_find (__lv_plugins_morph, morphname);

	morph->plugin = visual_plugin_load (ref);

	return VISUAL_OK;
}

/**
 * Realize the VisMorph. This also calls the plugin init function.
 *
 * @param morph Pointer to a VisMorph that needs to be realized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_PLUGIN_NULL or error values
 *	returned by visual_plugin_realize () on failure.
 */
int visual_morph_realize (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_log_return_val_if_fail (morph->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	return visual_plugin_realize (morph->plugin);
}

/**
 * Gives the by the plugin natively supported depths
 *
 * @param morph Pointer to a VisMorph of which the supported depth of it's
 * 	  encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_CONTEXT_* values which can be checked against using AND on succes, -1 on failure
 */
int visual_morph_get_supported_depth (VisMorph *morph)
{
	VisPluginData *plugin;
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_log_return_val_if_fail (morph->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL)
		return -VISUAL_ERROR_MORPH_PLUGIN_NULL;

	return morphplugin->vidoptions.depth;
}

VisVideoAttributeOptions *visual_morph_get_video_attribute_options (VisMorph *morph)
{
	VisPluginData *plugin;
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, NULL);
	visual_log_return_val_if_fail (morph->plugin != NULL, NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL)
		return NULL;

	return &morphplugin->vidoptions;
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_morph_set_video (VisMorph *morph, VisVideo *video)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	morph->dest = video;

	return VISUAL_OK;
}

/**
 * Set the time when the morph should be finished morphing.
 * The VisMorph keeps a local copy of the given time.
 *
 * @param morph Pointer to the VisMorph to which finish time is set.
 * @param time Pointer to the VisTime that contains the finish time.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_TIME_NULL or error values returned by
 * 	visual_time_copy () on failure.
 */
int visual_morph_set_time (VisMorph *morph, VisTime *time)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_log_return_val_if_fail (time != NULL, -VISUAL_ERROR_TIME_NULL);

	return visual_time_copy (&morph->morphtime, time);
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_rate (VisMorph *morph, float rate)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->rate = rate;

	return VISUAL_OK;
}

/**
 * Used to set the number of steps that a morph will take to finish.
 *
 * @param morph Pointer to a VisMorph to which the number of morph steps is set.
 * @param steps The number of steps that a morph should take.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_steps (VisMorph *morph, int steps)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->steps = steps;

	return VISUAL_OK;
}

/**
 * Used to set the method of morphing.
 *
 * @param morph Pointer to a VisMorph to which the method of morphing is set.
 * @param mode Method of morphing that is of type VisMorphMode.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_mode (VisMorph *morph, VisMorphMode mode)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->mode = mode;

	return VISUAL_OK;
}

/**
 * Some morph plugins can give a custom palette while morphing two 8 bits plugins.
 *
 * @param morph Pointer to a VisMorph of which the palette needs to be retrieved.
 *
 * @return The pointer to the custom palette on succes or NULL on failure.
 */
VisPalette *visual_morph_get_palette (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, NULL);

	return &morph->morphpal;
}

/**
 * Function that helps to check if a morph is done with it's morphing.
 *
 * @param morph Pointer to a VisMorph of which we want to know if it's done yet.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_is_done (VisMorph *morph)
{
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	if (morph->mode == VISUAL_MORPH_MODE_SET)
		return FALSE;

	if (morph->rate >= 1.0) {
		if (morph->mode == VISUAL_MORPH_MODE_TIME)
			visual_timer_stop (&morph->timer);

		if (morph->mode == VISUAL_MORPH_MODE_STEPS)
			morph->stepsdone = 0;

		return TRUE;
	}

	/* Always be sure ;) */
	if (morph->mode == VISUAL_MORPH_MODE_STEPS && morph->steps == morph->stepsdone)
		return TRUE;

	return FALSE;
}

/**
 * Some morph plugins request an VisAudio context to draw properly. Using this function
 * you can check if the VisMorphPlugin being used in the VisMorph requests this.
 *
 * @param morph Pointer to a VisMorph of which we want to know if it wants a VisAudio.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_MORPH_PLUGIN_NULL on failure. 
 */
int visual_morph_requests_audio (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morphplugin = get_morph_plugin (morph);
	
	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			_("The given morph does not reference any plugin"));

		return -VISUAL_ERROR_MORPH_PLUGIN_NULL;
	}

	return morphplugin->requests_audio;
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_AUDIO_NULL,
 * 	-VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_NULL on failure.
 */ 
int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2)
{
	VisMorphPlugin *morphplugin;
	VisTime elapsed;
	double usec_elapsed, usec_morph;
	
	visual_log_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_log_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			_("The given morph does not reference any plugin"));

		return -VISUAL_ERROR_MORPH_PLUGIN_NULL;
	}
	
	/* If we're morphing using the timer, start the timer. */
	if (visual_timer_is_active (&morph->timer) == FALSE)
		visual_timer_start (&morph->timer);

	if (morphplugin->palette != NULL)
		morphplugin->palette (morph->plugin, morph->rate, audio, &morph->morphpal, src1, src2);
	else {
		if (src1->pal != NULL && src2->pal != NULL)
			visual_palette_blend (&morph->morphpal, src1->pal, src2->pal, morph->rate);
	}

	morphplugin->apply (morph->plugin, morph->rate, audio, morph->dest, src1, src2);

	morph->dest->pal = visual_morph_get_palette (morph);

	/* On automatic morphing increase the rate. */
	if (morph->mode == VISUAL_MORPH_MODE_STEPS) {
		morph->rate += (1.000 / morph->steps);
		morph->stepsdone++;

		if (morph->rate > 1.0)
			morph->rate = 1;

	} else if (morph->mode == VISUAL_MORPH_MODE_TIME) {
		visual_timer_elapsed (&morph->timer, &elapsed);

		/**
		 * @todo: We might want to have a bigger type here, but long longs aren't atomic
		 * on most architectures, so that won't do for now, maybe when we can lock (for threading)
		 * we can look into that
		 */
		usec_elapsed = ((double) elapsed.tv_sec) * VISUAL_USEC_PER_SEC + elapsed.tv_usec;
		usec_morph = ((double) morph->morphtime.tv_sec) * VISUAL_USEC_PER_SEC + morph->morphtime.tv_usec;

		morph->rate = usec_elapsed / usec_morph;

		if (morph->rate > 1.0)
			morph->rate = 1;
	}


	return VISUAL_OK;
}

/**
 * @}
 */

