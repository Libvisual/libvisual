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
#include "WidgetScript.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDCore.h"
#include "debug.h"

using namespace LCD;

WidgetScript::WidgetScript(LCDCore *v, std::string n, Json::Value *section) :
    Widget(v, n, section, 0, 0, 0, WIDGET_TYPE_KEYPAD) {

    Json::Value *scriptFile = v->CFG_Fetch_Raw(section, "file", NULL);
    if(scriptFile) {
        FILE *file = fopen( scriptFile->asCString(), "rb");
        if( !file ) {
            LCDError("WidgetScript: Unable to open file <%s>", scriptFile->asCString());
            return;
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buffer = new char[size+1];
        buffer[size] = 0x0;
        if( fread( buffer, 1, size, file) != (unsigned long) size)
            return;
        std::string script = buffer;
        delete []buffer;
        fclose(file);
        v->Eval(script);
    }
}

WidgetScript::~WidgetScript() {
}

