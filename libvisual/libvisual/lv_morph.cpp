/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      ibvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include "lv_plugin_registry.h"
#include <algorithm>

namespace {

  LV::PluginList const&
  get_morph_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_MORPH);
  }

} // anonymous namespace

struct _VisMorph {
    VisObject      object;
    VisPluginData *plugin;
    VisVideo      *dest;
    float          progress;
    VisPalette    *morphpal;
    VisTime       *morphtime;
    VisTimer      *timer;
    VisMorphMode   mode;
};

static int visual_morph_init (VisMorph *morph, const char *morphname);

static void morph_dtor (VisObject *object);

static VisMorphPlugin *get_morph_plugin (VisMorph *morph);

static void morph_dtor (VisObject *object)
{
    auto morph = VISUAL_MORPH (object);

    visual_time_free (morph->morphtime);
    visual_timer_free (morph->timer);

    if (morph->dest)
        visual_video_unref (morph->dest);

    if (morph->plugin)
        visual_plugin_unload (morph->plugin);

    visual_palette_free (morph->morphpal);
}

static VisMorphPlugin *get_morph_plugin (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, nullptr);
    visual_return_val_if_fail (morph->plugin != nullptr, nullptr);

    return VISUAL_MORPH_PLUGIN (morph->plugin->info->plugin);
}

VisPluginData *visual_morph_get_plugin (VisMorph *morph)
{
    return morph->plugin;
}

const char *visual_morph_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_morph_plugin_list (), name);
}

const char *visual_morph_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (get_morph_plugin_list (), name);
}

VisMorph *visual_morph_new (const char *morphname)
{
    auto morph = visual_mem_new0 (VisMorph, 1);

    auto result = visual_morph_init (morph, morphname);
    if (result != VISUAL_OK) {
        visual_mem_free (morph);
        return nullptr;
    }

    return morph;
}

int visual_morph_init (VisMorph *morph, const char *morphname)
{
    visual_return_val_if_fail (morph != nullptr, -VISUAL_ERROR_MORPH_NULL);

    if (morphname && get_morph_plugin_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, "the plugin list is nullptr");

        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_init (VISUAL_OBJECT (morph), morph_dtor);

    /* Reset the VisMorph data */
    morph->plugin = nullptr;
    morph->dest = nullptr;
    morph->morphpal = visual_palette_new (256);
    morph->morphtime = visual_time_new ();
    morph->timer = visual_timer_new ();
    visual_morph_set_progress (morph, 0.0);

    visual_morph_set_mode (morph, VISUAL_MORPH_MODE_SET);

    if (!morphname)
        return VISUAL_OK;

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_MORPH, morphname)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    morph->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_MORPH, morphname);

    return VISUAL_OK;
}

int visual_morph_realize (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, VISUAL_ERROR_MORPH_NULL);
    visual_return_val_if_fail (morph->plugin != nullptr, VISUAL_ERROR_MORPH_PLUGIN_NULL);

    return visual_plugin_realize (morph->plugin);
}

VisVideoDepth visual_morph_get_supported_depth (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, VISUAL_VIDEO_DEPTH_NONE);
    visual_return_val_if_fail (morph->plugin != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    auto morphplugin = get_morph_plugin (morph);
    if (!morphplugin)
        return VISUAL_VIDEO_DEPTH_NONE;

    return morphplugin->vidoptions.depth;
}

VisVideoAttrOptions *visual_morph_get_video_attribute_options (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, nullptr);
    visual_return_val_if_fail (morph->plugin != nullptr, nullptr);

    auto morphplugin = get_morph_plugin (morph);
    if (!morphplugin)
        return nullptr;

    return &morphplugin->vidoptions;
}

void visual_morph_set_video (VisMorph *morph, VisVideo *video)
{
    visual_return_if_fail (morph != nullptr);
    visual_return_if_fail (video != nullptr);

    morph->dest = video;
    visual_video_ref (morph->dest);
}

void visual_morph_set_time (VisMorph *morph, VisTime *time)
{
    visual_return_if_fail (morph != nullptr);
    visual_return_if_fail (time != nullptr);

    visual_time_copy (morph->morphtime, time);
}

void visual_morph_set_progress (VisMorph *morph, float progress)
{
    visual_return_if_fail (morph != nullptr);

    morph->progress = progress;
}

void visual_morph_set_mode (VisMorph *morph, VisMorphMode mode)
{
    visual_return_if_fail (morph != nullptr);

    morph->mode = mode;
}

VisPalette *visual_morph_get_palette (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, nullptr);

    return morph->morphpal;
}

int visual_morph_is_done (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, TRUE);

    if (morph->mode == VISUAL_MORPH_MODE_SET)
        return FALSE;

    if (morph->progress >= 1.0) {
        if (morph->mode == VISUAL_MORPH_MODE_TIME)
            visual_timer_stop (morph->timer);

        return TRUE;
    }

    return FALSE;
}

int visual_morph_requests_audio (VisMorph *morph)
{
    visual_return_val_if_fail (morph != nullptr, -VISUAL_ERROR_MORPH_NULL);

    auto morphplugin = get_morph_plugin (morph);
    if (!morphplugin) {
        visual_log (VISUAL_LOG_ERROR,
            "The given morph does not reference any plugin");

        return -VISUAL_ERROR_MORPH_PLUGIN_NULL;
    }

    return morphplugin->requests_audio;
}

int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2)
{
    visual_return_val_if_fail (morph != nullptr, FALSE);
    visual_return_val_if_fail (audio != nullptr, FALSE);
    visual_return_val_if_fail (src1 != nullptr, FALSE);
    visual_return_val_if_fail (src2 != nullptr, FALSE);

    auto morphplugin = get_morph_plugin (morph);
    if (!morphplugin) {
        visual_log (VISUAL_LOG_ERROR,
            "The given morph does not reference any plugin");

        return -VISUAL_ERROR_MORPH_PLUGIN_NULL;
    }

    /* If we're morphing using the timer, start the timer. */
    if (!visual_timer_is_active (morph->timer))
        visual_timer_start (morph->timer);

    if (morphplugin->palette)
        morphplugin->palette (morph->plugin, morph->progress, audio, morph->morphpal, src1, src2);
    else {
        auto src1_pal = visual_video_get_palette (src1);
        auto src2_pal = visual_video_get_palette (src2);

        if (src1_pal && src2_pal)
            visual_palette_blend (morph->morphpal, src1_pal, src2_pal, morph->progress);
    }

    morphplugin->apply (morph->plugin, morph->progress, audio, morph->dest, src1, src2);

    visual_video_set_palette (morph->dest, visual_morph_get_palette (morph));

    /* On automatic morphing increase the progress. */
    if (morph->mode == VISUAL_MORPH_MODE_TIME) {
        double usec_elapsed = visual_timer_elapsed_usecs (morph->timer);
        double usec_morph = visual_time_to_usecs (morph->morphtime);

        morph->progress = std::min (usec_elapsed / usec_morph, 1.0);
    }

    return TRUE;
}
