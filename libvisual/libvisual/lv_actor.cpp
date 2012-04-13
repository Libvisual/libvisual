/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_actor.c,v 1.39.2.1 2006/03/04 12:32:47 descender Exp $
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
#include "gettext.h"
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>

namespace {


  inline LV::PluginList const&
  get_actor_plugin_list ()
  {
      return LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_ACTOR);
  }

  inline LV::PluginRef*
  find_actor_plugin (std::string const& name)
  {
      return LV::PluginRegistry::instance()->find_plugin (VISUAL_PLUGIN_TYPE_ACTOR, name);
  }

} // LV namespace


extern "C" {

static int actor_dtor (VisObject *object);

static VisActorPlugin *get_actor_plugin (VisActor *actor);
static int negotiate_video_with_unsupported_depth (VisActor *actor, VisVideoDepth rundepth, int noevent, int forced);
static int negotiate_video (VisActor *actor, int noevent);

static int actor_dtor (VisObject *object)
{
    VisActor *actor = VISUAL_ACTOR (object);

    if (actor->plugin != NULL) {
        {
            // FIXME: Hack to free songinfo
            VisActorPlugin *actplugin = (VisActorPlugin *) actor->plugin->info->plugin;
            visual_songinfo_free (actplugin->songinfo);
        }

        visual_plugin_unload (actor->plugin);
    }

    if (actor->ditherpal != NULL)
        visual_palette_free (actor->ditherpal);

    if (actor->transform != NULL)
        visual_object_unref (VISUAL_OBJECT (actor->transform));

    if (actor->fitting != NULL)
        visual_object_unref (VISUAL_OBJECT (actor->fitting));

    visual_songinfo_free (actor->songcompare);

    actor->plugin = NULL;
    actor->transform = NULL;
    actor->fitting = NULL;

    return VISUAL_OK;
}

static VisActorPlugin *get_actor_plugin (VisActor *actor)
{
    visual_return_val_if_fail (actor != NULL, NULL);
    visual_return_val_if_fail (actor->plugin != NULL, NULL);

    return VISUAL_ACTOR_PLUGIN (actor->plugin->info->plugin);
}

VisPluginData *visual_actor_get_plugin (VisActor *actor)
{
    return actor->plugin;
}

const char *visual_actor_get_next_by_name_gl (const char *name)
{
    const char *next = NULL;
    bool have_gl;

    do {
        next = visual_actor_get_next_by_name (next);
        if (next == NULL)
            return NULL;

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
        if (prev == NULL)
            return NULL;

        VisPluginData*  plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, prev);
        VisActorPlugin* actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

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
        if (next == NULL)
            return NULL;

        VisPluginData*  plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, next);
        VisActorPlugin* actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

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
        if (prev == NULL)
            return NULL;

        VisPluginData*  plugin    = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, prev);
        VisActorPlugin* actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

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
    VisActor *actor;
    int result;

    actor = visual_mem_new0 (VisActor, 1);

    result = visual_actor_init (actor, actorname);
    if (result != VISUAL_OK) {
        visual_mem_free (actor);
        return NULL;
    }

    /* Do the VisObject initialization */
    visual_object_set_allocated (VISUAL_OBJECT (actor), TRUE);
    visual_object_ref (VISUAL_OBJECT (actor));

    return actor;
}

int visual_actor_init (VisActor *actor, const char *actorname)
{
    VisPluginEnviron *enve;
    VisActorPluginEnviron *actenviron;

    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);

    if (actorname && get_actor_plugin_list ().empty ()) {
        visual_log (VISUAL_LOG_ERROR, _("the plugin list is empty"));

        return -VISUAL_ERROR_PLUGIN_NO_LIST;
    }

    /* Do the VisObject initialization */
    visual_object_clear (VISUAL_OBJECT (actor));
    visual_object_set_dtor (VISUAL_OBJECT (actor), actor_dtor);
    visual_object_set_allocated (VISUAL_OBJECT (actor), FALSE);

    /* Reset the VisActor data */
    actor->plugin = NULL;
    actor->video = NULL;
    actor->transform = NULL;
    actor->fitting = NULL;
    actor->ditherpal = NULL;

    actor->songcompare = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);

    if (actorname == NULL)
        return VISUAL_OK;

    if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_ACTOR, actorname)) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    actor->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, actorname);

    // FIXME: Hack to initialize songinfo
    {
        VisActorPlugin *actplugin = (VisActorPlugin *) actor->plugin->info->plugin;
        actplugin->songinfo = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);
    }

    /* Adding the VisActorPluginEnviron */
    actenviron = visual_mem_new0 (VisActorPluginEnviron, 1);

    visual_object_initialize (VISUAL_OBJECT (actenviron), TRUE, NULL);

    enve = visual_plugin_environ_new (VISUAL_ACTOR_PLUGIN_ENVIRON, VISUAL_OBJECT (actenviron));
    visual_plugin_environ_add (actor->plugin, enve);

    return VISUAL_OK;
}

int visual_actor_realize (VisActor *actor)
{
    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    return visual_plugin_realize (actor->plugin);
}

VisSongInfo *visual_actor_get_songinfo (VisActor *actor)
{
    VisActorPlugin *actplugin;

    visual_return_val_if_fail (actor != NULL, NULL);

    actplugin = get_actor_plugin (actor);
    visual_return_val_if_fail (actplugin != NULL, NULL);

    return actplugin->songinfo;
}

VisPalette *visual_actor_get_palette (VisActor *actor)
{
    VisActorPlugin *actplugin;

    visual_return_val_if_fail (actor != NULL, NULL);

    actplugin = get_actor_plugin (actor);

    if (actplugin == NULL) {
        visual_log (VISUAL_LOG_ERROR,
            _("The given actor does not reference any actor plugin"));
        return NULL;
    }

    if (actor->transform != NULL &&
        actor->video->depth == VISUAL_VIDEO_DEPTH_8BIT) {

        return actor->ditherpal;

    } else {
        return actplugin->palette (visual_actor_get_plugin (actor));
    }

    return NULL;
}

int visual_actor_video_negotiate (VisActor *actor, VisVideoDepth rundepth, int noevent, int forced)
{
    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
    visual_return_val_if_fail (actor->video != NULL, -VISUAL_ERROR_ACTOR_VIDEO_NULL);

    if (actor->transform) {
        visual_object_unref (VISUAL_OBJECT (actor->transform));
        actor->transform = NULL;
    }

    if (actor->fitting) {
        visual_object_unref (VISUAL_OBJECT (actor->fitting));
        actor->fitting = NULL;
    }

    if (actor->ditherpal) {
        visual_palette_free (actor->ditherpal);
        actor->ditherpal = NULL;
    }

    visual_log (VISUAL_LOG_INFO, "Negotiating plugin %s", actor->plugin->info->name);

    // Set up any required intermediary pixel buffers

    int depthflag = visual_actor_get_supported_depth (actor);

    if (!visual_video_depth_is_supported (depthflag, actor->video->depth) ||
	(forced && actor->video->depth != rundepth))
        return negotiate_video_with_unsupported_depth (actor, rundepth, noevent, forced);
    else
        return negotiate_video (actor, noevent);

    return -VISUAL_ERROR_IMPOSSIBLE;
}

static int negotiate_video_with_unsupported_depth (VisActor *actor, VisVideoDepth rundepth, int noevent, int forced)
{
    VisActorPlugin *actplugin = get_actor_plugin (actor);

    int depthflag = visual_actor_get_supported_depth (actor);

    VisVideoDepth req_depth = forced ? rundepth : visual_video_depth_get_highest_nogl (depthflag);

    /* If there is only GL (which gets returned by highest nogl if
     * nothing else is there, stop here */
    if (req_depth == VISUAL_VIDEO_DEPTH_GL)
        return -VISUAL_ERROR_ACTOR_GL_NEGOTIATE;

    int req_width  = actor->video->width;
    int req_height = actor->video->height;

    actplugin->requisition (visual_actor_get_plugin (actor), &req_width, &req_height);

    actor->transform = visual_video_new ();

    visual_video_set_depth (actor->transform, req_depth);
    visual_video_set_dimension (actor->transform, req_width, req_height);
    visual_video_allocate_buffer (actor->transform);

    if (actor->video->depth == VISUAL_VIDEO_DEPTH_8BIT)
        actor->ditherpal = visual_palette_new (256);

    if (!noevent) {
        visual_event_queue_add (actor->plugin->eventqueue,
                                visual_event_new_resize (req_width, req_height));
    }

    return VISUAL_OK;
}

static int negotiate_video (VisActor *actor, int noevent)
{
    VisActorPlugin *actplugin = get_actor_plugin (actor);

    int req_width  = actor->video->width;
    int req_height = actor->video->height;

    actplugin->requisition (visual_actor_get_plugin (actor), &req_width, &req_height);

    // Size fitting enviroment
    if (req_width != actor->video->width || req_height != actor->video->height) {
        if (actor->video->depth != VISUAL_VIDEO_DEPTH_GL) {
            actor->fitting = visual_video_new_with_buffer (req_width, req_height, actor->video->depth);
        }

        visual_video_set_dimension (actor->video, req_width, req_height);
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
int visual_actor_get_supported_depth (VisActor *actor)
{
    VisActorPlugin *actplugin;

    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    actplugin = get_actor_plugin (actor);

    if (actplugin == NULL)
        return -VISUAL_ERROR_ACTOR_PLUGIN_NULL;

    return actplugin->vidoptions.depth;
}

VisVideoAttributeOptions *visual_actor_get_video_attribute_options (VisActor *actor)
{
    VisActorPlugin *actplugin;

    visual_return_val_if_fail (actor != NULL, NULL);
    visual_return_val_if_fail (actor->plugin != NULL, NULL);

    actplugin = get_actor_plugin (actor);

    if (actplugin == NULL)
        return NULL;

    return &actplugin->vidoptions;
}

int visual_actor_set_video (VisActor *actor, VisVideo *video)
{
    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);

    actor->video = video;

    return VISUAL_OK;
}

int visual_actor_run (VisActor *actor, VisAudio *audio)
{
    VisActorPlugin *actplugin;
    VisPluginData *plugin;
    VisVideo *video;
    VisVideo *transform;
    VisVideo *fitting;

    /* We don't check for video, because we don't always need a video */
    /*
     * Really? take a look at visual_video_set_palette bellow
     */
    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->video != NULL, -VISUAL_ERROR_ACTOR_VIDEO_NULL);
    visual_return_val_if_fail (audio != NULL, -VISUAL_ERROR_NULL);

    actplugin = get_actor_plugin (actor);
    plugin = visual_actor_get_plugin (actor);

    if (actplugin == NULL) {
        visual_log (VISUAL_LOG_ERROR,
            _("The given actor does not reference any actor plugin"));

        return -VISUAL_ERROR_ACTOR_PLUGIN_NULL;
    }

    /* Songinfo handling */
    if (!visual_songinfo_compare (actor->songcompare, actplugin->songinfo) ||
        visual_songinfo_get_elapsed (actor->songcompare) != visual_songinfo_get_elapsed (actplugin->songinfo)) {

        visual_songinfo_mark (actplugin->songinfo);

        visual_event_queue_add (visual_plugin_get_eventqueue (plugin),
                                visual_event_new_newsong (actplugin->songinfo));

        visual_songinfo_copy (actor->songcompare, actplugin->songinfo);
    }

    video = actor->video;
    transform = actor->transform;
    fitting = actor->fitting;

    /*
     * This needs to happen before palette, render stuff, always, period.
     * Also internal vars can be initialized when params have been set in init on the param
     * events in the event loop.
     */
    visual_plugin_events_pump (actor->plugin);

    /* Set the palette to the target video */
    visual_video_set_palette (video, visual_actor_get_palette (actor));

    /* Yeah some transformation magic is going on here when needed */
    if (transform != NULL && (transform->depth != video->depth)) {
        actplugin->render (plugin, transform, audio);

        if (transform->depth == VISUAL_VIDEO_DEPTH_8BIT) {
            visual_video_set_palette (transform, visual_actor_get_palette (actor));
            visual_video_depth_transform (video, transform);
        } else {
            visual_video_set_palette (transform, actor->ditherpal);
            visual_video_depth_transform (video, transform);
        }
    } else {
        if (fitting != NULL && (fitting->width != video->width || fitting->height != video->height)) {
            actplugin->render (plugin, fitting, audio);
            visual_video_blit_overlay (video, fitting, 0, 0, FALSE);
        } else {
            actplugin->render (plugin, video, audio);
        }
    }

    return VISUAL_OK;
}

} // C extern
