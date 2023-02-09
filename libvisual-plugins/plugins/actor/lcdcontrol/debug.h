/* $Id$
 * $URL$
 *
 * Copyright (C) 1999, 2000 Michael Reinelt <michael@reinelt.co.at>
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

#ifndef _DEBUG_H_
#define _DEBUG_H_

extern int running_foreground;
extern int running_background;
extern int verbose_level;

#ifdef __cplusplus
extern "C" {
#endif

void message(const int level, const char *format, ...)
    __attribute__((format(__printf__, 2, 3)));

#ifdef __cplusplus
}
#endif

#define LCDDebug(args...) message(2, __FILE__ ": " args)
#define LCDInfo(args...) message(1, args)
#define LCDError(args...) message(0, args)

#endif
