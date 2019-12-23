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

#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <map>
#include <libvisual/libvisual.h>

#include "LCDCore.h"
#include "LCDBase.h"
#include "LCDText.h"
#include "WidgetText.h"
#include "WidgetBar.h"
#include "WidgetHistogram.h"
#include "WidgetIcon.h"
#include "WidgetBignums.h"
#include "WidgetFPS.h"
#include "WidgetVisualization.h"
#include "Widget.h"
#include "RGBA.h"
#include "debug.h"

using namespace LCD;

LCDText::LCDText(LCDCore *visitor) {
    visitor_ = visitor;
    TextRealBlit = 0;
    TextRealDefChar = 0;
    transition_tick_ = 0;
    transitioning_ = false;
    tentacle_move_ = 0;
    LayoutFB = 0;
    TransitionFB = 0;
    DisplayFB = 0;
/*
    wrapper_ = new LCDWrapper((LCDInterface *)this, 0);
    QObject::connect(visitor->GetWrapper(), SIGNAL(_LayoutChangeBefore()), 
        wrapper_, SLOT(LayoutChangeBefore()));
    QObject::connect(visitor->GetWrapper(), SIGNAL(_LayoutChangeAfter()),
        wrapper_, SLOT(LayoutChangeAfter()));
    transitioning_ = false;
*/
}

LCDText::~LCDText() {
    //delete wrapper_;
    if(!LayoutFB) return;
    for(int l = 0; l < LAYERS; l++) {
        free(LayoutFB[l]);
        free(TransitionFB[l]);
    }
    free(LayoutFB);
    free(TransitionFB);
    free(DisplayFB);
}

void LCDText::TextInit(int rows, int cols, int yres, 
    int xres, int _goto, int chars, int char0,
    int layers) {
    int n = rows * cols;
    GOTO_COST = _goto;
    CHARS = chars;
    CHAR0 = char0;
    YRES = yres;
    XRES = xres;
    LROWS = DROWS = rows;
    LCOLS = DCOLS = cols;
    LAYERS = layers;
    LayoutFB = (unsigned char **)malloc(sizeof(char) * cols * rows * layers);
    TransitionFB = (unsigned char **)malloc(sizeof(char) * cols * rows * layers);
    for(int l = 0; l < layers; l++) {
        LayoutFB[l] = (unsigned char *)malloc(sizeof(char) * cols * rows);//new unsigned char[n];
        memset(LayoutFB[l], ' ', n);
        TransitionFB[l] = (unsigned char *)malloc(sizeof(char) * cols * rows);//new unsigned char[n];
        memset(TransitionFB[l], ' ', n);
    }
    DisplayFB = (unsigned char *)malloc(sizeof(char) * cols * rows);;
    memset(DisplayFB, ' ', n);
}

int LCDText::ResizeLCD(int rows, int cols) {
    for(int l = 0; l < LAYERS; l++) {
        free(LayoutFB[l]);
        free(TransitionFB[l]);
    }
    free(LayoutFB);
    free(TransitionFB);
    free(DisplayFB);
    TextInit(rows, cols, YRES, XRES, GOTO_COST, CHARS, CHAR0, LAYERS);
    return -1;
}

void LCDText::TextSetSpecialChars() {
    for(int i = 0; i < (int)special_chars.size(); i++ ) {
        if (i > CHARS) {
            LCDError("Too many chars to process. Expected %d, got %" VISUAL_SIZE_T_FORMAT ".", CHARS, 
            special_chars.size());
            break;
        }
        TextSpecialCharChanged(i);
    } 
    //emit static_cast<LCDEvents *>(wrapper_)->_TextSpecialCharsSet();
}

void LCDText::TextSpecialCharChanged(int ch) {
    if( ch < 0 || ch >= CHARS || ch >= (int)special_chars.size() )
        return;
    if(TextRealDefChar)
        TextRealDefChar(this, ch, special_chars[ch]);
    else
        LCDError("LCDText: No TextRealDefChar");
}

inline char GetCell(unsigned char **buffer, int pos, int layers) {
	char cell = ' ';
	for(int l = layers - 1; l>=0; l--) {
		if (buffer[l][pos] != ' ')
			cell = buffer[l][pos];
	}
	return cell;
}

void LCDText::LayoutChangeBefore() {
    if(visitor_->ClearOnLayoutChange())
        TextClear();
    TextClearChars();
}

void LCDText::LayoutChangeAfter() {
    TextSetSpecialChars();
}

void LCDText::TextBlit(int row, int col, int height, 
    int width) {
    int lr, lc;                        /* layout  row/col */
    int dr, dc;                        /* display row/col */
    int p1, p2;                        /* start/end positon of changed area */
    int eq;                        /* counter for equal contents */
    char fb[LROWS * LCOLS];

    memset(fb, ' ', LROWS * LCOLS);
    for(int r = row; r < LROWS && r < row + height; r++) {
        for(int c = col; c < LCOLS && c < col + width; c++) {
            int n = r * LCOLS + c;
            fb[n] = GetCell(LayoutFB, n, LAYERS);
        }
    }

    /* loop over layout rows */
    for (lr = row; lr < LROWS && lr < row + height; lr++) {
        /* transform layout to display row */
        dr = lr;
        /* sanity check */
        if (dr < 0 || dr >= DROWS)
            continue;
        /* loop over layout cols */
        for (lc = col; lc < LCOLS && lc < col + width; lc++) {
            /* transform layout to display column */
            dc = lc;
            /* sanity check */
            if (dc < 0 || dc >= DCOLS)
                continue;
            /* find start of difference */
            if (DisplayFB[dr * DCOLS + dc] == fb[lr * LCOLS + lc])
                continue;
            /* find end of difference */
            for (p1 = dc, p2 = p1, eq = 0, lc++; lc < LCOLS && lc < col + width; lc++) {
                /* transform layout to display column */
                dc = lc;
                /* sanity check */
                if (dc < 0 || dc >= DCOLS)
                    continue;
                if (DisplayFB[dr * DCOLS + dc] == fb[lr * LCOLS + lc]) {
                    if (++eq > GOTO_COST)
                        break;
                } else {
                    p2 = dc;
                    eq = 0;
                }
            }
            /* send to display */
            memcpy(DisplayFB + dr * DCOLS + p1, fb + lr * LCOLS + p1, p2 - p1 + 1);
            if (TextRealBlit)
                TextRealBlit((LCDText *)visitor_->GetLCD(), dr, p1, 
                    DisplayFB + dr * DCOLS + p1, p2 - p1 + 1);
        }
    }
}

void LCDText::CleanBuffer(unsigned char **buf) {
    for(int l = 0; special_chars.size() > 0 && l < LAYERS; l++) {
        for(int n = 0; n < LROWS * LCOLS; n++) {
            if(buf[l][n] >= CHAR0 && buf[l][n] < CHAR0 + CHARS)
                buf[l][n] = ' ';
        }
    }
}

void LCDText::TextClear() {
    for(int l = 0; l < LAYERS; l++)
        memset(LayoutFB[l], ' ', LROWS * LCOLS);
    TextBlit(0, 0, LROWS, LCOLS);
}

void LCDText::TextClearChars() {
    special_chars.clear();
}

bool LCDText::TextAddChar(SpecialChar ch) {
    if((int)special_chars.size() >= CHARS)
        return false;
    special_chars.push_back(ch);
    return true;
}

void LCDText::TextGreet() {

}

void LCD::TextDraw(WidgetText *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    std::string txt = widget->GetBuffer();
    int length = widget->GetCols();
    int layer = widget->GetLayer();

    unsigned char *fb;
    if(lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    memcpy(fb + row * lcdText->LCOLS + col, txt.c_str(), length);

    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, 1, length);
}

void LCD::TextBarDraw(WidgetBar *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int length = widget->GetCols();
    int row = widget->GetRow();
    int col = widget->GetCol();
    int layer = widget->GetLayer();
    double end;

    int val1 = round(widget->GetVal1() * length );
    int val2 = round(widget->GetVal2() * length );

    if(val1 < 0) val1 = 0;
    if(val2 < 0) val2 = 0;

    int n = row * lcdText->LCOLS + col;

    unsigned char *fb;
    if(lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    for(int x = n; x < n + length; x++)
        fb[x] = ' ';

    if( widget->GetDirection() == DIR_EAST ) {
        if( val1 > val2)
            end = n + val1;
        else
            end = n + val2;

        for(int i = n; i < end; i++ ) {
            if( val1 == val2 || i < n + std::min(val1, val2))
                fb[i] = widget->GetChars()[0] + lcdText->CHAR0;
            else if( i < n + val2 ) 
                fb[i] = widget->GetChars()[1] + lcdText->CHAR0;
            else
                fb[i] = widget->GetChars()[2] + lcdText->CHAR0;
        }
        if( widget->GetStyle() == STYLE_HOLLOW and val1 == val2 ) {
            for(int i = n; i < end; i++ ) {
                if( i == n )
                    fb[i] = widget->GetChars()[1] + lcdText->CHAR0;
                else if( i < n + length )
                    fb[i] = widget->GetChars()[2] + lcdText->CHAR0;
                else if( i == n + length )
                    fb[i] = widget->GetChars()[3] + lcdText->CHAR0;
            }
        }
    } else if( widget->GetDirection() == DIR_WEST ) {
        if (val1 > val2 )
            end = n - val1;
        else
            end = n - val2;
        for(int i = n; i > end; i-- ) {
            if( val1 == val2 or i > n - std::min(val1, val2) )
                fb[i] = widget->GetChars()[0] + lcdText->CHAR0;
            else if( i > n - val2 )
                fb[i] = widget->GetChars()[1] + lcdText->CHAR0;
            else
                fb[i] = widget->GetChars()[2] + lcdText->CHAR0;
        }
        if( widget->GetStyle() == STYLE_HOLLOW && val1 == val2 ) {
            for(int i = n; i > n - length; i--) {
                if( i == n )
                    fb[i] = widget->GetChars()[3] + lcdText->CHAR0;
                else if( i > n - length - 1 )
                    fb[i] = widget->GetChars()[2] + lcdText->CHAR0;
                else if( i == n - length - 1 )
                    fb[i] = widget->GetChars()[3] + lcdText->CHAR0;

            }
        }
    }

    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, 1, length);
}

void LCD::TextHistogramDraw(WidgetHistogram *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    int length = widget->GetCols();
    int layer = widget->GetLayer();

    char *txt = new char[length];

    for(int i = 0; i < length; i++ ) {
        int val = round(widget->GetHistory()[i] * lcdText->YRES);
        if( val < 0 )
            val = 0;
        txt[i] = widget->GetCh()[val] + lcdText->CHAR0;
    }

    unsigned char *fb;
    if(lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    memcpy(fb + row * lcdText->LCOLS + col, txt, length);
    delete []txt;
    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, 1, length);
}

void LCD::TextIconDraw(WidgetIcon *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    int layer = widget->GetLayer();
    int n = row * lcdText->LCOLS + col;

    if((!lcdText->IsTransitioning() || (lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout()))) {
       lcdText->special_chars[widget->GetCh()] = widget->GetBitmap();
       lcdText->TextSpecialCharChanged(widget->GetCh());
    }

    unsigned char *fb;
    if(lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];
    fb[n] = (char) (widget->GetCh() + lcdText->CHAR0);
    if(!lcdText->IsTransitioning()) 
        lcdText->TextBlit(row, col, 1, 1);
}

void LCD::TextBignumsDraw(WidgetBignums *widget) {
    LCDText *lcd = (LCDText *)widget->GetVisitor()->GetLCD();

    if( lcd->CHARS < 8 || lcd->YRES < 8 || lcd->XRES < 6 )
        return;

    std::vector<SpecialChar> tmp;
    tmp.resize(8);
    for(int i = 0; i < 8; i++  ) {
        tmp[i] = SpecialChar(8);
    }

    for(int row = 0; row < 16; row++ ) {
        int i = row / 8;
        int rr = row % 8;
        for(int col = 0; col < 24; col++ ) {
            int j = col / 6;
            int cc = col % 6;
            int n = i * 4 + j;

            if( widget->GetFB()[row * 24 + col] == '.' ) {
                tmp[n][rr] = tmp[n][rr] ^ (1<<(5-cc));
            }
        }
    }

    for(int i = 0; i < 8; i++ ) {
        lcd->special_chars[widget->GetCh()[i]] = tmp[i];
        lcd->TextSpecialCharChanged(widget->GetCh()[i]);
    }

    int row = widget->GetRow();
    int col = widget->GetCol();
    int layer = widget->GetLayer();

    unsigned char *fb;
    if(lcd->IsTransitioning() && widget->GetLayoutBase() == lcd->GetTransitionLayout())
        fb = lcd->TransitionFB[layer];
    else
        fb = lcd->LayoutFB[layer];

    int n = 0;
    for(int i = 0; i < 2 && i + row < lcd->LROWS; i++) {
        for(int j = 0; j < 4 && j + col < lcd->LCOLS; j++ ) {
            fb[(row + i) * lcd->LCOLS + col + j] = 
                (char)(n + widget->GetCh()[0] + lcd->CHAR0);
            n++;
        }
    }
    if(!lcd->IsTransitioning()) 
        lcd->TextBlit(row, col, 2, 4);
}

void LCD::TextFPSDraw(WidgetFPS *widget) {
    LCDText *lcd = (LCDText *)widget->GetVisitor()->GetLCD();

    if( lcd->CHARS < 8 || lcd->YRES < 8 || lcd->XRES < 6 )
        return;

    std::vector<SpecialChar> tmp;
    tmp.resize(8);
    for(int i = 0; i < 8; i++  ) {
        tmp[i] = SpecialChar(8);
    }

    for(int row = 0; row < 16; row++ ) {
        int i = row / 8;
        int rr = row % 8;
        for(int col = 0; col < 24; col++ ) {
            int j = col / 6;
            int cc = col % 6;
            int n = i * 4 + j;

            if( widget->GetFB()[row * 24 + col] == '.' ) {
                tmp[n][rr] = tmp[n][rr] ^ (1<<(5-cc));
            }
        }
    }

    for(int i = 0; i < 8; i++ ) {
        lcd->special_chars[widget->GetCh()[i]] = tmp[i];
        lcd->TextSpecialCharChanged(widget->GetCh()[i]);
    }

    int row = widget->GetRow();
    int col = widget->GetCol();
    int layer = widget->GetLayer();

    unsigned char *fb;
    if(lcd->IsTransitioning() && widget->GetLayoutBase() == lcd->GetTransitionLayout())
        fb = lcd->TransitionFB[layer];
    else
        fb = lcd->LayoutFB[layer];

    int n = 0;
    for(int i = 0; i < 2 && i + row < lcd->LROWS; i++) {
        for(int j = 0; j < 4 && j + col < lcd->LCOLS; j++ ) {
            fb[(row + i) * lcd->LCOLS + col + j] = 
                (char)(n + widget->GetCh()[0] + lcd->CHAR0);
            n++;
        }
    }
    if(!lcd->IsTransitioning()) 
        lcd->TextBlit(row, col, 2, 4);
}

/*
void LCD::TextGifDraw(WidgetGif *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    std::vector<SpecialChar> tmp;
    for(int i = 0; i < widget->GetRows() * widget->GetCols(); i++ ) {
        tmp.push_back(SpecialChar(lcdText->YRES));
    }

    for(int row = 0; row < widget->GetRows() * lcdText->YRES; row++ ) {
        int i = row / lcdText->YRES;
        int rr = row % lcdText->YRES;
        for(int col = 0; col < widget->GetCols() * lcdText->XRES; col++ ) {
            int j = col / lcdText->XRES;
            int cc = col % lcdText->XRES;
            int n = i * widget->GetCols() + j;

            RGBA pxl = widget->GetBitmap()[row * widget->GetCols() * lcdText->XRES + col];
            if( !(pxl.R == widget->GetBackground().redQuantum() / 256 && 
                pxl.G == widget->GetBackground().greenQuantum() / 256 &&
                pxl.B == widget->GetBackground().blueQuantum() / 256) &&
                pxl.A == 255) {
                tmp[n][rr] = tmp[n][rr] ^ 1<<(lcdText->XRES - 1 - cc);
            }
        }
    } 

    std::vector<char> chars = widget->GetChars();

    for(int i = 0; i < (int)tmp.size() && i < (int)widget->GetChars().size(); i++ ){
        if((!lcdText->IsTransitioning() || (lcdText->IsTransitioning() && widget->GetLayoutBase() == lcdText->GetTransitionLayout()))) {
            lcdText->special_chars[widget->GetChars()[i]] = tmp[i];
            lcdText->TextSpecialCharChanged(widget->GetChars()[i]);
        }
    }

    int row = widget->GetRow();
    int col = widget->GetCol();
    int rows = widget->GetRows();
    int cols = widget->GetCols();
    int layer = widget->GetLayer();
    int n = 0;

    unsigned char *fb;
    if(lcdText->IsTransitioning() && 
        widget->GetLayoutBase() == lcdText->GetTransitionLayout())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];
    for(int i = 0; i < rows && i + row < (int)lcdText->LROWS; i++) {
        for(int j = 0; j < cols && j + col < (int)lcdText->LCOLS; j++ ) {
            fb[(row + i) * lcdText->LCOLS + col + j] = 
                (char)(n + widget->GetChars()[0] + lcdText->CHAR0);
            n += 1;
        }
    }

    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, widget->GetRows(), widget->GetCols());
}
*/

void TextVisualizationPeakDraw(WidgetVisualization *widget) {
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    int width = widget->GetCols();
    int height = widget->GetRows();
    int layer = widget->GetLayer();
    std::map<char, char> ch = widget->GetChars();
    unsigned char *fb;

    if(lcdText->IsTransitioning()) 
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    for(int y = 0; y < height && row + y < lcdText->LROWS; y++) {
        int val = (int)(((double)widget->GetHistory()[y][0] / 
            (double)SHRT_MAX) * (width / 2 - 1));
        int n = (row + y) * lcdText->LCOLS + col;

        for(int i = 0; i < (width / 2 - 1) - val; i++) {
            if(fb[n + i] == ch[0] )
                fb[n + i] = ch[1];
            else if(fb[n + i] == ch[1] )
                fb[n + i] = ch[2];
            else if(fb[n + i] == ch[2] )
                fb[n + i] = ch[3];
            else if(fb[n + i] == ch[3] )
                fb[n + i] = ch[4];
            else
                fb[n + i] = ' ';
        }

        for(int i = (width / 2 - 1) - val; i < (width / 2 - 1); i++) {
            fb[n + i] = ch[0];
        }

        fb[n + (width / 2 - 1)] = ch[5];

        val = (int)(((double)widget->GetHistory()[y][1] / 
            (double)SHRT_MAX) * (width / 2 - 1)) + width / 2;
        for(int i = width / 2; i < val; i++) {
            fb[n + i] = ch[0]; 
        }

        for(int i = val; i < width && i + col < lcdText->LCOLS; i++) {
            if(fb[n + i] == ch[0] )
                fb[n + i] = ch[1];
            else if(fb[n + i] == ch[1])
                fb[n + i] = ch[2];
            else if(fb[n + i] == ch[2])
                fb[n + i] = ch[3];
            else if(fb[n + i] == ch[3])
                fb[n + i] = ch[4];
            else
                fb[n + i] = ' ';
        }
    }

    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, height, width);
}

void TextVisualizationPCMDraw(WidgetVisualization *widget) {
/*
    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    int width = widget->GetCols();
    int height = widget->GetRows();
    int layer = widget->GetLayer();
    std::map<char, char> ch = widget->GetChars();
    unsigned char *buffer = (unsigned char *)widget->GetBuffer();
    proxy v = widget->GetProxy();

    unsigned char *fb;

    if(lcdText->IsTransitioning())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    std::vector<SpecialChar> tmp;
    for(int i = 0; i < 
        (height / lcdText->YRES) * (width / lcdText->XRES); i++ ) {
        tmp.push_back(SpecialChar(lcdText->YRES));
    }

    for(int r = 0; r < height; r++) {
        int i = r / lcdText->YRES;
        int rr = r % lcdText->YRES;
        for(int c = 0; c < width; c++) {
           int j = c / lcdText->XRES;
           int cc = c % lcdText->XRES;
           int n = i * (width / lcdText->XRES) + j;

           unsigned char pixel = buffer[r * width + c];
           unsigned char gray = (77 * v.pal->colors[pixel].r +
                                 150 * v.pal->colors[pixel].g +
                                 28 * v.pal->colors[pixel].b) / 255;
           bool black = (gray < 127);

           if(!black) {
              tmp[n][rr] = tmp[n][rr] ^ 1<<(lcdText->XRES - 1 - cc); 
           }
        }
    }

    if(ch[0])
    for(int i = 0; i < 
        (height / lcdText->YRES) * (width / lcdText->XRES); i++ ){
            lcdText->special_chars[ch[i]] = tmp[i];
            lcdText->TextSpecialCharChanged(ch[i]);
    }
    else 
    {
	LCDError("No special characters allocated.");
        return; 
    }

   int n = 0;
   for(int i = 0; i < height / lcdText->YRES && 
        i + row < lcdText->LROWS; i++) {
        for(int j = 0; j < width / lcdText->XRES && 
            j + col < lcdText->LCOLS; j++ ) {
            fb[(row + i) * lcdText->LCOLS + col + j] =
                (char)(n + widget->GetChars()[0] + lcdText->CHAR0);
            n += 1;
        }
    }

    if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, height, width);
    */
}

void TextVisualizationSpectrumDraw(WidgetVisualization *widget) {

    LCDText *lcdText = (LCDText *)widget->GetVisitor()->GetLCD();

    int row = widget->GetRow();
    int col = widget->GetCol();
    int width = widget->GetCols();
    int height = widget->GetRows();
    int layer = widget->GetLayer();
    std::map<char, char> ch = widget->GetChars();
    unsigned char *fb;

    if(lcdText->IsTransitioning())
        fb = lcdText->TransitionFB[layer];
    else
        fb = lcdText->LayoutFB[layer];

    int base = SHRT_MAX / height;

    for(int x = 0; x < width && col + x < lcdText->LCOLS; x++) {
        
        int val = widget->GetHistory()[0][x];

        for(int y = 1; y <= height && row + y - 1 < lcdText->LROWS; y++) {
            int n = (row + height - y) * lcdText->LCOLS + col + x;
            if(val > (y * base))
                fb[n] = ch[0];
            else if(val > ((y-1) * base))
                fb[n] = ch[1];
            else
                fb[n] = ' ';
        }

    }

   if(!lcdText->IsTransitioning())
        lcdText->TextBlit(row, col, height, width);
}

void TextVisualizationDraw(WidgetVisualization *widget) {
    if(widget->GetStyle() == STYLE_PEAK)
        TextVisualizationPeakDraw(widget);
    else if(widget->GetStyle() == STYLE_PCM)
        TextVisualizationPCMDraw(widget);
    else if(widget->GetStyle() == STYLE_SPECTRUM) 
        TextVisualizationSpectrumDraw(widget);
}

void LCDText::Transition() {
    switch(visitor_->GetDirection()) {
    case TRANSITION_LEFT:
    case TRANSITION_RIGHT:
    case TRANSITION_BOTH:
        TransitionLeftRight();
        break;
    case TRANSITION_UP:
    case TRANSITION_DOWN:
        TransitionUpDown();
        break;
    case TRANSITION_TENTACLE:
        TransitionTentacle();
        break;
    default:
        TransitionLeftRight();
    }
}

void LCDText::TransitionLeftRight() {
    int direction = visitor_->GetDirection();
    int col;
    unsigned char *left, *right;
    unsigned char layout[LROWS * LCOLS], transition[LROWS * LCOLS];

    // Hide last layout's special chars if new layout has special chars.
    for(int l = 0; special_chars.size() > 0 && l < LAYERS; l++) {
        for(int n = 0; n < LROWS * LCOLS; n++) {
            if(LayoutFB[l][n] >= CHAR0 && LayoutFB[l][n] < CHAR0 + CHARS)
                LayoutFB[l][n] = ' ';
            if(TransitionFB[l][n] >= CHAR0 && TransitionFB[l][n] < CHAR0 + CHARS)
                ;//TransitionFB[l][n] = ' ';
        }
    }

    memset(layout, ' ', LROWS * LCOLS);
    memset(transition, ' ', LROWS * LCOLS);

    for(int l = LAYERS - 1; l>=0; l--) {
        for(int n = 0; n < LROWS * LCOLS; n++) {
            if(LayoutFB[l][n] != ' ') 
                layout[n] = LayoutFB[l][n];
            if(TransitionFB[l][n] != ' ')
                transition[n] = TransitionFB[l][n];
        }
    }

    for(int row = 0; row < LROWS; row++) {
        int n = row * LCOLS;
        if( direction == TRANSITION_LEFT ||
            (direction == TRANSITION_BOTH && row % 2 == 0)) {
            col = LCOLS - transition_tick_ - 1;
            left = layout;
            right = transition;
        } else if( direction == TRANSITION_RIGHT || direction == TRANSITION_BOTH) {
            col = transition_tick_;
            left = transition;
            right = layout;
        } else {
            col = transition_tick_;
            left = transition;
            right = layout;
        }
        memcpy(DisplayFB + n, left + n + (LCOLS - col - 1), col);
        memcpy(DisplayFB + n + col, right + n, LCOLS - col);
        if(direction != TRANSITION_BOTH)
            DisplayFB[n + col] = ':';
        if(TextRealBlit)
            TextRealBlit((LCDText *)visitor_->GetLCD(), row, 0, DisplayFB + n, LCOLS);
    }
    if( ++transition_tick_ >= (int)LCOLS ) {
        transitioning_ = false;
        transition_tick_ = 0;
        //emit static_cast<LCDEvents *>(
        //    visitor_->GetWrapper())->_TransitionFinished();
        for(int l = 0; l < LAYERS; l++) {
            memcpy(LayoutFB[l], TransitionFB[l], LCOLS * LROWS);
            memset(TransitionFB[l], ' ', LCOLS * LROWS);
        }
        TextBlit(0, 0, LROWS, LCOLS);
    }
}

void LCDText::TransitionUpDown() {

    int direction = visitor_->GetDirection();
    int row;
    unsigned char *top, *bottom;
    unsigned char layout[LROWS * LCOLS], transition[LROWS * LCOLS];

    // Hide last layout's special chars if new layout has special chars.
    for(int l = LAYERS - 1; l>=0; l--) {
        for(int n = 0; special_chars.size() > 0 && n < LROWS * LCOLS; n++) {
            if(LayoutFB[l][n] >= CHAR0 && LayoutFB[l][n] < CHAR0 + CHARS)
                LayoutFB[l][n] = ' ';
        }
    }
    memset(layout, ' ', LROWS * LCOLS);
    memset(transition, ' ', LROWS * LCOLS);
 
    for(int n = 0; n < LROWS * LCOLS; n++) {
        layout[n] = GetCell(LayoutFB, n, LAYERS);
    	transition[n] = GetCell(TransitionFB, n, LAYERS);
    }

    if(direction == TRANSITION_UP) {
        top = layout;
        bottom = transition;
        row = LROWS - transition_tick_;
    } else {
        top = transition;
        bottom = layout;
        row = transition_tick_;
    }

    for(int r = 0; r < LROWS; r++) {
        int n = r * LCOLS;
            memset(DisplayFB + n, ' ', LCOLS);
            if(r < row) {
                memcpy(DisplayFB + n, top + n, LCOLS);
            } else {
                memcpy(DisplayFB + n, bottom + n, LCOLS);
            }
        if(TextRealBlit)
            TextRealBlit((LCDText *)visitor_->GetLCD(), r, 0, DisplayFB + n, LCOLS);
    }
    if( ++transition_tick_ >= (int)LROWS ) {
        transitioning_ = false;
        transition_tick_ = 0;
        //emit static_cast<LCDEvents *>(
        //    visitor_->GetWrapper())->_TransitionFinished();
        memset(DisplayFB, ' ', LCOLS * LROWS);
        for(int l = 0; l < LAYERS; l++) {
            memcpy(LayoutFB[l], TransitionFB[l], LCOLS * LROWS);
            memset(TransitionFB[l], ' ', LCOLS * LROWS);
        }
        TextBlit(0, 0, LROWS, LCOLS);
    }
}

void SaneCoords(LCDText *lcd, int *x, int *y1, int *y2) {
    if(*x >= lcd->LCOLS)
        *x = lcd->LCOLS - 1;
    else if(*x < 0)
        *x = 0;

    if(*y1 >= lcd->LROWS)
        *y1 = lcd->LROWS - 1;
    else if(*y1 < 0)
        *y1 = 0;

    if(*y2 >= lcd->LROWS)
        *y2 = lcd->LROWS - 1;
    else if(*y2 < 0)
        *y2 = 0;

/*
    *x = (int)((*x)+0.5);
    *y1 = (int)((*y1) + 0.5);
    *y2 = (int)((*y2) + 0.5);
*/
}

void VLineFromBuffer(LCDText *lcd, unsigned char *dst, unsigned char *src, int _x, int _y1, int _y2) {
    int x = _x;
    int y1 = _y1;
    int y2 = _y2;

    SaneCoords(lcd, &x, &y1, &y2);

    for(int i = y1; i < y2; i++) {
        dst[i * lcd->LCOLS + x] = src[i * lcd->LCOLS + x];
    }
}

void LCDText::TransitionTentacle() {

    int height1, height2;
    double add1, add2;
    double sinrate = tentacle_move_;
    double multiplier = 0;
    double multiadd = 1.0 / LCOLS;
    double rate = (LCOLS - transition_tick_) / (double)LCOLS;
    unsigned char layout[LCOLS * LROWS];

    memset(DisplayFB, (int)' ', LCOLS * LROWS);
    memset(layout, (int)' ', LCOLS * LROWS);

    // Hide last layout's special chars if new layout has special chars.
    for(int l = LAYERS - 1; l>=0; l--) {
        for(int n = 0; special_chars.size() > 0 && n < LROWS * LCOLS; n++) {
            if(LayoutFB[l][n] >= CHAR0 && LayoutFB[l][n] < CHAR0 + CHARS)
                LayoutFB[l][n] = ' ';
        }
    }

    for(int l = LAYERS - 1; l>=0; l--) {
        for(int n = 0; n < LCOLS * LROWS; n++) {
            if(LayoutFB[l][n] != ' ')
                layout[n] = LayoutFB[l][n];
            if(TransitionFB[l][n] != ' ')
                DisplayFB[n] = TransitionFB[l][n];
        }
    }

    for(int i = 0; i < LCOLS; i++) {

        add1 = (LROWS / 2) - ((LROWS / 2) * (rate * 1.5));
        add2 = (LROWS / 2) + ((LROWS / 2) * (rate * 1.5));

        height1 = (sin(sinrate) * ((LROWS / 8.0) * multiplier)) + add1;
        height2 = (sin(sinrate) * ((LROWS / 8.0) * multiplier)) + add2;
        multiplier+=multiadd;

        VLineFromBuffer(this, DisplayFB, layout, i, height1, height2);
        sinrate+=0.2;
        tentacle_move_+=0.01;
    }

    for(int i = 0; i < visitor_->GetLCD()->LROWS; i++) 
        if(TextRealBlit)
            TextRealBlit((LCDText *)visitor_->GetLCD(), i, 0, DisplayFB + i * LCOLS, LCOLS);

    if( ++transition_tick_ >= (int)LCOLS ) {
        transitioning_ = false;
        transition_tick_ = 0;
        //emit static_cast<LCDEvents *>(
        //    visitor_->GetWrapper())->_TransitionFinished();
        memset(DisplayFB, (int)'*', LCOLS * LROWS);

        for(int l = 0; l < LAYERS; l++) {
            memcpy(LayoutFB[l], TransitionFB[l], LCOLS * LROWS);
            memset(TransitionFB[l], ' ', LCOLS * LROWS);
        }
        TextBlit(0, 0, LROWS, LCOLS);
    }
}

void LCDText::TransitionCheckerBoard() {

}
