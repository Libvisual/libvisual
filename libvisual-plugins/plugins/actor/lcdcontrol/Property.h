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

#ifndef __PROPERTY__
#define __PROPERTY__

#include <json/json.h>
#include <string>

#include "CFG.h"

namespace LCD {

class LCDCore;

class LCDText;
class LCDGraphic;

class Property {
    bool is_valid;
    LCDCore *visitor_;
    Json::Value *expression_;
    std::string name_;
    std::string result_;

    public:
    Property(const Property &prop);
    Property(LCDCore *visitor, Json::Value *section, std::string name,
        Json::Value *defval);
    ~Property();
    bool Valid();
    int Eval();
    double P2N();
    int P2INT();
    std::string P2S();
    void SetValue(Json::Value val);
    Property operator=(Property prop);
};

}; // End namespace

#endif
