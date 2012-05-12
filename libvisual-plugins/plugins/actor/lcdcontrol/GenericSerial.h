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

#ifndef __GENERIC_SERIAL__
#define __GENERIC_SERIAL__

#include <string>
#include <termios.h>

namespace LCD {

class GenericSerial {
    speed_t baud_;
    std::string port_;
    int fd_;
    bool connected_;
    std::string device_name_;
    public:
    GenericSerial(std::string name);
    virtual ~GenericSerial() {}
    int SerialOpen(std::string port, int baud, int flags = 0);
    void SerialClose();
    int SerialPoll(unsigned char *str, int len);
    int SerialReadData(unsigned char *str, int len);
    int SerialWrite(unsigned char *str, int len);
};

}; // End namespace
#endif
