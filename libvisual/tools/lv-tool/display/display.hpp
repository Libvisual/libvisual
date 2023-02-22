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

#ifndef _LV_TOOL_DISPLAY_HPP
#define _LV_TOOL_DISPLAY_HPP

#include <libvisual/libvisual.h>
#include <memory>
#include <string_view>

class DisplayDriver;

class Display final
{
public:

    explicit Display (std::string_view driver_name);

    Display (Display const&) = delete;

    ~Display ();

    Display& operator= (Display const&) = delete;

    LV::VideoPtr create (VisVideoDepth depth,
                         VisVideoAttrOptions const* vidoptions,
                         unsigned int width,
                         unsigned int height,
                         bool resizable = true);

    void close ();

    void lock ();

    void unlock ();

    void update_all ();

    void update_rect (LV::Rect const& rect);

    LV::VideoPtr get_video () const;

    void set_title(std::string_view title);

    bool is_fullscreen () const;

    void set_fullscreen (bool fullscreen, bool autoscale);

    void drain_events (VisEventQueue& eventqueue);

private:

    class Impl;
    const std::unique_ptr<Impl> m_impl;
};

class DisplayLock final
{
public:

    DisplayLock (Display& display)
        : m_display (display)
    {
        m_display.lock ();
    }

    ~DisplayLock ()
    {
        m_display.unlock ();
    }

    DisplayLock (DisplayLock&) = delete;

    DisplayLock& operator= (DisplayLock&) = delete;

private:

    Display& m_display;
};

#endif // _LV_TOOL_DISPLAY_HPP
