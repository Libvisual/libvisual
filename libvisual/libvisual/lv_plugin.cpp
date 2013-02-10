/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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
#include <cstring>

namespace LV {

  class PluginData
  {
  public:

      VisPluginInfo const *info;

      EventQueue           eventqueue;
      ParamList            params;
      int                  plugflags;
      RandomContext        random;
      bool                 realized;
      void                *priv;

      PluginData ();
  };

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

      return nullptr;
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

      return nullptr;
  }

  PluginData::PluginData ()
      : info      {nullptr}
      , plugflags {0}
      , random    {LV::rand ()}
      , realized  {false}
      , priv      {nullptr}
  {
      // nothing
  }

} // LV namespace

void visual_plugin_events_pump (VisPluginData *plugin)
{
    visual_return_if_fail (plugin != nullptr);

    if (plugin->info->events) {
        plugin->info->events (plugin, &plugin->eventqueue);
    }
}

VisEventQueue *visual_plugin_get_event_queue (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    return &plugin->eventqueue;
}

VisPluginInfo const* visual_plugin_get_info (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    return plugin->info;
}

VisParamList *visual_plugin_get_params (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    return &plugin->params;
}

VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    return &plugin->random;
}

void *visual_plugin_get_specific (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    auto pluginfo = visual_plugin_get_info (plugin);
    visual_return_val_if_fail (pluginfo != nullptr, nullptr);

    return pluginfo->plugin;
}

static VisPluginData *visual_plugin_new ()
{
    return new VisPluginData;
}

static void visual_plugin_free (VisPluginData *plugin)
{
    delete plugin;
}

void visual_plugin_unload (VisPluginData *plugin)
{
    visual_return_if_fail (plugin != nullptr);

    if (plugin->realized) {
        plugin->info->cleanup (plugin);
    }

    visual_plugin_free (plugin);
}

VisPluginData *visual_plugin_load (VisPluginType type, const char *name)
{
    // FIXME: Check if plugin has already been loaded

    auto info = LV::PluginRegistry::instance()->get_plugin_info (type, name);
    if (!info) {
        return nullptr;
    }

    auto plugin = visual_plugin_new ();
    plugin->info = info;

    return plugin;
}

int visual_plugin_realize (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, FALSE);

    if (plugin->realized) {
        return TRUE;
    }

    visual_log (VISUAL_LOG_DEBUG, "Activating plugin '%s'", plugin->info->plugname);

    auto params = visual_plugin_get_params (plugin);
    params->set_event_queue (plugin->eventqueue);

    if (!plugin->info->init (plugin)) {
        visual_log (VISUAL_LOG_ERROR, "Failed to initialise plugin");
        return FALSE;
    }

    plugin->realized = TRUE;

    return TRUE;
}

int visual_plugin_is_realized (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, FALSE);

    return plugin->realized;
}

void visual_plugin_set_private (VisPluginData *plugin, void *priv)
{
    visual_return_if_fail (plugin != nullptr);

    plugin->priv = priv;
}

void *visual_plugin_get_private (VisPluginData *plugin)
{
    visual_return_val_if_fail (plugin != nullptr, nullptr);

    return plugin->priv;
}

int visual_plugin_get_api_version ()
{
    return VISUAL_PLUGIN_API_VERSION;
}
