/* $Id$
 * $URL$
 *
 * Copyright (C) 2003 Michael Reinelt <michael@reinelt.co.at>
 * Copyright (C) 2004 The LCD4Linux Team <lcd4linux-devel@users.sourceforge.net>
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

#ifndef __WIDGET_ICON__
#define __WIDGET_ICON__

#include <json/json.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "Widget.h"
#include "SpecialChar.h"
#include "Property.h"
#include "LCDTimer.h"

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetSuperscope : public Widget {
    public:
    Property *visible_;
    std::string init_, beat_, frame_, point_;
    int update_;
    int ch_;
    int index_;
    int channel_source;
    int drawmode;
    VisPalette *pal;

    double n, b, x, y, i, v, w, h, red, green, blue, linesize, skip, drawmode, t, d; 

    SpecialChar *bitmap_;
    VisVideo *video_;
    LCDTimer *timer_;

    void (*Draw)(WidgetSuperscope *);

    WidgetSuperscope(LCDCore *visitor, std::string name, Json::Value *section, int row, int col, int layer);
    ~WidgetSuperscope();
    void SetupChars();
    void Start();
    void Stop();
    SpecialChar GetBitmap() { return *bitmap_; }
    int GetCh() { return ch_; }
    Property *GetVisible() { return visible_; }

    void Update();
    void TextScroll() {}

    void Resize(int rows, int cols, int old_rows, int old_cols);
};

inline std::vector<std::string> &Split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline std::vector<std::string> Split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return Split(s, delim, elems);
}

}; // End namespace


#endif
