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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <ctype.h>
#include <errno.h>

#include "debug.h"
#include "Hash.h"
#include "PluginDiskstats.h"

using namespace std;
using namespace LCD;

PluginDiskstats *mDiskstats;

int PluginDiskstats::ParseDiskstats()
{
    int age;

    /* reread every 10 msec only */
    age = hash_age(&DISKSTATS, NULL);
    if (age > 0 && age <= 10)
        return 0;

    if (stream == NULL)
        stream = fopen("/proc/diskstats", "r");
    if (stream == NULL) {
        LCDError("fopen(/proc/diskstats) failed: %s", strerror(errno));
        return -1;
    }

    rewind(stream);

    while (!feof(stream)) {
        char buffer[1024];
        char dev[1024];
        char *beg, *end;
        unsigned int num, len;

        if (fgets(buffer, sizeof(buffer), stream) == NULL)
            break;

        /* fetch device name (3rd column) as key */
        num = 0;
        beg = buffer;
        end = beg;
        while (strlen(beg) > 0 && *beg) {
            while (*beg == ' ' && strlen(beg) > 0)
                beg++;
            end = beg + 1;
            while (strlen(end) > 0 && *end && *end != ' ')
                end++;
            if (num++ == 2)
                break;
            beg = *end && strlen(end) > 0 ? end + 1 : NULL;
        }
        len = end ? (unsigned) (end - beg) : strlen(beg);

        if (len > sizeof(dev))
            len = sizeof(dev);
        strncpy(dev, beg, len);
        dev[len] = '\0';

        hash_put_delta(&DISKSTATS, dev, buffer);

    }
    return 0;
}


double PluginDiskstats::Diskstats(std::string arg1, std::string arg2, int arg3)
{
    char dev[256], key[256];
    int delay;
    double value = 0;

    strncpy(dev, arg1.c_str(), sizeof(dev));
    strncpy(key, arg2.c_str(), sizeof(key));
    delay = arg3;

    if (ParseDiskstats() < 0) {
        LCDError("Unable to parse disk stats.");
        return 0.0;
    }

    value = hash_get_delta(&DISKSTATS, dev, key, delay);

    return value;
}


class diskstats_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::string_arg_t());
        args->add(new lua::string_arg_t());
        args->add(new lua::int_arg_t());
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const std::string ns() { return "diskstats"; }
    static const std::string name() { return "Disstats"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string arg1 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string arg2 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        int arg3 = dynamic_cast<lua::int_arg_t&>(*in[0]).value();
        double value = mDiskstats->Diskstats(arg1, arg2, arg3);
        dynamic_cast<lua::string_arg_t&>(*out[0]).value() = value;
    }
};

PluginDiskstats::PluginDiskstats(lua *script)
{
    int i;
    const char *header[] = { "major", "minor", "name",
        "reads", "read_merges", "read_sectors", "read_ticks",
        "writes", "write_merges", "write_sectors", "write_ticks",
        "in_flight", "io_ticks", "time_in_queue", NULL
    };

    hash_create(&DISKSTATS);
    hash_set_delimiter(&DISKSTATS, " \n");
    for (i = 0; header[i]; i++) {
        hash_set_column(&DISKSTATS, i, header[i]);
    }

    stream = NULL;

    script->register_function<diskstats_t>();
}

PluginDiskstats::~PluginDiskstats()
{
    if (stream != NULL) {
        fclose(stream);
        stream = NULL;
    }
    hash_destroy(&DISKSTATS);
}


