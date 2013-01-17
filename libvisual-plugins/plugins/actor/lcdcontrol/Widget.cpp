/* TODO: Chain child widgets, to call the next widget's start method in the parent's stop function. Start draws immediately, so that should draw over the previous widget -- the parent -- for sure. No bugs allowed.
 */
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

#include <iostream>
#include <string>
#include <json/json.h>

#include "Widget.h"
#include "RGBA.h"
#include "LCDCore.h"
#include "debug.h"

using namespace LCD;

Widget::Widget(LCDCore *visitor, std::string name, Json::Value *config,
     int row, int col, 
     int layer, int type ) {
    
    visitor_ = visitor;

    lcd_type_ = visitor->GetType();
    if(name.empty())
        return;
    int pos1 = name.find(":");
    int pos2 = name.rfind(":");
    name_ = name;
    section_ = config;
    if(row >= visitor->GetLCD()->LROWS) row_ = visitor->GetLCD()->LROWS - 1;
    else row_ = row;
    if(col >= visitor->GetLCD()->LCOLS) col_ = visitor->GetLCD()->LCOLS - 1;
    else col_ = col;
    layer_ = layer;
    type_ = type;
    if(pos1 < 0 || pos2 < 0)
        return;
    layout_base_ = name.substr(0, pos1);
    widget_base_ = name.substr(pos1 + 1, pos2-pos1-1);
    started_ = false;
}

Widget::~Widget() {
    delete section_;
}
    
int Widget::WidgetColor(Json::Value *section, std::string key, RGBA *C) {

    C->R = 0;
    C->G = 0;
    C->B = 0;
    C->A = 0;

    Json::Value *val = NULL;
    
    val = visitor_->CFG_Fetch_Raw(section, key);

    if (val == NULL)
        return 0;

    if (color2RGBA(val->asCString(), C) < 0) {
        LCDError("widget '%s': ignoring illegal %s color '%s'", 
            name_.c_str(), key.c_str(), val->asCString());
        return 0;
    }

    delete val;
    return 1;
}


void Widget::SetupChars() {}
void Widget::Start() {}
void Widget::Stop() {}
