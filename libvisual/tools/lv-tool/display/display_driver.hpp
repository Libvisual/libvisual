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

#ifndef _LV_TOOL_DISPLAY_DRIVER_HPP
#define _LV_TOOL_DISPLAY_DRIVER_HPP

#include <string_view>
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

    virtual bool is_fullscreen () const = 0;

    virtual void set_fullscreen (bool fullscreen, bool autoscale) = 0;

    virtual void update_rect (LV::Rect const& rect) = 0;

    virtual void drain_events (VisEventQueue& eventqueue) = 0;

    virtual LV::VideoPtr get_video () const = 0;

    virtual void set_title (std::string_view title) = 0;

    virtual ~DisplayDriver () {}
};

#endif // _LV_TOOL_DISPLAY_DRIVER_HPP
