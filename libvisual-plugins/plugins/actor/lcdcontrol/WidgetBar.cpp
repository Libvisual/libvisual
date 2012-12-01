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

#include "WidgetBar.h"
#include "LCDCore.h"
#include "LCDText.h"

using namespace LCD;

int SCHARS[][9] = {
    {31, 31, 31, 31, 31, 31, 31, 31, -1},
    {31, 16, 16, 16, 16, 16, 16, 31, -1},
    {31,  0,  0,  0,  0,  0,  0, 31, -1},
    {31,  1,  1,  1,  1,  1,  1, 31, -1},
    {31, 31, 31, 31,  0,  0,  0,  0, -1},
    { 0,  0,  0,  0, 31, 31, 31, 31, -1}};

extern void TextBarDraw(WidgetBar *w);
extern void GraphicBarDraw(WidgetBar *w);

void bar_update(void *data)
{
    WidgetBar *widget = (WidgetBar *)data;
    widget->Update();
}

WidgetBar::WidgetBar(LCDCore *v, std::string n, Json::Value *section, 
    int row, int col, int layer) : Widget(v, n, section, row, col, layer,
    WIDGET_TYPE_BAR | WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {

    lcd_type_ = v->GetType();

    if(lcd_type_ == LCD_TEXT)
        Draw = TextBarDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicBarDraw;
    else
        Draw = NULL;

    expression_ = new Property(v, section, "expression", new Json::Value("0"));
    expression2_ = new Property(v, section, "expression2", NULL);
    expr_min_ = new Property(v, section, "min", NULL);
    expr_max_ = new Property(v, section, "max", NULL);

    color_valid_[0] = WidgetColor(section, "barcolor0", &color_[0]);
    color_valid_[1] = WidgetColor(section, "barcolor1", &color_[1]);

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    Json::Value *val = v->CFG_Fetch(section, "length", new Json::Value(10));
    cols_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section, "height", new Json::Value(1));
    rows_ = val->asInt();
    delete val;

    //LCDError("1 cols %d, rows %d", cols_, rows_);
    int diff = (col_ + cols_) - v->GetLCD()->LCOLS;
    if(diff > 0) cols_ -= diff;

    diff = (row_ + rows_) - v->GetLCD()->LROWS;
    if(diff > 0) rows_ -= diff;

    //LCDError("2 cols %d, rows %d", cols_, rows_);
    val = v->CFG_Fetch_Raw(section, "direction", new Json::Value("E"));
    if( val->asString() == "E" ) {
        direction_ = DIR_EAST;
    } else if ( val->asString() == "W" ) {
        direction_ = DIR_WEST;
    } else {
        LCDError("Widget %s has unknown direction '%s'; Use (E)ast or (W)est. Using (E).",
            name_.c_str(), val->asCString());
        direction_ = DIR_EAST;
    }
    delete val;

    val = v->CFG_Fetch(section, "update", new Json::Value(1000));
    update_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "style", new Json::Value("N"));
    
    if( val->asString() == "H" ) {
        style_ = STYLE_HOLLOW;
    } else if (val->asString() == "N") {
        style_ = STYLE_NORMAL;
    } else {
        LCDError("Widget %s has unknown style '%s'; known styles are 'N' or 'H'; using 'N'.",
           name_.c_str(), val->asCString());
        style_ = STYLE_NORMAL;
    }

    val1_ = val2_ = 0.0;
    min_ = max_ = 0.0;

    timer_ = v->timers_->AddTimer(bar_update, this, update_, true);
/*
    timer_ = new QTimer();
    timer_->setSingleShot(false);
    timer_->setInterval(update_);
    QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(Update()));

    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
        this, SLOT(Resize(int, int, int, int)));
*/
}

WidgetBar::~WidgetBar() {
    Stop();
    //delete timer_;
    delete expression_;
    delete expression2_;
    delete expr_min_;
    delete expr_max_;
}

void WidgetBar::Resize(int rows, int cols, int old_rows, int old_cols) {
    int xres = visitor_->GetLCD()->XRES;
    int yres = visitor_->GetLCD()->YRES;
    float y = rows_ * yres / (float)old_rows;
    float x = cols_ * xres / (float)old_cols;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    rows_ = (int)((visitor_->GetLCD()->LROWS * y / yres) + 0.5);
    cols_ = (int)((visitor_->GetLCD()->LCOLS * x / xres) + 0.5);
    row_ = (int)((visitor_->GetLCD()->LROWS * r / yres) + 0.5);
    col_ = (int)((visitor_->GetLCD()->LCOLS * c / xres) + 0.5);
    Update();
}

void WidgetBar::SetupChars() {
    LCDText *lcd = (LCDText *)visitor_->GetLCD();
    ch_.clear();
    if( style_ == STYLE_HOLLOW and not expression2_->Valid()) {
        for( int i = 0; i < 4; i++ ) {
            for( int j = 0; j < (int)lcd->special_chars.size(); j++ ) {
                SpecialChar c(SCHARS[i], 8);
                if( lcd->special_chars[j] == c ) {
                    ch_[i] = j;
                }
            }

        }
        for(int i = 0; i < 6; i++ ) {
            std::map<const int, char>::iterator it = ch_.find(i);
            if(it != ch_.end()) {
                if((int)lcd->special_chars.size() >= lcd->CHARS ) {
                    LCDError("Can not allot char for widget: %s",name_.c_str());
                    update_ = -1;
                    return;
                }
                lcd->special_chars.push_back(SpecialChar(SCHARS[i], 8));
                ch_[i] = lcd->special_chars.size() - 1;
                lcd->TextSpecialCharChanged(ch_[i]);
            }
        }
    } else if (style_ == STYLE_NORMAL ) {
        for(unsigned int j = 0; j < lcd->special_chars.size(); j++ ) {
            if( lcd->special_chars[j] == SpecialChar(SCHARS[0], 8)) {
                ch_[0] = j;
            }
            if( lcd->special_chars[j] == SpecialChar(SCHARS[4], 8)) {
                ch_[1] = j;
            }
            if( lcd->special_chars[j] == SpecialChar(SCHARS[5], 8)) {
                ch_[2] = j;
            }
        }
        std::map<const int, char>::iterator it;

        it = ch_.find(0);
        if(it == ch_.end()) {
            if((int)lcd->special_chars.size() >= lcd->CHARS ) {
                LCDError("Can not allot char for bar widget: %s",name_.c_str());
                update_ = -1;
                return;
            }
            lcd->special_chars.push_back(SpecialChar(SCHARS[0], 8));
            ch_[0] = lcd->special_chars.size() - 1;
            lcd->TextSpecialCharChanged(ch_[0]);
        }

        it = ch_.find(1);
        if(it == ch_.end() && expression2_->Valid()) {
            if( (int)lcd->special_chars.size() >= lcd->CHARS ) {
                LCDError("Can not allot char for bar widget: %s",name_.c_str());
                update_ = -1;
                return;
            }
            lcd->special_chars.push_back(SpecialChar(SCHARS[4], 8));
            ch_[1] = lcd->special_chars.size() - 1;
            lcd->TextSpecialCharChanged(ch_[1]);
        }

        it = ch_.find(2);
        if(it == ch_.end() && expression2_->Valid()) {
            if( (int)lcd->special_chars.size() >= lcd->CHARS ) {
                LCDError("Can not allot char for bar widget: %s",name_.c_str());
                update_ = -1;
                return;
            }
            lcd->special_chars.push_back(SpecialChar(SCHARS[5], 8));
            ch_[2] = lcd->special_chars.size() - 1;
            lcd->TextSpecialCharChanged(ch_[2]);
        }
        
    } else {
        LCDError("%s: Either choose style (H)ollow or have a 2nd expression.", 
            widget_base_.c_str());
    }
}

void WidgetBar::Start() {
    if( update_ < 0 )
        return;
    //timer_->start();
    Update();
}

void WidgetBar::Stop() {
    ch_.clear();
    //timer_->stop();
}

void WidgetBar::Update() {
    expression_->Eval();
    double val1 = expression_->P2N();
    double val2 = val1;
    if( expression2_->Valid() ) {
        expression2_->Eval();
        val2 = expression2_->P2N();
    }

    double max, min;
    if( expr_min_->Valid() ) {
        expr_min_->Eval();
        min = expr_min_->P2N();
    } else {
        min = min_;
        if( val1 < min )
            min = val1;
        if( val2 < min )
            min = val2;
    }

    if( expr_max_->Valid() ) {
        expr_max_->Eval();
        max = expr_max_->P2N();
    } else {
        max = max_;
        if( val1 > max )
            max = val1;
        if( val2 > max )
            max = val2;
    }

    min_ = min;
    max_ = max;

    if( max > min ) {
        val1_ = (val1 - min ) / (max - min);
        val2_ = (val2 - min ) / (max - min);
    } else {
        val1_ = 0.0;
        val2_ = 0.0;
    }

    if(Draw) 
        Draw(this);
    else
        LCDError("WidgetBar: no draw method");
}

