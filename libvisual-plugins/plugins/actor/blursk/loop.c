/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of Blurks-libvisual.
 *
 * Blurks-libvisual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blurks-libvisual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blurks-libvisual.  If not, see <http://www.gnu.org/licenses/>.
 */

/* loop.c */

/*  Blursk - visualization plugin for XMMS
 *  Copyright (C) 1999  Steve Kirkendall
 *
 *  Portions of this file are derived from the XMMS "Blur Scope" plugin.
 *  XMMS is Copyright (C) 1998-1999  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "actor_blursk.h"
#include "blursk.h"

# define BLUR   src = *srcref++; \
        *dest++ = (src[-bpl] + src[0] \
            + src[bpl - 1] + src[bpl + 1]) >> 2; \
        bpl = -bpl;

# define SHARP  *dest++ = **srcref++;

# define SMEAR  src = *srcref++; \
        pix = (src[-bpl - 1] + src[bpl - 1] \
            + src[0] + src[1]) >> 2; \
        if (pix < *orig++) \
            pix = orig[-1]; \
        *dest++ = pix; \
        bpl = -bpl;

# define MELT   src = *srcref++; \
        pix = *orig++; \
        if (pix < 160) \
            pix = (src[-bpl] + src[0] \
                + src[bpl - 1] + src[bpl + 1]) >> 2; \
        *dest++ = pix; \
        bpl = -bpl;


void loopblur(void)
{
    unsigned int i = img_chunks;
    int bpl = img_bpl;
    unsigned char *dest, *src, **srcref;

    i = img_chunks;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        BLUR
        BLUR
        BLUR
        BLUR
        BLUR
        BLUR
        BLUR
        BLUR

    } while (--i != 0);
}

void loopsmear(void)
{
    unsigned int i = img_chunks;
    int bpl = img_bpl;
    unsigned char *dest, *src, *orig, **srcref, pix;

    i = img_chunks;
    dest = img_tmp;
    srcref = img_source;
    orig = img_buf;
    do
    {
        SMEAR
        SMEAR
        SMEAR
        SMEAR
        SMEAR
        SMEAR
        SMEAR
        SMEAR

    } while (--i != 0);
}

void loopmelt(void)
{
    unsigned int i = img_chunks;
    int bpl = img_bpl;
    unsigned char *dest, *src, *orig, **srcref, pix;

    i = img_chunks;
    dest = img_tmp;
    srcref = img_source;
    orig = img_buf;
    do
    {
        MELT
        MELT
        MELT
        MELT
        MELT
        MELT
        MELT
        MELT

    } while (--i != 0);
}


void loopsharp(void)
{
    unsigned int i;
    unsigned char *dest, **srcref;

    i = img_chunks;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        SHARP
        SHARP
        SHARP
        SHARP
        SHARP
        SHARP
        SHARP
        SHARP

    } while (--i != 0);
}

void loopreduced1(void)
{
    unsigned int i;
    int bpl;
    unsigned char *dest, *src, **srcref;

    i = img_chunks;
    bpl = img_bpl;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        BLUR
        SHARP
        SHARP
        SHARP
        BLUR
        SHARP
        SHARP
        SHARP
    } while (--i != 0);
}

void loopreduced2(void)
{
    unsigned int i;
    int bpl;
    unsigned char *dest, *src, **srcref;

    i = img_chunks;
    bpl = img_bpl;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        SHARP
        BLUR
        SHARP
        SHARP
        SHARP
        BLUR
        SHARP
        SHARP

    } while (--i != 0);
}

void loopreduced3(void)
{
    unsigned int i;
    int bpl;
    unsigned char *dest, *src, **srcref;

    i = img_chunks;
    bpl = img_bpl;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        SHARP
        SHARP
        BLUR
        SHARP
        SHARP
        SHARP
        BLUR
        SHARP

    } while (--i != 0);
}

void loopreduced4(void)
{
    unsigned int i;
    int bpl;
    unsigned char *dest, *src, **srcref;

    i = img_chunks;
    bpl = img_bpl;
    dest = img_tmp;
    srcref = img_source;
    do
    {
        SHARP
        SHARP
        SHARP
        BLUR
        SHARP
        SHARP
        SHARP
        BLUR

    } while (--i != 0);
}

void loopfade(int change)
{
    register unsigned char *ptr;
    unsigned char   limit;
    unsigned int    i;

    /* Fade the pixels */
    if (change < 0)
    {
        change = -change;
        ptr = img_buf;
        i = img_chunks;
        do
        {
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
            if (*ptr > change) *ptr -= change; else *ptr = 0;
            ptr++;
        } while (--i != 0);
    }
    else
    {
        limit = 255 - change;
        ptr = img_buf;
        i = img_chunks;
        do
        {
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
            if (*ptr < limit) *ptr += change; else *ptr = 255;
            ptr++;
        } while (--i != 0);
    }
}

/* Interpolate between pixels, doubling the image width.  It is assumed that
 * the source is in img_buf, the destination is img_tmp, and img_tmp is large
 * enough to hold the double-width image.
 */
void loopinterp(void)
{
    unsigned int i = img_chunks;
    unsigned char *dest, *src, prev;

    i = img_chunks;
    dest = img_tmp;
    src = img_buf;
    do
    {
        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

        prev = *dest++ = *src++;
        *dest++ = (prev + *src) >> 1;

    } while (--i != 0);
}
