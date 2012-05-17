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
#include <Magick++.h>

#include "WidgetGif.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDCore.h"

using namespace LCD;

extern void TextGifDraw(WidgetGif *w);
extern void GraphicGifDraw(WidgetGif *w);

WidgetGif::WidgetGif(LCDCore *v, std::string name, Json::Value *config,
        int row, int col, int layer) : Widget(v, name, config, row, col, layer, 
        WIDGET_TYPE_GIF | WIDGET_TYPE_SPECIAL | WIDGET_TYPE_RC) {

    if(lcd_type_ == LCD_TEXT)
        Draw = TextGifDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicGifDraw;
    else
        Draw = NULL;

    update_ = NULL;
    visible_ = NULL;
    file_ = NULL;
    bitmap_ = NULL;

    started_ = false;
    has_chars_ = false;

    Json::Value *val;
    val = v->CFG_Fetch(section_, "start", new Json::Value(0));
    int start = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "end", new Json::Value(-1));
    int end = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "xpoint", new Json::Value(0));
    xpoint_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "ypoint", new Json::Value(0));
    ypoint_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "width", new Json::Value(-1));
    cols_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "height", new Json::Value(-1));
    rows_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "layer", new Json::Value(0));
    layer_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "showbackground", new Json::Value(0));
    showbackground_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section_, "inverted", new Json::Value(0));
    inverted_ = val->asInt();
    delete val;

    update_ = new Property(v, section_, "update", new Json::Value("500"));
    visible_ = new Property(v, section_, "visible", new Json::Value("1"));

    file_ = v->CFG_Fetch_Raw(section_, "file", new Json::Value(""));

    if( strcmp(file_->asCString(), "") == 0 ) {
        LCDError("You must specify a GIF file: %s", name_.c_str());
        update_->SetValue(Json::Value(-1));
        return;
    }

    Magick::readImages(&image_, file_->asString());
    Magick::coalesceImages(&image_, image_.begin(), image_.end());

    if(inverted_) {
        for_each(image_.begin(), image_.end(), Magick::negateImage());
    }

    if(image_.size() == 0) {
        LCDError("Gif: Image read failed <%s>", file_->asCString());
        update_ = new Property(v, section_, "", new Json::Value("-1"));
        return;
    }

    background_ = image_.begin()->backgroundColor();

    has_transparent_ = false;

    if(image_.begin()->matte()) {
        has_transparent_ = true;
        transparent_ = image_.begin()->matteColor();
    }

    framePtr_ = image_.begin();

    for(int i = 0; i < start && framePtr_ != image_.end(); i++)
        framePtr_++;

    start_ = framePtr_;

    if( end < 0 )
        end_ = image_.end();
    else {
        for(int i = start; i < end && framePtr_ != image_.end(); i++)
            framePtr_++;
        end_ = framePtr_;
    }

    framePtr_ = start_;


    if( rows_ == -1 ) {
        rows_ = image_.begin()->rows();
    }

    if( cols_ == -1 ) {
        cols_ = image_.begin()->columns();
    }

    for(int row = 0; row < rows_; row++) {
        for(int col = 0; col < cols_; col++) {
            Magick::Color pixel = (++image_.begin())->pixelColor(col, row);
        }
    } 

    bitmap_ = new RGBA[cols_ * rows_];

    cols_ = cols_ / v->GetLCD()->XRES;
    rows_ = rows_ / v->GetLCD()->YRES;

#define min(a, b) (a<b?a:b)
#define max(a, b) (a>b?a:b)

    if(col_ + cols_ > v->GetLCD()->LCOLS) {
        int diff = (col_ + cols_) - v->GetLCD()->LCOLS;
        if((cols_-=diff) < 0) cols_ = 0;
    }
    if(row_ + rows_ > v->GetLCD()->LROWS) {
        int diff = (row_ + rows_) - v->GetLCD()->LROWS;
        if((rows_-=diff) < 0) rows_ = 0;
    }

    x2_ = col_ + cols_;
    y2_ = row_ + rows_;

/*
    timer_ = new QTimer();
    timer_->setSingleShot(false);
    timer_->setInterval(update_->P2INT());
    QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(Update()));

    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
        this, SLOT(Resize(int, int, int, int)));
*/
}


WidgetGif::~WidgetGif() {
    Stop();
    if(update_) delete update_;
    if(visible_) delete visible_;
    if(bitmap_) delete []bitmap_;
    if(timer_) delete timer_;
    if(file_) delete file_;
}

void WidgetGif::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    row_ = round(rows * r / yres);
    col_ = round(cols * c / xres);
    Update();
}

void WidgetGif::Update() {
    if(update_->P2INT() < 0)
        return;

    int xres = visitor_->GetLCD()->XRES;
    int yres = visitor_->GetLCD()->YRES;

    int cols = cols_ * xres;
    int rows = rows_ * yres;

    memset(bitmap_, 0, cols * rows * sizeof(RGBA));

    visible_->Eval();

    for(int row = ypoint_; row + ypoint_ < rows; 
        row++ ) {
        for(int col = xpoint_; col + xpoint_ < cols; 
            col++ ) {
            if(visible_->P2INT()) {
                Magick::Color pixel = framePtr_->pixelColor(col, row);
//LCDError("pixel %d %d %d", pixel.redQuantum() / 256, pixel.greenQuantum() / 256, pixel.blueQuantum() / 256);
                if(showbackground_ == 0 && pixel == background_) {
                    continue;
                }
                bitmap_[row * cols + col].R = pixel.redQuantum() / 256;
                bitmap_[row * cols + col].G = pixel.greenQuantum() / 256;
                bitmap_[row * cols + col].B = pixel.blueQuantum() / 256;
                if(!has_transparent_ || pixel != transparent_)
                    bitmap_[row * cols + col].A = 255;
            }
        }
    }

    
    if(++framePtr_ == end_)
        framePtr_ = start_;

    if(Draw)
        Draw(this);
    else
        LCDError("WidgetGif: No draw method.");

}

void WidgetGif::Start() {
    if(update_->P2INT() < 0)
        return;
    std::map<std::string, Widget *> widgets;
    widgets = visitor_->GetWidgets();
    for(std::map<std::string, Widget *>::iterator it =
        widgets.begin();
        it != widgets.end(); it++) {
        if(it->second->GetWidgetBase() == widget_base_ &&
            it->second->GetLayoutBase() == layout_base_ &&
            it->second->GetStarted()) {
            started_ = true;
            break;
        }
    }
    if(!started_) {
        timer_->start();
        started_ = true;
    }
    Update();
}

void WidgetGif::Stop() {
    timer_->stop();
    started_ = false;
    has_chars_ = false;
    ch_.clear();
}

void WidgetGif::SetupChars() {
    LCDText *lcd = (LCDText *)visitor_->GetLCD();
    ch_.resize(rows_ * cols_);
    std::map<std::string, Widget *> widgets1;
    std::map<std::string, Widget *> widgets2;
    widgets1 = visitor_->GetWidgets();
    widgets2 = visitor_->GetWidgets();
    for(std::map<std::string, Widget *>::iterator ii=widgets1.begin(); 
        ii != widgets1.end(); ii++) {
        if(ii->second->GetWidgetBase() == widget_base_ &&
            ((WidgetGif *)ii->second)->HasChars()) {
            for(int i = 0; i < rows_ * cols_; i++ ) {
                if(i >= (int)lcd->CHARS) {
                    LCDError("1) GIF too large: %s, %d", name_.c_str(), 
                        lcd->CHARS);
                    if(update_) delete update_;
                    update_ = new Property(visitor_, section_, "", 
                        new Json::Value("-1"));
                    return;
                }
                ch_[i] = ((WidgetGif *)widgets2[ii->first])->GetChars()[i];
            }
            return;
        }
    }
    for(int i = 0; i < rows_ * cols_; i++) {
       if( (int)lcd->special_chars.size() >= lcd->CHARS ) {
           LCDError("2) GIF too large: %s", name_.c_str());
           if( update_) delete update_;
           update_ = new Property(visitor_, section_, "", 
               new Json::Value("-1"));
           return;
       }
       lcd->special_chars.push_back(SpecialChar(lcd->YRES));
       ch_[i] = lcd->special_chars.size() - 1; 
    }
    has_chars_ = true;
}
