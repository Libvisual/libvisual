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

#include <sys/time.h>
#include <libvisual/libvisual.h>

#include "PluginFPS.h"
#include "Stats.h"
#include "debug.h"

using namespace LCD;

class statavg_t {
    
    public:
    static const lua::args_t *in_args()
    {
        lua::args_t *args = new lua::args_t();
        return args;
    }

    static const lua::args_t *out_args()
    {
        lua::args_t *args = new lua::args_t();
        args->add(new lua::int_arg_t());
        return args;
    }

    static const std::string ns() { return "fps"; }
    static const std::string name() { return "avg"; }

    static void calc(const lua::args_t& in, lua::args_t &out)
    {
        static std::vector<double> hist;
        static double then = now_ms();

        double now = now_ms();
        double diff = now - then;
        then = now;

        hist.push_back(diff);

        double avg = 0.0;
        for(unsigned int i = 0; i < hist.size(); i++)
        {
            avg+=hist[i];
        }
        avg = avg / hist.size();

        double val = avg;

        visual_log(VISUAL_LOG_INFO, "AVERAGE FPS %f", val);
        dynamic_cast<lua::int_arg_t&>(*out[0]).value() = val;

        if(hist.size() > 20)
        {
            std::vector<double> tmp;
            for(unsigned int i = 1; i < hist.size(); i++)
            {
                tmp.push_back(hist[i]);
            }
            hist = tmp;
        }
    }
};

PluginFPS::PluginFPS(lua *script) {
    script->register_function<statavg_t>();
}


PluginFPS::~PluginFPS() {
}


