/* Libvisual - The audio visualisation framework cli tool
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

#ifndef _LV_TOOL_DISPLAY_DRIVER_HPP
#define _LV_TOOL_DISPLAY_DRIVER_HPP

#include <string>
#include <libvisual/libvisual.h>

class Display;

class DisplayDriver {
public:

    virtual LV::VideoPtr create (VisVideoDepth depth,
                                 VisVideoAttrOptions const* vidoptions,
                                 unsigned int width,
                                 unsigned int height,
                                 bool resizable) = 0;

    virtual void close  () = 0;

    virtual void lock   () = 0;

    virtual void unlock () = 0;

    virtual void set_fullscreen (bool fullscreen, bool autoscale) = 0;

    virtual void update_rect (LV::Rect const& rect) = 0;

    virtual void drain_events (VisEventQueue& eventqueue) = 0;

    virtual LV::VideoPtr get_video () const = 0;

    virtual void set_title(std::string const& title) = 0;

    virtual ~DisplayDriver () {}
};

#endif // _LV_TOOL_DISPLAY_DRIVER_HPP
