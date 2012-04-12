/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>,
 * Copyright (C) 2012 Daniel Hiepler <daniel@niftylight.de>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel@niftylight.de>
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


#ifndef _LV_TOOL_DISPLAY_HPP
#define _LV_TOOL_DISPLAY_HPP

#include <string>
#include <libvisual/libvisual.h>
#include <libvisual/lv_scoped_ptr.hpp>

class SADisplayDriver;

class SADisplay {
public:

    explicit SADisplay (std::string const& driver_name);

    ~SADisplay ();

    bool create (VisVideoDepth depth, VisVideoAttributeOptions const* vidoptions,
        unsigned int width, unsigned int height, bool resizable = true);

    void close ();

    void lock ();

    void unlock ();

    void update_all ();

    void update_rect (LV::Rect const& rect);

    VisVideo* get_video () const;

    void set_fullscreen (bool fullscreen, bool autoscale);

    void drain_events (VisEventQueue& eventqueue);

    void set_fps_limit (unsigned int fps);

    unsigned int get_fps_total () const;

    float get_fps_average () const;

    VisVideo *get_screen () const;

private:

    class Impl;
    LV::ScopedPtr<Impl> m_impl;
};

#endif // _LV_TOOL_DISPLAY_HPP
