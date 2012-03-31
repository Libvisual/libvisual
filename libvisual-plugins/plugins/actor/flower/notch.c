/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Antti Silvast <asilvast@iki.fi>
 *
 * Authors: Antti Silvast <asilvast@iki.fi>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: notch.c,v 1.3 2006/01/22 13:25:26 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "notch.h"
#include <libvisual/libvisual.h>

NOTCH_FILTER *  init_notch(float cutoff) {
    NOTCH_FILTER * l=malloc(sizeof(NOTCH_FILTER));
    float steep = 0.99;;
    float r = steep * 0.99609375;
    float f = cos(VISUAL_MATH_PI * cutoff / SAMPLING_RATE);
    l->cutoff = cutoff;
    l->a0 = (1 - r) * sqrt( r * (r - 4 * (f * f) + 2) + 1);
    l->b1 = 2 * f * r;
    l->b2 = -(r * r);

    l->x1 = 0.0;
    l->x2 = 0.0;
    return l;
}

float process_notch(NOTCH_FILTER * l, float x0) {
    float outp = l->a0 * x0 + l->b1 * l->x1 + l->b2 * l->x2;
    l->x2 = l->x1;
    l->x1 = outp;

    return outp;
}

