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
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>

#include "GenericSerial.h"
#include "debug.h"

using namespace LCD;

GenericSerial::GenericSerial(std::string device_name) {
    device_name_ = device_name;
    connected_ = false;
    port_ = "";
    baud_ = 0;
}

int GenericSerial::SerialOpen(std::string port, int baud, int flags) {
    struct termios portset;

    port_ = port;

    switch (baud) {
    case 1200:
        baud_ = B1200;
        break;
    case 2400:
        baud_ = B2400;
        break;
    case 4800:
        baud_ = B4800;
        break;
    case 9600:
        baud_ = B9600;
        break;
    case 19200:
        baud_ = B19200;
        break;
    case 38400:
        baud_ = B38400;
        break;
    case 57600:
        baud_ = B57600;
        break;
    case 115200:
        baud_ = B115200;
        break;
#ifdef B230400
    case 230400:
        baud_ = B230400;
        break;
#endif
    default:
        LCDError("%s: Serial: unsupported speed '%d'", device_name_.c_str(), baud);
        return -1;
    }    
    LCDError("Using port '%s' at %d baud", port.c_str(), baud);
    fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if( fd_ == -1 ) {
        LCDError("%s: Open(%s) failed: %s", device_name_.c_str(),
            port.c_str(), strerror(errno));
        return -1;
    }

    if( tcgetattr(fd_, &portset) == -1) {
        LCDError("%s: tcgetattr(%s) failed: %s", device_name_.c_str(), 
            port.c_str(), strerror(errno));
        return -1;
    }

    //cfmakeraw(&portset);
    //input modes
    portset.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|ICRNL
                  |IXON|IXOFF);
    portset.c_iflag |= IGNPAR;

    //output modes
    portset.c_oflag &= ~(OPOST|ONLCR|OCRNL|ONOCR|ONLRET|OFILL
                  |OFDEL|NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY);

    //control modes
    portset.c_cflag &= ~(CSIZE|PARENB|PARODD|HUPCL|CRTSCTS);
    portset.c_cflag |= CREAD|CS8|CSTOPB|CLOCAL;
    portset.c_cflag |= flags;

    //local modes
    portset.c_lflag &= ~(ISIG|ICANON|IEXTEN|ECHO);
    portset.c_lflag |= NOFLSH;

    cfsetispeed(&portset, baud_);
    cfsetospeed(&portset, baud_);
    if(tcsetattr(fd_, TCSANOW, &portset) == -1 ) {
        LCDError("%s: tcsetattr(%s) failed: %s", device_name_.c_str(), 
            port.c_str(), strerror(errno));
        return -1;
    }

    connected_ = true;
    return fd_;
}

void GenericSerial::SerialClose() {
    LCDInfo("%s: closing port %s", device_name_.c_str(), port_.c_str());
    close(fd_);
    connected_ = false;
}

int GenericSerial::SerialPoll(unsigned char *string, int len) {
    if(!connected_)
        return -1;
    char buff[len + 1];
    int ret = read(fd_, buff, len);
    if( ret < 0 && errno != EAGAIN ) {
        LCDError("%s: read(%s) failed: %s", device_name_.c_str(), port_.c_str(), strerror(errno));
    }
    for(int i = 0; i < ret; i++ ) {
        string[i] = buff[i];
    }
    return ret;
}

int GenericSerial::SerialReadData(unsigned char *string, int len) {
    int run, ret;
    for( run = 0; run < 16; run++) {
        ret = SerialPoll(string, len);
        if(ret >= 0 || errno != EAGAIN )
            break;
        //LCDInfo("%s: read(%s): EAGAIN", device_name_.c_str(), port_.c_str());
        usleep(1000);
    }
/*
    if ( ret > 0 && ret != len && len > 0 ) {
        LCDInfo("%s partial read(%s): len=%d ret=%d", device_name_.c_str(), 
            port_.c_str(), len, ret);
    }
*/
    return ret;
}

int GenericSerial::SerialWrite(unsigned char *string, int len) {
    static int error = 0;
    int run, ret;

    if(!connected_)
        return -1;

    for(run = 0; run < 10; run++) {
        ret = write(fd_, string, len);
        if( ret >= 0 || errno != EAGAIN)
            break;
        if( run > 0 )
            LCDInfo("%s: write(%s): EAGAIN #%d",device_name_.c_str(),port_.c_str(),run);
        usleep(1000);
    }

    if( ret < 0 ) {
        if(++error > 10) {
            LCDError("%s: too many errors", device_name_.c_str());
        }
    } else if ( ret != len) {
        LCDInfo("%s: partial write(%s): len=%d ret=%d", device_name_.c_str(), 
        port_.c_str(), len, ret);
    } else {
        error = 0;
    }
    return ret;
}
