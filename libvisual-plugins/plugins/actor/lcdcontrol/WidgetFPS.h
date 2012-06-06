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

#ifndef __WIDGET_FPS_H__
#define __WIDGET_FPS_H__

#include <json/json.h>
#include <string>
#include <sstream>
#include <vector>
#include <libvisual/libvisual.h>

#include "Widget.h"
#include "Font_8x16.h"
#include "LCDText.h"
#include "Property.h"
#include "LCDTimer.h"
#include "Stats.h"
#include "debug.h"

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetFPS : public Widget {
    std::vector<char> FB_;
    std::vector<char> ch_;
    bool flipped_;
    int update_;

    Stats stats_;

    LCDTimer *timer_;

    void (*Draw)(WidgetFPS *);
    
    public:
    WidgetFPS(LCDCore *visitor, std::string name, Json::Value *section, 
        int row, int col, int layer);
    ~WidgetFPS();
    void TextScroll() {};
    void SetupChars();
    void Update();
    void Start();
    void Stop();
    std::vector<char> GetFB() { return FB_; }
    std::vector<char> GetCh() { return ch_; }

    void Resize(int rows, int cols, int old_rows, int old_cols);
};

}; // End namespace


#endif
