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

#include <time.h>

#ifdef __OPENMP
#include <omp.h>
#endif

#include "WidgetFPS.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDCore.h"

using namespace LCD;

void fps_update(void *data)
{
    WidgetFPS *widget = (WidgetFPS *)data;
    widget->Update();
}

extern void TextFPSDraw(WidgetFPS *w);
extern void GraphicFPSDraw(WidgetFPS *w);

extern int Font_8x16[10][16];


WidgetFPS::WidgetFPS(LCDCore *v, std::string n, Json::Value *section, 
    int row, int col, int layer) : Widget(v, n, section, row, col, layer,
    WIDGET_TYPE_SPECIAL) {

    if(lcd_type_ == LCD_TEXT)
        Draw = TextFPSDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicFPSDraw;
    else
        Draw = NULL;

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    timer_ = v->timers_->AddTimer(fps_update, this, 200, true);

    stats_init( &stats_ );
}

WidgetFPS::~WidgetFPS() {
    Stop();
}

void WidgetFPS::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / xres) + 0.5);
    Update();
}

void WidgetFPS::SetupChars() {
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

void WidgetFPS::Update() {

    if(flipped_)
        stats_startFrame(&stats_);
    else

        stats_endFrame(&stats_);

    flipped_ = !flipped_;

    unsigned int value = (int)stats_.avgFrame;

    std::ostringstream oss;
    oss << value;
    std::stringstream strm;
    strm << oss.str();
    std::string text;
    strm >> text;

    FB_.clear();
    FB_.resize(16*24);

    int pad = 3 - text.size();


#ifdef __OPENMP
# pragma omp parallel
# pragma omp for
#endif

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
        LCDError("WidgetFPS: no draw method.");
}

void WidgetFPS::Start() {
    if(update_ < 0)
        return;
    timer_->Start();
}

void WidgetFPS::Stop() {
    timer_->Stop();
}


