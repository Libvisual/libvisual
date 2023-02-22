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

#include "display.hpp"
#include "display_driver_factory.hpp"
#include <libvisual/libvisual.h>
#include <stdexcept>
#include <string>

using namespace std::string_literals;

class Display::Impl
{
public:

    std::unique_ptr<DisplayDriver> driver;

    Impl ()
        : driver (nullptr)
    {}

    ~Impl ()
    {}
};

Display::Display (std::string_view driver_name)
  : m_impl (new Impl)
{
    m_impl->driver.reset (DisplayDriverFactory::instance().make (driver_name, *this));

    if (!m_impl->driver) {
        std::string const driver_name_str {driver_name};
        throw std::runtime_error {"Failed to load display driver '"s + driver_name_str + "'. Valid driver set? (\"--driver\" parameter)"};
    }
}

Display::~Display ()
{
}

LV::VideoPtr Display::get_video () const
{
    return m_impl->driver->get_video ();
}

LV::VideoPtr Display::create (VisVideoDepth depth,
                              VisVideoAttrOptions const* vidoptions,
                              unsigned int width,
                              unsigned int height,
                              bool resizable)
{
    visual_log (VISUAL_LOG_INFO, "Attempting to create display (%dx%d %s)",
                width, height, visual_video_depth_name (depth));

    return m_impl->driver->create (depth, vidoptions, width, height, resizable);
}

void Display::close ()
{
    visual_log (VISUAL_LOG_INFO, "Closing display");

    m_impl->driver->close ();
}

void Display::set_title(std::string_view title)
{
    m_impl->driver->set_title (title);
}

void Display::lock ()
{
    m_impl->driver->lock ();
}

void Display::unlock ()
{
    m_impl->driver->unlock ();
}

void Display::update_all ()
{
    LV::VideoPtr video = get_video ();
    LV::Rect rect (video->get_width (), video->get_height ());

    m_impl->driver->update_rect (rect);
}

void Display::update_rect (LV::Rect const& rect)
{
    m_impl->driver->update_rect (rect);
}

bool Display::is_fullscreen () const
{
    return m_impl->driver->is_fullscreen ();
}

void Display::set_fullscreen (bool fullscreen, bool autoscale)
{
    m_impl->driver->set_fullscreen (fullscreen, autoscale);
}

void Display::drain_events (VisEventQueue& eventqueue)
{
    m_impl->driver->drain_events (eventqueue);
}
