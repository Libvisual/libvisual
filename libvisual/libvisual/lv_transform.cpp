/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_transform.c,v 1.8 2006/01/27 20:18:26 synap Exp $
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
#include "lv_transform.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"

namespace LV {

  PluginList const& transform_plugin_get_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_TRANSFORM);
  }

} // LV namespace

static int visual_transform_init (VisTransform *transform, const char *transformname);

static void transform_dtor (VisObject *object);

static VisTransformPlugin *get_transform_plugin (VisTransform *transform);

int visual_transform_init (VisTransform *transform, const char *transformname);

static void transform_dtor (VisObject *object)
{
    auto transform = VISUAL_TRANSFORM (object);

    if (transform->pal)
        visual_palette_free (transform->pal);

    if (transform->plugin)
        visual_plugin_unload (transform->plugin);
}

static VisTransformPlugin *get_transform_plugin (VisTransform *transform)
{
    visual_return_val_if_fail (transform != nullptr, nullptr);
    visual_return_val_if_fail (transform->plugin != nullptr, nullptr);

    return VISUAL_TRANSFORM_PLUGIN (transform->plugin->info->plugin);
}

VisPluginData *visual_transform_get_plugin (VisTransform *transform)
{
    return transform->plugin;
}

const char *visual_transform_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (LV::transform_plugin_get_list (), name);
}

const char *visual_transform_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (LV::transform_plugin_get_list (), name);
}

VisTransform *visual_transform_new (const char *transformname)
{
    auto transform = visual_mem_new0 (VisTransform, 1);

    auto result = visual_transform_init (transform, transformname);
    if (result != VISUAL_OK) {
        visual_mem_free (transform);
        return nullptr;
    }

    return transform;
}

int visual_transform_init (VisTransform *transform, const char *transformname)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transformname && !LV::transform_plugin_get_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, "the plugin list is nullptr");
        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_init (VISUAL_OBJECT (transform), transform_dtor);

    /* Reset the VisTransform data */
    transform->plugin = nullptr;
    transform->video = nullptr;
    transform->pal = nullptr;

    if (!transformname)
        return VISUAL_OK;

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_TRANSFORM, transformname)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    transform->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_TRANSFORM, transformname);

    return VISUAL_OK;
}

int visual_transform_realize (VisTransform *transform)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != nullptr, -VISUAL_ERROR_PLUGIN_NULL);

    return visual_plugin_realize (transform->plugin);
}

int visual_transform_video_negotiate (VisTransform *transform)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != nullptr, -VISUAL_ERROR_PLUGIN_NULL);

    auto depthflag = visual_transform_get_supported_depth (transform);

    if (!visual_video_depth_is_supported (depthflag, visual_video_get_depth (transform->video)))
        return -VISUAL_ERROR_TRANSFORM_NEGOTIATE;

    visual_event_queue_add (transform->plugin->eventqueue,
                            visual_event_new_resize (visual_video_get_width (transform->video),
                                                     visual_video_get_height (transform->video)));

    visual_plugin_events_pump (transform->plugin);

    return -VISUAL_OK;
}

int visual_transform_get_supported_depth (VisTransform *transform)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != nullptr, -VISUAL_ERROR_PLUGIN_NULL);

    auto transplugin = get_transform_plugin (transform);

    if (!transplugin)
        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;

    return transplugin->vidoptions.depth;
}

VisVideoAttrOptions *visual_transform_get_video_attribute_options (VisTransform *transform)
{
    visual_return_val_if_fail (transform != nullptr, nullptr);
    visual_return_val_if_fail (transform->plugin != nullptr, nullptr);

    auto transplugin = get_transform_plugin (transform);

    if (!transplugin)
        return nullptr;

    return &transplugin->vidoptions;
}

int visual_transform_set_video (VisTransform *transform, VisVideo *video)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transform->video && transform->video != video) {
        visual_video_unref (transform->video);
    }

    transform->video = video;

    if (transform->video) {
        visual_transform_set_palette (transform, visual_video_get_palette (video));
        visual_video_ref (transform->video);
    } else {
        visual_transform_set_palette (transform, nullptr);
    }

    return VISUAL_OK;
}

int visual_transform_set_palette (VisTransform *transform, VisPalette *palette)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transform->pal)
        visual_palette_free (transform->pal);

    transform->pal = palette ? visual_palette_clone (palette) : 0;

    return VISUAL_OK;
}

int visual_transform_run (VisTransform *transform, VisAudio *audio)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);

    int ret;

    if (transform->video) {
        if ((ret = visual_transform_run_video (transform, audio)) != VISUAL_OK)
            return ret;
    }

    if (transform->pal) {
        if ((ret = visual_transform_run_palette (transform, audio)) != VISUAL_OK)
            return ret;
    }

    return VISUAL_OK;
}

int visual_transform_run_video (VisTransform *transform, VisAudio *audio)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->video != nullptr, -VISUAL_ERROR_TRANSFORM_VIDEO_NULL);

    auto transplugin = get_transform_plugin (transform);
    auto plugin = visual_transform_get_plugin (transform);

    if (!transplugin) {
        visual_log (VISUAL_LOG_ERROR,
            "The given transform does not reference any transform plugin");

        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;
    }

    visual_plugin_events_pump (plugin);

    transplugin->video (plugin, transform->video, audio);

    return VISUAL_OK;
}

int visual_transform_run_palette (VisTransform *transform, VisAudio *audio)
{
    visual_return_val_if_fail (transform != nullptr, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->pal != nullptr, -VISUAL_ERROR_TRANSFORM_PALETTE_NULL);

    auto transplugin = get_transform_plugin (transform);
    auto plugin = visual_transform_get_plugin (transform);

    if (!transplugin) {
        visual_log (VISUAL_LOG_ERROR,
            "The given transform does not reference any transform plugin");

        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;
    }

    visual_plugin_events_pump (plugin);

    transplugin->palette (plugin, transform->pal, audio);

    return VISUAL_OK;
}
