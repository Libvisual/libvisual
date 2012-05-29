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

#ifndef _WIDGET_TEXT_H_
#define _WIDGET_TEXT_H_

#include <string>
#include <libvisual/libvisual.h>

#include "Property.h"
#include "Widget.h"
#include "LCDTimer.h"

namespace LCD {

class LCDCore;

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT, ALIGN_MARQUEE, ALIGN_AUTOMATIC, ALIGN_PINGPONG } TEXT_ALIGN;
typedef enum { SCROLL_RIGHT, SCROLL_LEFT } SCROLL_DIRECTION;

class WidgetText : public Widget {
    Property *prefix_;      /* label on the left side */
    Property *postfix_;     /* label on the right side */
    Property *value_;       /* value of text widget */
    Property *style_;       /* text style (plain/bold/slant) */
    std::string string_;    /* formatted value */
    std::string buffer_;    /* string with 'width+1' bytes allocated  */
    int offset_;            /* Increment by pixel */
    int precision_;         /* number of digits after the decimal point */
    TEXT_ALIGN align_;      /* alignment: L(eft), C(enter), R(ight), M(arquee), A(utomatic) */
    int update_;            /* update interval */
    int scroll_;            /* marquee starting point */
    int speed_;             /* marquee scrolling speed */
    int direction_;         /* pingpong direction, 0=right, 1=left */
    int delay_;             /* pingpong scrolling, wait before switch direction */
    bool bold_;

    LCDTimer *timer_;
    LCDTimer *scroll_timer_;

    public:
    WidgetText(LCDCore *visitor, std::string name, Json::Value *section, 
        int row, int col, int layer);
    ~WidgetText();
    void (*Draw)(WidgetText *w);
    std::string GetBuffer() { return buffer_; }
    int GetOffset() { return offset_; }
    int GetDirection() { return direction_; }
    bool GetBold() { return bold_; }
    void Start();
    void Stop();

    // Slots
    void Update();
    void TextScroll();

    void Resize(int rows, int cols, int old_rows, int old_cols);
};

}; // End namespace


#endif
