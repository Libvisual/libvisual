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

#include "simple_example.hpp"
#include <libvisual/libvisual.h>
#include <SDL.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);
    visual_log_set_verbosity (VISUAL_LOG_ERROR);

    int exit_code = EXIT_SUCCESS;

    try {
        if (SDL_Init (SDL_INIT_VIDEO)) {
            throw std::runtime_error { std::string {"Failed to initialize SDL: "} + SDL_GetError () };
        }
        std::atexit (SDL_Quit);

        SimpleExample example;
        example.run ();
    }
    catch (std::exception& error) {
        std::cerr << "Error! " << error.what () << std::endl;
        exit_code = EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error occurred\n";
        exit_code = EXIT_FAILURE;
    }

    LV::System::destroy ();

    return exit_code;
}
