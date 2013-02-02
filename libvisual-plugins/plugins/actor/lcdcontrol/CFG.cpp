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

#include <string>
#include <iostream>
#include <stdio.h>
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <libvisual/libvisual.h>

#include "CFG.h"
#include "debug.h"

using namespace LCD;

extern std::string lcdcontrol_config;

CFG::CFG() {
    main_root_ = true;
    root_ = NULL;
}

CFG::CFG(Json::Value *config) {
    main_root_ = false;
    root_ = config;
}

CFG::~CFG() {
    if(main_root_)
        delete root_;
}

std::string CFG::CFG_Source() {
    std::string path = key_ + ".source";
    Json::Value *source = CFG_Fetch_Raw(root_, path, new Json::Value("")); 
    std::string name = source->asString();
    delete source;
    return name;
}

bool CFG::CFG_Init( std::string path ) {

    std::ifstream stream_in;
    std::ofstream stream_out;

    stream_in.open(path, std::ios::binary);

    if(not stream_in.is_open())
    {
        stream_out.open(path);
        stream_out << lcdcontrol_config;
        stream_out.close();
        stream_in.open(path, std::ios::binary);
    }
    if(not stream_in.is_open())
    {
        visual_log(VISUAL_LOG_CRITICAL, "LCDControl could not load a config file.");
        return false;
    }

    root_ = new Json::Value();

    bool r = reader_.parse( stream_in, *root_, true );

    stream_in.close();

    if( r ) {
        return true;
    } else {
        LCDError("%s", reader_.getFormatedErrorMessages().c_str());
        return false;
    }
}

// Programmer must free memory
Json::Value *CFG::CFG_Fetch_Raw(Json::Value *section, std::string key, 
    Json::Value *defval) {

    int pos = key.find(".");
    if( pos > -1 ) {
        std::string k = key.substr(0, pos);
        Json::Value *subSection = CFG_Fetch_Raw(section, k);
        if(!subSection)
            return defval;
        Json::Value *root_ = CFG_Fetch_Raw(subSection, 
            key.substr(pos + 1, key.length()), defval);
        delete subSection;
        return root_;
    }

    if(!section->isMember(key)) { 
        return defval;
    }

    Json::Value *val = new Json::Value(section->get(key, 
        defval ? *defval : Json::Value()));

    /*if(defval && (val->type() != defval->type())) {
        LCDError("CFG: Possible value type mismatch <%s>", key.c_str());
        delete val;
        return defval;
    }*/

    if( val->isNull() ) {
        delete val;
        val = NULL;
    }

    if(val && defval) {
        delete defval;
        defval = NULL;
    }
    return val ? val : defval;
}

// Programmer must free memory
Json::Value *CFG::CFG_Fetch(Json::Value *section, std::string key, 
    Json::Value *defval) {

    Json::Value *val = CFG_Fetch_Raw(section, key, defval ? 
        new Json::Value(*defval) : defval);

    if(!val)
        return defval;

    if( val->isNumeric() ) {
        if( defval ) delete defval;
        return val;
    } else if ( val->isString() ) {
        Json::Value *val2 = new Json::Value(Eval(val->asCString()));
        return val2 ? val2 : defval;
    }
    return defval;
}
