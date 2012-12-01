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

#ifndef __WIDGET_GIF__
#define __WIDGET_GIF__

#include <vector>
#include <json/json.h>
#include <Magick++.h>

#include "Widget.h"
#include "Property.h"
#include "RGBA.h"

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetGif : public Widget {
    Property *update_;            /* update interval (msec) */
    Property *visible_;
    Json::Value *file_;
    int xpoint_;
    int ypoint_;
    //int *ascii_;
    Magick::Color transparent_;
    Magick::Color background_;
    bool has_transparent_;
    int showbackground_;
    int inverted_;
    bool has_chars_;
    std::vector<char> ch_;
    RGBA *bitmap_;
    std::list<Magick::Image> image_;
    std::list<Magick::Image>::iterator framePtr_;
    std::list<Magick::Image>::iterator start_;
    std::list<Magick::Image>::iterator end_;
    void (*Draw)(WidgetGif *w);

    public:
    WidgetGif(LCDCore *visitor, std::string name, Json::Value *section, 
    int row, int col, int layer);
    ~WidgetGif();
    void Start();
    void Stop();
    void SetupChars();
    bool HasChars() { return has_chars_; };
    RGBA *Bitmap() { return bitmap_; };
    std::vector<char> GetChars() { return ch_; };
    int GetXpoint() { return xpoint_; };
    int GetYpoint() { return ypoint_; };
    int GetInverted() { return inverted_; }
    Magick::Color GetBackground() { return background_; };
    Magick::Color GetTransparent() { return transparent_; };
    RGBA *GetBitmap() { return bitmap_; };
    Property *GetVisible() { return visible_; }

    // Slots
    void Update();
    void TextScroll() {}
    
    public slots:
    void Resize(int rows, int cols, int old_rows, int old_cols);
};

}; // End namespace


#endif
