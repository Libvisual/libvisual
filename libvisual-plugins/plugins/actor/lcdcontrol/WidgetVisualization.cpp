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
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>

#include "WidgetVisualization.h"
#include "LCDCore.h"
#include "LCDText.h"
#include "LCDGraphic.h"

#include <libvisual/libvisual.h>

using namespace LCD;

extern void TextVisualizationDraw(WidgetVisualization *w);

extern void GraphicVisualizationDraw(WidgetVisualization *w);

void visualization_update(void *data)
{
    WidgetVisualization *widget = (WidgetVisualization *)data;
    widget->Update();
}

void visualization_morph_update(void *data)
{
    WidgetVisualization *widget = (WidgetVisualization *)data;
    widget->VisualMorph();
    
}

int VISUALIZATION_CHARS[6][9] = {
    {31, 31, 31, 31, 31, 31, 31, 31, -1},
    {21, 14, 31, 14, 31, 14, 31, 10, -1},
    { 0, 14,  4, 14,  4, 14,  4,  0, -1},
    { 0,  0,  4, 14, 14,  4,  0,  0, -1},
    { 0,  0,  0,  4,  4,  0,  0,  0, -1},
    {14, 14, 14, 14, 14, 14, 14, 14, -1}
};

WidgetVisualization::WidgetVisualization(LCDCore *v, std::string n, Json::Value *section, int row, int col, int layer) : Widget(v, n, section, row, col, layer,
    WIDGET_TYPE_VISUALIZATION | WIDGET_TYPE_RC | WIDGET_TYPE_SPECIAL) {

    LCDError("widgetvisualization: layer: %d", layer_);

    if(lcd_type_ == LCD_TEXT)
        Draw = TextVisualizationDraw;
    else if(lcd_type_ == LCD_GRAPHIC)
        Draw = GraphicVisualizationDraw;
    else
        Draw = 0;

    fg_valid_ = WidgetColor(section, "foreground", &fg_color_);
    bg_valid_ = WidgetColor(section, "background", &bg_color_);

    Json::Value *val = v->CFG_Fetch(section, "width", new Json::Value(10));
    cols_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section, "height", new Json::Value(1));
    rows_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "direction", new Json::Value("E"));
    if( val->asString() == "E" )
        direction_ = DIR_EAST;
    else if (val->asString() == "W" )
        direction_ = DIR_WEST;
    else {
        LCDError("Widget %s has unknown direction '%s'; Use (E)ast or (W)est. Using E.",
            name_.c_str(), val->asCString());
        direction_ = DIR_EAST;
    }
    delete val;

    val = v->CFG_Fetch(section, "mono", new Json::Value(0));
    mono_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "style", new Json::Value("peak"));
    if(val->asString() == "peak") {
        style_ = STYLE_PEAK;
    } else if (val->asString() == "pcm") {
        style_ = STYLE_PCM;
    } else if (val->asString() == "spectrum") {
        style_ = STYLE_SPECTRUM;
    }
    delete val;

    val = v->CFG_Fetch_Raw(section, "actor", new Json::Value("oinksie"));
    std::string actor = val->asString(); 
    delete val;

    val = v->CFG_Fetch_Raw(section, "input", new Json::Value("debug"));
    std::string input = val->asString();
    delete val;

    val = v->CFG_Fetch_Raw(section, "morph", new Json::Value("alphablend"));
    morph_ = val->asString();
    delete val;

    val = v->CFG_Fetch(section, "morph-timeout", new Json::Value(4000));
    morph_timeout_ = val->asInt();
    delete val;

    val = v->CFG_Fetch_Raw(section, "skip-actors", new Json::Value(""));
    skip_actors_ = val->asString();
    delete val;

    val = v->CFG_Fetch(section, "alpha", new Json::Value(255));
    alpha_ = val->asInt();
    delete val;

    val = v->CFG_Fetch(section, "update", new Json::Value(500));
    update_ = val->asInt();
    delete val;
   
    history_.resize(rows_);

    timer_ = v->timers_->AddTimer(visualization_update, this, update_, true);

    morph_timer_ = v->timers_->AddTimer(visualization_morph_update, this, morph_timeout_, true);
    last_update_.tv_sec = last_update_.tv_usec = 0;

    peak_buffer_ = new char[cols_ * rows_];
    memset(peak_buffer_, 5, cols_ * rows_);

    // -------------------- libvisual related below -----------------

    if(style_ != STYLE_PCM) 
        return;

    visual_log_set_verbosity(VISUAL_LOG_INFO);

    proxy_.plugin = strdup(actor.c_str());

    // Find an actor
/*
    if(!visual_actor_valid_by_name(proxy_.plugin)) {
        LCDError("WidgetVisualization: Actor plugin not found -- trying to find one.");
        proxy_.plugin = visual_actor_get_next_by_name_nogl(0);
    }

    if(!visual_actor_valid_by_name(proxy_.plugin)) {
        LCDError("WidgetVisualization: Unable to load an actor");
        update_ = -1;
        return;
    }
*/

    LCDError("WidgetVisualization: Using <%s> actor - requested <%s>", proxy_.plugin, actor.c_str());

    bin_ = new LV::Bin();
    bin_->set_supported_depth(VISUAL_VIDEO_DEPTH_ALL);
    bin_->switch_set_style(VISUAL_SWITCH_STYLE_MORPH);

    actor_ = visual_actor_new(actor.c_str());

    input_ = visual_input_new(input.c_str());
    
    // Set depth
    int depthflag = visual_actor_get_supported_depth(actor_);

    depth_ = visual_video_depth_get_highest_nogl(depthflag);

    // ----

    bin_->set_depth(depth_);
    bin_->connect(actor_, input_);

    proxy_.video = visual_video_new_with_buffer(cols_, rows_, depth_);

    visual_actor_set_video(actor_, proxy_.video);

    visual_actor_video_negotiate(actor_, depth_, FALSE, TRUE);

    bin_->set_video(video_);
    bin_->realize();
    bin_->sync(FALSE);
    bin_->depth_changed();

    //FIXME visual_bin_set_morph_by_name(bin_, (char *)morph_.c_str());

    //FIXME proxy_.video->pal = visual_palette_new(256);

/*
    memset(&current_songinfo_, 0, sizeof(VisSongInfo));

    visual_songinfo_init(&current_songinfo_, VISUAL_SONGINFO_TYPE_NULL);
    visual_songinfo_set_simple_name(&current_songinfo_, (char *)"");
    visual_songinfo_set_song(&current_songinfo_, (char *)"");
    visual_songinfo_set_artist(&current_songinfo_, (char *)"");
    visual_songinfo_set_album(&current_songinfo_, (char *)"");
*/

    DoParams();

/*
    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
        this, SLOT(Resize(int, int, int, int)));
    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeBefore(int, int)),
        this, SLOT(ResizeBefore(int, int)));
    QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeAfter()),
        this, SLOT(ResizeAfter()));
*/
    LCDError("WidgetVisualization %s", name_.c_str());
}

void WidgetVisualization::DoParams() {
    // params
    Json::Value *actorVal = visitor_->CFG_Fetch_Raw(section_, std::string("params.") + proxy_.plugin);
    if(actorVal) {
        VisActor *actor = actor_;
        VisPluginData *plugin = visual_actor_get_plugin(actor);
        VisParamContainer *params = visual_plugin_get_params(plugin);
        Json::Value::Members members = actorVal->getMemberNames();
        for(std::vector<std::string>::iterator it = members.begin();
            it != members.end(); it++) {
            VisParamEntry *entry = visual_param_container_get(params, it->c_str());
            if(entry) {
                Json::Value *val = visitor_->CFG_Fetch_Raw(actorVal, *it);
                switch(entry->type) {
                case VISUAL_PARAM_ENTRY_TYPE_STRING:
                    if(val->isString()) {
                        visual_param_entry_set_string(entry, (char *)val->asCString());
                    }
                    break;
                case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
                    if(val->isInt()) {
                        visual_param_entry_set_integer(entry, val->asInt());
                    }
                    break;
                case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
                    if(val->isDouble()) {
                        visual_param_entry_set_float(entry, (float)val->asDouble());
                    }
                    break;
                case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
                    if(val->isDouble()) {
                        visual_param_entry_set_double(entry, val->asDouble());
                    }
                    break;
                case VISUAL_PARAM_ENTRY_TYPE_COLOR:
                    if(val->isString()) {
                        int r, g, b;
                        sscanf(val->asCString(), "r=%d,g=%d,b=%d", &r, &g, &b);
                        visual_param_entry_set_color(entry, r, g, b);
                    }
                    break;
                default:
                    break;  
                }
                delete val;
            }
        }
        delete actorVal;
    }
}

WidgetVisualization::~WidgetVisualization() {
    delete []peak_buffer_;
}

void  WidgetVisualization::Resize(int rows, int cols, int old_rows, int old_cols) {
    float y = rows_ / (float)old_rows;
    float x = cols_ / (float)old_cols;
    float r = row_ / (float)old_rows;
    float c = col_ / (float)old_cols;
    rows_ = round(rows * y);
    cols_ = round(cols * x);
    row_ = round(rows * r);
    col_ = round(cols * c);

    visual_video_free_buffer(proxy_.video);
    visual_object_unref(VISUAL_OBJECT(proxy_.video));

    proxy_.video = visual_video_new_with_buffer(cols_, rows_, VISUAL_VIDEO_DEPTH_32BIT);

    visual_actor_set_video(actor_, proxy_.video);

    visual_actor_video_negotiate(actor_, VISUAL_VIDEO_DEPTH_32BIT, FALSE, TRUE);
}

void WidgetVisualization::ResizeBefore(int rows, int cols) {

}

void WidgetVisualization::ResizeAfter() {
}


bool WidgetVisualization::MyGetPlaytime(const int &playtime) {
    //visual_songinfo_set_elapsed(&current_songinfo_, playtime);
    return false;
}

void WidgetVisualization::SetupChars() {
LCDError("SetupChars");
    ch_.clear();
    LCDText *lcd = (LCDText *)visitor_->GetLCD();

    int size;
    if(style_ == STYLE_PCM)
        size = 8;
    else if(style_ == STYLE_PEAK)
        size = 6;
    else
        size = 2;


/*
    if(style_ == STYLE_PCM) {
        std::map<std::string, Widget *> widgets1;
        std::map<std::string, Widget *> widgets2;
        widgets1 = visitor_->GetWidgets();
        widgets2 = visitor_->GetWidgets();
        for(std::map<std::string, Widget *>::iterator ii=widgets1.begin();
            ii != widgets1.end(); ii++) {
            if(ii->second->GetWidgetBase() == widget_base_ &&
                ((WidgetVisualization *)ii->second)->HasChars()) {
                for(int i = 0; i < size; i++ ) {
                    ch_[i] = ((WidgetVisualization *)widgets2[ii->first])->GetChars()[i];
                }
                has_chars_ = true;
                return;
            }
        }
    }
*/

    if( (int)lcd->special_chars.size() >= lcd->CHARS - size) {
        update_ = -1;
        LCDError("Widget %s - unable to allocate special chars. CHARS: %d, chars_cache: %d, size: %d", 
            name_.c_str(), lcd->CHARS, lcd->special_chars.size(), size);
        return;
    }

    for(int c = 0; c < size; c++) {
        if(style_ != STYLE_PCM)
            lcd->special_chars.push_back(SpecialChar(VISUALIZATION_CHARS[c], lcd->YRES));
        else
            lcd->special_chars.push_back(SpecialChar(lcd->YRES));

        ch_[c] = lcd->special_chars.size() - 1;
        lcd->TextSpecialCharChanged(ch_[c]);
    }

    has_chars_ = true;
}

void WidgetVisualization::Update() {
    if(style_ == STYLE_PEAK)
        UpdatePeak();
    else if(style_ == STYLE_PCM)
        UpdatePCM();
    else if(style_ == STYLE_SPECTRUM)
        UpdateSpectrum();
}

void WidgetVisualization::UpdatePeak() {
/*
    short buffer[2];

    //int ret = xmmsc_visualization_chunk_get(client_.getConnection(), vis_, buffer, 0, 0);

    if( ret < 0) {
        update_ = -1;
        Stop();
        LCDError("WidgetVisualization: Unable to get chunk -- stopping.");
        return;
    } else if (ret == 0) {
        return;
    }

    timeval now;
    gettimeofday(&now, 0);
    unsigned long time = (now.tv_sec - last_update_.tv_sec) * 
        1000 + (now.tv_usec - last_update_.tv_usec) / 1000;

    if(time < (unsigned int)update_ && buffer[0] != 0 && buffer[1] != 0)
        return;

    gettimeofday(&last_update_, 0);

    std::vector<std::map<char, short> > tmp = history_;

    history_[rows_ - 1][0] = buffer[0];
    history_[rows_ - 1][1] = buffer[1];

    for(int i = rows_ - 2; i >= 0; i--)
        history_[i] = tmp[i+1];

    if(Draw)
        Draw(this);
    else
        LCDError("WidgetVisualization: now draw method");
*/
}


void WidgetVisualization::UpdatePCM() 
{
    if(bin_->depth_changed())
    {
        // Set depth
        int depthflag = visual_actor_get_supported_depth(visual_bin_get_actor(bin_));
    
        depth_ = visual_video_depth_get_highest_nogl(depthflag);

        visual_video_free_buffer(proxy_.video);

        video_ = visual_video_new_with_buffer(cols_, rows_, depth_);

        bin_->set_video(video_);

        bin_->sync(TRUE);
    }

    bin_->run();

    //proxy_.pal = visual_actor_get_palette(actor_);

    if(Draw)
        Draw(this);
}

void WidgetVisualization::VisualMorph() {
    LCDError("VisualMorph");
    proxy_.plugin = visual_actor_get_next_by_name_nogl(proxy_.plugin);

    if(!proxy_.plugin) {
        VisualMorph();
        return;
    }

    if(strstr(skip_actors_.c_str(), proxy_.plugin) != 0) {
        VisualMorph();
        return;
    }

    //FIXME visual_bin_set_morph_by_name(bin_, (char *)morph_.c_str());
    visual_bin_switch_actor_by_name(bin_, (char *)proxy_.plugin);

    DoParams();
}

void WidgetVisualization::UpdateSpectrum() {
/*
    short buffer[256];

    int ret = xmmsc_visualization_chunk_get(client_.getConnection(), vis_, buffer, 0, 0);

    if( ret < 0) {
        update_ = -1;
        Stop();
        LCDError("WidgetVisualization: Unable to get chunk -- stopping.");
        return;
    } else if ( ret == 0) {
        return;
    }

    timeval now;
    gettimeofday(&now, 0);
    unsigned long time = (now.tv_sec - last_update_.tv_sec) *
        1000 + (now.tv_usec - last_update_.tv_usec) / 1000;

    if(time < (unsigned int)update_)
        return;

    gettimeofday(&last_update_, 0);

    for(int i = 0; i < 256; i++)
        history_[0][i] = buffer[i];

    if(Draw)
        Draw(this);
    else
        LCDError("WidgetVisualization: no draw method");
*/
}

void WidgetVisualization::Start() {
    if(update_ < 0)
        return;

   std::map<std::string, Widget *> widgets;
    widgets = visitor_->GetWidgets();
    for(std::map<std::string, Widget *>::iterator it =
        widgets.begin();
        it != widgets.end(); it++) {
        if(it->second->GetWidgetBase() == widget_base_ &&
            it->second->GetLayoutBase() == layout_base_ &&
            it->second->GetStarted()) {
            started_ = true;
            break;
        }
    }
    if(!started_) {
        timer_->Start();
        if(morph_timeout_ > 0 && style_ == STYLE_PCM)
            morph_timer_->Start();
        started_ = true;
    }
    //Draw(this);
    LCDError("WidgetVisualization::Start %s", name_.c_str());
}

void WidgetVisualization::Stop() {
    timer_->Stop();
    if(style_ == STYLE_PCM) {
            morph_timer_->Stop();
    }
    ch_.clear();
    has_chars_ = false;
    started_ = false;
    LCDError("WidgetVisualization::Stop %s", name_.c_str());
}

