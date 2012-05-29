/* $Id$
 * $URL$
 *
 * Copyright (C) 2007 Volker Gering <v.gering@t-online.de>
 * Copyright (C) 2004, 2005, 2006, 2007 The LCD4Linux Team <lcd4linux-devel@users.sourceforge.net>
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
#include <unistd.h>
#include <cstring>
#include <ctype.h>

#include "debug.h"
#include "qprintf.h"

#include <sys/types.h>              /* socket() */
#include <sys/socket.h>             /* socket() */
#include <sys/ioctl.h>              /* SIOCGIFNAME */
#include <net/if.h>             /* ifreq{} */
#include <errno.h>              /* errno */
#include <netinet/in.h>             /* inet_ntoa() */
#include <arpa/inet.h>              /* inet_ntoa() */

#include "PluginNetinfo.h"
#include "Evaluator.h"

using namespace LCD;

/* socket descriptor:
 * -2   initial state
 * -1   Error: message not printed
 * -3   Error: message printed, deactivate plugin
 */
int socknr = -2;

int PluginNetinfo::OpenNet() {

    if (socknr == -3)
        return -1;
    if (socknr == -2) {
        socknr = socket(PF_INET, SOCK_DGRAM, 0);
    }
    if (socknr == -1) {
        LCDError("%s: socket(PF_INET, SOCK_DGRAM, 0) failed: %s", "plugin_netinfo", strerror(errno));
        LCDError("  deactivate plugin netinfo");
        socknr = -3;
        return -1;
    }

    return 0;
}


double PluginNetinfo::Exists(std::string arg1) {
    char buf[10240];
    struct ifconf ifcnf;
    struct ifreq *ifreq;
    int len;
    double value = 0.0;             // netdev doesn't exists
    char devname[80];

    if (socknr < 0) {
        /* no open socket */
        return value;
    }

    ifcnf.ifc_len = sizeof(buf);
    ifcnf.ifc_buf = buf;
    if (ioctl(socknr, SIOCGIFCONF, &ifcnf) < 0) {
        /* Error getting list of devices */
        LCDError("%s: ioctl(IFCONF) for %s failed: %s", "plugin_netinfo", 
            arg1.c_str(), strerror(errno));
        return value;
    }
    if (0 == ifcnf.ifc_len) {
        /* no interfaces found */
        return value;
    }

    ifreq = (struct ifreq *) buf;
    len = sizeof(struct ifreq);
    strncpy(devname, arg1.c_str(), sizeof(devname));

    while (ifreq && *((char *) ifreq) && ((char *) ifreq) < buf + ifcnf.ifc_len) {
        if (*((char *) ifreq) && strncmp(ifreq->ifr_name, devname, sizeof(devname)) == 0) {
            /* found */
            value = 1.0;
            return value;
        }

        (*(char **) &ifreq) += len;
    }

    /* device doesn't exists */
    return value;
}


/* get MAC address (hardware address) of network device */
std::string PluginNetinfo::Hwaddr(std::string arg1) {
    static int errcount = 0;
    struct ifreq ifreq;
    unsigned char *hw;
    char value[18];

    if (socknr < 0) {
        /* no open socket */
        return "";
    }

    strncpy(ifreq.ifr_name, arg1.c_str(), sizeof(ifreq.ifr_name));
    if (ioctl(socknr, SIOCGIFHWADDR, &ifreq) < 0) {
        errcount++;
        if (1 == errcount % 1000) {
            LCDError("%s: ioctl(IFHWADDR %s) failed: %s", "plugin_netinfo", ifreq.ifr_name, strerror(errno));
            LCDError("  (skip next 1000 Errors)");
        }
        return "";
    }
    hw = (unsigned char *) ifreq.ifr_hwaddr.sa_data;
    qprintf(value, sizeof(value), "%02x:%02x:%02x:%02x:%02x:%02x",
            *hw, *(hw + 1), *(hw + 2), *(hw + 3), *(hw + 4), *(hw + 5));

    return value;
}


/* get ip address of network device */
std::string PluginNetinfo::Ipaddr(std::string arg1) {
    static int errcount = 0;
    struct ifreq ifreq;
    struct sockaddr_in *sin;
    char value[16];

    if (socknr < 0) {
        /* no open socket */
        return "";
    }

    strncpy(ifreq.ifr_name, arg1.c_str(), sizeof(ifreq.ifr_name));
    if (ioctl(socknr, SIOCGIFADDR, &ifreq) < 0) {
        errcount++;
        if (1 == errcount % 1000) {
            LCDError("%s: ioctl(IFADDR %s) failed: %s", "plugin_netinfo", ifreq.ifr_name, strerror(errno));
            LCDError("  (skip next 1000 Errors)");
        }
        return "";
    }
    sin = (struct sockaddr_in *) &ifreq.ifr_addr;
    qprintf(value, sizeof(value), "%s", inet_ntoa(sin->sin_addr));

    return value;
}


/* get ip netmask of network device */
std::string PluginNetinfo::Netmask(std::string arg1) {
    static int errcount = 0;
    struct ifreq ifreq;
    struct sockaddr_in *sin;
    char value[16];

    if (socknr < 0) {
        /* no open socket */
        return "";
    }

    strncpy(ifreq.ifr_name, arg1.c_str(), sizeof(ifreq.ifr_name));
    if (ioctl(socknr, SIOCGIFNETMASK, &ifreq) < 0) {
        errcount++;
        if (1 == errcount % 1000) {
            LCDError("%s: ioctl(IFNETMASK %s) failed: %s", "plugin_netinfo", 
                ifreq.ifr_name, strerror(errno));
            LCDError("  (skip next 1000 Errors)");
        }
        return "";
    }
    sin = (struct sockaddr_in *) &ifreq.ifr_netmask;
    qprintf(value, sizeof(value), "%s", inet_ntoa(sin->sin_addr));

    return "";
}


/* get ip broadcast address of network device */
std::string PluginNetinfo::Bcaddr(std::string arg1) {
    static int errcount = 0;
    struct ifreq ifreq;
    struct sockaddr_in *sin;
    char value[16];

    if (socknr < 0) {
        /* no open socket */
        return "";
    }

    strncpy(ifreq.ifr_name, arg1.c_str(), sizeof(ifreq.ifr_name));
    if (ioctl(socknr, SIOCGIFBRDADDR, &ifreq) < 0) {
        errcount++;
        if (1 == errcount % 1000) {
            LCDError("%s: ioctl(IFBRDADDR %s) failed: %s", "plugin_netinfo", ifreq.ifr_name, strerror(errno));
            LCDError("  (skip next 1000 Errors)");
        }
        return "";
    }
    sin = (struct sockaddr_in *) &ifreq.ifr_broadaddr;
    qprintf(value, sizeof(value), "%s", inet_ntoa(sin->sin_addr));

    return value; 
}


PluginNetinfo::PluginNetinfo() {
    socknr = -2;
    OpenNet();
}

PluginNetinfo::~PluginNetinfo() {
    close(socknr);
}

void PluginNetinfo::Connect(Evaluator *visitor) {
/*
    QScriptEngine *engine = visitor->GetEngine();
    QScriptValue val = engine->newObject();
    QScriptValue objVal = engine->newQObject(val, this);
    engine->globalObject().setProperty("netinfo", objVal);
*/
}

