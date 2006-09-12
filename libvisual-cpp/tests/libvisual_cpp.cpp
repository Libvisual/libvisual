// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005-2006 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: libvisual_cpp.cpp,v 1.3 2006-09-12 02:40:36 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#include <libvisual-cpp/libvisual_cpp.hpp>
#include <libvisual/libvisual.h>
#include <iostream>

void throw_unexpected_error ()
    throw ()
{
    throw std::runtime_error ("throw unexpected exception");
}

int main (int argc, char **argv)
{
    std::cout << "Libvisual version: " << Lv::get_lv_version () << "\n";

    // startup test
    std::cout << "Startup test\n";

    try
    {
        if (Lv::init (argc, argv) != VISUAL_OK)
            throw std::runtime_error ("Lv::init (argc, argv) fail");

        if (!Lv::is_init ())
            throw std::runtime_error ("Lv::is_init () returns false");

        if (Lv::quit () != VISUAL_OK)
            throw std::runtime_error ("Lv::quit () fail");

        if (Lv::init () != VISUAL_OK)
            throw std::runtime_error ("Lv::init (NULL, NULL) fail");

        if (!Lv::is_init ())
            throw std::runtime_error ("Lv::is_init () returns false");
    }
    catch (std::runtime_error& error)
    {
        std::cout << error.what () << std::endl;
    }

    // uncaught exception test

    try
    {
        std::cout << "Unexpected exception test\n";
        throw_unexpected_error ();
    }
    catch (Lv::UnexpectedError& error)
    {
        std::cout << "UnexpectedError caught\n";
        std::cout << error.what () << std::endl;
    }

    std::cout << "Uncaught exception test\n";
    throw std::runtime_error ("test uncaught exception");
}
