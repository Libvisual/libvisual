/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
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
#include "lv_actor.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>

namespace {

  LV::PluginList const&
  get_actor_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_ACTOR);
  }

} // anonymous namespace

struct _VisActor {
	VisObject      object;      /**< Parent */
	VisPluginData *plugin;      /**< Plugin object */
	VisVideo      *video;
	VisVideo      *transform;
	VisVideo      *fitting;
	VisPalette    *ditherpal;
	VisSongInfo   *songcompare;
};

static int visual_actor_init (VisActor *actor, const char *actorname);
static void actor_dtor (VisObject *object);
static VisActorPlugin *get_actor_plugin (VisActor *actor);

static int negotiate_video_with_unsupported_depth (VisActor *actor, VisVideoDepth rundepth, bool noevent, bool forced);
static int negotiate_video (VisActor *actor, bool noevent);

static void actor_dtor (VisObject *object)
{
    auto actor = VISUAL_ACTOR (object);

    if (actor->plugin) {
        {
            // FIXME: Hack to free songinfo
            VisActorPlugin *actplugin = reinterpret_cast<VisActorPlugin*> (actor->plugin->info->plugin);
            visual_songinfo_free (actplugin->songinfo);
        }

        visual_plugin_unload (actor->plugin);
    }

    if (actor->ditherpal)
        visual_palette_free (actor->ditherpal);

    if (actor->transform)
        visual_video_unref (actor->transform);

    if (actor->fitting)
        visual_video_unref (actor->fitting);

    if (actor->video)
        visual_video_unref (actor->video);

    visual_songinfo_free (actor->songcompare);
}

static VisActorPlugin *get_actor_plugin (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, nullptr);
    visual_return_val_if_fail (actor->plugin != nullptr, nullptr);

    return VISUAL_ACTOR_PLUGIN (actor->plugin->info->plugin);
}

VisPluginData *visual_actor_get_plugin (VisActor *actor)
{
    return actor->plugin;
}

const char *visual_actor_get_next_by_name_gl (const char *name)
{
    const char *next = nullptr;
    bool have_gl;

    do {
        next = visual_actor_get_next_by_name (next);
        if (!next)
            return nullptr;

        VisPluginData*  plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, next);
        VisActorPlugin* actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (!have_gl);

    return next;
}

const char *visual_actor_get_prev_by_name_gl (const char *name)
{
    const char *prev = name;
    bool have_gl;

    do {
        prev = visual_actor_get_prev_by_name (prev);
        if (!prev)
            return nullptr;

        auto plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, prev);
        auto actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (!have_gl);

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
        auto actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (have_gl);

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
        auto actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (have_gl);

    return prev;
}

const char *visual_actor_get_next_by_name (const char *name)
{
    return LV::plugin_get_next_by_name (get_actor_plugin_list (), name);
}

const char *visual_actor_get_prev_by_name (char const* name)
{
    return LV::plugin_get_prev_by_name (get_actor_plugin_list (), name);
}

VisActor *visual_actor_new (const char *actorname)
{
    auto actor = visual_mem_new0 (VisActor, 1);

    auto result = visual_actor_init (actor, actorname);
    if (result != VISUAL_OK) {
        visual_mem_free (actor);
        return nullptr;
    }

    return actor;
}

int visual_actor_init (VisActor *actor, const char *name)
{
    visual_return_val_if_fail (actor != nullptr, -VISUAL_ERROR_ACTOR_NULL);

    if (name && get_actor_plugin_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, "the plugin list is empty");

        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_init (VISUAL_OBJECT (actor), actor_dtor);

    /* Reset the VisActor data */
    actor->plugin    = nullptr;
    actor->video     = nullptr;
    actor->transform = nullptr;
    actor->fitting   = nullptr;
    actor->ditherpal = nullptr;

    actor->songcompare = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);

    if (!name) {
        return VISUAL_OK;
    }

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_ACTOR, name)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    actor->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, name);

    // FIXME: Hack to initialize songinfo
    {
        auto actplugin = reinterpret_cast<VisActorPlugin*> (actor->plugin->info->plugin);
        actplugin->songinfo = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);
    }

    return VISUAL_OK;
}

int visual_actor_realize (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != nullptr, VISUAL_ERROR_ACTOR_PLUGIN_NULL);

    return visual_plugin_realize (actor->plugin);
}

VisVideo *visual_actor_get_video (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, nullptr);

    return actor->video;
}

VisSongInfo *visual_actor_get_songinfo (VisActor *actor)
{
    VisActorPlugin *actplugin;

    visual_return_val_if_fail (actor != nullptr, nullptr);

    actplugin = get_actor_plugin (actor);
    visual_return_val_if_fail (actplugin != nullptr, nullptr);

    return actplugin->songinfo;
}

VisPalette *visual_actor_get_palette (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, nullptr);

    auto actplugin = get_actor_plugin (actor);

    if (!actplugin) {
        visual_log (VISUAL_LOG_ERROR,
            "The given actor does not reference any actor plugin");
        return nullptr;
    }

    if (actor->transform &&
        actor->video->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
        return actor->ditherpal;
    } else {
        return actplugin->palette (visual_actor_get_plugin (actor));
    }

    return nullptr;
}

int visual_actor_video_negotiate (VisActor *actor, VisVideoDepth rundepth, int noevent, int forced)
{
    visual_return_val_if_fail (actor != nullptr, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != nullptr, -VISUAL_ERROR_PLUGIN_NULL);
    visual_return_val_if_fail (actor->video != nullptr, -VISUAL_ERROR_ACTOR_VIDEO_NULL);

    if (actor->transform) {
        visual_video_unref (actor->transform);
        actor->transform = nullptr;
    }

    if (actor->fitting) {
        visual_video_unref (actor->fitting);
        actor->fitting = nullptr;
    }

    if (actor->ditherpal) {
        visual_palette_free (actor->ditherpal);
        actor->ditherpal = nullptr;
    }

    visual_log (VISUAL_LOG_INFO, "Negotiating plugin %s", actor->plugin->info->name);

    // Set up any required intermediary pixel buffers

    auto supported_depths = visual_actor_get_supported_depths (actor);

	if (!visual_video_depth_is_supported (supported_depths, actor->video->get_depth ()) ||
        (forced && actor->video->get_depth () != rundepth)) {
        return negotiate_video_with_unsupported_depth (actor, rundepth, noevent, forced);
    }
    else {
        return negotiate_video (actor, noevent);
    }

    return -VISUAL_ERROR_IMPOSSIBLE;
}

static int negotiate_video_with_unsupported_depth (VisActor *actor, VisVideoDepth rundepth, bool noevent, bool forced)
{
    auto actplugin = get_actor_plugin (actor);

    if (forced && rundepth == VISUAL_VIDEO_DEPTH_GL) {
        return -VISUAL_ERROR_ACTOR_GL_NEGOTIATE;
    }

    auto supported_depths = visual_actor_get_supported_depths (actor);

    if (supported_depths == VISUAL_VIDEO_DEPTH_NONE) {
        visual_log (VISUAL_LOG_ERROR, "Cannot find supported colour depth for rendering actor!");
        return -VISUAL_ERROR_IMPOSSIBLE;
    }

    auto req_depth = forced ? rundepth : visual_video_depth_get_highest_nogl (supported_depths);

    int req_width  = visual_video_get_width (actor->video);
    int req_height = visual_video_get_height (actor->video);

    actplugin->requisition (visual_actor_get_plugin (actor), &req_width, &req_height);

    actor->transform = visual_video_new_with_buffer (req_width, req_height, req_depth);

    if (actor->video->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
        actor->ditherpal = visual_palette_new (256);
    }

    if (!noevent) {
        visual_event_queue_add (actor->plugin->eventqueue,
                                visual_event_new_resize (req_width, req_height));
    }

    return VISUAL_OK;
}

static int negotiate_video (VisActor *actor, bool noevent)
{
    auto actplugin = get_actor_plugin (actor);
    visual_return_val_if_fail (actplugin != nullptr, -VISUAL_ERROR_IMPOSSIBLE);

    int req_width  = actor->video->get_width ();
    int req_height = actor->video->get_height ();

    actplugin->requisition (visual_actor_get_plugin (actor), &req_width, &req_height);

    // Size fitting enviroment
    if (req_width != actor->video->get_width () || req_height != actor->video->get_height ()) {
        if (actor->video->get_depth () != VISUAL_VIDEO_DEPTH_GL) {
            actor->fitting = visual_video_new_with_buffer (req_width, req_height, actor->video->get_depth ());
        }

        actor->video->set_dimension (req_width, req_height);
    }

    // FIXME: This should be moved into the if block above. It's out
    // here because plugins depend on this to receive information
    // about initial dimensions
    if (!noevent) {
        visual_event_queue_add (actor->plugin->eventqueue,
                                visual_event_new_resize (req_width, req_height));
    }

    return VISUAL_OK;
}

/**
 * Gives the by the plugin natively supported depths
 *
 * @param actor Pointer to a VisActor of which the supported depth of it's
 *    encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_DEPTH_* values which can be checked against using AND on success,
 *  -VISUAL_ERROR_ACTOR_NULL, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_ACTOR_PLUGIN_NULL on failure.
 */
VisVideoDepth visual_actor_get_supported_depths (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, VISUAL_VIDEO_DEPTH_NONE);
    visual_return_val_if_fail (actor->plugin != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    auto actplugin = get_actor_plugin (actor);
    if (!actplugin) {
        return VISUAL_VIDEO_DEPTH_NONE;
    }

    return actplugin->vidoptions.depth;
}

VisVideoAttrOptions *visual_actor_get_video_attribute_options (VisActor *actor)
{
    visual_return_val_if_fail (actor != nullptr, nullptr);
    visual_return_val_if_fail (actor->plugin != nullptr, nullptr);

    auto actplugin = get_actor_plugin (actor);
    if (!actplugin)
        return nullptr;

    return &actplugin->vidoptions;
}

void visual_actor_set_video (VisActor *actor, VisVideo *video)
{
    visual_return_if_fail (actor != nullptr);

    if (actor->video && actor->video != video) {
        visual_video_unref (actor->video);
    }

    actor->video = video;

    if (actor->video) {
        visual_video_ref (actor->video);
    }
}

void visual_actor_run (VisActor *actor, VisAudio *audio)
{
    /* We don't check for video, because we don't always need a video */
    /*
     * Really? take a look at visual_video_set_palette bellow
     */
    visual_return_if_fail (actor != nullptr);
    visual_return_if_fail (actor->video != nullptr);
    visual_return_if_fail (audio != nullptr);

    auto actplugin = get_actor_plugin (actor);
    auto plugin = visual_actor_get_plugin (actor);

    if (!actplugin) {
        visual_log (VISUAL_LOG_ERROR, "The given actor does not reference any actor plugin");
        return;
    }

    /* Songinfo handling */
    if (!visual_songinfo_compare (actor->songcompare, actplugin->songinfo) ||
        actor->songcompare->get_elapsed () != actplugin->songinfo->get_elapsed ()) {

        actplugin->songinfo->mark ();

        visual_event_queue_add (visual_plugin_get_eventqueue (plugin),
                                visual_event_new_newsong (actplugin->songinfo));

        visual_songinfo_copy (actor->songcompare, actplugin->songinfo);
    }

    auto video     = actor->video;
    auto transform = actor->transform;
    auto fitting   = actor->fitting;

    /*
     * This needs to happen before palette, render stuff, always, period.
     * Also internal vars can be initialized when params have been set in init on the param
     * events in the event loop.
     */
    visual_plugin_events_pump (actor->plugin);

    /* Set the palette to the target video */
    auto palette = visual_actor_get_palette (actor);
    if (palette) {
        video->set_palette (*palette);
    }

    if (transform && (transform->get_depth () != video->get_depth ())) {
        actplugin->render (plugin, transform, audio);

        if (transform->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
            transform->set_palette (*visual_actor_get_palette (actor));
        } else {
            transform->set_palette (*actor->ditherpal);
        }

        video->convert_depth (transform);
    } else {
        if (fitting && (fitting->get_width () != video->get_width () || fitting->get_height () != video->get_height ())) {
            actplugin->render (plugin, fitting, audio);
            video->blit (fitting, 0, 0, false);
        } else {
            actplugin->render (plugin, video, audio);
        }
    }
}
