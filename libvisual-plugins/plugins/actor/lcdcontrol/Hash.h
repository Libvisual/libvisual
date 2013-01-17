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

#ifndef _HASH_H_
#define _HASH_H_

/* struct timeval */
#include <sys/time.h>


typedef struct {
    int size;
    char *value;
    struct timeval timestamp;
} HASH_SLOT;


typedef struct {
    char *key;
    int val;
} HASH_COLUMN;

typedef struct {
    char *key;
    int index;
    int nSlot;
    HASH_SLOT *Slot;
} HASH_ITEM;


typedef struct {
    int sorted;
    struct timeval timestamp;
    int nItems;
    HASH_ITEM *Items;
    int nColumns;
    HASH_COLUMN *Columns;
    char *delimiter;
} HASH;



void hash_create(HASH * Hash);

int hash_age(HASH * Hash, const char *key);

void hash_set_column(HASH * Hash, const int number, const char *column);
void hash_set_delimiter(HASH * Hash, const char *delimiter);

char *hash_get(HASH * Hash, const char *key, const char *column);
double hash_get_delta(HASH * Hash, const char *key, const char *column, const int delay);
double hash_get_regex(HASH * Hash, const char *key, const char *column, const int delay);

void hash_put(HASH * Hash, const char *key, const char *value);
void hash_put_delta(HASH * Hash, const char *key, const char *value);

void hash_destroy(HASH * Hash);


#endif
