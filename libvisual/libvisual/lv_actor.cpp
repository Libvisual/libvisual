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
#include "lv_list.h"
#include "gettext.h"
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>

namespace LV {

  namespace {

    struct PluginHasName
        : public std::unary_function<bool, VisPluginRef *>
    {
        std::string name;

        PluginHasName (std::string const& name_)
            : name (name_)
        {}

        bool operator() (VisPluginRef* ref) const
        {
            return name == ref->info->plugname;
        }
    };

  } // anonymous namespace

  ActorPluginList actor_plugins;

  VisPluginRef* actor_plugin_find (std::string const& name)
  {
      ActorPluginList::iterator iter =
          std::find_if (actor_plugins.begin (),
                        actor_plugins.end (),
                        PluginHasName (name));

      return (iter != actor_plugins.end ()) ? *iter : 0;
  }

  ActorPluginList const& actor_plugin_get_list ()
  {
      return actor_plugins;
  }

} // LV namespace


extern "C" {

static int actor_dtor (VisObject *object);

static VisActorPlugin *get_actor_plugin (VisActor *actor);
static int negotiate_video_with_unsupported_depth (VisActor *actor, int rundepth, int noevent, int forced);
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
    const char *next;
    bool have_gl;

    do {
        next = visual_actor_get_next_by_name (next);
        if (next == NULL)
            return NULL;

        VisPluginRef*   ref       = LV::actor_plugin_find (next);
        VisPluginData*  plugin    = visual_plugin_load (ref);
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

        VisPluginRef*   ref       = LV::actor_plugin_find (prev);
        VisPluginData*  plugin    = visual_plugin_load (ref);
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

        VisPluginRef*   ref       = LV::actor_plugin_find (next);
        VisPluginData*  plugin    = visual_plugin_load (ref);
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

        VisPluginRef*   ref       = LV::actor_plugin_find (prev);
        VisPluginData*  plugin    = visual_plugin_load (ref);
        VisActorPlugin* actplugin = VISUAL_ACTOR_PLUGIN (plugin->info->plugin);

        have_gl = (actplugin->vidoptions.depth & VISUAL_VIDEO_DEPTH_GL) > 0;

        visual_plugin_unload (plugin);

    } while (have_gl);

    return prev;
}

const char *visual_actor_get_next_by_name (const char *name)
{
    for (unsigned int i = 0; i < LV::actor_plugins.size (); i++)
    {
        if (std::strcmp (LV::actor_plugins[i]->info->plugname, name) == 0)
        {
            unsigned int next_i = (i + 1) % LV::actor_plugins.size ();
            return LV::actor_plugins[next_i]->info->plugname;
        }
    }

    return NULL;
}

const char *visual_actor_get_prev_by_name (char const* name)
{
    for (unsigned int i = 0; i < LV::actor_plugins.size (); i++)
    {
        if (std::strcmp (LV::actor_plugins[i]->info->plugname, name) == 0)
        {
            unsigned int prev_i = (i + LV::actor_plugins.size () - 1) % LV::actor_plugins.size ();
            return LV::actor_plugins[prev_i]->info->plugname;
        }
    }

    return NULL;
}

int visual_actor_valid_by_name (char const* name)
{
    return std::find_if (LV::actor_plugins.begin (),
                         LV::actor_plugins.end (),
                         LV::PluginHasName (name)) != LV::actor_plugins.end ();
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
    VisPluginRef *ref = NULL;
    VisPluginEnviron *enve;
    VisActorPluginEnviron *actenviron;

    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);

    if ((actorname != 0) && LV::actor_plugins.empty ()) {
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

    ref = LV::actor_plugin_find (actorname);
    if (ref == NULL) {
        return -VISUAL_ERROR_PLUGIN_NOT_FOUND;
    }

    actor->plugin = visual_plugin_load (ref);

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

int visual_actor_video_negotiate (VisActor *actor, int rundepth, int noevent, int forced)
{
    int depthflag;

    visual_return_val_if_fail (actor != NULL, -VISUAL_ERROR_ACTOR_NULL);
    visual_return_val_if_fail (actor->plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
    visual_return_val_if_fail (actor->plugin->ref != NULL, -VISUAL_ERROR_PLUGIN_REF_NULL);
    visual_return_val_if_fail (actor->video != NULL, -VISUAL_ERROR_ACTOR_VIDEO_NULL);

    if (actor->transform != NULL) {
        visual_object_unref (VISUAL_OBJECT (actor->transform));

        actor->transform = NULL;
    }

    if (actor->fitting != NULL) {
        visual_object_unref (VISUAL_OBJECT (actor->fitting));

        actor->fitting = NULL;
    }

    if (actor->ditherpal != NULL) {
        visual_object_unref (VISUAL_OBJECT (actor->ditherpal));

        actor->ditherpal = NULL;
    }

    depthflag = visual_actor_get_supported_depth (actor);

    visual_log (VISUAL_LOG_INFO, "negotiating plugin %s", actor->plugin->info->name);

    /* Set up depth transformation enviroment */
    if (visual_video_depth_is_supported (depthflag, actor->video->depth) != TRUE ||
            (forced == TRUE && actor->video->depth != rundepth))
        /* When the depth is not supported, or if we only switch the depth and not
         * the size */
        return negotiate_video_with_unsupported_depth (actor, rundepth, noevent, forced);
    else
        return negotiate_video (actor, noevent);

    return -VISUAL_ERROR_IMPOSSIBLE;
}

static int negotiate_video_with_unsupported_depth (VisActor *actor, int rundepth, int noevent, int forced)
{
    VisActorPlugin *actplugin = get_actor_plugin (actor);
    int depthflag = visual_actor_get_supported_depth (actor);

    /* Depth transform enviroment, it automaticly
     * fits size because it can use the pitch from
     * the dest video context */
    actor->transform = visual_video_new ();

    visual_log (VISUAL_LOG_INFO, _("run depth %d forced %d"), rundepth, forced);

    if (forced == TRUE)
        visual_video_set_depth (actor->transform, VisVideoDepth (rundepth));
    else
        visual_video_set_depth (actor->transform,
                visual_video_depth_get_highest_nogl (depthflag));

    visual_log (VISUAL_LOG_INFO, _("transpitch1 %d depth %d bpp %d"), actor->transform->pitch, actor->transform->depth,
            actor->transform->bpp);
    /* If there is only GL (which gets returned by highest nogl if
     * nothing else is there, stop here */
    if (actor->transform->depth == VISUAL_VIDEO_DEPTH_GL)
        return -VISUAL_ERROR_ACTOR_GL_NEGOTIATE;

    visual_video_set_dimension (actor->transform, actor->video->width, actor->video->height);
    visual_log (VISUAL_LOG_INFO, _("transpitch2 %d %d"), actor->transform->width, actor->transform->pitch);

    actplugin->requisition (visual_actor_get_plugin (actor), &actor->transform->width, &actor->transform->height);
    visual_log (VISUAL_LOG_INFO, _("transpitch3 %d"), actor->transform->pitch);

    if (noevent == FALSE) {
        visual_event_queue_add_resize (&actor->plugin->eventqueue, actor->transform,
                actor->transform->width, actor->transform->height);
        visual_plugin_events_pump (actor->plugin);
    } else {
        /* Normally a visual_video_set_dimension get's called within the
         * event handler, but we won't come there right now so we've
         * got to set the pitch ourself */
        visual_video_set_dimension (actor->transform,
                actor->transform->width, actor->transform->height);
    }

    visual_log (VISUAL_LOG_INFO, _("rundepth: %d transpitch %d"), rundepth, actor->transform->pitch);
    visual_video_allocate_buffer (actor->transform);

    if (actor->video->depth == VISUAL_VIDEO_DEPTH_8BIT)
        actor->ditherpal = visual_palette_new (256);

    return VISUAL_OK;
}

static int negotiate_video (VisActor *actor, int noevent)
{
    VisActorPlugin *actplugin = get_actor_plugin (actor);
    int tmpwidth, tmpheight, tmppitch;

    tmpwidth = actor->video->width;
    tmpheight = actor->video->height;
    tmppitch = actor->video->pitch;

    /* Pump the resize events and handle all the pending events */
    actplugin->requisition (visual_actor_get_plugin (actor), &actor->video->width, &actor->video->height);

    if (noevent == FALSE) {
        visual_event_queue_add_resize (&actor->plugin->eventqueue, actor->video,
                actor->video->width, actor->video->height);

        visual_plugin_events_pump (actor->plugin);
    }

    /* Size fitting enviroment */
    if (tmpwidth != actor->video->width || tmpheight != actor->video->height) {
        if (actor->video->depth != VISUAL_VIDEO_DEPTH_GL) {
            actor->fitting = visual_video_new_with_buffer (actor->video->width,
                    actor->video->height, actor->video->depth);
        }

        visual_video_set_dimension (actor->video, tmpwidth, tmpheight);
    }

    /* Set the pitch seen this is the framebuffer context */
    visual_video_set_pitch (actor->video, tmppitch);

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
    if (visual_songinfo_compare (actor->songcompare, actplugin->songinfo) == FALSE ||
        visual_songinfo_get_elapsed (actor->songcompare) != visual_songinfo_get_elapsed (actplugin->songinfo)) {

        visual_songinfo_mark (actplugin->songinfo);

        visual_event_queue_add_newsong (
            visual_plugin_get_eventqueue (plugin),
            actplugin->songinfo);

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

    visual_video_set_palette (video, visual_actor_get_palette (actor));

    /* Set the palette to the target video */
    video->pal = visual_actor_get_palette (actor);

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
