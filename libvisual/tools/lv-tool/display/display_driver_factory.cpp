// lv-tool - Libvisual commandline tool
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Daniel Hiepler <daniel@niftylight.de>
//          Chong Kai Xiong <kaixiong@codeleft.sg>
//          Dennis Smit <ds@nerds-incorporated.org>
//
// This file is part of lv-tool.
//
// lv-tool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// lv-tool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with lv-tool.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"
#include "display_driver_factory.hpp"
#include "stdout_driver.hpp"

#if HAVE_SDL
#include "sdl_driver.hpp"
#include "stdout_sdl_driver.hpp"
#endif

#include <unordered_map>

typedef std::unordered_map<std::string, DisplayDriverCreator> CreatorMap;

class DisplayDriverFactory::Impl
{
public:

    CreatorMap creators;
};

DisplayDriverFactory::DisplayDriverFactory ()
    : m_impl (new Impl)
{
    add_driver ("stdout", stdout_driver_new);
#if defined(HAVE_SDL)
    add_driver ("sdl", sdl_driver_new);
    add_driver ("stdout_sdl", stdout_sdl_driver_new);
#endif
}

DisplayDriverFactory::~DisplayDriverFactory ()
{
    // nothing to do
}

void DisplayDriverFactory::add_driver (std::string_view name, Creator const& creator)
{
    std::string const name_str {name};
    m_impl->creators[name_str] = creator;
}

DisplayDriver* DisplayDriverFactory::make (std::string_view name, Display& display)
{
    std::string const name_str {name};
    auto entry = m_impl->creators.find (name_str);

    if (entry == m_impl->creators.end ()) {
        return nullptr;
    }

    return entry->second (display);
}

bool DisplayDriverFactory::has_driver (std::string_view name) const
{
    std::string const name_str {name};
    return (m_impl->creators.find (name_str) != m_impl->creators.end ());
}

DisplayDriverList DisplayDriverFactory::get_driver_list () const
{
    DisplayDriverList list;

    list.reserve (m_impl->creators.size ());

    for (auto creator : m_impl->creators)
        list.push_back (creator.first);

    return list;
}
