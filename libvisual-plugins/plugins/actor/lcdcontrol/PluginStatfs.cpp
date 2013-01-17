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

PluginStatfs *mStatfs;

double PluginStatfs::Statfs(std::string path, std::string key)
{
    struct statfs buf;
    double value;

    //LCDError("path: %s, key: %s", path, key);

    if (statfs(path.c_str(), &buf) != 0) {
        LCDError("statfs(%s) failed: %s", path.c_str(), strerror(errno));
        return 0.0;
    }

    if (strcasecmp(key.c_str(), "type") == 0) {
        value = buf.f_type;
    } else if (strcasecmp(key.c_str(), "bsize") == 0) {
        value = buf.f_bsize;
    } else if (strcasecmp(key.c_str(), "blocks") == 0) {
        value = buf.f_blocks;
    } else if (strcasecmp(key.c_str(), "bfree") == 0) {
        value = buf.f_bfree;
    } else if (strcasecmp(key.c_str(), "bavail") == 0) {
        value = buf.f_bavail;
    } else if (strcasecmp(key.c_str(), "files") == 0) {
        value = buf.f_files;
    } else if (strcasecmp(key.c_str(), "ffree") == 0) {
        value = buf.f_ffree;
#if 0
    } else if (strcasecmp(key, "fsid") == 0) {
        value = buf.f_fsid;
#endif
    } else if (strcasecmp(key.c_str(), "namelen") == 0) {
        value = buf.f_namelen;
    } else {
        LCDError("statfs: unknown field '%s'", key.c_str());
        value = -1;
    }

    return value;
}


class statfs_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::string_arg_t());
        args->add(new lua::string_arg_t());
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const std::string ns() { return "statfs"; }
    static const std::string name() { return "Statfs"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string arg1 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string arg2 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        double stat = mStatfs->Statfs(arg1, arg2);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = stat;
    }
};

PluginStatfs::PluginStatfs() {

}

PluginStatfs::PluginStatfs(lua *script) {
    script->register_function<statfs_t>();
    mStatfs = this;
}

PluginStatfs::~PluginStatfs() { 
    if(this == mStatfs)
        mStatfs = NULL; 
}
