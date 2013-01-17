/* $Id$
 * $URL$
 *
 * Copyright (C) 1999, 2000 Michael Reinelt <michael@reinelt.co.at>
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

#ifndef __LCD_TEXT__
#define __LCD_TEXT__

#include <vector>
#include <string>
#include "LCDBase.h"
#include "SpecialChar.h"

namespace LCD {

class WidgetText;
class WidgetIcon;
class WidgetBar;
class WidgetHistogram;
class WidgetBignums;
class WidgetFPS;

class LCDCore;

class LCDGraphic;

class LCDText: public LCDBase {
    std::string transition_layout_;
    public:
    unsigned char **LayoutFB;
    unsigned char **TransitionFB;
    unsigned char *DisplayFB;
    void (*TextRealBlit) (LCDText *obj, int row, int col,
        unsigned char *data, int len);
    void (*TextRealDefChar) (LCDText *obj, const int ascii, 
        SpecialChar matrix);
    std::vector<SpecialChar> special_chars;
    int GOTO_COST;
    int CHARS;
    int CHAR0;
    int transition_tick_;
    bool transitioning_;
    double tentacle_move_;
    LCDCore *visitor_;
    LCDText(LCDCore *visitor);
    virtual ~LCDText();
    void TextInit(int rows, int cols, int yres, 
        int xres, int _goto, int chars, int char0,
        int layers);
    void TextBlit(int row, int col, int  height, 
        int width);
    int ResizeLCD(int rows, int cols);
    void CleanBuffer(unsigned char **buf);
    void TextClear();
    void TextClearChars();
    bool TextAddChar(SpecialChar ch);
    void TextGreet();
    void Transition();
    void TransitionLeftRight();
    void TransitionUpDown();
    void TransitionTentacle();
    void TransitionCheckerBoard();

    bool IsTransitioning() { return transitioning_; }
    void SignalTransitionStart(std::string layout) { 
        CleanBuffer(LayoutFB);
        TextBlit(0, 0, LROWS, LCOLS);
        transitioning_ = true; transition_layout_ = layout;
    }
    void SignalTransitionEnd() { }
    std::string GetTransitionLayout() { return transition_layout_; }

    // Slots
    void TextSetSpecialChars();
    void LayoutChangeBefore();
    void LayoutChangeAfter();
    void TextSpecialCharChanged(int ch);
    void ChangeLayout() {}
    void LayoutTransition() {}
    void TransitionFinished() {}
    void KeypadEvent(int k) {}

};

void TextDraw(WidgetText *w);

void TextBarDraw(WidgetBar *w);

void TextHistogramDraw(WidgetHistogram *w);

void TextIconDraw(WidgetIcon *w);

void TextBignumsDraw(WidgetBignums *w);

void TextFPSDraw(WidgetFPS *w);

}; // End namespace

#endif
