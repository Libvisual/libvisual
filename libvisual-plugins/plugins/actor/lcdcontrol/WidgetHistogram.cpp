/* $Id$
 * $URL$
 *
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

#include <json/json.h>

#include "WidgetHistogram.h"
#include "LCDCore.h"
#include "LCDText.h"
#include "LCDGraphic.h"

using namespace LCD;

extern void TextHistogramDraw(WidgetHistogram *w);
extern void GraphicHistogramDraw(WidgetHistogram *w);

void histogram_update(void *data)
{
    WidgetHistogram *widget = (WidgetHistogram *)data;
    widget->Update();
}

WidgetHistogram::WidgetHistogram(LCDCore *v, std::string n, Json::Value *section, 
    int row, int col, int layer) : Widget(v, n, section, row, col, layer, 
    WIDGET_TYPE_HISTOGRAM | WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {


    if(lcd_type_ == LCD_TEXT)
        Draw = TextHistogramDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicHistogramDraw;
    else
        Draw = NULL;

    min_ = max_ = 0;
    string_ = "";
    offset_ = 0;

    expression_ = new Property(v, section, "expression", new Json::Value("return 0"));
    expr_min_ = new Property(v, section, "min", NULL);
    expr_max_ = new Property(v, section, "max", NULL);

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    Json::Value *val = v->CFG_Fetch_Raw(section, "width", new Json::Value(10));
    cols_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "height", new Json::Value(1));
    rows_ = val->asInt();
    delete val;

    LCDError("1 cols_ %d, rows_ %d", cols_, rows_);
    int diff = (col_ + cols_) - v->GetLCD()->LCOLS;
    if(diff > 0) cols_ -= diff;

    diff = (row_ + rows_) - v->GetLCD()->LROWS;
    if(diff > 0) rows_ -= diff;
    LCDError("2 cols_ %d, rows_ %d", cols_, rows_);

    val = v->CFG_Fetch_Raw(section, "gap", new Json::Value(0));
    gap_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "direction", new Json::Value("E"));
    if( val->asString() == "E" )
        direction_ = DIR_EAST;
    else if (val->asString() == "W" )
        direction_ = DIR_WEST;
    else {
        LCDError("Widget %s has unknown direction '%s'; Use (E)ast or (W)est. Using E.",
            name_.c_str(), val->asCString());
        direction_ = DIR_EAST;
    }
    delete val;

    val = v->CFG_Fetch(section, "update", new Json::Value(1000));
    update_ = val->asInt();
    delete val;
   
    history_.resize(cols_);

    timer_ = v->timers_->AddTimer(histogram_update, this, update_, true);

/*
    timer_ = new QTimer();
    timer_->setSingleShot(false);
    timer_->setInterval(update_);
    QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(Update()));

    QObject::connect(v->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
        this, SLOT(Resize(int, int, int, int)));
*/
}

WidgetHistogram::~WidgetHistogram() {
    Stop();
    delete expression_;
    delete expr_min_;
    delete expr_max_;
}

void WidgetHistogram::SetupChars() {
    ch_.clear();
    LCDText *lcd = (LCDText *)visitor_->GetLCD();
    for(int c = 0; c < (int)lcd->YRES; c++ ) {
        SpecialChar buffer(lcd->YRES);;
        for(int i = lcd->YRES - 1; i >= 0; i-- ) {
            buffer[lcd->YRES - i - 1] = (i < c ? pow(2.0, (double)lcd->XRES)-1-gap_ : 0);
        }

        bool reused = false;

        for(unsigned int i = 0; i < lcd->special_chars.size(); i++ ) {
            if( buffer == lcd->special_chars[i] ) {
                ch_[c] = i;
                reused = true;
                break;
            }
        }

        if( lcd->CHARS < (int)lcd->special_chars.size() ) {
            update_ = -1;
            LCDError("Widget %s - unable to allocate special chars", 
                name_.c_str());
            return;
        }

        if( reused ) continue;
        lcd->special_chars.push_back(buffer);
        ch_[c] = lcd->special_chars.size() - 1;
        lcd->TextSpecialCharChanged(ch_[c]);
    }
}

void WidgetHistogram::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float y = rows_ * yres / (float)old_rows;
    float x = cols_ * xres / (float)old_cols;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    rows_ = (int)((rows * y / yres) + 0.5);
    cols_ = (int)((cols * x / xres) + 0.5);
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / yres) + 0.5);
    history_.resize(cols_);
    Update();
}

void WidgetHistogram::Update() {

    if(lcd_type_ == LCD_GRAPHIC && direction_ == DIR_EAST)
        offset_++;
    else if(lcd_type_ == LCD_GRAPHIC && direction_ == DIR_WEST)
        offset_--;

    if(lcd_type_ == LCD_GRAPHIC && abs(offset_) < visitor_->GetLCD()->XRES) {
        if(Draw)
            Draw(this);
        return;
    } else {
        offset_ = 0;
    }

    expression_->Eval();
    double val = expression_->P2N();

    double min;
    double max;

    if( expr_min_->Valid() ) {
        expr_min_->Eval();
        min = expr_min_->P2N();
    } else {
        min = min_;
        if( val < min )
            min = val;
    }

    if( expr_max_->Valid() ) {
        expr_max_->Eval();
        max = expr_max_->P2N();
    } else {
        max = max_;
        if( val > max )
            max = val;
    }

    min_ = min;
    max_ = max;

    if( max > min ) 
        val = (val - min) / (max - min);
    else
        val = 0.0;

    if( direction_ == DIR_EAST ) {
        std::vector<double> tmp = history_;
        history_[0] = val;
        for(int i = 1; i < cols_; i++ ) {
            history_[i] = tmp[i-1];
        }
    } else {
        std::vector<double> tmp = history_;
        history_[cols_-1] = val;
        for(int i = 0; i < cols_ - 1; i++ ) {
            history_[i] = tmp[i+1];
        }
    }

    if(Draw)
        Draw(this);
}

void WidgetHistogram::Start() {
    if(update_ < 0)
        return;
    timer_->Start();
    Update();
}

void WidgetHistogram::Stop() {
    timer_->Stop();
    ch_.clear();
}

