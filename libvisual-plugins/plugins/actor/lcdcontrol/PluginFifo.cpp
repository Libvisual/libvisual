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
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#include "debug.h"
#include "PluginFifo.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

using namespace LCD;

PluginFifo *mFifo;

void PluginFifo::ConfigureFifo(void)
{
/*
    memset(fifopath, 0, 1024);
    std::string path = "lcdcontrol.fifo";
    Json::Value *s = visitor_->CFG_Fetch_Raw(visitor_->CFG_Get_Root(), path);
    if (!s) {
        LCDInfo("[FIFO] empty '%s.fifopath' entry from %s, assuming '/tmp/lcdcontrol.fifo'", 
            path.c_str(), visitor_->CFG_Source().c_str());
        strcpy(fifopath, "/etc/lcdcontrol.fifo");
        return;
    } else {
        strcpy(fifopath, s->asCString());
        LCDInfo("[FIFO] read '%s.fifopath', value is '%s'", 
            path.c_str(), fifopath);
    }
    delete s;
*/
}


void PluginFifo::RemoveFifo(void)
{
    LCDDebug("Removing FIFO \"%s\"\n", fd.path);
    if (unlink(fd.path) < 0) {
        LCDError("Could not remove FIFO \"%s\": %s\n", fd.path, strerror(errno));
        return;
    }
    fd.created = 0;
}


void PluginFifo::CloseFifo(void)
{
    struct stat st;
    if (fd.input >= 0) {
        close(fd.input);
        fd.input = -1;
    }
    if (fd.created && (stat(fd.path, &st) == 0))
        RemoveFifo();
}

int PluginFifo::MakeFifo(void)
{
    if (mkfifo(fd.path, 0666) < 0) {
        LCDError("Couldn't create FIFO \"%s\": %s\n", fd.path, strerror(errno));
        return -1;
    }
    fd.created = 1;
    return 0;
}


int PluginFifo::CheckFifo(void)
{
    struct stat st;
    if (stat(fd.path, &st) < 0) {
        if (errno == ENOENT) {

            /* Path doesn't exist */
            return MakeFifo();
        }
        LCDError("Failed to stat FIFO \"%s\": %s\n", fd.path, strerror(errno));
        return -1;
    }
    if (!S_ISFIFO(st.st_mode)) {
        LCDError("\"%s\" already exists, but is not a FIFO\n", fd.path);
        return -1;
    }
    return 0;
}


int PluginFifo::OpenFifo(void)
{
    if (CheckFifo() < 0)
        return -1;
    fd.input = open(fd.path, O_RDONLY | O_NONBLOCK);
    if (fd.input < 0) {
        LCDError("Could not open FIFO \"%s\" for reading: %s\n", fd.path, strerror(errno));
        CloseFifo();
        return -1;
    }
    return 0;
}


void PluginFifo::StartFifo(void)
{
    static int started = 0;

    if (started)
        return;

    started = 1;

    ConfigureFifo();
    fd.path = fifopath;
    fd.input = -1;
    fd.created = 0;
    OpenFifo();

    /* ignore broken pipe */
    signal(SIGPIPE, SIG_IGN);

    memset(msg, 0, FIFO_BUFFER_SIZE);

}


std::string PluginFifo::Fiforead()
{
    char buf[FIFO_BUFFER_SIZE];
    unsigned int i;
    int bytes = 1;

    StartFifo();

    memset(buf, 0, FIFO_BUFFER_SIZE);
    strcat(buf, "ERROR");

    if (CheckFifo() == 0) {
        memset(buf, 0, FIFO_BUFFER_SIZE);

        while (bytes > 0 && errno != EINTR) {
            bytes = read(fd.input, buf, FIFO_BUFFER_SIZE);
        }

        if (bytes < 0 || (errno > 0 && errno != EAGAIN)) {
            LCDError("[FIFO] Error %i: %s", errno, strerror(errno));
        } else {
            if (strlen(buf) > 0) {
                strcpy(msg, buf);
            }
            for (i = 0; i < strlen(buf); i++) {
                if (msg[i] < 0x20)
                    msg[i] = ' ';
            }
        }
    }
    return msg;
}

class fifo_t {
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::string_arg_t());
        return args;
    }

    static const std::string ns() { return "fifo"; }
    static const std::string name() { return "Read"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        std::string fmt = dynamic_cast<lua::string_arg_t&>(*in[0]).value();
        std::string str = mFifo->Fiforead();
        dynamic_cast<lua::string_arg_t&>(*out[0]).value() = str;
    }
};

PluginFifo::PluginFifo(lua *script) {
    fd.path = NULL;
    fd.input = -1;
    fd.created = 0;
    
}

PluginFifo::~PluginFifo() {
    CloseFifo();
}

