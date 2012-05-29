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

#include <stdlib.h>
#include <string.h>

#include "RGBA.h"

int LCD::color2RGBA(const char *color, RGBA * C)
{
    char *e;
    unsigned long l;

    if (color == NULL || *color == '\0') {
        return -1;
    }

    l = strtoul(color, &e, 16);
    if (e != NULL && *e != '\0') {
        return -1;
    }

    if (strlen(color) == 8) {
        /* RGBA */
        C->A = (l >> 24) & 0xff;
        C->R = (l >> 16) & 0xff;
        C->G = (l >> 8) & 0xff;
        C->B = (l >> 0) & 0xff;
    } else {
        /* RGB */
        C->A = 0xff;
        C->R = (l >> 16) & 0xff;
        C->G = (l >> 8) & 0xff;
        C->B = l & 0xff;
    }
    return 0;
}

