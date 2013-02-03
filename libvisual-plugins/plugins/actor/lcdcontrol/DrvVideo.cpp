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

#include "DrvVideo.h"

using namespace LCD;

// LCDGraphic RealBlit
void DrvVideoBlit(LCDGraphic *lcd, const int row, const int col,
    const int height, const int width) {
    ((DrvVideo *)lcd)->DrvBlit(row, col, height, width);
}


// Constructor
DrvVideo::DrvVideo(std::string name, LCDControl *v,
    Json::Value *config, int layers, VisEventQueue *eventqueue) :
    LCDCore(v, name, config, LCD_GRAPHIC, eventqueue, (LCDGraphic *)this),
    LCDGraphic((LCDCore *)this) {
    LCDError("DrvVideo");

    GraphicRealBlit = DrvVideoBlit;

    Json::Value *val = CFG_Fetch_Raw(config, name + ".cols", new Json::Value(SCREEN_W));
    cols_ = val->asInt();
    delete val;

    val = CFG_Fetch_Raw(config, name + ".rows", new Json::Value(SCREEN_H));
    rows_ = val->asInt();
    delete val;

    val = CFG_Fetch_Raw(config, name + ".update", new Json::Value(30));
    update_ = val->asInt();
    delete val;

    GraphicInit(rows_, cols_, 8, 8, layers);

    video_ = visual_video_new_with_buffer(cols_, rows_, VISUAL_VIDEO_DEPTH_32BIT);
}

// Destructor
DrvVideo::~DrvVideo() {
    visual_video_unref(video_);
}

// Initialize device and libusb
void DrvVideo::SetupDevice() {
    if(update_ < 0)
        return;
}

// Deinit driver
void DrvVideo::TakeDown() {
    Disconnect();
}

// Configuration setup
void DrvVideo::CFGSetup() {
    LCDCore::CFGSetup();
}

// Connect -- generic method called from main code
void DrvVideo::Connect() {
    if(update_ < 0)
        return;
    LCDError("DrvVideo::Connect()");
    connected_ = true;
    GraphicClear();
    GraphicStart();
}

// Disconnect -- deinit
void DrvVideo::Disconnect() {
    connected_ = false;
}

void DrvVideo::DrvUpdateImg() {
    //unsigned int size = visual_video_get_size(video_);
    //uint8_t *data = (uint8_t *)visual_video_get_pixels(video_);

    
}

// Driver-side blit method
void DrvVideo::DrvBlit(const int row, const int col, 
    const int height, const int width) {
/*
    uint32_t *pixels = (uint32_t *)visual_video_get_pixels(video_);

    for(int r = row; r < row + height; r++) {
        for(int c = col; c < col + width; c++) {
            //We don't need this because LCDGraphic has a VisVideo* getter now.
            pixels[r * cols_ + c] = GraphicRGB(r, c).ToInt();
        }
    }
*/  
}

// Clear the LCD
void DrvVideo::DrvClear() {
    
}

