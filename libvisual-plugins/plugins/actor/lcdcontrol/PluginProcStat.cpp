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
#include <string>
#include <ctype.h>
#include <errno.h>

#include "debug.h"
#include "qprintf.h"
#include "Hash.h"
#include "PluginProcStat.h"

using namespace LCD;

PluginProcStat *procstat;

void PluginProcStat::HashPut1(const char *key1, const char *val) {
    hash_put_delta(&Stat, key1, val);
}


void PluginProcStat::HashPut2(const char *key1, const char *key2, 
    const char *val) {
    char key[32];

    qprintf(key, sizeof(key), "%s.%s", key1, key2);
    HashPut1(key, val);
}


void PluginProcStat::HashPut3(const char *key1, const char *key2, 
    const char *key3, const char *val) {
    char key[32];

    qprintf(key, sizeof(key), "%s.%s.%s", key1, key2, key3);
    HashPut1(key, val);
}


int PluginProcStat::ParseProcStat(void) {
    int age;

    /* reread every 10 msec only */
    age = hash_age(&Stat, NULL);
    if (age > 0 && age <= 10)
        return 0;

    if (stream == NULL)
        stream = fopen("/proc/stat", "r");
    if (stream == NULL) {
        LCDError("fopen(/proc/stat) failed: %s", strerror(errno));
        return -1;
    }

    rewind(stream);

    while (!feof(stream)) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stream) == NULL)
            break;

        if (strncmp(buffer, "cpu", 3) == 0) {
            const char *key[] = { "user", "nice", "system", "idle", "iow", "irq", "sirq" };
            char delim[] = " \t\n";
            char *cpu, *beg, *end;
            int i;

            cpu = buffer;

            /* skip "cpu" or "cpu0" block */
            if ((end = strpbrk(buffer, delim)) != NULL)
                *end = '\0';
            beg = end ? end + 1 : NULL;

            for (i = 0; i < 7 && beg != NULL; i++) {
                while (strlen(beg) > 0 && strchr(delim, *beg))
                    beg++;
                if ((end = strpbrk(beg, delim)))
                    *end = '\0';
                HashPut2(cpu, key[i], beg);
                beg = end ? end + 1 : NULL;
            }
        }

        else if (strncmp(buffer, "page ", 5) == 0) {
            const char *key[] = { "in", "out" };
            char delim[] = " \t\n";
            char *beg, *end;
            int i;

            for (i = 0, beg = buffer + 5; i < 2 && beg != NULL; i++) {
                while (strlen(beg) > 0 && strchr(delim, *beg))
                    beg++;
                if ((end = strpbrk(beg, delim)))
                    *end = '\0';
                HashPut2("page", key[i], beg);
                beg = end ? end + 1 : NULL;
            }
        }

        else if (strncmp(buffer, "swap ", 5) == 0) {
            const char *key[] = { "in", "out" };
            char delim[] = " \t\n";
            char *beg, *end;
            int i;

            for (i = 0, beg = buffer + 5; i < 2 && beg != NULL; i++) {
                while (strlen(beg) > 0 && strchr(delim, *beg))
                    beg++;
                if ((end = strpbrk(beg, delim)))
                    *end = '\0';
                HashPut2("swap", key[i], beg);
                beg = end ? end + 1 : NULL;
            }
        }

        else if (strncmp(buffer, "intr ", 5) == 0) {
            char delim[] = " \t\n";
            char *beg, *end, num[4];
            int i;

            for (i = 0, beg = buffer + 5; i < 17 && beg != NULL; i++) {
                while (strlen(beg) > 0 && strchr(delim, *beg))
                    beg++;
                if ((end = strpbrk(beg, delim)))
                    *end = '\0';
                if (i == 0)
                    strcpy(num, "sum");
                else
                    qprintf(num, sizeof(num), "%d", i - 1);
                HashPut2("intr", num, beg);
                beg = end ? end + 1 : NULL;
            }
        }

        else if (strncmp(buffer, "disk_io:", 8) == 0) {
            const char *key[] = { "io", "rio", "rblk", "wio", "wblk" };
            char delim[] = " ():,\t\n";
            char *dev, *beg, *end, *p;
            int i;

            dev = buffer + 8;
            while (dev != NULL) {
                while (strlen(dev) > 0 && strchr(delim, *dev))
                    dev++;
                if ((end = strchr(dev, ')')))
                    *end = '\0';
                while ((p = strchr(dev, ',')) != NULL)
                    *p = ':';
                beg = end ? end + 1 : NULL;
                for (i = 0; i < 5 && beg != NULL; i++) {
                    while (strlen(beg) > 0 && strchr(delim, *beg))
                        beg++;
                    if ((end = strpbrk(beg, delim)))
                        *end = '\0';
                    HashPut3("disk_io", dev, key[i], beg);
                    beg = end ? end + 1 : NULL;
                }
                dev = beg;
            }
        }

        else {
            char delim[] = " \t\n";
            char *beg, *end;

            beg = buffer;
            if ((end = strpbrk(beg, delim)))
                *end = '\0';
            beg = end ? end + 1 : NULL;
            if ((end = strpbrk(beg, delim)))
                *end = '\0';
            while (strlen(beg) > 0 && strchr(delim, *beg))
                beg++;
            HashPut1(buffer, beg);
        }
    }
    return 0;
}


std::string PluginProcStat::ProcStat(std::string arg1) {
    char *string;

    string = hash_get(&Stat, arg1.c_str(), NULL);
    if (string == NULL)
        string = const_cast<char *>("");
    return string;
}

class procstat1_t {
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

    static const std::string ns() { return "procstat"; }
    static const std::string name() { return "Procstat"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string key = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string str = procstat->ProcStat(key);
        dynamic_cast<lua::string_arg_t&>(*out[0]).value() = str;
    }
};


double PluginProcStat::ProcStat(std::string arg1, double arg2)
{
    double number;

    number = hash_get_delta(&Stat, arg1.c_str(), 
        NULL, arg2);
    return number;
}

class procstat2_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
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

    static const std::string ns() { return "procstat"; }
    static const std::string name() { return "ProcstatDelta"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string arg1 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        double arg2 = dynamic_cast<lua::int_arg_t&>(*in[1]).value();
        double val = procstat->ProcStat(arg1, arg2);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;
    }
};


double PluginProcStat::Cpu(std::string arg1, int arg2) {
    //const char *key;
    std::string key = arg1;
    int delay = arg2;
    double cpu_user, cpu_nice, cpu_system, cpu_idle, cpu_total;
    double cpu_iow, cpu_irq, cpu_sirq;

    if (ParseProcStat() < 0) {
        LCDInfo("Unable to parse /proc/stat");
        return 0;
    }

    cpu_user = hash_get_delta(&Stat, "cpu.user", NULL, delay);
    cpu_nice = hash_get_delta(&Stat, "cpu.nice", NULL, delay);
    cpu_system = hash_get_delta(&Stat, "cpu.system", NULL, delay);
    cpu_idle = hash_get_delta(&Stat, "cpu.idle", NULL, delay);

    /* new fields for kernel 2.6 */
    /* even if we dont have this param (ie kernel 2.4) */
    /* the return is 0.0 and not change the results */
    cpu_iow = hash_get_delta(&Stat, "cpu.iow", NULL, delay);
    cpu_irq = hash_get_delta(&Stat, "cpu.irq", NULL, delay);
    cpu_sirq = hash_get_delta(&Stat, "cpu.sirq", NULL, delay);

    cpu_total = cpu_user + cpu_nice + cpu_system + cpu_idle + cpu_iow + cpu_irq + cpu_sirq;

    double value = 0.0;

    if (key == "user")
        value = cpu_user;
    else if (key == "nice")
        value = cpu_nice;
    else if (key =="system")
        value = cpu_system;
    else if (key == "idle")
        value = cpu_idle;
    else if (key == "iowait")
        value = cpu_iow;
    else if (key == "irq")
        value = cpu_irq;
    else if (key == "softirq")
        value = cpu_sirq;
    else if (key == "busy")
        value = cpu_total - cpu_idle;

    if (cpu_total > 0.0)
        value = 100 * value / cpu_total;
    else
        value = 0.0;

    return value;
}

class procstat3_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
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

    static const std::string ns() { return "procstat"; }
    static const std::string name() { return "Cpu"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string arg1 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        double arg2 = dynamic_cast<lua::int_arg_t&>(*in[1]).value();
        double val = procstat->Cpu(arg1, arg2);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;
    }
};


double PluginProcStat::Disk(std::string arg1, std::string arg2, double arg3) {
    const char *dev, *key;
    char buffer[32];
    int delay;
    double value;

    if (ParseProcStat() < 0) {
        LCDInfo("Unable to parse /proc/stat");
        return 0;
    }

    dev = arg1.c_str();
    key = arg2.c_str();
    delay = arg3;

    qprintf(buffer, sizeof(buffer), "disk_io\\.%s\\.%s", dev, key);
    value = hash_get_regex(&Stat, buffer, NULL, delay);

    return value;
}

class procstat4_t {
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

    static const std::string ns() { return "procstat"; }
    static const std::string name() { return "Disk"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string arg1 = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string arg2 = dynamic_cast<lua::string_arg_t&>(*in[1]).value();
        double arg3 = dynamic_cast<lua::int_arg_t&>(*in[2]).value();
        double val = procstat->Disk(arg1, arg2, arg3);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;
    }
};


PluginProcStat::PluginProcStat(lua *script) {
    procstat = this;
    script->register_function<procstat1_t>();
    script->register_function<procstat2_t>();
    script->register_function<procstat3_t>();
    script->register_function<procstat4_t>();
    stream = NULL;
    hash_create(&Stat);
}

PluginProcStat::~PluginProcStat() {
    if (stream != NULL) {
        fclose(stream);
        stream = NULL;
    }
    hash_destroy(&Stat);
}


