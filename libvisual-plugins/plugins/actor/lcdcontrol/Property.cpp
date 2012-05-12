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
#include <cstdlib>
#include <stdio.h>
#include <json/json.h>

#include "Property.h"
#include "LCDCore.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "CFG.h"
#include "Evaluator.h"
#include "debug.h"


using namespace LCD;

Property::Property(LCDCore *v, Json::Value *section, std::string name, 
    Json::Value *defval) {

    visitor_ = v;
    name_ = name;
    is_valid = false;
    expression_ = v->CFG_Fetch_Raw(section, name, defval);
    if( expression_ != NULL && expression_->isString()) {
        result_ = v->Eval(expression_->asCString());
        is_valid = true;
    } else if (expression_ != NULL ) {
        LCDError("Property: <%s> has no expression_ or is not a string field.", name.c_str());
        LCDError("%s", expression_->toStyledString().c_str());
    }
}

Property::Property(const Property &prop) {
    is_valid = prop.is_valid;
    result_ = prop.result_;
    expression_ = prop.expression_;
}

Property::~Property() {
    delete expression_;
}

bool Property::Valid() {
    return is_valid;
}

int Property::Eval() {
    if(!is_valid) 
        return -1;

    int update = 1;

    std::string old;

    result_ = visitor_->Eval(expression_->asCString());

    if(old == result_ ) {
        update = 0;
    }

    return update;
}

double Property::P2N() {
    return strtod(result_.c_str(), NULL);
}

int Property::P2INT() {
    return P2N();
}

std::string Property::P2S() {
    return result_;
}


void Property::SetValue(Json::Value val) {

}

Property Property::operator=(Property prop) {
    return Property(prop);
}
