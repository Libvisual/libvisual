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

/* 
 * exported functions:
 *
 * message (level, format, ...)
 *   passes the arguments to vsprintf() and
 *   writes the resulting string either to stdout
 *   or syslog.
 *   this function should not be called directly,
 *   but the macros info(), debug() and LCDError()
 *
 */

//#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "debug.h"

int running_foreground = 0;
int running_background = 0;

int verbose_level = 0;

void message(const int level, const char *format, ...)
{
    va_list ap;
    char buffer[256];
    static int log_open = 0;

    if (level > verbose_level)
	return;

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if (!running_background) {

#ifdef WITH_CURSES
	extern int curses_Error(char *);
	if (!curses_error(buffer))
#endif
	    fprintf(level ? stdout : stderr, "%s\n", buffer);
    }

    if (running_foreground)
	return;

    if (!log_open) {
	openlog("LCDControl", LOG_PID, LOG_USER);
	log_open = 1;
    }

    switch (level) {
    case 0:
	syslog(LOG_ERR, "%s", buffer);
	break;
    case 1:
	syslog(LOG_INFO, "%s", buffer);
	break;
    default:
	syslog(LOG_DEBUG, "%s", buffer);
    }
}
