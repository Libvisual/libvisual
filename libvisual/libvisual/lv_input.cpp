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
#include "lv_input.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"
#include <stdexcept>
#include <string>

namespace LV {

  class Input::Impl
  {
  public:
      VisPluginData*              plugin;
      Audio                       audio;
      std::function<bool(Audio&)> callback;

      Impl ();
      ~Impl ();

      VisInputPlugin* get_input_plugin () const;
  };

  Input::Impl::Impl ()
      : plugin {nullptr}
  {
      // nothing
  }

  Input::Impl::~Impl ()
  {
      if (plugin) {
          visual_plugin_unload (plugin);
      }
  }

  VisInputPlugin* Input::Impl::get_input_plugin () const
  {
      return static_cast<VisInputPlugin*> (visual_plugin_get_info (plugin)->plugin);
  }

  bool Input::available(std::string_view name) {
      return LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_INPUT, name);
  }

  InputPtr Input::load (std::string_view name)
  {
      try {
          return {new Input{name}, false};
      }
      catch (std::exception& error) {
          visual_log (VISUAL_LOG_ERROR, "%s", error.what ());
          return nullptr;
      }
  }

  Input::Input (std::string_view name)
      : m_impl      {new Impl}
      , m_ref_count {1}
  {
      if (!available (name)) {
          throw std::runtime_error {"Input plugin not found"};
      }

      m_impl->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_INPUT, std::string {name}.c_str ());
      if (!m_impl->plugin) {
          throw std::runtime_error {"Failed to load input plugin"};
      }
  }

  Input::~Input ()
  {
      // nothing
  }

  bool Input::realize ()
  {
      if (m_impl->callback) {
          return true;
      }

      return visual_plugin_realize (m_impl->plugin);
  }

  VisPluginData* Input::get_plugin ()
  {
      return m_impl->plugin;
  }

  void Input::set_callback (std::function<bool(Audio&)> const& callback)
  {
      m_impl->callback = callback;
  }

  Audio const& Input::get_audio ()
  {
      return m_impl->audio;
  }

  bool Input::run ()
  {
      if (m_impl->callback) {
          m_impl->callback (m_impl->audio);
          return true;
      }

      auto input_plugin = m_impl->get_input_plugin ();

      if (!input_plugin) {
          visual_log (VISUAL_LOG_ERROR, "The input plugin is not loaded correctly.");
          return false;
      }

      input_plugin->upload (m_impl->plugin, &m_impl->audio);

      return true;
  }
}
