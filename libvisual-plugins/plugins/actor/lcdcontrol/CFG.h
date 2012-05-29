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

#ifndef __CFG_H__
#define __CFG_H__

#include <string>
#include "json/json.h"
#include "Evaluator.h"

namespace LCD {

class CFG : public virtual Evaluator {
    Json::Reader reader_;
    Json::Value *root_;
    bool main_root_;
    protected:
    std::string key_;
    public:
    CFG();
    CFG(Json::Value *config);
    virtual ~CFG();
    std::string CFG_Source();
    bool CFG_Init( std::string path);
    Json::Value *CFG_Fetch_Raw(Json::Value *section, std::string key, 
        Json::Value *defval = NULL);
    Json::Value *CFG_Fetch(Json::Value *section, std::string key, 
        Json::Value *defval = NULL);
    virtual std::string CFG_Key() { return key_;};
    virtual Json::Value *CFG_Get_Root() { return root_; }
    virtual void CFG_Set_Root(Json::Value *r) { root_ = r; }
};

};

#endif
