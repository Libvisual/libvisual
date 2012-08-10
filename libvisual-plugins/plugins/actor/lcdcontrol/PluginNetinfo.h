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

#ifndef __PLUGIN_NETINFO_H__
#define __PLUGIN_NETINFO_H__

#include <string>

#include "luascript.h"

namespace LCD {

class PluginNetinfo {

    int socknr;
    int OpenNet();

    public:
    PluginNetinfo(lua *script);
    ~PluginNetinfo();

    double Exists(std::string arg1);
    std::string Hwaddr(std::string arg1);
    std::string Ipaddr(std::string arg1);
    std::string Netmask(std::string arg1);
    std::string Bcaddr(std::string arg1);
};

}; // End namespace

#endif
