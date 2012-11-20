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

#ifndef __WIDGET__
#define __WIDGET__

#include <iostream>
#include <json/json.h>
#include <string>

#include "RGBA.h"

namespace LCD {

typedef enum { DIR_EAST = 1, DIR_WEST = 2, DIR_NORTH = 4, DIR_SOUTH = 8 } DIRECTION;

class LCDText;
class LCDGraphic;

class LCDCore;

class Widget {
    //Q_OBJECT
 
    protected:
    LCDCore *visitor_;
    Json::Value *section_;
    std::string name_;
    bool started_;
    int type_;
    int lcd_type_;
    int layer_;
    int col_;
    int row_;
    int cols_;
    int rows_;
    int x2_;
    int y2_;
    RGBA fg_color_;
    RGBA bg_color_;
    bool fg_valid_;
    bool bg_valid_;
    std::string widget_base_;
    std::string layout_base_;
    
    public:
    Widget(LCDCore *visitor, std::string name, Json::Value *config, 
        int row, int col, 
        int layer, int type);
    virtual ~Widget();
    virtual void Start();
    virtual void Stop();
    virtual void SetupChars();
    LCDCore *GetVisitor() { return visitor_; };
    bool GetStarted() const { return started_; }
    int GetRow() const { return row_; }
    int GetCol() const { return col_; }
    int GetCols() { return cols_; }
    int GetRows() { return rows_; }
    int GetType() const { return type_; }
    std::string GetName() const { return name_; }
    std::string GetWidgetBase() const { return widget_base_; }
    std::string GetLayoutBase() const { return layout_base_; }
    int GetLayer() const { return layer_; }
    RGBA GetFGColor() const { return fg_color_; }
    RGBA GetBGColor() const { return bg_color_; }
    bool GetFGValid() const { return fg_valid_; }
    bool GetBGValid() const { return bg_valid_; }
    int WidgetColor(Json::Value *section, std::string name, RGBA *color); 

    virtual void Update() = 0;
    virtual void TextScroll() = 0;
};

}; // End namespace

#define WIDGET_TYPE_RC (1<<0)
#define WIDGET_TYPE_XY (1<<1)
#define WIDGET_TYPE_GPO (1<<2)
#define WIDGET_TYPE_TIMER (1<<3)
#define WIDGET_TYPE_KEYPAD (1<<4)
#define WIDGET_TYPE_TEXT (1<<5)
#define WIDGET_TYPE_BAR (1<<6)
#define WIDGET_TYPE_ICON (1<<7)
#define WIDGET_TYPE_HISTOGRAM (1<<8)
#define WIDGET_TYPE_BIGNUMS (1<<9)
#define WIDGET_TYPE_GIF (1<<10)
#define WIDGET_TYPE_VISUALIZATION (1<<11)
#define WIDGET_TYPE_NIFTY (1<<12)
#define WIDGET_TYPE_SPECIAL (1<<13)

#endif
