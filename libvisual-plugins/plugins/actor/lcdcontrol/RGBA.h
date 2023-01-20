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

#ifndef _RGB_H_
#define _RGB_H_

#include <libvisual/libvisual.h>

namespace LCD {
/*
typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
} RGBA;
*/
class RGBA {
    public:
    VisColor rgb;
    RGBA() : rgb(0, 0, 0, 255) {}
    RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) :
        rgb(r, g, b, a){
        R = r; G = g; B = b; A = a;
    }
    RGBA(int val) {
        
	    visual_color_set_from_uint32(&rgb, val);
    	R = rgb.r;
	    G = rgb.g;
    	B = rgb.b;
	    A = rgb.a;
    }
    uint32_t ToInt()
    {
        return visual_color_to_uint32(&rgb);
    }
    VisColor *ToColor()
    {
        return &rgb;
    }
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;

    bool operator==(const RGBA &rhv) const {
        return R == rhv.R && G == rhv.G && B == rhv.B && A == rhv.A;
    }
    bool operator!=(const RGBA &rhv) const {
        return R != rhv.R || G != rhv.G || B != rhv.B || A != rhv.A;
    }
};

int color2RGBA(const char *color, RGBA * C);

}; // End namespace

#endif
