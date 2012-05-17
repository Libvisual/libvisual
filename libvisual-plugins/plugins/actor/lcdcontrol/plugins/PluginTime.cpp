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

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cstring>
#include <string>

#include "debug.h"
#include "PluginTime.h"
#include "Evaluator.h"

using namespace LCD;

unsigned int PluginTime::Time()
{
    return time(NULL);
}


std::string PluginTime::Strftime(std::string arg1, double arg2)
{
    char value[256];
    time_t t = arg2;

    value[0] = '\0';
    strftime(value, sizeof(value), arg1.c_str(), localtime(&t));

    return value;
}

std::string PluginTime::Strftime_tz(std::string arg1, double arg2, std::string arg3)
{
    char value[256] = "";
    time_t t = arg2;
    char *tz = (char *)arg3.c_str();
    char *old_tz;

    old_tz = getenv("TZ");

    /*
     * because the next setenv() call may overwrite that string, we
     * duplicate it here
     */
    if (old_tz) {
        old_tz = strdup(old_tz);
    }

    setenv("TZ", tz, 1);
    tzset();

    strftime(value, sizeof(value), arg1.c_str(), localtime(&t));

    if (old_tz) {
        setenv("TZ", old_tz, 1);
    } else {
        unsetenv("TZ");
    }
    tzset();

    free(old_tz);

    return value;
}


unsigned long PluginTime::GetTimeOfDay() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

void PluginTime::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("time", objVal);
*/
}

