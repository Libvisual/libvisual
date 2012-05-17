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

#ifndef __PLUGIN_LCD_H__
#define __PLUGIN_LCD_H__

#include <string>
#include <vector>

#include "SpecialChar.h"
using namespace std;

namespace LCD {

class LCDCore;

class PluginLCD {

    LCDCore *visitor_;
    int type_;

    void Setup();

    public:
    PluginLCD(LCDCore *visitor);
    ~PluginLCD();

    public :
    void Transition(int i);
    void SelectLayout(string layout);
    int RemoveWidget(string widget);
    string AddWidget(string layout, int row, int col, 
        int layer, string object);
    int MoveWidget(string widget, int rows, int cols);
    int ResizeLCD(int rows, int cols);
    void SendData(int row, int col, int layer, 
        string data);
    void SetSpecialChar(int ch, SpecialChar matrix);
    void AddSpecialChar(SpecialChar matrix);
    void Clear();
    void ClearChars();
    string GetCurrentLayout();
    int GetRows();
    int GetCols();
    int GetXres();
    int GetYres();
    string GetType();

    void TickUpdate();
    void SetTimeout(int val);

    void _TickUpdate();
    void _KeypadEvent(const int);
};

}; // End namespace

#endif
