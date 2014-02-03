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

#ifndef __LCD_GRAPHIC_H__
#define __LCD_GRAPHIC_H__

#include <string>
#include <vector>
#include <sys/time.h>
#include <libvisual/libvisual.h>

#include "RGBA.h"
#include "LCDBase.h"
#include "LCDTimer.h"

namespace LCD {

class WidgetText;
class WidgetIcon;
class WidgetBar;
class WidgetHistogram;
class WidgetBignums;
class WidgetGif;
class WidgetVisualization;
class WidgetFPS;

class SpecialChar;

class LCDCore;

/*
class LCDGraphicInterface {
    public:
    virtual ~LCDGraphicInterface() {}
    virtual void ResizeBefore(int rows, int cols) = 0;
    virtual void ResizeAfter() = 0;
    virtual void LayoutChangeBefore() = 0;
    virtual void LayoutChangeAfter() = 0;
};
*/
/*
class LCDGraphicWrapper {
    LCDGraphicInterface *wrappedObject_;
    public:
    LCDGraphicWrapper(LCDGraphicInterface *visitor) { wrappedObject_ = visitor; }

    void ResizeBefore(int rows, int cols) { wrappedObject_->ResizeBefore(rows, cols); }
    void ResizeAfter() { wrappedObject_->ResizeAfter(); }
    void LayoutChangeBefore() { wrappedObject_->LayoutChangeBefore(); }
    void LayoutChangeAfter() { wrappedObject_->LayoutChangeAfter(); }

};
*/

class LCDGraphic : public LCDBase {

    struct _GraphicWindow {
        int R;
        int C;
        int H;
        int W;
    } update_window_;

    float tentacle_move_;

    //LCDGraphicUpdateThread *update_thread_;
    //LCDGraphicWrapper *graphic_wrapper_;
    int refresh_rate_;

    bool fill_;

    int transition_tick_;
    bool transitioning_;
    std::string transition_layout_;

    bool is_resizing_;

    VisVideo *video_;

    LCDTimer *timer_;


    void Transition();
    void TransitionLeftRight();
    void TransitionUpDown();
    void TransitionTentacle();
    void VlineFromBuffer(RGBA *dest, RGBA *src, int x, int y1, int y2);
    void SaneCoords(int *x, int *y1, int *y2);
    void TransitionAlphaBlend();
    void AlphaBlendBuffer(RGBA **dest, RGBA **src1, RGBA **src2, float alpha);
    int ResizeLCD(int rows, int cols);
    void ResizeBefore(int rows, int cols);
    void ResizeAfter();
    void LayoutChangeBefore();
    void LayoutChangeAfter();

    protected:
    LCDCore *visitor_;

    public:
    RGBA FG_COL;
    RGBA BG_COL;
    RGBA BL_COL;
    RGBA NO_COL;


    bool INVERTED;

    RGBA **DisplayFB;
    RGBA **LayoutFB;
    RGBA **TransitionFB;

    VisVideo **VideoFB;

    //std::vector<std::vector<RGBA>> DisplayFB;
    //std::vector<std::vector<RGBA>> LayoutFB;
    //std::vector<std::vector<RGBA>> TransitionFB;

    LCDGraphic(LCDCore *visitor);
    virtual ~LCDGraphic();
    void (*GraphicRealBlit) (LCDGraphic *lcd, const int row, const int col, 
        const int height, const int width);
    void GraphicStart();
    void GraphicStop();
    LCDCore *GetVisitor() { return visitor_; }
    void GraphicUpdate(int row, int col, int height, int width);
    void GraphicDraw();
    void GraphicInit(const int rows, const int cols,
        const int yres, const int xres, const int layers, bool clear_on_layout_change = true);
    void GraphicClear();
    void GraphicFill();
    void GraphicWindow(int, int, int, int *, int *);
    void GraphicBlit(int, int, int, int);
    void GraphicBlitAll();
    RGBA GraphicBlend(int, int, RGBA **buffer = (RGBA **)NULL);
    static RGBA BlendRGBA(RGBA col1, RGBA col2, uint8_t alpha);

    RGBA GraphicRGB(const int row, const int col);
    unsigned char GraphicGray(const int row, const int col);
    unsigned char GraphicBlack(const int row, const int col);
    VisVideo *GetVideo();
    VisVideo *GetVideo(int layer);
    void GraphicRender(int layer, int row, int col, RGBA fg, RGBA bg, const char *txt, bool bold, int offset, std::string layout);
    bool IsTransitioning() { return transitioning_; }
    void SignalTransitionStart(std::string layout) { transitioning_ = true; transition_layout_ = layout;}
    void SignalTransitionEnd() { 
        for(int l = 0; l < LAYERS; l++)
            for(int n = 0; n < LROWS * LCOLS; n++)
                TransitionFB[l][n] = NO_COL;
        if(fill_) {
            for(int n = 0; n < LROWS * LCOLS; n++)
                ;//DisplayFB[0][n] = BG_COL;
        }
    }
    std::string GetTransitionLayout() { return transition_layout_; }

   void DrawSpecialChar(const int row, const int col,
        const int height, const int width, const int layer,
        const RGBA fg, const RGBA bg, SpecialChar ch, std::string layout);

};

/*
class LCDGraphicUpdateThread {
    LCDGraphic *visitor_;

    protected:
    void run() { visitor_->GraphicDraw(); }

    public:
    LCDGraphicUpdateThread(LCDGraphic *v) { visitor_ = v; }
};
*/

void GraphicDraw(WidgetText *w);

void GraphicIconDraw(WidgetIcon *w);

void GraphicBarDraw(WidgetBar *w);

void GraphicHistogramDraw(WidgetHistogram *w);

void GraphicBignumsDraw(WidgetBignums *w);

void GraphicFPSDraw(WidgetFPS *w);

void GraphicGifDraw(WidgetGif *w);

}; // End namespace

#endif
