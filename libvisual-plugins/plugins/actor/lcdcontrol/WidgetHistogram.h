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

#ifndef __WIDGET_HISTOGRAM_H__
#define __WIDGET_HISTOGRAM_H__

#include <json/json.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <libvisual/libvisual.h>

#include "Property.h"
#include "Widget.h"
#include "RGBA.h"
#include "LCDTimer.h"
#include "debug.h"

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetHistogram : public Widget {
    int min_;
    int max_;
    std::string string_;
    Property *expression_;
    Property *expr_min_;
    Property *expr_max_;
    int gap_;
    int direction_;
    int offset_;
    int update_;
    std::vector<double> history_;
    std::map<char, char> ch_;

    LCDTimer *timer_;

    void (*Draw)(WidgetHistogram *);

    public:
    WidgetHistogram(LCDCore *visitor, std::string name, 
        Json::Value *section, int row, int col, int layer);
    ~WidgetHistogram();
    void SetupChars();
    void Start();
    void Stop();
    std::map<char, char> GetCh() { return ch_; }
    std::vector<double> GetHistory() { return history_; }
    int GetOffset() { return offset_; }

    void Update();
    void TextScroll() {}

    void Resize(int rows, int cols, int old_rows, int old_cols);
};

}; // End namespace


#endif
