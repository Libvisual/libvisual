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

#ifndef __WIDGET_VISUALIZATION_H__
#define __WIDGET_VISUALIZATION_H__

#include <json/json.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#include "Property.h"
#include "Widget.h"
#include "RGBA.h"
#include "LCDTimer.h"
#include "debug.h"

#include <libvisual/libvisual.h>

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class WidgetVisualization : public Widget {

    int direction_;
    int mono_;
    int style_;
    int update_;
    timeval last_update_;
    int vis_;
    std::vector<std::map<char, short> > history_;
    std::map<char, char> ch_;
    bool has_chars_;
    char *peak_buffer_;

    LCDTimer *timer_;
    LCDTimer *morph_timer_;

    // libvisual stuff
/*
    VisSongInfo current_songinfo_;
    VisSongInfo final_songinfo_;
*/
    void *buffer_;

    public:
    LV::Video *video_;
    LV::Bin *bin_;
    LV::Actor *actor_;
    VisAudio *audio_;

    VisVideoDepth depth_;
    std::string morph_plugin_;
    std::string actor_plugin_;
    std::string input_plugin_;
    std::string skip_actors_;
    int morph_timeout_;
    bool morph_chosen_;
    char alpha_;

    void (*Draw)(WidgetVisualization *widget);

    void DoParams();
    bool MyCurrentID( const int &id );
    bool MyGetPlaytime( const int &playtime );
    bool ErrorHandler( const std::string &function, const std::string &error );

    WidgetVisualization(LCDCore *visitor, std::string name, 
        Json::Value *section, int row, int col, int layer);
    ~WidgetVisualization();
    void SetupChars();
    void Start();
    void Stop();
    int GetStyle() { return style_; }
    void *GetBuffer() { return buffer_; }
    char *GetPeakBuffer() { return peak_buffer_; }
    bool HasChars() { return has_chars_; }
    VisVideoDepth GetDepth() { return depth_; }
    std::map<char, char> GetChars() { return ch_; }
    std::vector<std::map<char, short> > GetHistory() { return history_; }

    void Update();
    void UpdatePeak();
    void UpdatePCM();
    void UpdateSpectrum();
    void TextScroll() {}

    void VisualMorph();
    void Resize(int rows, int cols, int old_rows, int old_cols);
    void ResizeBefore(int rows, int cols);
    void ResizeAfter();

};

}; // End namespace

#define STYLE_PEAK 0
#define STYLE_PCM 1
#define STYLE_SPECTRUM 2

#endif
