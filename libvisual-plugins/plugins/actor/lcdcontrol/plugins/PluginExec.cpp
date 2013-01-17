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

#include <sys/time.h>
#include <list>
#include <errno.h>
#include <string>

#include "PluginExec.h"
#include "Evaluator.h"
#include "debug.h"
#include "Hash.h"
#include "qprintf.h"

// CRC borrowed from LCD4Linux
#define CRCPOLY 0x8408

static unsigned short CRC(const char *s)
{
    int i;
    unsigned short crc;

    /* seed value */
    crc = 0xffff;

    while (*s != '\0') {
        crc ^= *s++;
        for (i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY : 0);
    }
    return crc;
}

using namespace LCD;

int PluginExec::DoExec(char *cmd, char *key, int delay) {
    
    int age = hash_age(&hash_, key);

    if(age < 0) {
        hash_put(&hash_, key, "");

        if(delay < 10)
            delay = 10;

        //threads_.push_back(new PluginExecThread(cmd, key, delay));
        return 0;
    }

    if(age < 10)
        return 0;

/*
    for(std::list<PluginExecThread *>::iterator it = threads_.begin();
        it != threads_.end(); it++) {
        if(strcmp(key, (*it)->GetKey()) == 0) {
            (*it)->GetMutex().lock();
            hash_put(&hash_, key, (*it)->GetRet());
            (*it)->GetMutex().unlock();
            return 0;
        }
    }
*/

    LCDError("internal error: could not find thread exec-%s", key);
    return -1;
}

std::string PluginExec::Exec(std::string arg1, int delay)
{
    const char *cmd;
    char key[5], *val;

    cmd = arg1.c_str();

    qprintf(key, sizeof(key), "%x", CRC(cmd));

    if(DoExec((char *)cmd, key, delay) < 0) {
        return "";
    }

    val = hash_get(&hash_, key, NULL);
    return val ? val : "";
}


/*
void PluginExecThread::run() {
    running_ = true;
    FILE *pipe;
    char buffer[MEM_SIZE];
    int len;

    putenv((char *)"PATH=/usr/local/bin:/usr/bin:/bin");

    while(running_) {
        pipe = popen(cmd_, "r");

        if(pipe == NULL) {
            LCDError("exec error: could not run pipe '%s': %s", cmd_, strerror(errno));
            len = 0;
        } else {
            len = fread(buffer, 1, MEM_SIZE - 1, pipe);
            if(len <= 0) {
                LCDError("exec error: could not read from pipe '%s': %s", cmd_, strerror(errno));
                len = 0;
             }
             pclose(pipe);
        }

        buffer[len] = '\0';

        while(len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }

        mutex_.lock();
        strncpy(ret_, buffer, MEM_SIZE);
        mutex_.unlock();

        usleep(delay_);
    }
}
*/

void PluginExec::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("exec", objVal);
*/
}

void PluginExec::Disconnect() {
/*
    for(std::list<PluginExecThread *>::iterator it = threads_.begin();
        it != threads_.end(); it++) {
        (*it)->Stop();
    }
*/
}

