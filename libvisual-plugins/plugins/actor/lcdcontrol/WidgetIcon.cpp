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

#include <json/json.h>
#include <string>
#include <vector>
#include <map>

#include "LCDCore.h"
#include "Widget.h"
#include "WidgetIcon.h"
#include "SpecialChar.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "debug.h"

using namespace LCD;

void icon_update(void *data)
{
    WidgetIcon *icon = (WidgetIcon *)data;
    icon->Update();
}

extern void TextIconDraw(WidgetIcon *w);
extern void GraphicIconDraw(WidgetIcon *w);

WidgetIcon::WidgetIcon(LCDCore *v, std::string n, 
    Json::Value *section, int row, int col, int layer) :
    Widget(v, n, section, row, col, layer, WIDGET_TYPE_ICON | 
    WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {


    if(lcd_type_ == LCD_TEXT)
        Draw = TextIconDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicIconDraw;
    else
        Draw = NULL;

    cols_ = 1;
    rows_ = 1;

    visible_ = new Property(v, section, "visible", new Json::Value("return 1"));

    Json::Value *val = v->CFG_Fetch_Raw(section, "speed", new Json::Value(500));
    update_ = val->asInt();
    delete val;

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    val = v->CFG_Fetch_Raw(section, "bitmap");

    if(!val) {
        update_ = -1;
        return;
    }

    std::vector<SpecialChar> ch;

    for(int i = 0; i < v->GetLCD()->YRES; i++) {
        std::stringstream strm;
        std::string str;
        strm << "row" << i + 1;
        strm >> str;
        Json::Value *row = v->CFG_Fetch_Raw(val, str);
        if(!row) break;
        std::vector<std::string> line = Split(row->asString(), '|');
        if( line[line.size() - 1] == "" )
            line.erase(line.end());
        for(unsigned int j = 0; j < line.size(); j++) {
            std::string segment = line[j];
            if( j >= ch.size() )
                ch.push_back(SpecialChar(v->GetLCD()->YRES));
            for(unsigned int c = 0; c < segment.size(); c++ ) {
                if( segment[c] == '*' )
                    ch[j][i] ^= 1<<c;
            }
        }
        delete row;    
    }
    delete val;

    data_ = ch;

    index_ = 0;

    started_ = false;

    ch_ = -1;

    bitmap_ = new SpecialChar(data_[0].Size());

    timer_ = v->timers_->AddTimer(icon_update, this, update_, true);
}


WidgetIcon::~WidgetIcon() {
    Stop();
    delete visible_;
    delete bitmap_;
}

void WidgetIcon::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / xres) + 0.5);
    Update();
}

void WidgetIcon::SetupChars() {
    std::map<std::string, Widget *> widgets = visitor_->GetWidgets();
    for(std::map<std::string, Widget *>::iterator it = 
        widgets.begin();
        it != widgets.end(); it++) {
        if(it->second->GetWidgetBase() == widget_base_ &&
            ((WidgetIcon *)it->second)->GetCh() >= 0) {
                ch_ = ((WidgetIcon *)it->second)->GetCh();
                return;
        }
    }
    LCDText *lcd = (LCDText *)visitor_->GetLCD();
    if((int)lcd->special_chars.size() >= lcd->CHARS) {
        LCDError("Can not allot char for widget: %s", name_.c_str());
        update_ = -1;
        return;
    }
    lcd->special_chars.push_back(SpecialChar(lcd->YRES));
    ch_ = lcd->special_chars.size() - 1;
}

void WidgetIcon::Update() {

    *bitmap_ = data_[index_++];

    if(index_ >= (int)data_.size())
        index_ = 0;
    
    Draw(this);
}

void WidgetIcon::Start() {
    if(update_ < 0) 
        return;
    started_ = false;
    if(type_ == LCD_TEXT)
    {
        std::map<std::string, Widget *> widgets;
        widgets = visitor_->GetWidgets();
        for(std::map<std::string, Widget *>::iterator it = widgets.begin();
            it != widgets.end() && lcd_type_ == LCD_TEXT; it++) {
            if(it->second->GetWidgetBase() == widget_base_ && 
                it->second->GetStarted()) {
                started_ = true;
                break;
            }
        }
    }
    if(!started_) {
        timer_->Start();
        started_ = true;
    } 
    Update();
}

void WidgetIcon::Stop() {
    timer_->Stop();
    started_ = false;
    ch_ = -1;
}

