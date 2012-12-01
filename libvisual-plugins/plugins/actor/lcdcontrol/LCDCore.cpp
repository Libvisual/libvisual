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
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <iostream>

#include "LCDCore.h"
#include "LCDControl.h"
#include "CFG.h"
#include "Evaluator.h"
#include "LCDText.h"
#include "LCDGraphic.h"
//#include "PluginLCD.h"

#include "Widget.h"
#include "WidgetText.h"
#include "WidgetBar.h"
#include "WidgetIcon.h"
#include "WidgetHistogram.h"
#include "WidgetBignums.h"
#include "WidgetKey.h"
#include "WidgetTimer.h"
#include "WidgetScript.h"
#include "WidgetVisualization.h"
#include "WidgetFPS.h"

#include "debug.h"
//#include "Generator.h"

using namespace LCD;

void change_layout(void *data)
{
    LCDCore *core = (LCDCore *)data;
    core->ChangeLayout();
}

void layout_transition(void *data)
{
    LCDCore *core = (LCDCore *)data;
    core->LayoutTransition();
}

LCDCore::LCDCore(LCDControl *app, std::string name, Json::Value *config, int t, VisEventQueue *eventqueue, LCDBase *lcd) : 
    CFG(config), gen_(0, this, true) {
    app_ = app;
    type_ = t;
    eventqueue_ = eventqueue;
    lcd_ = lcd;
    name_ = name;
    layout_timeout_ = 0;
    transitions_off_ = false;
    // initialize true so property initialization doesn't trigger transitions
    is_transitioning_ = false;

    timers_ = app->GetTimers();
    timer_ = timers_->AddTimer(change_layout, this, 12000, false);
    transition_timer_ = timers_->AddTimer(layout_transition, this, 100, true);

/*
    wrapper_ = new LCDWrapper((LCDInterface *)this, (QObject *)NULL);
    timer_ = new QTimer();
    timer_->setSingleShot(true);
    transition_timer_ = new QTimer();
    transition_timer_->setSingleShot(false);
    QObject::connect(timer_, SIGNAL(timeout()), wrapper_, SLOT(ChangeLayout()));
    QObject::connect(transition_timer_, SIGNAL(timeout()), wrapper_,
        SLOT(LayoutTransition()));
    QObject::connect(wrapper_, SIGNAL(_TransitionFinished()), 
        wrapper_, SLOT(TransitionFinished()));
    QObject::connect(wrapper_, SIGNAL(_KeypadEvent(const int)),
        wrapper_, SLOT(KeypadEvent(const int)));
    gen_index_ = 0;

    pluginLCD = new PluginLCD(this);
    QScriptValue val = engine_->newObject();
    QScriptValue objVal = engine_->newQObject(val, pluginLCD);
    engine_->globalObject().setProperty("lcd", objVal);
*/

/*
    if( !CFG_Init("libvisual_config.js") ) {
        LCDError("Couldn't load configuration.");
    }
*/
}

LCDCore::~LCDCore() {
    //delete pluginLCD;
    for(std::map<std::string, Widget *>::iterator w = widgets_.begin(); 
        w != widgets_.end(); w++) {
        delete w->second;
    }
    delete timers_;
}

bool LCDCore::IsActive() {
    return app_->IsActive();
}

void LCDCore::CFGSetup() {
    int i = 1;
    std::stringstream strm;
    std::string str;

    //app_->ProcessVariables(CFG_Get_Root(), (Evaluator *)this);

    Json::Value *section = CFG_Fetch_Raw(CFG_Get_Root(), name_);
    if(!section) {
        LCDError("Device <%s> doesn't exist.", name_.c_str());
        return;
    }

    Json::Value *val = CFG_Fetch(section, "layout-timeout", new Json::Value(0));
    layout_timeout_ = val->asInt();
    delete val;

    val = CFG_Fetch(section, "transition-speed", new Json::Value(200));
    transition_speed_ = val->asInt();
    delete val;

    val = CFG_Fetch_Raw(section, "clear_on_layout_change", new Json::Value(true));
    clear_on_layout_change_ = val->asBool();
    delete val;

    val = CFG_Fetch_Raw(section, "transitions-off", new Json::Value(false));
    transitions_off_ = val->asBool();
    delete val;

    Json::Value *layout = CFG_Fetch_Raw(section, "layout0");

    while(layout) {
        layouts_.push_back(layout->asCString());
        strm << "layout" << i;
        strm >> str;
        strm.clear();
        strm.str("");
        delete layout;
        layout = CFG_Fetch_Raw(section, str);
        i++;
    }

    if(i == 1) { delete layout; return; }

    Json::Value *widget = CFG_Fetch_Raw(section, "widget0");

    i = 1;
    while(widget && widget->isString()) {
        static_widgets_.push_back(widget->asCString());
        strm << "widget" << i;
        strm >> str;
        strm.clear();
        strm.str("");
        delete widget;
        widget = CFG_Fetch_Raw(section, str);
        i++;
    }

    if(i == 1) delete widget;

    for(unsigned int i = 0; i < layouts_.size(); i++ ) {
        layout = CFG_Fetch_Raw(CFG_Get_Root(), layouts_[i]);
        if(!layout) {
            LCDError("Missing layout <%s>", layouts_[i].c_str());
            continue;
        }
        Json::Value *val = CFG_Fetch(layout, "keyless", new Json::Value(0));
        if(val->asInt()) {
            keyless_layouts_[layouts_[i]] = true;
        }
        delete val;

        LCDError("layers: %d", lcd_->LAYERS);

        for(int layer = 0; layer < lcd_->LAYERS; layer++) {
            strm << "layer" << layer + 1;
            strm >> str;
            strm.clear();
            strm.str("");
            Json::Value *cfg_layer = CFG_Fetch_Raw(layout, str);
            if(!cfg_layer)
                continue;
            for(int row = 0; row < lcd_->LROWS; row++) {
                strm << "row" << row + 1; 
                strm >> str;
                strm.clear();
                strm.str("");
                Json::Value *cfg_row = CFG_Fetch_Raw(cfg_layer, str);
                if(!cfg_row)
                    continue;
                for(int col = 0; col < lcd_->LCOLS; col++) {
                    strm << "col" << col + 1;
                    strm >> str;
                    strm.clear();
                    strm.str("");
                    Json::Value *cfg_col = CFG_Fetch_Raw(cfg_row, str);

                    if(!cfg_col || !cfg_col->isString()) {
                        if(cfg_col) delete cfg_col;
                        continue;
                    }

                    widget_template w = widget_template();
                    w.key = cfg_col->asString();
                    w.row = row;
                    w.col = col;
                    w.layer = layer;
                    widget_templates_[layouts_[i]].push_back(w);
                    delete cfg_col;
                }
                delete cfg_row;
            }
            delete cfg_layer;
        }

        for(int row = 0; row < lcd_->LROWS; row++ ) {
            strm << "row" << row + 1;
            strm >> str;
            strm.clear();
            strm.str("");
            Json::Value *cfg_row = CFG_Fetch_Raw(layout, str);
            if(!cfg_row) 
                continue;
            for(int col = 0; col < lcd_->LCOLS; col++ ) {
                strm << "col" << col + 1;
                strm >> str;
                strm.clear();
                strm.str("");
                Json::Value *cfg_col = CFG_Fetch_Raw(cfg_row, str);

                if(!cfg_col || !cfg_col->isString()) {
                    if(cfg_col) delete cfg_col;
                    continue;
                }

                widget_template w = widget_template();
                w.key = cfg_col->asString();
                w.row = row;
                w.col = col;
                w.layer = 0;
                widget_templates_[layouts_[i]].push_back(w);
                delete cfg_col;
            }
            delete cfg_row;
        }
    }

    for(unsigned int j = 0; j < static_widgets_.size(); j++ ) {
        widget_template w = widget_template();
        w.key = static_widgets_[j];
        w.row = 0;
        w.col = 0;
        widget_templates_[name_].push_back(w);
    }

    delete section;
    delete layout;
    delete widget;
}

void LCDCore::BuildLayouts() {
   std::stringstream strm;
   std::string name;
   for(std::map<std::string, std::vector<widget_template> >::iterator l = 
       widget_templates_.begin(); l != widget_templates_.end(); l++) {
       std::vector<widget_template> widgets = l->second;
       for(unsigned int i = 0; i < widgets.size(); i++ ) {
           Json::Value *widget_v = CFG_Fetch_Raw(CFG_Get_Root(), widgets[i].key);
           if(!widget_v) {
               LCDError("No widget named <%s>", widgets[i].key.c_str());
               continue;
           }
           Json::Value *type = CFG_Fetch_Raw(widget_v, "type");
           if(!type) {
               LCDError("Widget <%s> has no type!", widgets[i].key.c_str());
               delete widget_v;
               continue;
           }
    
           Widget *widget = (Widget *)NULL;
    
           std::string name;
           int j = 0;
           strm << l->first << ":" << widgets[i].key << ":" << j;
           strm >> name;
           strm.clear();
           strm.str("");
    
           while(widgets_.find(name) != widgets_.end()) {
               j++;
               strm << l->first << ":" << widgets[i].key << ":" << j;
               strm >> name;
               strm.clear();
               strm.str("");
           }

           if(type->asString() == "text") {
               widget = (Widget *) new WidgetText(this, name, widget_v, 
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "bar") {
               widget = (Widget *) new WidgetBar(this, name, widget_v,
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "icon") {
               widget = (Widget *) new WidgetIcon(this, name, widget_v,
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "histogram") {
               widget = (Widget *) new WidgetHistogram(this, name, widget_v,
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "fps") {
               widget = (Widget *) new WidgetFPS(this, name, widget_v,
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "bignums") {
               widget = (Widget *) new WidgetBignums(this, name, widget_v,
                   widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "visualization") {
               widget = (Widget *) new WidgetVisualization(this, name, widget_v,
                    widgets[i].row, widgets[i].col, widgets[i].layer);
           } else if (type->asString() == "key") {
               widget = (Widget *) new WidgetKey(this, name, widget_v);
           } else if (type->asString() == "timer") {
               widget = (Widget *) new WidgetTimer(this, name, widget_v); 
           } else if (type->asString() == "script") {
               widget = (Widget *) new WidgetScript(this, name, widget_v);
           } else {
                   LCDError("Unknown widget type: %s", type->asCString());
           }
           if(widget) {
               widgets_[name] = widget;
           } //else LCDError("No widget: %s", type->asCString());
           delete type;
       }
   }
}

void LCDCore::StartLayout(std::string key) {
    if(key == "") {
        gen_(current_layout_, last_layout_);
    } else {
        current_layout_ = key;
    }

    LCDError("StartLayout: %s", current_layout_.c_str());
    lcd_->LayoutChangeBefore();
    std::map<std::string, Widget *> widgets = widgets_;
    for(std::map<std::string,Widget *>::iterator w = widgets.begin(); 
        w != widgets.end(); w++){

	if(!w->second) LCDError("w->second is null");
    	if(w->second && (current_layout_ == w->second->GetLayoutBase() || w->second->GetLayoutBase() == name_ )) {
            if( type_ == LCD_TEXT &&
                (w->second->GetType() & WIDGET_TYPE_SPECIAL)) {

                    w->second->SetupChars();
            }
            w->second->Start();
    	}
    }

    lcd_->LayoutChangeAfter();

    Json::Value *timeout = CFG_Fetch(CFG_Get_Root(), 
        current_layout_ + ".timeout", new Json::Value(layout_timeout_));

    if(timeout->asInt() > 0)
        timer_->Start(timeout->asInt());

    delete timeout;

    Json::Value *val = CFG_Fetch_Raw(CFG_Get_Root(), current_layout_ + 
        ".clear_on_layout_change", new Json::Value(clear_on_layout_change_));

    clear_on_layout_change_ = val->asBool();

    LCDError("StartLayout end: %s", current_layout_.c_str());
    delete val;
}

void LCDCore::StopLayout(std::string layout) {
    std::map<std::string, Widget *> widgets = widgets_;
    for(std::map<std::string,Widget *>::iterator w = widgets.begin();
        w != widgets.end(); w++){
        
        if(layout != w->second->GetLayoutBase() )
            continue;

        w->second->Stop();
    }    
}

void LCDCore::ChangeLayout() {
    if(is_transitioning_) {
        timer_->Start();
        return;
    }
    LCDError("ChangeLayout");
    Json::Value *t = CFG_Fetch_Raw(CFG_Get_Root(), 
        current_layout_ + ".transition");
    if(!t or transitions_off_) {
        StopLayout(current_layout_);
        StartLayout();
    } else {
        StartTransition(t->asString());
        delete t;
        if(type_ & LCD_TEXT) {
            LCDText *text = ((LCDText *)lcd_);
            text->CleanBuffer(text->LayoutFB);
        }
    }
}

void LCDCore::Transition(int i) {
    if(is_transitioning_)
        return;
    gen_ = layoutGenerator(i, this);
    ChangeLayout();
    LCDError("LCDCore::Transition %d", i);
}

void LCDCore::StartTransition(std::string transition) {
    int t;
    if(transition.size() < 1) {
        LCDError("Transition not defined <%s>", current_layout_.c_str());
        StartLayout();
        return;
    }
    switch(transition.c_str()[0]) {
        case 'R':
            t = TRANSITION_RIGHT;
            break;
        case 'L':
            t = TRANSITION_LEFT;
            break;
        case 'B':
            t = TRANSITION_BOTH;
            break;
        case 'U':
            t = TRANSITION_UP;
            break;
        case 'D':
            t = TRANSITION_DOWN;
            break;
        case 'T':
            t = TRANSITION_TENTACLE;
            break;
        case 'A':
            t = TRANSITION_ALPHABLEND;
            break;
        default:
            t = TRANSITION_RIGHT;
            break;
    }
    gen_(current_layout_, last_layout_);
    lcd_->SignalTransitionStart(current_layout_);
    direction_ = t;
    StartLayout(current_layout_);
    is_transitioning_ = true;
    Json::Value *val = CFG_Fetch_Raw(CFG_Get_Root(), 
        current_layout_ + ".transition-speed", new Json::Value(transition_speed_));
    int speed = val->asInt();
    delete val;
    transition_timer_->Start(speed);
    LayoutTransition();
    LCDError("Transition started -- speed: %d", transition_speed_);
}

void LCDCore::LayoutTransition() {
    lcd_->Transition();
}

void LCDCore::TransitionFinished() {
    transition_timer_->Stop();
    StopLayout(last_layout_);
    is_transitioning_ = false;
    lcd_->SignalTransitionEnd();
    timer_->Start();
}

std::map<std::string, Widget *> LCDCore::GetWidgets() {
    return widgets_;
}

std::string LCDCore::CFG_Key() {
    return key_;
}

void LCDCore::KeypadEvent(const int key) {
    LCDError("KeypadEvent(%d) %s", key, current_layout_.c_str());

    std::map<std::string, bool>::iterator it = 
        keyless_layouts_.find(current_layout_);

    if(it != keyless_layouts_.end())
        return;

    for(std::map<std::string, Widget *>::iterator w = widgets_.begin();
        w != widgets_.end(); w++) {
        if( w->second->GetType() & WIDGET_TYPE_KEYPAD )
            ((WidgetKey*)w->second)->KeyPressed(key);
    }
}

int LCDCore::ResizeLCD(int rows, int cols) {
    StopLayout(current_layout_);
    //int old_rows = lcd_->LROWS;
    //int old_cols = lcd_->LCOLS;
    if(lcd_->ResizeLCD(rows, cols) == 0) {
        //emit static_cast<LCDEvents *>(wrapper_)->_ResizeLCD(rows, cols, old_rows, old_cols);
    } else {
        LCDError("LCDCore::ResizeLCD: Unable to resize LCD");
        return -1;
    }
    StartLayout(current_layout_);
    return 0;
}

void LCDCore::SelectLayout(std::string layout) {
    if(is_transitioning_)
        return;
    for(unsigned int i = 0; i < layouts_.size(); i++) {
        if(layouts_[i] == layout) {
            StopLayout(current_layout_);
            StartLayout(layout);
        }
    }
}

int LCDCore::RemoveWidget(std::string name) {
    std::map<std::string, Widget *>::iterator it = widgets_.find(name);
    if(it != widgets_.end()) {
        delete it->second;
        widgets_.erase(it);
        return 0;
    }
    return -1;
}

std::string LCDCore::AddWidget(std::string layout, int row, 
    int col, int layer, std::string object) {
    std::stringstream strm;
    std::string name;
    strm << layout << ":" << "script_widget" << 0;
    strm >> name;
    strm.clear();
    strm.str("");
    int i = 0;
    while(widgets_.find(name) != widgets_.end()) {
        strm << layout << ":" << "script_widget" << ":" << ++i;
        strm >> name;
        strm.clear();
        strm.str("");
    }

    Json::Reader reader;
    Json::Value *root = new Json::Value();;
    if(!reader.parse(object, *root)) {
        LCDError("Error parsing script: %s", reader.getFormatedErrorMessages().c_str());
        return "";
    }
    
    Json::Value *type = CFG_Fetch_Raw(root, "type");

    if(type) {
           Widget *widget = (Widget *)NULL;
           if(type->asString() == "text") {
               widget = (Widget *) new WidgetText(this, name, root,
                   row, col, layer);
           } else if (type->asString() == "bar") {
               widget = (Widget *) new WidgetBar(this, name, root,
                   row, col, layer);
           } else if (type->asString() == "icon") {
               widget = (Widget *) new WidgetIcon(this, name, root,
                   row, col, layer);
           } else if (type->asString() == "histogram") {
               widget = (Widget *) new WidgetHistogram(this, name, root,
                   row, col, layer);
           } else if (type->asString() == "bignums") {
               widget = (Widget *) new WidgetBignums(this, name, root,
                   row, col, layer);
           } else if (type->asString() == "key") {
               widget = (Widget *) new WidgetKey(this, name, root);
           } else if (type->asString() == "timer") {
               widget = (Widget *) new WidgetTimer(this, name, root);
           } else if (type->asString() == "script") {
               widget = (Widget *) new WidgetScript(this, name, root);
           } else {
               LCDError("Unknown widget type: %s", type->asCString());
           }
           if(widget) {
               widgets_[name] = widget;
               widget->Start();
           }
           delete type;
           return name;
    } else {
        LCDError("Widget has no type <%s>", object.c_str());
    }
    return "";
}

int LCDCore::MoveWidget(std::string widget, int row, int col) {
    return 0;
}
