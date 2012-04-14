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

namespace LV {

  const char *plugin_get_next_by_name (PluginList const& list, const char *name)
  {
      for (unsigned int i = 0; i < list.size (); i++)
      {
          if (std::strcmp (list[i].info->plugname, name) == 0)
          {
              unsigned int next_i = (i + 1) % list.size ();
              return list[next_i].info->plugname;
          }
      }

      return NULL;
  }

  const char *plugin_get_prev_by_name (PluginList const& list, const char *name)
  {
      for (unsigned int i = 0; i < list.size (); i++)
      {
          if (std::strcmp (list[i].info->plugname, name) == 0)
          {
              unsigned int prev_i = (i + list.size () - 1) % list.size ();
              return list[prev_i].info->plugname;
          }
      }

      return NULL;
  }

} // LV namespace

static int plugin_environ_dtor (VisObject *object);
static int plugin_dtor (VisObject *object);

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

    if (plugin->params)
        visual_object_unref (VISUAL_OBJECT (plugin->params));

    delete plugin->eventqueue;

    visual_collection_destroy (VISUAL_COLLECTION (&plugin->environment));

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

VisPluginInfo const* visual_plugin_get_info (VisPluginData *plugin)
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
    visual_return_val_if_fail (plugin != NULL, NULL);

    VisPluginInfo const* pluginfo = visual_plugin_get_info (plugin);
    visual_return_val_if_fail (pluginfo != NULL, NULL);

    return pluginfo->plugin;
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
    visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

    if (plugin->realized)
        plugin->info->cleanup (plugin);

    if (plugin->info->plugin != NULL)
        visual_object_unref (VISUAL_OBJECT (plugin->info->plugin));

    visual_param_container_set_eventqueue (plugin->params, NULL);

    visual_object_unref (VISUAL_OBJECT (plugin));

    return VISUAL_OK;
}

VisPluginData *visual_plugin_load (VisPluginType type, const char *name)
{
    // FIXME: Check if plugin has already been loaded

	VisPluginInfo const* info = LV::PluginRegistry::instance()->get_plugin_info (type, name);
	if (!info)
		return 0;

    VisPluginData *plugin = visual_plugin_new ();

    plugin->info     = info;
    plugin->realized = FALSE;
    plugin->random   = new LV::RandomContext (LV::Time::now ().to_usecs ());

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

