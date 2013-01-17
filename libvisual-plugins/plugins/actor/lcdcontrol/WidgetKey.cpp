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

#include <string>
#include <json/json.h>

#include "Widget.h"
#include "WidgetKey.h"
#include "Property.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDCore.h"
#include "debug.h"

using namespace LCD;

WidgetKey::WidgetKey(LCDCore *v, std::string n, Json::Value *section) :
    Widget(v, n, section, 0, 0, 0, WIDGET_TYPE_KEYPAD) {

    Json::Value *val = v->CFG_Fetch_Raw(section, "expression", new Json::Value(""));
    code_ = val->asString();
    delete val;

    val = v->CFG_Fetch(section, "key", new Json::Value(-1));
    key_ = val->asInt();
    delete val;

    started_ = false;
}

WidgetKey::~WidgetKey() {
}

void WidgetKey::KeyPressed(int k) {
     if( started_ && k == key_ )
         Update();
}

void WidgetKey::Update() {
    LCDError("WidgetKey::Update() %d", key_);
    visitor_->Eval(code_.c_str());
}

void WidgetKey::Start() {
    started_ = true;
}

void WidgetKey::Stop() {
    started_ = false;
}

