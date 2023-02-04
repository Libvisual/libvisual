// Simple example of using Libvisual.
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
//          Scott Sibley <sisibley@gmail.com>
//          Max Howell <max.howell@methylblue.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _LV_EXAMPLES_SIMPLE_EXAMPLE
#define _LV_EXAMPLES_SIMPLE_EXAMPLE

#include <libvisual/libvisual.h>
#include <SDL.h>

class SimpleExample
{
public:

    SimpleExample ();

    ~SimpleExample ();

    void run ();

private:

    LV::VideoPtr m_screen;
    SDL_Surface* m_sdl_screen;
    LV::Palette  m_palette;

    LV::Bin      m_bin;

    std::string  m_actor_name;
    std::string  m_input_name;
    std::string  m_morph_name;

    LV::VideoPtr create_display (int width, int height, VisVideoDepth depth);
    void         resize_display (int width, int height);
    void         set_palette (LV::Palette const& palette);
    void         morph_to_actor (std::string const& actor, std::string const& morph);
    bool         handle_events  ();
    void         render ();
};

#endif // _LV_EXAMPLES_SIMPLE_EXAMPLE
