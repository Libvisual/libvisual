/* $Id$
 * $URL$
 *
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
#include <math.h>

#include "debug.h"
#include "qprintf.h"
#include "Hash.h"
#include "PluginNetStat.h"
#include "Evaluator.h"

using namespace LCD;

int PluginNetStat::ParseNetStat() {
    const char *DELIMITER = " :\t\n";
    int age;
    int row, col;

    age = hash_age(&NetStat, NULL);
    if(age >= 0 && age <= 1)
        return 0;
    if( Stream == NULL )
        Stream = fopen("/proc/net/tcp", "r");
    if( Stream == NULL ) {
        LCDError("fopen(/proc/net/tcp) failed: %s", strerror(errno));
        return -1;
    }

    rewind(Stream);
    row = 0;
    linecount = 0;

    while(!feof(Stream)) {
        char buffer[256];
        char line[16];
        char *beg, *end;
        unsigned int len;

        if(fgets(buffer, sizeof(buffer), Stream) == NULL)
            break;

        switch(++row) {
        case 1:
            if(first_time) {
                first_time = false;
                col = 0;
                beg = buffer;

                    while(beg) {
                    switch(col) {
                    case 2:
                        hash_set_column(&NetStat, col++, "local_port");
                        while(strlen(beg) > 0 && strchr(DELIMITER, *beg))
                            beg++;
                        break;
                    case 4:
                        hash_set_column(&NetStat, col++, "rem_port");
                        while(strlen(beg) > 0 && strchr(DELIMITER, *beg))
                            beg++;
                        break;
                    default:
                        char key[32];
                        while(strlen(beg) > 0 && strchr(DELIMITER, *beg))
                            beg++;
                        if((end = strpbrk(beg, DELIMITER)) != NULL)
                            *end = '\0';
                        qprintf(key, sizeof(key), "%s", beg);
                        hash_set_column(&NetStat, col++, key);
                        beg = end ? end + 1 : NULL;
                        break;
                        }
                }
            }
        default:
            beg = buffer;
            while(*beg && *beg == ' ')
                beg++;
            end = beg + 1;
            while(*end && *end != ':')
                end++;
            len = end - beg;
            if( len >= sizeof(line))
                len = sizeof(line) - 1;
            strncpy(line, beg, len);
            line[len] = '\0';

            hash_put_delta(&NetStat, line, buffer);
            linecount++;
        }
    }
    return 0;
}

std::string PluginNetStat::Netstat(std::string arg1, std::string arg2) {
    std::string line, column;

    if(ParseNetStat() < 0) {
        return "Error";
    }

    line = arg1;
    column = arg2;

    const char *val = hash_get(&NetStat, line.c_str(), column.c_str());

    if(val == NULL)
        return "";

    char buffer[17];
    buffer[0] = 0;
    if(column == "local_address" || column == "rem_address" ) {
        char *tmp = (char*)val;
        unsigned int s_addr1, s_addr2;
        sscanf(tmp, "%4X", &s_addr1);
        tmp+=4;
        sscanf(tmp, "%X", &s_addr2);
        unsigned long s_addr = (s_addr2 << 16) + s_addr1;
        unsigned int a1 = s_addr / pow(256, 3);
        unsigned int a2 = (s_addr % (int)pow(256, 3)) / pow(256, 2);
        unsigned int a3 = ((s_addr % (int)pow(256, 3)) % 
            (int)pow(256, 2)) / pow(256, 1);
        unsigned int a4 = (((s_addr % (int)pow(256, 3)) % 
            (int)pow(256, 2)) % (int)pow(256, 1)) / pow(256, 0);
        qprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", a1, a2, a3, a4);
        
    } else if(column == "local_port" || column == "rem_port" ) {
        unsigned int s_port;
        sscanf(val, "%X", &s_port);
        qprintf(buffer, sizeof(buffer), "%d", s_port);
    }
        
    return buffer[0] == 0 ? val : buffer;
}

int PluginNetStat::LineCount() {
    if(ParseNetStat() < 0) {
        return -1;
    }
    return linecount;
}

PluginNetStat::PluginNetStat() {
    first_time = true;
    linecount = 0;
    Stream = NULL;
    hash_create(&NetStat);
    hash_set_delimiter(&NetStat, " :\t\n");
}

PluginNetStat::~PluginNetStat() {
    if(Stream != NULL) {
        fclose(Stream);
        Stream = NULL;
    }
    hash_destroy(&NetStat);
}

void PluginNetStat::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("netstat", objVal);
*/
}

