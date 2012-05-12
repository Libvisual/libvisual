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

#ifndef __PLUGIN_EXEC_H__
#define __PLUGIN_EXEC_H__

#include <list>
#include <string>

#include "Hash.h"

namespace LCD {

class Evaluator;

class PluginExecThread;

class PluginExec {
    HASH hash_;
    //std::list<PluginExecThread *> threads_;
    int DoExec(char *cmd, char *key, int delay);

    public:
    PluginExec() { hash_create(&hash_); }
    ~PluginExec() { Disconnect(); hash_destroy(&hash_); }
    void Connect(Evaluator *visitor);
    void Disconnect();

    std::string Exec(std::string arg1, int timeout);

};

#define MEM_SIZE 4096

/*
class PluginExecThread {
    bool running_;
    char *cmd_;
    char *ret_;
    char *key_;
    int delay_;

    protected:
    void run();

    public:
    PluginExecThread(char *cmd, char *key, int delay) { 
        storage_.setLocalData(new char[MEM_SIZE]);
        cmd_ = strdup(cmd);
        key_ = strdup(key);
        delay_ = delay;
        start();
    }
    ~PluginExecThread() {
        free(cmd_);
        free(key_);
    }
    void Stop() { running_ = false; }
    char *GetKey() { return key_; }
    char *GetRet() { 
        if(!storage_.hasLocalData()) 
            return NULL; 
        return storage_.localData();
    }
};
*/

};

#endif
