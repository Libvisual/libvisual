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

#include <stdlib.h>
#include <cstring>
#include <string>
#include <errno.h>
#include <sys/vfs.h>
#include <iostream>

#include "debug.h"

#include "PluginStatfs.h"
#include "Evaluator.h"

using namespace LCD;

double PluginStatfs::Statfs(std::string arg1, std::string arg2)
{
    struct statfs buf;
    char *path, *key;
    double value;

    path = strdup(arg1.c_str());
    key = strdup(arg2.c_str());

    //LCDError("path: %s, key: %s", path, key);

    if (statfs(path, &buf) != 0) {
        LCDError("statfs(%s) failed: %s", path, strerror(errno));
        free(path);
        free(key);
        return 0.0;
    }

    if (strcasecmp(key, "type") == 0) {
        value = buf.f_type;
    } else if (strcasecmp(key, "bsize") == 0) {
        value = buf.f_bsize;
    } else if (strcasecmp(key, "blocks") == 0) {
        value = buf.f_blocks;
    } else if (strcasecmp(key, "bfree") == 0) {
        value = buf.f_bfree;
    } else if (strcasecmp(key, "bavail") == 0) {
        value = buf.f_bavail;
    } else if (strcasecmp(key, "files") == 0) {
        value = buf.f_files;
    } else if (strcasecmp(key, "ffree") == 0) {
        value = buf.f_ffree;
#if 0
    } else if (strcasecmp(key, "fsid") == 0) {
        value = buf.f_fsid;
#endif
    } else if (strcasecmp(key, "namelen") == 0) {
        value = buf.f_namelen;
    } else {
        LCDError("statfs: unknown field '%s'", key);
        value = -1;
    }

    free(path);
    free(key);
    return value;
}

void PluginStatfs::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("statfs", objVal);
*/
}

