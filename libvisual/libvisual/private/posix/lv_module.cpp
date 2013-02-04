/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
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
#include "lv_module.hpp"
#include "lv_common.h"
#include <stdexcept>
#include <dlfcn.h>

namespace LV {

  class Module::Impl
  {
  public:

      void*       handle;
      std::string path;
  };

  ModulePtr Module::load (std::string const& path)
  {
      try {
          return {new Module {path}, false};
      }
      catch (std::exception& error) {
          visual_log (VISUAL_LOG_ERROR, "%s", error.what ());
          return nullptr;
      }
  }

  Module::Module (std::string const& path)
      : m_impl (new Impl)
      , m_ref_count (1)
  {
      visual_log (VISUAL_LOG_DEBUG, "Loading shared object: %s", path.c_str ());

      m_impl->handle = dlopen (path.c_str(), RTLD_LAZY);
      m_impl->path = path;

      if (!m_impl->handle) {
          std::string msg = "Failed to load shared object (" + path + "): " + dlerror();
          throw std::runtime_error {msg};
      }
  }

  Module::~Module ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Unloading shared object: %s", m_impl->path.c_str ());

      dlclose (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return dlsym (m_impl->handle, name.c_str ());
  }

  std::string const& Module::path_suffix ()
  {
      static std::string str (".so");
      return str;
  }

} // LV namespace
