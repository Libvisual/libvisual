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

#ifndef __WIDGET_BAR_H__
#define __WIDGET_BAR_H__

#include <json/json.h>
#include <string>
#include <map>
#include <libvisual/libvisual.h>

#include "Property.h"
#include "Widget.h"
#include "RGBA.h"
#include "LCDTimer.h"
#include "debug.h"

namespace LCD {

class LCDCore;

class LCDText;
class LCDGraphic;

typedef enum { STYLE_HOLLOW = 1, STYLE_NORMAL = 2 } STYLE;

class WidgetBar : public Widget {
    Property *expression_;
    Property *expression2_;
    Property *expr_min_;
    Property *expr_max_;

    int color_valid_[2];
    RGBA color_[2];

    DIRECTION direction_;
    int update_;
    STYLE style_;

    double val1_;
    double val2_;
    double min_;
    double max_;

    std::map<int, char> ch_;

    LCDTimer *timer_;

    void (*Draw)(WidgetBar *);

    public:
    WidgetBar(LCDCore *visitor, std::string name, 
        Json::Value *section, int row, int col, int layer);
    ~WidgetBar();
    void SetupChars();
    void Start();
    void Stop();
    DIRECTION GetDirection() { return direction_; }
    STYLE GetStyle() { return style_; }
    double GetVal1() { return val1_; }
    double GetVal2() { return val2_; }
    std::map<int, char> &GetChars() { return ch_; }
    int *GetColorValid() { return color_valid_; }
    RGBA *GetColor() { return color_; }

    void Update();
    void TextScroll() {};

    void Resize(int rows, int cols, int old_rows, int old_cols);
};


}; // End namespace

#endif
