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

#ifndef __GENERIC_H__
#define __GENERIC_H__

#define LCD_CONTROL 0
#define LCD_TEXT 1
#define LCD_GRAPHIC 2

#include <json/json.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <iostream>

#include <libvisual/libvisual.h>

#include "CFG.h"
#include "Evaluator.h"
#include "LCDBase.h"
#include "LCDText.h"
#include "LCDGraphic.h"
#include "LCDTimer.h"

#include "Widget.h"
#include "Generator.h"
#include "LCDTimer.h"

#define TRANSITION_RIGHT 0
#define TRANSITION_LEFT 1
#define TRANSITION_BOTH 2
#define TRANSITION_UP 3
#define TRANSITION_DOWN 4
#define TRANSITION_TENTACLE 5
#define TRANSITION_ALPHABLEND 6

namespace LCD {

//class PluginLCD;
class LCDControl;

struct widget_template {
    std::string key;
    int row;
    int col;
    int layer;
};

class LCDCore: public virtual Evaluator, public CFG {
    std::vector<std::string> layouts_;
    std::string current_layout_;
    std::string last_layout_;
    std::vector<std::string> static_widgets_;
    std::map<std::string, std::vector<widget_template> > widget_templates_;
    std::map<std::string, Widget *> widgets_;
    std::map<std::string, bool> keyless_layouts_;
    int type_;
    int layout_timeout_;
    int transition_speed_;
    int direction_;
    bool is_transitioning_;
    bool clear_on_layout_change_;
    bool transitions_off_;
    LCDTimer *timer_;
    LCDTimer *transition_timer_;
    VisEventQueue *eventqueue_;

    //PluginLCD *pluginLCD;

    protected:
    LCDBase *lcd_;
    std::string name_;
    LCDControl *app_;

    public:
    LCDCore(LCDControl *app, std::string name, Json::Value *config, 
        int type, VisEventQueue *eventqueue, LCDBase *lcd = (LCDBase *)NULL);
    virtual ~LCDCore();
    virtual void CFGSetup();
    void BuildLayouts();
    void StartLayout(std::string key = "");
    int GetType() { return type_; }
    LCDBase *GetLCD() { return lcd_; }
    virtual void Connect(){};
    virtual void SetupDevice(){};
    virtual void TakeDown(){};
    VisVideo *GetVideo(){ return lcd_->GetVideo(); };
    std::map<std::string, Widget *> GetWidgets();
    std::string CFG_Key();
    std::vector<std::string> GetLayouts() { return layouts_; }
    int GetDirection() { return direction_; }
    std::string GetCurrentLayout() { return current_layout_; }
    std::string GetLastLayout() { return last_layout_; }
    std::string GetName() { return name_; }
    LCDControl *GetApp() { return app_; }
    bool ClearOnLayoutChange() { return clear_on_layout_change_; }
    bool IsActive();
    void TextSetSpecialChars() {}
    void LayoutChangeBefore() {}
    void LayoutChangeAfter() {}
    void TextSpecialCharChanged(int i) {}
    void ChangeLayout();
    void StopLayout(std::string layout);
    void StartTransition(std::string transition);
    void LayoutTransition();
    void TransitionFinished();
    void Transition(int);
    void KeypadEvent(const int k);
    int ResizeLCD(int row, int col);
    void SelectLayout(std::string layout);
    int RemoveWidget(std::string name);
    std::string AddWidget(std::string layout, int row, int col, 
        int layer, std::string object);
    int MoveWidget(std::string widget, int rows, int cols);

    LCDTimerBin *timers_;

    int gen_index_;

    _generator(layoutGenerator) {
        LCDCore *obj;
        layoutGenerator(int j, LCDCore *v, bool init = false) {
            obj = v;
	    if(init) obj->gen_index_ = 1;
            obj->gen_index_+=j-1;
            if(obj->gen_index_ >= (int)obj->GetLayouts().size())
                obj->gen_index_ = 0;
            if(obj->gen_index_ < 0)
                obj->gen_index_ = obj->GetLayouts().size() - 1;
        }
        _emit(std::string)
            while(true) {
                _yield(obj->GetLayouts()[obj->gen_index_++]);
                if(obj->gen_index_ >= (int)obj->GetLayouts().size())
                    obj->gen_index_ = 0;
            }
        _stop;
    };

    void InitGen(int i = 0) { gen_index_ = i; }
    private:
    layoutGenerator gen_;
};

}; // End namespace


#endif
