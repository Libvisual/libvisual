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

#include "config.h"
#include "lv_morph.h"
#include "lv_common.h"
#include "gettext.h"

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

	visual_return_val_if_fail (morph != NULL, NULL);
	visual_return_val_if_fail (morph->plugin != NULL, NULL);

	morphplugin = VISUAL_MORPH_PLUGIN (morph->plugin->info->plugin);

	return morphplugin;
}

VisPluginData *visual_morph_get_plugin (VisMorph *morph)
{
	        return morph->plugin;
}

VisList *visual_morph_get_list ()
{
	return __lv_plugins_morph;
}

const char *visual_morph_get_next_by_name (const char *name)
{
	return visual_plugin_get_next_by_name (visual_morph_get_list (), name);
}

const char *visual_morph_get_prev_by_name (const char *name)
{
	return visual_plugin_get_prev_by_name (visual_morph_get_list (), name);
}

int visual_morph_valid_by_name (const char *name)
{
	if (visual_plugin_find (visual_morph_get_list (), name) == NULL)
		return FALSE;
	else
		return TRUE;
}

VisMorph *visual_morph_new (const char *morphname)
{
	VisMorph *morph;
	int result;

	morph = visual_mem_new0 (VisMorph, 1);

	result = visual_morph_init (morph, morphname);
	if (result != VISUAL_OK) {
		visual_mem_free (morph);
		return NULL;
	}

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (morph), TRUE);
	visual_object_ref (VISUAL_OBJECT (morph));

	return morph;
}

int visual_morph_init (VisMorph *morph, const char *morphname)
{
	VisPluginRef *ref;

	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	if (__lv_plugins_morph == NULL && morphname != NULL) {
		visual_log (VISUAL_LOG_ERROR, _("the plugin list is NULL"));

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
	if (ref == NULL) {
		return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
	}

	morph->plugin = visual_plugin_load (ref);

	return VISUAL_OK;
}

int visual_morph_realize (VisMorph *morph)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_return_val_if_fail (morph->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	return visual_plugin_realize (morph->plugin);
}

int visual_morph_get_supported_depth (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_return_val_if_fail (morph->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL)
		return -VISUAL_ERROR_MORPH_PLUGIN_NULL;

	return morphplugin->vidoptions.depth;
}

VisVideoAttributeOptions *visual_morph_get_video_attribute_options (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_return_val_if_fail (morph != NULL, NULL);
	visual_return_val_if_fail (morph->plugin != NULL, NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL)
		return NULL;

	return &morphplugin->vidoptions;
}

int visual_morph_set_video (VisMorph *morph, VisVideo *video)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	morph->dest = video;

	return VISUAL_OK;
}

int visual_morph_set_time (VisMorph *morph, VisTime *time)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_return_val_if_fail (time != NULL, -VISUAL_ERROR_TIME_NULL);

	return visual_time_copy (&morph->morphtime, time);
}

int visual_morph_set_rate (VisMorph *morph, float rate)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->rate = rate;

	return VISUAL_OK;
}

int visual_morph_set_steps (VisMorph *morph, int steps)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->steps = steps;

	return VISUAL_OK;
}

int visual_morph_set_mode (VisMorph *morph, VisMorphMode mode)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morph->mode = mode;

	return VISUAL_OK;
}

VisPalette *visual_morph_get_palette (VisMorph *morph)
{
	visual_return_val_if_fail (morph != NULL, NULL);

	return &morph->morphpal;
}

int visual_morph_is_done (VisMorph *morph)
{
	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

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

int visual_morph_requests_audio (VisMorph *morph)
{
	VisMorphPlugin *morphplugin;

	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_ERROR,
			_("The given morph does not reference any plugin"));

		return -VISUAL_ERROR_MORPH_PLUGIN_NULL;
	}

	return morphplugin->requests_audio;
}

int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2)
{
	VisMorphPlugin *morphplugin;
	VisTime elapsed;
	double usec_elapsed, usec_morph;

	visual_return_val_if_fail (morph != NULL, -VISUAL_ERROR_MORPH_NULL);
	visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_AUDIO_NULL);
	visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	morphplugin = get_morph_plugin (morph);

	if (morphplugin == NULL) {
		visual_log (VISUAL_LOG_ERROR,
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
		usec_elapsed = ((double) elapsed.sec) * VISUAL_USEC_PER_SEC + elapsed.usec;
		usec_morph = ((double) morph->morphtime.sec) * VISUAL_USEC_PER_SEC + morph->morphtime.usec;

		morph->rate = usec_elapsed / usec_morph;

		if (morph->rate > 1.0)
			morph->rate = 1;
	}


	return VISUAL_OK;
}

