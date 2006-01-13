// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_config.hpp,v 1.2 2006-01-13 06:51:53 descender Exp $
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

// Notes:
//
// * ConfigRegistry/ConfigRegistrySection is incomplete and sorta
//   forgotten at this point of time. synap say this will be revamped so
//   all this may disappear before you know it (2005/09/27).
//
// * functions such as visual_config_registry_find() are not so
//   simple to wrap directly. They return/accept VisConfigSection
//   pointers and use them internally. Need to think of how to properly
//   copy and destroy temporary VisConfigSection wrappers returned
//   from find().

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

      inline void sync ()
      {
          visual_config_registry_sync (&vis_config_registry ());
      }

      inline const VisConfigRegistry& vis_config_registry () const
      {
          return reinterpret_cast<const VisConfigRegistry&> (vis_object ());
      }

      inline VisConfigRegistry& vis_config_registry ()
      {
          return reinterpret_cast<VisConfigRegistry&> (vis_object ());
      }

  private:

      ConfigRegistry (const ConfigRegistry&);
      const ConfigRegistry& operator = (const ConfigRegistry&);

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

  class ConfigRegistrySection
      : public Object
  {
  public:

      ConfigRegistrySection ()
          : Object (vis_config_registry_section_to_object (visual_config_registry_section_new ()))
      {}

      ConfigRegistrySection (const std::string& filename, const std::string& name)
          : Object (vis_config_registry_section_to_object (create_from_file (filename, name)))
      {}

      inline const VisConfigRegistrySection& vis_config_registry_section () const
      {
          return reinterpret_cast<const VisConfigRegistrySection&> (vis_object ());
      }

      inline VisConfigRegistrySection& vis_config_registry_section ()
      {
          return reinterpret_cast<VisConfigRegistrySection&> (vis_object ());
      }

  private:

      ConfigRegistrySection (const ConfigRegistrySection&);
      const ConfigRegistrySection& operator = (const ConfigRegistrySection&);

      static inline VisObject *vis_config_registry_section_to_object (VisConfigRegistrySection *section)
      {
          return reinterpret_cast<VisObject *> (section);
      }

      static VisConfigRegistrySection *create_from_file (const std::string& filename,
                                                         const std::string& name)
      {
          VisConfigRegistrySection *section = visual_config_registry_section_open (name.c_str (),
                                                                                   filename.c_str ());

          if (!section)
              throw FileError (std::string ("Failed to open registry file: ") + filename);

          return section;
      }
  };

} // namespace Lv

#endif // #ifndef LVCPP_CONFIG_HPP
