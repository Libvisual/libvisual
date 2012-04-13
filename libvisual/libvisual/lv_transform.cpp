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
#include "gettext.h"

namespace LV {

  PluginList const& transform_plugin_get_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_TRANSFORM);
  }

} // LV namespace


static int transform_dtor (VisObject *object);

static VisTransformPlugin *get_transform_plugin (VisTransform *transform);

int visual_transform_init (VisTransform *transform, const char *transformname);

static int transform_dtor (VisObject *object)
{
    VisTransform *transform = VISUAL_TRANSFORM (object);

    if (transform->pal)
        visual_palette_free (transform->pal);

    if (transform->plugin != NULL)
        visual_plugin_unload (transform->plugin);

    transform->plugin = NULL;

    return VISUAL_OK;
}

static VisTransformPlugin *get_transform_plugin (VisTransform *transform)
{
    VisTransformPlugin *transplugin;

    visual_return_val_if_fail (transform != NULL, NULL);
    visual_return_val_if_fail (transform->plugin != NULL, NULL);

    transplugin = VISUAL_TRANSFORM_PLUGIN (transform->plugin->info->plugin);

    return transplugin;
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
    VisTransform *transform;
    int result;

    transform = visual_mem_new0 (VisTransform, 1);

    result = visual_transform_init (transform, transformname);
    if (result != VISUAL_OK) {
        visual_mem_free (transform);
        return NULL;
    }

    /* Do the VisObject initialization */
    visual_object_set_allocated (VISUAL_OBJECT (transform), TRUE);
    visual_object_ref (VISUAL_OBJECT (transform));

    return transform;
}

int visual_transform_init (VisTransform *transform, const char *transformname)
{
    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transformname && !LV::transform_plugin_get_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, _("the plugin list is NULL"));
        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_clear (VISUAL_OBJECT (transform));
    visual_object_set_dtor (VISUAL_OBJECT (transform), transform_dtor);
    visual_object_set_allocated (VISUAL_OBJECT (transform), FALSE);

    /* Reset the VisTransform data */
    transform->plugin = NULL;
    transform->video = NULL;
    transform->pal = NULL;

    if (transformname == NULL)
        return VISUAL_OK;

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_TRANSFORM, transformname)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    transform->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_TRANSFORM, transformname);

    return VISUAL_OK;
}

int visual_transform_realize (VisTransform *transform)
{
    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    return visual_plugin_realize (transform->plugin);
}

int visual_transform_video_negotiate (VisTransform *transform)
{
    int depthflag;

    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    depthflag = visual_transform_get_supported_depth (transform);

    if (!visual_video_depth_is_supported (depthflag, transform->video->depth))
        return -VISUAL_ERROR_TRANSFORM_NEGOTIATE;

    visual_event_queue_add (transform->plugin->eventqueue,
                            visual_event_new_resize (transform->video->width,
                                                     transform->video->height));

    visual_plugin_events_pump (transform->plugin);

    return -VISUAL_OK;
}

int visual_transform_get_supported_depth (VisTransform *transform)
{
    VisTransformPlugin *transplugin;

    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    transplugin = get_transform_plugin (transform);

    if (transplugin == NULL)
        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;

    return transplugin->vidoptions.depth;
}

VisVideoAttributeOptions *visual_transform_get_video_attribute_options (VisTransform *transform)
{
    VisTransformPlugin *transplugin;

    visual_return_val_if_fail (transform != NULL, NULL);
    visual_return_val_if_fail (transform->plugin != NULL, NULL);

    transplugin = get_transform_plugin (transform);

    if (transplugin == NULL)
        return NULL;

    return &transplugin->vidoptions;
}

int visual_transform_set_video (VisTransform *transform, VisVideo *video)
{
    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);

    transform->video = video;

    if (video != NULL)
        visual_transform_set_palette (transform, video->pal);
    else
        visual_transform_set_palette (transform, NULL);

    return VISUAL_OK;
}

int visual_transform_set_palette (VisTransform *transform, VisPalette *palette)
{
    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transform->pal)
        visual_palette_free (transform->pal);

    transform->pal = palette ? visual_palette_clone (palette) : 0;

    return VISUAL_OK;
}

int visual_transform_run (VisTransform *transform, VisAudio *audio)
{
    int ret;
    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);

    if (transform->video != NULL) {
        if ((ret = visual_transform_run_video (transform, audio)) != VISUAL_OK)
            return ret;
    }

    if (transform->pal != NULL) {
        if ((ret = visual_transform_run_palette (transform, audio)) != VISUAL_OK)
            return ret;
    }

    return VISUAL_OK;
}

int visual_transform_run_video (VisTransform *transform, VisAudio *audio)
{
    VisTransformPlugin *transplugin;
    VisPluginData *plugin;

    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->video != NULL, -VISUAL_ERROR_TRANSFORM_VIDEO_NULL);

    transplugin = get_transform_plugin (transform);
    plugin = visual_transform_get_plugin (transform);

    if (transplugin == NULL) {
        visual_log (VISUAL_LOG_ERROR,
            _("The given transform does not reference any transform plugin"));

        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;
    }

    visual_plugin_events_pump (plugin);

    transplugin->video (plugin, transform->video, audio);

    return VISUAL_OK;
}

int visual_transform_run_palette (VisTransform *transform, VisAudio *audio)
{
    VisTransformPlugin *transplugin;
    VisPluginData *plugin;

    visual_return_val_if_fail (transform != NULL, -VISUAL_ERROR_TRANSFORM_NULL);
    visual_return_val_if_fail (transform->pal != NULL, -VISUAL_ERROR_TRANSFORM_PALETTE_NULL);

    transplugin = get_transform_plugin (transform);
    plugin = visual_transform_get_plugin (transform);

    if (transplugin == NULL) {
        visual_log (VISUAL_LOG_ERROR,
            _("The given transform does not reference any transform plugin"));

        return -VISUAL_ERROR_TRANSFORM_PLUGIN_NULL;
    }

    visual_plugin_events_pump (plugin);

    transplugin->palette (plugin, transform->pal, audio);

    return VISUAL_OK;
}
