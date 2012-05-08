/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>,
 * Copyright (C) 2012 Daniel Hiepler <daniel@niftylight.de>
 *                    Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel@niftylight.de>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

#include "display.hpp"
#include "display_driver_factory.hpp"
#include <libvisual/libvisual.h>
#include <stdexcept>
#include <string>

class SADisplay::Impl
{
public:

    LV::ScopedPtr<SADisplayDriver> driver;
    unsigned int     frames_drawn;
    LV::Timer        timer;

    Impl ()
        : driver       (0)
        , frames_drawn (0)
    {}

    ~Impl ()
    {}
};

SADisplay::SADisplay (std::string const& driver_name)
  : m_impl (new Impl)
{
    m_impl->driver.reset (DisplayDriverFactory::instance().make (driver_name, *this));

    if (!m_impl->driver) {
        throw std::runtime_error ("Failed to load display driver '" + driver_name + "'");
    }
}

SADisplay::~SADisplay ()
{
}

LV::VideoPtr SADisplay::get_video () const
{
    return m_impl->driver->get_video ();
}

LV::VideoPtr SADisplay::create (VisVideoDepth depth,
                                VisVideoAttrOptions const* vidoptions,
                                unsigned int width,
                                unsigned int height,
                                bool resizable)
{
    return m_impl->driver->create (depth, vidoptions, width, height, resizable);
}

void SADisplay::close ()
{
    m_impl->driver->close ();
}

void SADisplay::set_title(std::string const& title)
{
    m_impl->driver->set_title(title);
}

void SADisplay::lock ()
{
    m_impl->driver->lock ();
}

void SADisplay::unlock ()
{
    m_impl->driver->unlock ();
}

void SADisplay::update_all ()
{
    LV::VideoPtr video = get_video ();
    LV::Rect rect (0, 0, video->get_width (), video->get_height ());

    m_impl->frames_drawn++;

    if (!m_impl->timer.is_active ())
        m_impl->timer.start ();

    m_impl->driver->update_rect (rect);
}

void SADisplay::update_rect (LV::Rect const& rect)
{
    m_impl->driver->update_rect (rect);
}

void SADisplay::set_fullscreen (bool fullscreen, bool autoscale)
{
    m_impl->driver->set_fullscreen (fullscreen, autoscale);
}

void SADisplay::drain_events (VisEventQueue& eventqueue)
{
    m_impl->driver->drain_events (eventqueue);
}

void SADisplay::set_fps_limit (unsigned int fps)
{
    // FIXME: Implement this
}

unsigned int SADisplay::get_fps_total () const
{
    return m_impl->frames_drawn;
}

float SADisplay::get_fps_average () const
{
    return m_impl->frames_drawn / m_impl->timer.elapsed ().to_secs ();
}
