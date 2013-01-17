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

#include "WidgetBignums.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDCore.h"

using namespace LCD;

void bignums_update(void *data)
{
    WidgetBignums *widget = (WidgetBignums *)data;
    widget->Update();
}

extern void TextBignumsDraw(WidgetBignums *w);
extern void GraphicBignumsDraw(WidgetBignums *w);

extern int Font_8x16[10][16];

WidgetBignums::WidgetBignums(LCDCore *v, std::string n, Json::Value *section, 
    int row, int col, int layer) : Widget(v, n, section, row, col, layer,
    WIDGET_TYPE_BIGNUMS | WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {

    if(lcd_type_ == LCD_TEXT)
        Draw = TextBignumsDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicBignumsDraw;
    else
        Draw = NULL;

    expression_ = new Property(v, section, "expression", new Json::Value("return 0"));
    expr_min_ = new Property(v, section, "min", NULL);
    expr_max_ = new Property(v, section, "max", NULL);

    Json::Value *val = v->CFG_Fetch_Raw(section, "layer", new Json::Value(0));
    layer_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "update", new Json::Value(500));
    update_ = val->asInt();
    delete val;

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    min_ = max_ = 0.0;

    timer_ = v->timers_->AddTimer(bignums_update, this, update_, true);

/*
    timer_ = new QTimer();
    timer_->setSingleShot(false);
    timer_->setInterval(update_);
    QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(Update()));

    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
        this, SLOT(Resize(int, int, int, int)));
*/
}

WidgetBignums::~WidgetBignums() {
    Stop();
    delete expression_;
    delete expr_min_;
    delete expr_max_;
}

void WidgetBignums::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / xres) + 0.5);
    Update();
}

void WidgetBignums::SetupChars() {
    ch_.clear();
    LCDText *lcd = (LCDText *)visitor_->GetLCD();
    for(int i = 0; i < 8; i++) {
        if((int)lcd->special_chars.size() >= lcd->CHARS ) {
            LCDError("Can not allot char for widget: %s", name_.c_str());
            update_ = -1;
            return;
        }
        lcd->special_chars.push_back(SpecialChar(8));
        ch_.push_back(lcd->special_chars.size() - 1);
    }
}

void WidgetBignums::Update() {

    double max, min, val;

    expression_->Eval();
    val = expression_->P2N();

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

    int value;
    if(max > min)
        value = (int)((val - min) / (max - min) * 100);
    else
        value = 0;

    std::ostringstream oss;
    oss << value;
    std::stringstream strm;
    strm << oss.str();
    std::string text;
    strm >> text;

    FB_.clear();
    FB_.resize(16*24);

    int pad = 3 - text.size();
    for(int i = 0; i < (int)text.size(); i++) {
        int c = text[i] - '0';
        for(int row = 0; row < 16; row++) {
            int mask = 1<<8;
            for(int col = 0; col < 8; col++) {
                mask >>= 1;
                if(Font_8x16[c][row] & mask)
                    FB_[row * 24 + (i + pad) * 8 + col] = '.';
            }
        }
    }
    
    if(Draw)
        Draw(this);
    else
        LCDError("WidgetBignums: no draw method.");
}

void WidgetBignums::Start() {
    if(update_ < 0)
        return;
    timer_->Start();
}

void WidgetBignums::Stop() {
    timer_->Stop();
}
