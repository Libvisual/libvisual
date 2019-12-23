/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include "lv_actor.h"
#include "lv_plugin_registry.h"

namespace {

  LV::PluginList const&
  get_actor_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_ACTOR);
  }

} // anonymous namespace

const char *visual_actor_get_prev_by_name_gl (const char *name)
{
    const char *prev = name;
    bool have_gl;

    do {
        prev = visual_actor_get_prev_by_name (prev);
        if (!prev)
            return nullptr;

        auto plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, prev);
        auto actplugin = (VisActorPlugin *) visual_plugin_get_info (plugin)->plugin;

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (!have_gl);

    return prev;
}

const char *visual_actor_get_next_by_name_gl (const char *name)
{
    (void)name;

    const char *next = nullptr;
    bool have_gl;

    do {
        next = visual_actor_get_next_by_name (next);
        if (!next)
            return nullptr;

        VisPluginData*  plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, next);
        VisActorPlugin* actplugin = (VisActorPlugin *) visual_plugin_get_info (plugin)->plugin;

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (!have_gl);

    return next;
}

const char *visual_actor_get_prev_by_name_nogl (const char *name)
{
    const char *prev = name;
    bool have_gl;

    do {
        prev = visual_actor_get_prev_by_name (prev);
        if (!prev)
            return nullptr;

        auto plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, prev);
        auto actplugin = (VisActorPlugin *) visual_plugin_get_info (plugin)->plugin;

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (have_gl);

    return prev;
}

const char *visual_actor_get_next_by_name_nogl (const char *name)
{
    const char *next = name;
    bool have_gl;

    do {
        next = visual_actor_get_next_by_name (next);
        if (!next)
            return nullptr;

        auto plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, next);
        auto actplugin = (VisActorPlugin *) visual_plugin_get_info (plugin)->plugin;

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (have_gl);

    return next;
}

const char *visual_actor_get_prev_by_name (const char *name)
{
    return LV::plugin_get_prev_by_name (get_actor_plugin_list (), name);
}

const char *visual_actor_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_actor_plugin_list (), name);
}

VisActor *visual_actor_new (const char *name)
{
    auto self = LV::Actor::load (name);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

void visual_actor_ref (VisActor *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_add_ref (self);
}

void visual_actor_unref (VisActor *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_release (self);
}

int visual_actor_realize (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->realize ();
}

void visual_actor_run (VisActor *self, VisAudio *audio)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (audio != nullptr);

    self->run (*audio);
}

VisPluginData *visual_actor_get_plugin (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_plugin ();
}

VisSongInfo *visual_actor_get_songinfo (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<VisSongInfo*> (self->get_songinfo ());
}

VisPalette *visual_actor_get_palette (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<VisPalette*> (self->get_palette ());
}


VisVideoDepth visual_actor_get_supported_depths (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    return self->get_supported_depths ();
}

VisVideoAttrOptions *visual_actor_get_video_attribute_options (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return const_cast<VisVideoAttrOptions*> (self->get_video_attribute_options ());
}

void visual_actor_set_video (VisActor *self, VisVideo *video)
{
    visual_return_if_fail (self != nullptr);

    self->set_video (video);
}

VisVideo *visual_actor_get_video (VisActor *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_video ().get ();
}

int visual_actor_video_negotiate (VisActor *self, VisVideoDepth run_depth, int noevent, int forced)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->video_negotiate (run_depth, noevent, forced);
}
