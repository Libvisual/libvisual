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
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <string>

#include "debug.h"
#include "PluginUptime.h"

using namespace LCD;

PluginUptime *mUptime;

static char *itoa(char *buffer, const size_t size, unsigned int value)
{
    char *p;

    /* sanity checks */
    if (buffer == NULL || size < 2)
        return (NULL);

    /* p points to last char */
    p = buffer + size - 1;

    /* set terminating zero */
    *p = '\0';

    do {
        *--p = value % 10 + '0';
        value = value / 10;
    } while (value != 0 && p > buffer);

    return p;
}


char *struptime(const unsigned int uptime, const char *format)
{
    static char string[256];
    const char *src;
    char *dst;
    int len, size;

    src = format;
    dst = string;
    len = 0;

    /* leave room for terminating zero  */
    size = sizeof(string) - 1;

    while (len < size) {

        if (*src == '%') {
            src++;

            if (strchr("sSmMhHd", *src) != NULL) {
                char buffer[12], *s;
                unsigned int value = 0;
                int leading_zero = 0;
                switch (*src++) {
                case 's':
                    value = uptime;
                    break;
                case 'S':
                    value = uptime % 60;
                    leading_zero = 1;
                    break;
                case 'm':
                    value = uptime / 60;
                    break;
                case 'M':
                    value = (uptime / 60) % 60;
                    leading_zero = 1;
                    break;
                case 'h':
                    value = uptime / 60 / 60;
                    break;
                case 'H':
                    value = (uptime / 60 / 60) % 24;
                    leading_zero = 1;
                    break;
                case 'd':
                    value = uptime / 60 / 60 / 24;
                    break;
                }

                if (leading_zero && value < 10) {
                    len++;
                    *dst++ = '0';
                }

                s = itoa(buffer, sizeof(buffer), value);
                while (len < size && *s != '\0') {
                    len++;
                    *dst++ = *s++;
                }

            } else if (*src == '%') {
                len++;
                *dst++ = '%';

            } else {
                len += 2;
                *dst++ = '%';
                *dst++ = *src++;
            }

        } else {
            len++;
            *dst++ = *src;
            if (*src++ == '\0')
                break;
        }
    }

    /* enforce terminating zero */
    if (len >= size && *(dst - 1) != '\0') {
        len++;
        *dst = '\0';
    }

    return string;
}


double PluginUptime::GetUptime()
{
    char buffer[36];
    int i;

    if (fd == -2)
        fd = open("/proc/uptime", O_RDONLY);
    if (fd < 0)
        return -1;

    lseek(fd, 0, SEEK_SET);

    i = read(fd, buffer, sizeof(buffer) - 1);
    if (i < 0)
        return -1;

    buffer[i - 1] = '\0';

    /* ignore the 2nd value from /proc/uptime */
    return strtod(buffer, NULL);
}


std::string PluginUptime::Uptime(std::string fmt) {
    int age;
    struct timeval now;

    gettimeofday(&now, NULL);

    age = (now.tv_sec - last_value.tv_sec) * 1000 + (now.tv_usec - last_value.tv_usec) / 1000;
    /* reread every 100 msec only */
    if (fd == -2 || age == 0 || age > 100) {
        uptime = GetUptime();
        if (uptime < 0.0) {
            LCDError("parse(/proc/uptime) failed!");
            return "Error";
        }

        memcpy(&last_value, &now, sizeof(timeval));
    }

    char *buffer = struptime(uptime, fmt.c_str());
    std::string str = buffer;
    return str;
}

class uptime1_t {
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

    static const std::string ns() { return "uptime"; }
    static const std::string name() { return "UptimeFmt"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string fmt = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string str = mUptime->Uptime(fmt);
        dynamic_cast<lua::string_arg_t&>(*out[0]).value() = str;
    }
};

double PluginUptime::Uptime() {
    int age;
    struct timeval now;

    gettimeofday(&now, NULL);

    age = (now.tv_sec - last_value.tv_sec) * 1000 + (now.tv_usec - last_value.tv_usec) / 1000;
    if (fd == -2 || age == 0 || age > 100) {
        uptime = GetUptime();
        if (uptime < 0.0) {
            LCDError("parse(/proc/uptime) failed!");
            return 0;
        }

        memcpy(&last_value, &now, sizeof(timeval));
    }

    return uptime;
}

class uptime2_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const std::string ns() { return "uptime"; }
    static const std::string name() { return "Uptime"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        double val = mUptime->Uptime();
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;
    }
};

PluginUptime::PluginUptime() {
    fd = -2;
    uptime = 0;
}

PluginUptime::PluginUptime(lua *script) {
    script->register_function<uptime1_t>();
    script->register_function<uptime2_t>(); // FIXME this may be broken
    uptime = 0;    
    fd = -2;
    mUptime = this;
}

PluginUptime::~PluginUptime() {
    if (fd > 0)
        close(fd);
    fd = -2;
}


