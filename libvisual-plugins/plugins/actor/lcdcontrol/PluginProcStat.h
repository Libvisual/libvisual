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

#ifndef __PLUGIN_PROC_STAT_H__
#define __PLUGIN_PROC_STAT_H__

#include <string>
#include "Hash.h"
#include "luascript.h"

namespace LCD {

class PluginProcStat {

  HASH Stat;
  FILE *stream;

  void HashPut1(const char *key1, const char *val);
  void HashPut2(const char *key1, const char *key2, const char *val);
  void HashPut3(const char *key1, const char *key2, const char *key3,
                const char *val);
  int ParseProcStat();

public:
  PluginProcStat(lua *script);
  ~PluginProcStat();

  std::string ProcStat(std::string arg1);
  double ProcStat(std::string arg1, double arg2);
  double Cpu(std::string arg1, int arg2);
  double Disk(std::string arg1, std::string arg2, double arg3);
};

}; // namespace LCD

#endif
