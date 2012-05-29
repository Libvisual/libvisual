/* $Id: hash.c 840 2007-09-09 12:17:42Z michael $
 * $URL: https://ssl.bulix.org/svn/lcd4linux/trunk/hash.c $
 *
 * hashes (associative arrays)
 *
 * Copyright (C) 2003 Michael Reinelt <michael@reinelt.co.at>
 * Copyright (C) 2004 The LCD4Linux Team <lcd4linux-devel@users.sourceforge.net>
 *
 * This file is part of LCD4Linux.
 *
 * LCD4Linux is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * LCD4Linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* 
 * exported functions:
 *
 * void hash_create (HASH *Hash);
 *   initializes hash
 *
 * int hash_age (HASH *Hash, char *key, char **value);
 *   return time of last hash_put
 *
 * void hash_put (HASH *Hash, char *key, char *val);
 *   set an entry in the hash
 *
 * void hash_put_delta (HASH *Hash, char *key, char *val);
 *   set a delta entry in the hash
 *
 * char *hash_get (HASH *Hash, char *key);
 *   fetch an entry from the hash
 *
 * double hash_get_delta (HASH *Hash, char *key, int delay);
 *   fetch a delta antry from the hash
 *
 * double hash_get_regex (HASH *Hash, char *key, int delay);
 *   fetch one or more entries from the hash
 *
 * void hash_destroy (HASH *Hash);
 *   releases hash
 *
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "debug.h"
#include "Hash.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

/* number of slots for delta processing */
#define DELTA_SLOTS 64

/* string buffer chunk size */
#define CHUNK_SIZE 16


/* initialize a new hash table */
void hash_create(HASH * Hash)
{
    Hash->sorted = 0;

    Hash->timestamp.tv_sec = 0;
    Hash->timestamp.tv_usec = 0;

    Hash->nItems = 0;
    Hash->Items = NULL;

    Hash->nColumns = 0;
    Hash->Columns = NULL;

    Hash->delimiter = strdup(" \t\n");
}


/* bsearch compare function for hash items */
static int hash_lookup_item(const void *a, const void *b)
{
    char *key = (char *) a;
    HASH_ITEM *item = (HASH_ITEM *) b;

    return strcasecmp(key, item->key);
}


/* qsort compare function for hash items */
static int hash_sort_item(const void *a, const void *b)
{
    HASH_ITEM *ha = (HASH_ITEM *) a;
    HASH_ITEM *hb = (HASH_ITEM *) b;

    return strcasecmp(ha->key, hb->key);
}


/* bsearch compare function for hash headers */
static int hash_lookup_column(const void *a, const void *b)
{
    char *key = (char *) a;
    HASH_COLUMN *column = (HASH_COLUMN *) b;

    return strcasecmp(key, column->key);
}


/* qsort compare function for hash headers */
static int hash_sort_column(const void *a, const void *b)
{
    HASH_COLUMN *ha = (HASH_COLUMN *) a;
    HASH_COLUMN *hb = (HASH_COLUMN *) b;

    return strcasecmp(ha->key, hb->key);
}


/* split a value into columns and  */
/* return the nth column in a string */
/* WARNING: does return a pointer to a static string!! */
static char *split(const char *val, const int column, const char *delimiter)
{
    static char buffer[256];
    int num;
    size_t len;
    const char *beg, *end;

    if (column < 0)
	return (char *) val;
    if (val == NULL)
	return NULL;

    num = 0;
    len = 0;
    beg = val;
    end = beg;
    while (beg && *beg) {
	while (strchr(delimiter, *beg))
	    beg++;
	end = strpbrk(beg, delimiter);
	if (num++ == column)
	    break;
	beg = end ? end + 1 : NULL;
    }
    if (beg != NULL) {
	len = end ? (size_t) (end - beg) : strlen(beg);
	if (len >= sizeof(buffer))
	    len = sizeof(buffer) - 1;
	strncpy(buffer, beg, len);
    }

    buffer[len] = '\0';
    return buffer;
}


/* search an entry in the hash table: */
/* If the table is flagged "sorted", the entry is looked */
/* up using the bsearch function. If the table is  */
/* unsorted, it will be searched in a linear way */
static HASH_ITEM *hash_lookup(HASH * Hash, const char *key, const int do_sort)
{
    HASH_ITEM *Item = NULL;

    /* maybe sort the array */
    if (do_sort && !Hash->sorted) {
	qsort(Hash->Items, Hash->nItems, sizeof(HASH_ITEM), hash_sort_item);
	Hash->sorted = 1;
    }

    /* no key was passed */
    if (key == NULL)
	return NULL;

    /* lookup using bsearch */
    if (Hash->sorted) {
	Item = (HASH_ITEM *)bsearch(key, Hash->Items, Hash->nItems, sizeof(HASH_ITEM), hash_lookup_item);
    }

    /* linear search */
    if (Item == NULL) {
	int i;
	for (i = 0; i < Hash->nItems; i++) {
	    if (strcmp(key, Hash->Items[i].key) == 0) {
		Item = &(Hash->Items[i]);
		break;
	    }
	}
    }

    return Item;

}


/* return the age in milliseconds of an entry from the hash table */
/* or from the hash table itself if key is NULL */
/* returns -1 if entry does not exist */
int hash_age(HASH * Hash, const char *key)
{
    HASH_ITEM *Item;
    struct timeval now, *timestamp;

    if (key == NULL) {
	timestamp = &(Hash->timestamp);
    } else {
	Item = hash_lookup(Hash, key, 1);
	if (Item == NULL)
	    return -1;
	timestamp = &(Item->Slot[Item->index].timestamp);
    }

    gettimeofday(&now, NULL);

    return (now.tv_sec - timestamp->tv_sec) * 1000 + (now.tv_usec - timestamp->tv_usec) / 1000;
}


/* add an entry to the column header table */
void hash_set_column(HASH * Hash, const int number, const char *column)
{
    if (Hash == NULL)
	return;

    Hash->nColumns++;
    Hash->Columns = (HASH_COLUMN *)realloc(Hash->Columns, Hash->nColumns * sizeof(HASH_COLUMN));
    Hash->Columns[Hash->nColumns - 1].key = strdup(column);
    Hash->Columns[Hash->nColumns - 1].val = number;

    qsort(Hash->Columns, Hash->nColumns, sizeof(HASH_COLUMN), hash_sort_column);

}


/* fetch a column number by column header */
static int hash_get_column(HASH * Hash, const char *key)
{
    HASH_COLUMN *Column;

    if (key == NULL || *key == '\0')
	return -1;

    Column = (HASH_COLUMN *)bsearch(key, Hash->Columns, Hash->nColumns, sizeof(HASH_COLUMN), hash_lookup_column);
    if (Column == NULL)
	return -1;

    return Column->val;
}


/* set column delimiters */
void hash_set_delimiter(HASH * Hash, const char *delimiter)
{
    if (Hash->delimiter != NULL)
	free(Hash->delimiter);
    Hash->delimiter = strdup(delimiter);
}


/* get a string from the hash table */
char *hash_get(HASH * Hash, const char *key, const char *column)
{
    HASH_ITEM *Item;
    int c;

    Item = hash_lookup(Hash, key, 1);
    if (Item == NULL)
	return NULL;

    c = hash_get_column(Hash, column);
    return split(Item->Slot[Item->index].value, c, Hash->delimiter);
}


/* get a delta value from the delta table */
double hash_get_delta(HASH * Hash, const char *key, const char *column, const int delay)
{
    HASH_ITEM *Item;
    HASH_SLOT *Slot1, *Slot2;
    int i, c;
    double v1, v2;
    double dv, dt;
    struct timeval now, end;

    /* lookup item */
    Item = hash_lookup(Hash, key, 1);
    if (Item == NULL)
	return 0.0;

    /* this is the "current" Slot */
    Slot1 = &(Item->Slot[Item->index]);

    /* fetch column number */
    c = hash_get_column(Hash, column);

    /* if delay is zero, return absolute value */
    if (delay == 0)
	return atof(split(Slot1->value, c, Hash->delimiter));

    /* prepare timing values */
    now = Slot1->timestamp;
    end.tv_sec = now.tv_sec;
    end.tv_usec = now.tv_usec - 1000 * delay;
    while (end.tv_usec < 0) {
	end.tv_sec--;
	end.tv_usec += 1000000;
    }

    /* search delta slot */
    Slot2 = &(Item->Slot[Item->index]);
    for (i = 1; i < Item->nSlot; i++) {
	Slot2 = &(Item->Slot[(Item->index + i) % Item->nSlot]);
	if (Slot2->timestamp.tv_sec == 0)
	    break;
	if (timercmp(&(Slot2->timestamp), &end, <))
	    break;
    }

    /* empty slot => try the one before */
    if (Slot2->timestamp.tv_sec == 0) {
	i--;
	Slot2 = &(Item->Slot[(Item->index + i) % Item->nSlot]);
    }

    /* not enough slots available... */
    if (i == 0)
	return 0.0;

    /* delta value, delta time */
    v1 = atof(split(Slot1->value, c, Hash->delimiter));
    v2 = atof(split(Slot2->value, c, Hash->delimiter));
    dv = v1 - v2;
    dt = (Slot1->timestamp.tv_sec - Slot2->timestamp.tv_sec)
	+ (Slot1->timestamp.tv_usec - Slot2->timestamp.tv_usec) / 1000000.0;

    if (dt > 0.0 && dv >= 0.0)
	return dv / dt;
    return 0.0;
}


/* get a delta value from the delta table */
/* key may contain regular expressions, and the sum  */
/* of all matching entries is returned. */
double hash_get_regex(HASH * Hash, const char *key, const char *column, const int delay)
{
    double sum;
    regex_t preg;
    int i, err;

    err = regcomp(&preg, key, REG_ICASE | REG_NOSUB);
    if (err != 0) {
	char buffer[32];
	regerror(err, &preg, buffer, sizeof(buffer));
	LCDError("error in regular expression: %s", buffer);
	regfree(&preg);
	return 0.0;
    }

    /* force the table to be sorted by requesting anything */
    hash_lookup(Hash, NULL, 1);

    sum = 0.0;
    for (i = 0; i < Hash->nItems; i++) {
	if (regexec(&preg, Hash->Items[i].key, 0, NULL, 0) == 0) {
	    sum += hash_get_delta(Hash, Hash->Items[i].key, column, delay);
	}
    }
    regfree(&preg);
    return sum;
}


/* insert a key/val pair into the hash table */
/* If the entry does already exist, it will be overwritten, */
/* and the table stays sorted (if it has been before). */
/* Otherwise, the entry is appended at the end, and  */
/* the table will be flagged 'unsorted' afterwards */

static HASH_ITEM *hash_set(HASH * Hash, const char *key, const char *value, const int delta)
{
    HASH_ITEM *Item;
    HASH_SLOT *Slot;
    int size;

    Item = hash_lookup(Hash, key, 0);

    if (Item == NULL) {

	/* add entry */
	Hash->sorted = 0;
	Hash->nItems++;
	Hash->Items = (HASH_ITEM *)realloc(Hash->Items, Hash->nItems * sizeof(HASH_ITEM));

	Item = &(Hash->Items[Hash->nItems - 1]);
	Item->key = strdup(key);
	Item->index = 0;
	Item->nSlot = delta;
	Item->Slot = (HASH_SLOT *)malloc(Item->nSlot * sizeof(HASH_SLOT));
	memset(Item->Slot, 0, Item->nSlot * sizeof(HASH_SLOT));

    } else {

	/* maybe enlarge delta table */
	if (Item->nSlot < delta) {
	    Item->nSlot = delta;
	    Item->Slot = (HASH_SLOT *)realloc(Item->Slot, Item->nSlot * sizeof(HASH_SLOT));
	}

    }

    if (Item->nSlot > 1) {
	/* move the pointer to the next free slot, wrap around if necessary */
	if (--Item->index < 0)
	    Item->index = Item->nSlot - 1;
    }

    /* create entry */
    Slot = &(Item->Slot[Item->index]);
    size = strlen(value) + 1;

    /* maybe enlarge value buffer */
    if (size > Slot->size) {
	/* buffer is either empty or too small */
	/* allocate memory in multiples of CHUNK_SIZE */
	Slot->size = CHUNK_SIZE * (size / CHUNK_SIZE + 1);
	Slot->value = (char *)realloc(Slot->value, Slot->size);
    }

    /* set value */
    strcpy(Slot->value, value);

    /* set timestamps */
    gettimeofday(&(Hash->timestamp), NULL);
    Slot->timestamp = Hash->timestamp;

    return Item;
}


/* insert a string into the hash table */
/* without delta processing */
void hash_put(HASH * Hash, const char *key, const char *value)
{
    hash_set(Hash, key, value, 1);
}


/* insert a string into the hash table */
/* with delta processing */
void hash_put_delta(HASH * Hash, const char *key, const char *value)
{
    hash_set(Hash, key, value, DELTA_SLOTS);
}


void hash_destroy(HASH * Hash)
{
    int i;

    if (Hash->Items) {

	/* free all headers */
	for (i = 0; i < Hash->nColumns; i++) {
	    if (Hash->Columns[i].key)
		free(Hash->Columns[i].key);
	}

	/* free header table */
	free(Hash->Columns);

	/* free all items */
	for (i = 0; i < Hash->nItems; i++) {
	    if (Hash->Items[i].key)
		free(Hash->Items[i].key);
	    if (Hash->Items[i].Slot)
		free(Hash->Items[i].Slot);
	}

	/* free items table */
	free(Hash->Items);
    }

    Hash->sorted = 0;
    Hash->nItems = 0;
    Hash->Items = NULL;
}
