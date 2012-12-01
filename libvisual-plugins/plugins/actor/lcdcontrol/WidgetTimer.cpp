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

#include <string>
#include <json/json.h>

#include "Widget.h"
#include "WidgetTimer.h"
#include "Property.h"
#include "LCDCore.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "debug.h"

using namespace LCD;

WidgetTimer::WidgetTimer(LCDCore *v, std::string n, Json::Value *section) :
    Widget(v, n, section, 0, 0, 0, WIDGET_TYPE_TIMER) {

    expression_ = new Property(v, section, "expression", new Json::Value(""));

    Json::Value *val = v->CFG_Fetch(section, "update", new Json::Value(-1));
    update_ = val->asInt();
    delete val;

/*
    timer_ = new QTimer();
    timer_->setSingleShot(false);
    timer_->setInterval(update_);
    //QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(Update()));
*/
}

WidgetTimer::~WidgetTimer() {
    Stop();
    delete expression_;
}

void WidgetTimer::Start() {
    if( update_ < 0)
        return;
    //timer_->start();
    Update();
}

void WidgetTimer::Stop() {
    //timer_->stop();
}

void WidgetTimer::Update() {
    if(!expression_->Valid())
        return;
    expression_->Eval();
}


