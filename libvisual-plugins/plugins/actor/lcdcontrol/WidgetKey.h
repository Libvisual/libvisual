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

#ifndef __WIDGET_KEY_H__
#define __WIDGET_KEY_H__

#include <string>
#include <json/json.h>

#include "Widget.h"
#include "Property.h"

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetKey : public Widget {
    std::string code_;
    int key_;
    bool started_;

    public:
    WidgetKey(LCDCore *v, std::string name, Json::Value *config);
    ~WidgetKey();
    void Start();
    void Stop();
    void KeyPressed(int k);

    void TextScroll() {}
    void Update();
};

}; // End namespace


#endif
