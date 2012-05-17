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
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include "PluginLoadavg.h"
#include "debug.h"

using namespace LCD;

int fd;

int GetLoadavg(double loadavg[], int nelem)
{
    char buf[65], *p;
    ssize_t nread;
    int i;

    if (fd == -2)
        fd = open("/proc/loadavg", O_RDONLY);
    if (fd < 0)
        return -1;

    lseek(fd, 0, SEEK_SET);
    nread = read(fd, buf, sizeof buf - 1);

    if (nread < 0)
        return -1;
    buf[nread - 1] = '\0';

    if (nelem > 3)
        nelem = 3;
    p = buf;
    for (i = 0; i < nelem; ++i) {
        char *endp;
        loadavg[i] = strtod(p, &endp);
        if (endp == NULL || endp == p)
            /* This should not happen.  The format of /proc/loadavg
               must have changed.  Don't return with what we have,
               signal an Error.  */
            return -1;
        p = endp;
    }

    return i;
}


double Loadavg(int arg1)
{
    static int nelem = -1;
    int index, age;
    static double loadavg[3];
    static struct timeval last_value;
    struct timeval now;

    gettimeofday(&now, NULL);

    age = (now.tv_sec - last_value.tv_sec) * 1000 + (now.tv_usec - last_value.tv_usec) / 1000;
    /* reread every 10 msec only */
    if (nelem == -1 || age == 0 || age > 10) {

        nelem = GetLoadavg(loadavg, 3);
        if (nelem < 0) {
            LCDError("getloadavg() failed!");
            return 0.0;
        }
        last_value = now;
    }

    index = arg1;
    if (index < 1 || index > nelem) {
        LCDError("loadavg(%d): index out of range!", index);
        return 0.0;
    }

    return loadavg[index - 1];
}

class loadavg_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const std::string ns() { return "loadavg"; }
    static const std::string name() { return "Loadavg"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        int key = dynamic_cast<lua::int_arg_t&>(*in[0]).value();
        double val = Loadavg(key);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;
    }
};

PluginLoadavg::PluginLoadavg(lua *script) {
    script->register_function<loadavg_t>();
    fd = -2;
}


PluginLoadavg::~PluginLoadavg() {
    if (fd > 0)
        close(fd);
    fd = -2;
}
