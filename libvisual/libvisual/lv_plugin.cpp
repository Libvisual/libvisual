/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_plugin.c,v 1.80.2.1 2006/03/04 12:32:47 descender Exp $
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
#include "lv_plugin.h"
#include "lv_common.h"
#include "lv_libvisual.h"
#include "lv_util.h"
#include "lv_plugin_registry.h"
#include "gettext.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace LV {

  const char *plugin_get_next_by_name (PluginList const& list, const char *name)
  {
      for (unsigned int i = 0; i < list.size (); i++)
      {
          if (std::strcmp (list[i]->info->plugname, name) == 0)
          {
              unsigned int next_i = (i + 1) % list.size ();
              return list[next_i]->info->plugname;
          }
      }

      return NULL;
  }

  const char *plugin_get_prev_by_name (PluginList const& list, const char *name)
  {
      for (unsigned int i = 0; i < list.size (); i++)
      {
          if (std::strcmp (list[i]->info->plugname, name) == 0)
          {
              unsigned int prev_i = (i + list.size () - 1) % list.size ();
              return list[prev_i]->info->plugname;
          }
      }

      return NULL;
  }

} // LV namespace

static int plugin_info_dtor (VisObject *object);
static int plugin_ref_dtor (VisObject *object);
static int plugin_environ_dtor (VisObject *object);
static int plugin_dtor (VisObject *object);

static char* copy_info_string (char const* str)
{
    return visual_strdup (str ? str : "(not specified)");
}

static void free_info_string (char const* str)
{
    // HACK: We're forced to cast away the const because the plugin system
    // duplicates the VisPluginInfo strings for storage.

    visual_mem_free (const_cast<char*> (str));
}

static int plugin_info_dtor (VisObject *object)
{
    VisPluginInfo *pluginfo = VISUAL_PLUGININFO (object);

    free_info_string (pluginfo->plugname);
    free_info_string (pluginfo->name);
    free_info_string (pluginfo->author);
    free_info_string (pluginfo->version);
    free_info_string (pluginfo->about);
    free_info_string (pluginfo->help);
    free_info_string (pluginfo->license);

    return VISUAL_OK;
}

static int plugin_ref_dtor (VisObject *object)
{
    VisPluginRef *ref = VISUAL_PLUGINREF (object);

    if (ref->usecount > 0)
        visual_log (VISUAL_LOG_ERROR, _("A plugin reference with %d instances has been destroyed."), ref->usecount);

    if (ref->file)
        visual_mem_free (ref->file);

    if (ref->info) {
        visual_object_unref (VISUAL_OBJECT (ref->info));
    }

    return VISUAL_OK;
}

static int plugin_environ_dtor (VisObject *object)
{
    VisPluginEnviron *enve = VISUAL_PLUGINENVIRON (object);

    if (enve->environment)
        visual_object_unref (enve->environment);

    enve->environment = NULL;

    return VISUAL_OK;
}

static int plugin_dtor (VisObject *object)
{
    VisPluginData *plugin = VISUAL_PLUGINDATA (object);

    delete plugin->random;

    if (plugin->ref)
        visual_object_unref (VISUAL_OBJECT (plugin->ref));

    if (plugin->params)
        visual_object_unref (VISUAL_OBJECT (plugin->params));

    delete plugin->eventqueue;

    visual_collection_destroy (VISUAL_COLLECTION (&plugin->environment));

    return VISUAL_OK;
}

VisPluginInfo *visual_plugin_info_new ()
{
    VisPluginInfo *pluginfo;

    pluginfo = visual_mem_new0 (VisPluginInfo, 1);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (pluginfo), TRUE, plugin_info_dtor);

    return pluginfo;
}

VisPluginInfo *visual_plugin_info_clone (VisPluginInfo const* info)
{
    VisPluginInfo *clone = visual_plugin_info_new ();
    visual_plugin_info_copy (clone, info);
    return clone;
}

int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo const* src)
{
    visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);
    visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);

    visual_mem_copy (dest, src, sizeof (VisPluginInfo));

    dest->plugname = copy_info_string (src->plugname);
    dest->name     = copy_info_string (src->name);
    dest->author   = copy_info_string (src->author);
    dest->version  = copy_info_string (src->version);
    dest->about    = copy_info_string (src->about);
    dest->help     = copy_info_string (src->help);
    dest->license  = copy_info_string (src->license);

    return VISUAL_OK;
}

int visual_plugin_events_pump (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    if (plugin->info->events != NULL) {
        plugin->info->events (plugin, plugin->eventqueue);

        return VISUAL_OK;
    }

    return -VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER;
}

VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != NULL, NULL);

    return plugin->eventqueue;
}

VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != NULL, NULL);

    return plugin->info;
}

VisParamContainer *visual_plugin_get_params (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != NULL, NULL);

    return plugin->params;
}

VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != NULL, NULL);

    return plugin->random;
}

void *visual_plugin_get_specific (VisPluginData *plugin)
{
    VisPluginInfo *pluginfo;

    visual_return_val_if_fail (plugin != NULL, NULL);

    pluginfo = visual_plugin_get_info (plugin);
    visual_return_val_if_fail (pluginfo != NULL, NULL);

    return pluginfo->plugin;
}

VisPluginRef *visual_plugin_ref_new ()
{
    VisPluginRef *ref;

    ref = visual_mem_new0 (VisPluginRef, 1);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (ref), TRUE, plugin_ref_dtor);

    return ref;
}

VisPluginData *visual_plugin_new ()
{
    VisPluginData *plugin;

    plugin = visual_mem_new0 (VisPluginData, 1);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (plugin), TRUE, plugin_dtor);

    plugin->params = visual_param_container_new ();
    plugin->eventqueue = new LV::EventQueue;

    return plugin;
}

int visual_plugin_unload (VisPluginData *plugin)
{
    VisPluginRef *ref;

    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    ref = plugin->ref;

    /* Not loaded */
    if (plugin->handle == NULL) {
        visual_object_unref (VISUAL_OBJECT (plugin));

        visual_log (VISUAL_LOG_ERROR, _("Tried unloading a plugin that never has been loaded."));

        return -VISUAL_ERROR_PLUGIN_HANDLE_NULL;
    }

    if (plugin->realized)
        plugin->info->cleanup (plugin);

    if (plugin->info->plugin != NULL)
        visual_object_unref (VISUAL_OBJECT (plugin->info->plugin));

    if (plugin->info != NULL)
        visual_object_unref (VISUAL_OBJECT (plugin->info));

#if defined(VISUAL_OS_WIN32)
    FreeLibrary (plugin->handle);
#else
    dlclose (plugin->handle);
#endif

    plugin->info = NULL;

    if (ref != NULL) {
        if (ref->usecount > 0)
            ref->usecount--;
    }

    visual_param_container_set_eventqueue (plugin->params, NULL);

    visual_object_unref (VISUAL_OBJECT (plugin));

    return VISUAL_OK;
}

VisPluginData *visual_plugin_load (VisPluginRef *ref)
{
    visual_return_val_if_fail (ref != NULL, NULL);
    visual_return_val_if_fail (ref->info != NULL, NULL);

    /* Check if this plugin is reentrant */
    if (ref->usecount > 0 && (ref->info->flags & VISUAL_PLUGIN_FLAG_NOT_REENTRANT)) {
        visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin %s, the plugin is already loaded and is not reentrant."),
                ref->info->plugname);

        return NULL;
    }

#if defined(VISUAL_OS_WIN32)
    HMODULE handle = LoadLibrary (ref->file);
#else
    void* handle = dlopen (ref->file, RTLD_LAZY);
#endif

    if (!handle) {
#if defined(VISUAL_OS_WIN32)
        visual_log (VISUAL_LOG_ERROR, "Cannot load plugin: win32 error code: %ld", GetLastError ());
#else
        visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin: %s"), dlerror ());
#endif
        return NULL;
    }

    VisPluginGetInfoFunc get_plugin_info;

#if defined(VISUAL_OS_WIN32)
    get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (GetProcAddress (handle, "get_plugin_info"));
#else
    get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (dlsym (handle, "get_plugin_info"));
#endif

    if (!get_plugin_info) {
#if defined(VISUAL_OS_WIN32)
        visual_log (VISUAL_LOG_ERROR, "Cannot initialize plugin: win32 error code: %ld", GetLastError ());

        FreeLibrary (handle);
#else
        visual_log (VISUAL_LOG_ERROR, _("Cannot initialize plugin: %s"), dlerror ());

        dlclose (handle);
#endif

        return NULL;
    }

    VisPluginInfo const* plugin_info = get_plugin_info ();

    if (!plugin_info) {
        visual_log (VISUAL_LOG_ERROR, _("Cannot get plugin info while loading."));

#if defined(VISUAL_OS_WIN32)
        FreeLibrary (handle);
#else
        dlclose (handle);
#endif

        return NULL;
    }

    VisPluginData *plugin = visual_plugin_new ();
    plugin->ref = ref;
    visual_object_ref (VISUAL_OBJECT (ref));

    plugin->info = visual_plugin_info_clone (plugin_info);
    plugin->realized = FALSE;
    plugin->handle = handle;

    // Now the plugin is set up and ready to be realized, also random
    // seed its random context
    LV::Time time = LV::Time::now ();
    plugin->random = new LV::RandomContext (time.to_usecs ());

    return plugin;
}

int visual_plugin_realize (VisPluginData *plugin)
{
    VisParamContainer *paramcontainer;

    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    if (plugin->realized) {
        return -VISUAL_ERROR_PLUGIN_ALREADY_REALIZED;
    }

    paramcontainer = visual_plugin_get_params (plugin);
    visual_param_container_set_eventqueue (paramcontainer, plugin->eventqueue);

    visual_log (VISUAL_LOG_DEBUG, "Activating plugin '%s'", plugin->info->plugname);
    plugin->info->init (plugin);
    plugin->realized = TRUE;

    return VISUAL_OK;
}

VisPluginRef *visual_plugin_get_reference (char const* plugin_path)
{
    visual_return_val_if_fail (plugin_path != NULL, NULL);

#if defined(VISUAL_OS_WIN32)
    HMODULE handle = LoadLibrary (plugin_path);
#else
    void* handle = dlopen (plugin_path, RTLD_LAZY);
#endif

    if (!handle) {
#if defined(VISUAL_OS_WIN32)
        visual_log (VISUAL_LOG_ERROR, "Cannot load plugin: win32 error code: %ld", GetLastError());
#else
        visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin: %s"), dlerror ());
#endif

        return NULL;
    }

#if defined(VISUAL_OS_WIN32)
    int* plugin_version = reinterpret_cast<int*> (GetProcAddress (handle, VISUAL_PLUGIN_VERSION_TAG));
#else
    int* plugin_version = static_cast<int*> (dlsym (handle, VISUAL_PLUGIN_VERSION_TAG));
#endif

    if (!plugin_version || *plugin_version != VISUAL_PLUGIN_API_VERSION) {
        visual_log (VISUAL_LOG_ERROR, _("Plugin %s is not compatible with version %s of libvisual"),
                plugin_path, visual_get_version ());

#if defined(VISUAL_OS_WIN32)
        FreeLibrary (handle);
#else
        dlclose (handle);
#endif

        return NULL;
    }

    VisPluginGetInfoFunc get_plugin_info;

#if defined(VISUAL_OS_WIN32)
    get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (GetProcAddress (handle, "get_plugin_info"));
#else
    get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (dlsym (handle, "get_plugin_info"));
#endif

    if (!get_plugin_info) {
#if defined(VISUAL_OS_WIN32)
        visual_log (VISUAL_LOG_ERROR, "Cannot initialize plugin: win32 error code: %ld", GetLastError ());

        FreeLibrary (handle);
#else
        visual_log (VISUAL_LOG_ERROR, _("Cannot initialize plugin: %s"), dlerror ());

        dlclose (handle);
#endif

        return NULL;
    }

    VisPluginInfo const* plugin_info = get_plugin_info ();

    if (!plugin_info) {
        visual_log (VISUAL_LOG_ERROR, _("Cannot get plugin info"));

#if defined(VISUAL_OS_WIN32)
        FreeLibrary (handle);
#else
        dlclose (handle);
#endif

        return NULL;
    }

    VisPluginRef* ref = visual_plugin_ref_new ();

    ref->info = visual_plugin_info_clone (plugin_info);
    ref->file = visual_strdup (plugin_path);

#if defined(VISUAL_OS_WIN32)
    FreeLibrary (handle);
#else
    dlclose (handle);
#endif

    return ref;
}

int visual_plugin_get_api_version ()
{
    return VISUAL_PLUGIN_API_VERSION;
}

VisPluginEnviron *visual_plugin_environ_new (const char *type, VisObject *envobj)
{
    VisPluginEnviron *enve;

    enve = visual_mem_new0 (VisPluginEnviron, 1);

    /* Do the VisObject initialization */
    visual_object_initialize (VISUAL_OBJECT (enve), TRUE, plugin_environ_dtor);

    enve->type = type;
    enve->environment = envobj;

    return enve;
}

int visual_plugin_environ_add (VisPluginData *plugin, VisPluginEnviron *enve)
{
    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
    visual_return_val_if_fail (enve != NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL);
    visual_return_val_if_fail (enve->type != NULL, -VISUAL_ERROR_NULL);

    visual_plugin_environ_remove (plugin, enve->type);

    return visual_list_add (&plugin->environment, enve);
}

int visual_plugin_environ_remove (VisPluginData *plugin, const char *type)
{
    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
    visual_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

    VisPluginEnviron *enve;
    VisListEntry *le = NULL;

    while ((enve = static_cast<VisPluginEnviron*>(visual_list_next (&plugin->environment, &le))) != NULL) {

        /* Remove from list */
        if (strcmp (enve->type, type) == 0) {
            visual_list_delete (&plugin->environment, &le);

            visual_object_unref (VISUAL_OBJECT (enve));

            return VISUAL_OK;
        }
    }

    return VISUAL_OK;
}

VisObject *visual_plugin_environ_get (VisPluginData *plugin, const char *type)
{
    visual_return_val_if_fail (plugin != NULL, NULL);
    visual_return_val_if_fail (type != NULL, NULL);

    VisPluginEnviron *enve;
    VisListEntry *le = NULL;

    while ((enve = static_cast<VisPluginEnviron*> (visual_list_next (&plugin->environment, &le))) != NULL) {

        if (strcmp (enve->type, type) == 0)
            return enve->environment;
    }

    return NULL;
}

