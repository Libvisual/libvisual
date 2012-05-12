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

#include <stdio.h>
#include <cstring>

#include "debug.h"
#include "PluginFile.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

using namespace LCD;

/* function 'readline' */
/* takes two arguments, file name and line number */
/* returns text of that line */

std::string PluginFile::Readline(std::string arg1, int arg2) {
    char value[80], val2[80];
    FILE *fp;
    int reqline, i, size;

    reqline = arg2;
    fp = fopen(arg1.c_str(), "r");
    if (!fp) {
        LCDError("readline couldn't open file '%s'", arg1.c_str());
        value[0] = '\0';
    } else {
        i = 0;
        while (!feof(fp) && i++ < reqline) {
            fgets(val2, sizeof(val2), fp);
            size = strcspn(val2, "\r\n");
            strncpy(value, val2, size);
            value[size] = '\0';
            /* more than 80 chars, chew up rest of line */
            while (!feof(fp) && strchr(val2, '\n') == NULL) {
                fgets(val2, sizeof(val2), fp);
            }
        }
        fclose(fp);
        if (i <= reqline) {
            LCDError("readline requested line %d but file only had %d lines", reqline, i - 1);
            value[0] = '\0';
        }
    }

    return value;
}

void PluginFile::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("file", objVal);
*/
}

