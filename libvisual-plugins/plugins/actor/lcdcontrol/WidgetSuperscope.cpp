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
#include "WidgetSuperscope.h"
#include "SpecialChar.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "debug.h"

using namespace LCD;

enum scope_runnable {
    SCOPE_RUNNABLE_INIT = 0,
    SCOPE_RUNNABLE_FRAME,
    SCOPE_RUNNABLE_BEAT,
    SCOPE_RUNNABLE_POINT,
};

typedef struct {
    char *runnables[4];

    double n, b, x, y, i, v, w, h, red, green, blue, linesize, skip, drawmode, t, d;
    LVAVSPipeline *pipeline;


    int          channel_source;
    int             color_pos;
    VisPalette       pal;

    int             needs_init;

    AVSGfxColorCycler   *cycler;
} SuperScopePrivate;

int lv_superscope_render (WidgetSuperscope *priv, VisVideo *video, VisAudio *audio);

void icon_update(void *data)
{
    WidgetSuperscope *icon = (WidgetSuperscope *)data;
    icon->Update();
}

int scope_load_runnable(SuperScopePrivate *priv, ScopeRunnable runnable, char *buf)
{
/*
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
*/
    return 0;
}

int scope_run(SuperScopePrivate *priv, ScopeRunnable runnable)
{
    //avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

extern void TextSuperscopeDraw(WidgetSuperscope *w);
extern void GraphicSuperscopeDraw(WidgetSuperscope *w);

WidgetSuperscope::WidgetSuperscope(LCDCore *v, std::string n, 
    Json::Value *section, int row, int col, int layer) :
    Widget(v, n, section, row, col, layer, WIDGET_TYPE_ICON | 
    WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {


    if(lcd_type_ == LCD_TEXT)
        Draw = TextSuperscopeDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicSuperscopeDraw;
    else
        Draw = NULL;

    cols_ = 1;
    rows_ = 1;

    visible_ = new Property(v, section, "visible", new Json::Value("return 1"));
    init_ = new Property(v, section, "point", new Json::Value("return"));
    beat_ = new Property(v, section, "beat", new Json::Value("return"));
    frame_ = new Property(v, section, "frame", new Json::Value("return"));
    point_ = new Property(v, section, "point", new Json::Value("return"));

    Json::Value *val = v->CFG_Fetch_Raw(section, "speed", new Json::Value(500));
    update_ = val->asInt();
    delete val;

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    data_ = ch;

    index_ = 0;

    started_ = false;

    ch_ = -1;

    video_ = visual_video_new_with_buffer(width, height, VISUAL_VIDEO_DEPTH_32BIT);

    timer_ = v->timers_->AddTimer(icon_update, this, update_, true);
}


WidgetSuperscope::~WidgetSuperscope() {
    Stop();
    delete visible_;
    delete bitmap_;
    visual_video_unref(video_);
}

void WidgetSuperscope::Resize(int rows, int cols, int old_rows, int old_cols) {
    int yres = visitor_->GetLCD()->YRES;
    int xres = visitor_->GetLCD()->XRES;
    float r = row_ * yres / (float)old_rows;
    float c = col_ * xres / (float)old_cols;
    row_ = (int)((rows * r / yres) + 0.5);
    col_ = (int)((cols * c / xres) + 0.5);
    Update();
}

void WidgetSuperscope::SetupChars() {
    std::map<std::string, Widget *> widgets = visitor_->GetWidgets();
    for(std::map<std::string, Widget *>::iterator it = 
        widgets.begin();
        it != widgets.end(); it++) {
        if(it->second->GetWidgetBase() == widget_base_ &&
            ((WidgetSuperscope *)it->second)->GetCh() >= 0) {
                ch_ = ((WidgetSuperscope *)it->second)->GetCh();
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

void WidgetSuperscope::Update() {
    
    
    if(Draw) Draw(this);
}

void WidgetSuperscope::Start() {
    if(update_ < 0) 
        return;
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
    if(!started_) {
        timer_->Start();
        started_ = true;
    } 
    Update();
}

void WidgetSuperscope::Stop() {
    timer_->Stop();
    started_ = false;
    ch_ = -1;
}

int lv_superscope_render (SuperScopePrivate *priv, VisVideo *video, VisAudio *audio)
{
    SuperScopePrivate *priv = visual_plugin_get_private (plugin);
    LVAVSPipeline *pipeline = priv->pipeline;
    int *buf = pipeline->framebuffer;
    int isBeat;
    int i;

    VisBuffer pcm;
    float pcmbuf[BEAT_ADV_MAX];
    int size = BEAT_ADV_MAX/2;

    isBeat = pipeline->isBeat;

    if(priv->needs_init) {
        priv->needs_init = FALSE;
        scope_run(priv, SCOPE_RUNNABLE_INIT);
    }

    int a, l, lx = 0, ly = 0, x = 0, y = 0;
    int32_t current_color;
    int ws=(priv->channel_source&4)?1:0;
    int xorv=(ws*128)^128;
    uint16_t fa_data[576];

    if((priv->channel_source&3) >= 2)
    {
        for(x = 0; x < size; x++) {
            pcmbuf[x] = pipeline->audiodata[ws^1][0][x] / 2 + pipeline->audiodata[ws^1][1][x] / 2;
        }
    }
    else 
    {
        for(x = 0; x < size; x++) {
            pcmbuf[x] = pipeline->audiodata[ws^1][priv->channel_source&3][x];
        }
    }
    
    priv->color_pos++;

    if(priv->color_pos >= priv->pal.ncolors * 64) priv->color_pos = 0;

    {
        int p = priv->color_pos/64;
        int r = priv->color_pos&63;
        int c1, c2;
        int r1, r2, r3;
        c1 = visual_color_to_uint32(&priv->pal.colors[p]);
        if(p+1 < priv->pal.ncolors)
            c2=visual_color_to_uint32(&priv->pal.colors[p+1]);
