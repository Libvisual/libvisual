/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LCD_BASE__
#define __LCD_BASE__

#include <string>

namespace LCD {

class LCDBase {
    public:
    virtual ~LCDBase() {}
    virtual void Transition() = 0;
    virtual void SignalTransitionStart(std::string layout) = 0;
    virtual void SignalTransitionEnd() = 0;
    virtual int ResizeLCD(int rows, int cols) = 0;

    virtual void ResizeBefore(int rows, int cols) = 0;
    virtual void ResizeAfter() = 0;
    virtual void LayoutChangeBefore() = 0;
    virtual void LayoutChangeAfter() = 0;

    virtual VisVideo *GetVideo() = 0;
    int XRES;
    int YRES;
    int LROWS;
    int LCOLS;
    int DROWS;
    int DCOLS;
    int LAYERS;
    bool clear_on_layout_change_;
};

}; // End namespace

#endif
