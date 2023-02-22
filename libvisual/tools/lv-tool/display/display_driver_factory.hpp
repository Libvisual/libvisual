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

#ifndef _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
#define _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP

#include "display_driver.hpp"
#include <memory>
#include <functional>
#include <string_view>
#include <vector>

typedef std::function<DisplayDriver* (Display& display)> DisplayDriverCreator;

typedef std::vector<std::string> DisplayDriverList;

class DisplayDriverFactory final
{
public:

    typedef DisplayDriverCreator Creator;

    static DisplayDriverFactory& instance ()
    {
	    static DisplayDriverFactory m_instance;
	    return m_instance;
    }

    DisplayDriver* make (std::string_view name, Display& display);

    void add_driver (std::string_view name, Creator const& creator);

    bool has_driver (std::string_view name) const;

    DisplayDriverList get_driver_list () const;

private:

    class Impl;
    const std::unique_ptr<Impl> m_impl;

    DisplayDriverFactory ();

    ~DisplayDriverFactory ();
};

#endif // _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
