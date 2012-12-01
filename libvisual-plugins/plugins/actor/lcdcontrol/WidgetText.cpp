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

#include <string>
#include <sstream>
#include <iomanip>

#include "Property.h"
#include "Widget.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "WidgetText.h"
#include "LCDCore.h"
#include "debug.h"

using namespace LCD;

extern void TextDraw(WidgetText *w);

extern void GraphicDraw(WidgetText *w);

template <class T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&), int precision)
{
  std::ostringstream oss;
  oss.setf(std::ios_base::floatfield, std::ios::fixed);
  oss << std::setprecision(precision) << f << t;
  return oss.str();
}

void text_update(void *data)
{
    WidgetText *text = (WidgetText *)data;
    text->Update();
}

void text_scroll(void *data)
{
    WidgetText *text = (WidgetText *)data;
    text->TextScroll();
}

WidgetText::WidgetText(LCDCore *v, std::string name, Json::Value *config,
        int row, int col, int layer) : Widget(v, name, config, row, col, layer,
        WIDGET_TYPE_TEXT | WIDGET_TYPE_RC) {

    if(lcd_type_ == LCD_TEXT)
        Draw = TextDraw;
    else if (lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicDraw;


    string_ = "";
    offset_ = 0;

    prefix_ = new Property(v, section_, "prefix", new Json::Value(""));
    postfix_ = new Property(v, section_, "postfix", new Json::Value(""));
    style_ = new Property(v, section_, "style", new Json::Value(""));
    value_ = new Property(v, section_, "expression", new Json::Value(""));

    fg_valid_ = WidgetColor(section_, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section_, "background", &bg_color_);

    Json::Value *val;
    val = v->CFG_Fetch(section_, "length", new Json::Value(10));
    cols_ = val->asInt();
    rows_ = 1;
    delete val;
    
    if( col_ + cols_ >= v->GetLCD()->LCOLS) {
        int diff = (col_ + cols_) - v->GetLCD()->LCOLS;
        cols_-=diff;
    }

    scroll_ = cols_;

    if( lcd_type_ == LCD_TEXT && cols_ + col_ >= v->GetLCD()->LCOLS ) {
        cols_ -= cols_ + col - v->GetLCD()->LCOLS;
    } else if (lcd_type_ == LCD_GRAPHIC && cols_ + col_ >= 
        v->GetLCD()->LCOLS / v->GetLCD()->XRES ) {
        cols_ -= cols_ + col_ -
            v->GetLCD()->LCOLS / v->GetLCD()->XRES;
    }

    val = v->CFG_Fetch(section_, "precision", new Json::Value(0xBABE));
    precision_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section_, "align", new Json::Value("L"));
    const char *c = val->asCString();
    switch ((*c)) {
    case 'L':
        align_ = ALIGN_LEFT;
        break;
    case 'C':
        align_ = ALIGN_CENTER;
        break;
    case 'R':
        align_ = ALIGN_RIGHT;
        break;
    case 'M':
        align_ = ALIGN_MARQUEE;
        break;
    case 'A':
        align_ = ALIGN_AUTOMATIC;
        break;
    case 'P':
        align_ = ALIGN_PINGPONG;
        break;
    default:
        LCDError("widget %s has unknown alignment '%s', using 'L'", widget_base_.c_str(), c);
        align_ = ALIGN_LEFT;
    }
    delete val;

    val = v->CFG_Fetch(section_, "direction", new Json::Value(SCROLL_RIGHT));
    direction_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "update", new Json::Value(1000));
    update_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "speed", new Json::Value(500));
    speed_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "bold", new Json::Value(0));
    bold_ = val->asInt();
    delete val;

    timer_ = v->timers_->AddTimer(text_update, this, update_, true);
    scroll_timer_ = v->timers_->AddTimer(text_scroll, this, speed_, true);
}


WidgetText::~WidgetText() {
    Stop();
    delete prefix_;
    delete postfix_;
    delete style_;
    delete value_;
}

void WidgetText::Resize(int rows, int cols, int old_rows, int old_cols) {
    LCDInfo("WidgetText::REsize");
    int xres = visitor_->GetLCD()->XRES;
    int yres = visitor_->GetLCD()->YRES;
    float x = cols_ * xres / (float)old_cols;
    float r  = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    LCDInfo("WidgetText::REsize cols: %d, row: %d, col: %d, x: %f, r: %f, c: %f", cols_, row_, col_, x, r, c);
    cols_ = (int)((cols * x / xres) + 0.5);
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / xres) + 0.5);
    LCDInfo("WidgetText::REsize cols: %d, row: %d, col: %d, x: %f, r: %f, c: %f", cols_, row_, col_, x, r, c);
    Update();
}

void WidgetText::TextScroll()
{

    std::string pre = prefix_->P2S();
    std::string post = postfix_->P2S();

    std::string str = string_;


    int num, len, width, pad;
    int srcPtr = 0, dstPtr = 0;
    std::string src, dst;

    num = 0;
    len = str.length();
    width = cols_ - pre.length() - post.length();
    if (width < 0)
        width = 0;

    if(direction_ == SCROLL_RIGHT && (align_ == ALIGN_MARQUEE || align_ == ALIGN_PINGPONG || align_ == ALIGN_AUTOMATIC)) {
        offset_++;
    } else if (align_ == ALIGN_MARQUEE || align_ == ALIGN_PINGPONG || align_ == ALIGN_AUTOMATIC) {
        offset_--;
    }

    if(abs(offset_) <= visitor_->GetLCD()->XRES && (align_ == ALIGN_MARQUEE || align_ == ALIGN_PINGPONG || (align_ == ALIGN_AUTOMATIC && len > width))) {
        if(Draw != NULL) {
            Draw(this);
            return;
        }
    } else {
        offset_ = 0;
    }

    switch (align_) {
    case ALIGN_LEFT:
        pad = 0;
        break;
    case ALIGN_CENTER:
        pad = (width - len) / 2;
        if (pad < 0)
            pad = 0;
        break;
    case ALIGN_RIGHT:
        pad = width - len;
        if (pad < 0)
            pad = 0;
        break;
    case ALIGN_AUTOMATIC:
        if (len <= width) {
            pad = 0;
            break;
        }
    case ALIGN_MARQUEE:
        pad = width - scroll_;
        scroll_++;
        if (scroll_ >= width + len)
            scroll_ = cols_;
        break;
    case ALIGN_PINGPONG:
#define PINGPONGWAIT 2

        /* scrolling is not necessary - center the str */
        if (len <= width) {
            pad = (width - len) / 2;
        } else {
            if (direction_ == SCROLL_RIGHT)
                scroll_++;      /* scroll right */
            else
                scroll_--;      /* scroll left */

            /*pad = if positive, add leading space characters, else offset of str begin */
            pad = 0 - scroll_;

            if (pad < 0 - (len - width)) {
                if (delay_-- < 1) {     /* wait before switch direction */
                    direction_ = SCROLL_LEFT;     /* change scroll direction */
                    delay_ = PINGPONGWAIT;
                    scroll_ -= PINGPONGWAIT;
                }               /* else debug("wait1"); */
                pad = 0 - (len - width);
            } else if (pad > 0) {
                if (delay_-- < 1) {
                    direction_ = SCROLL_RIGHT;
                    delay_ = PINGPONGWAIT;
                    scroll_ += PINGPONGWAIT;
                }               /* else debug("wait2"); */
                pad = 0;
            }

        }
        break;
    default:                        /* not reached  */
        pad = 0;
    }

    dst.clear();
    dst.resize(cols_);

    for(int i = 0; i < cols_; i++)
    {
        dst[i] = ' ';
    }

    dstPtr = 0;
    srcPtr = 0;

    num = 0;


    /* process prefix */
/*
    src = pre;
    while (num < cols_) {
        if (srcPtr == (int)src.length())
            break;
        dst[dstPtr++] = src[srcPtr++];
        num++;
    }
*/

    src = str;
    srcPtr = 0;

    int offset = pad;
    if(offset < 0)
        offset = 0;
    /* wrap around on the beginning */
/*
    while (pad > 0 && num < cols_) {
        if(align_ == ALIGN_MARQUEE) 
            dst[dstPtr++] = src[(src.size() - offset) + srcPtr++];
        else
            dst[dstPtr++] = ' ';
        num++;
        pad--;
    }
*/

    /* skip src chars (marquee) */
    std::string tmp = src;
    while (pad < 0 && tmp != "") {
        src = tmp.substr(1, tmp.length()); 
        tmp = src;
        pad++;
    }

    /* copy content */
    while (num < cols_) {
        if (srcPtr >= (int)src.length())
            break;
        dst[dstPtr++] = src[srcPtr++];
        num++;
    }

    /* wrap around on end */
    src = post;
    len = src.length();
    srcPtr = 0;
    if(num < cols_ - len && align_ == ALIGN_MARQUEE) {
        dst[dstPtr++] = '*';
        num++;
    }

    // Pad end
/*
    while (num < cols_ - len) {
        if(align_ == ALIGN_MARQUEE)
            dst[dstPtr++] = str[srcPtr++];
        else
            dst[dstPtr++] = ' ';
        num++;
    }
*/

    srcPtr = 0;

    /* process postfix */
/*
    while (num < cols_) {
        if (srcPtr >= (int)src.length())
            break;
        dst[dstPtr++] = src[srcPtr++];
        num++;
    }
*/
    buffer_ = dst;

    /* finally, draw it! */
    if (Draw)
        Draw(this);
    else
        LCDError("WidgetText: No draw method.");
}

void WidgetText::Update()
{
    std::string str;
    int update = 0;

    /* evaluate properties */
    update += prefix_->Eval();
    update += postfix_->Eval();
    update += style_->Eval();

    /* evaluate value */
    value_->Eval();

    /* str or number? */
    if (precision_ == 0xBABE) {
        str = value_->P2S();
    } else {
        double number = value_->P2N();
        int width = cols_ - prefix_->P2S().size() - postfix_->P2S().size();
        int precision = precision_;
        /* print zero bytes so we can specify NULL as target  */
        /* and get the length of the resulting str */
        int size = snprintf((char *)NULL, 0, (char *)"%.*f", precision, number);
        /* number does not fit into field width: try to reduce precision */
        if (width < 0)
            width = 0;
        if (size > width && precision > 0) {
            int delta = size - width;
            if (delta > precision)
                delta = precision;
            precision -= delta;
            size -= delta;
            /* zero precision: omit decimal point, too */
            if (precision == 0)
                size--;
        }
        /* number still doesn't fit: display '*****'  */
        if (size > width) {
            str.resize(width);
            for(int i = 0; i < width; i++) 
                str[i] = '*';
        } else {
            str = to_string<double>(number, std::dec, precision);
        }
    }

    if (string_.empty() || str != string_ ) {
        update++;
        string_ = str;
    } 

    /* something has changed and should be updated */
    if (update) {

        /* Init pingpong scroller. start scrolling left (wrong way) to get a delay */
        if (align_ == ALIGN_PINGPONG) {
            direction_ = 0;
            delay_ = PINGPONGWAIT;
        }
        /* if there's a marquee scroller active, it has its own */
        /* update callback timer, so we do nothing here; otherwise */
        /* we simply call this scroll callback directly */
        if (align_ != ALIGN_MARQUEE || align_ != ALIGN_AUTOMATIC ||
            align_ != ALIGN_PINGPONG) {
            TextScroll();
        }

    }

}

void WidgetText::Start() {
    timer_->Start();
    scroll_timer_->Start();
    Update();
}

void WidgetText::Stop() {
    timer_->Stop();
    scroll_timer_->Stop();
}

