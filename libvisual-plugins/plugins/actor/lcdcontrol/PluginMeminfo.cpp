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
#include <stdio.h>
#include <cstring>
#include <ctype.h>
#include <errno.h>

#include "debug.h"

#include "Hash.h"
#include "PluginMeminfo.h"

using namespace LCD;

PluginMeminfo *plugmeminfo;

int PluginMeminfo::ParseMeminfo()
{
    int age;

    /* reread every 10 msec only */
    age = hash_age(&MemInfo, NULL);
    if (age > 0 && age <= 10)
        return 0;

    if (stream == NULL)
        stream = fopen("/proc/meminfo", "r");
    if (stream == NULL) {
        LCDError("fopen(/proc/meminfo) failed: %s", strerror(errno));
        return -1;
    }

    rewind(stream);

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), stream)) {
        char *c, *key, *val;
        c = strchr(buffer, ':');
        if (c == NULL)
            continue;
        key = buffer;
        val = c + 1;
        /* strip leading blanks from key */
        while (isspace(*key))
            *key++ = '\0';
        /* strip trailing blanks from key */
        do
            *c = '\0';
        while (isspace(*--c));
        /* strip leading blanks from value */
        while (isspace(*val))
            *val++ = '\0';
        /* strip trailing blanks from value */
        for (c = val; *c != '\0'; c++);
        while (isspace(*--c))
            *c = '\0';
        /* skip lines that do not end with " kB" */
        if (*c == 'B' && *(c - 1) == 'k' && *(c - 2) == ' ') {
            /* strip trailing " kB" from value */
            *(c - 2) = '\0';
            /* add entry to hash table */
            hash_put(&MemInfo, key, val);
        }
    }
    return 0;
}

std::string PluginMeminfo::Meminfo(std::string arg1)
{
    std::string key;
    const char * val;

    if (ParseMeminfo() < 0) {
        return "";
    }

    key = arg1;
    val = hash_get(&MemInfo, key.c_str(), NULL);
    if (val == NULL)
        val = "";

    return val;
}

class meminfo_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::string_arg_t());
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::string_arg_t());
        return args;
    }

    static const std::string ns() { return "meminfo"; }
    static const std::string name() { return "Meminfo"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string key = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string str = plugmeminfo->Meminfo(key);
        dynamic_cast<lua::string_arg_t&>(*out[0]).value() = str;
    }
};


PluginMeminfo::PluginMeminfo(lua *script)
{
    plugmeminfo = this;
    stream = NULL;
    hash_create(&MemInfo);
    script->register_function<meminfo_t>();
}


PluginMeminfo::~PluginMeminfo(void)
{
    if (stream != NULL) {
        fclose(stream);
        stream = NULL;
    }
    hash_destroy(&MemInfo);
}


