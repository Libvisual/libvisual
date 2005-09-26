// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_config.hpp,v 1.1 2005-09-26 14:06:06 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LVCPP_CONFIG_HPP
#define LVCPP_CONFIG_HPP

#include <libvisual/lv_config.h>
#include <lv_object.hpp>
#include <lv_error.hpp>
#include <string>

namespace Lv
{
  class ConfigRegistrySection;

  class ConfigRegistry
      : public Object
  {
  public:

      ConfigRegistry ()
          : Object (vis_config_registry_to_object (visual_config_registry_new ()))
      {}

      ConfigRegistry (const std::string& filename)
          : Object (vis_config_registry_to_object (create_from_file (filename)))
      {}

  private:

      static inline VisObject *vis_config_registry_to_object (VisConfigRegistry *registry)
      {
          return reinterpret_cast<VisObject *> (registry);
      }

      static VisConfigRegistry *create_from_file (const std::string& filename)
      {
          VisConfigRegistry *registry = visual_config_registry_open (filename.c_str ());

          if (!registry)
              throw FileError (std::string ("Failed to open registry file: ") + filename);

          return registry;
      }
  };

} // namespace Lv

#endif // #ifndef LVCPP_CONFIG_HPP
