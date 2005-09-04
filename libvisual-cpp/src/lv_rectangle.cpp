// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_rectangle.cpp,v 1.1 2005-09-04 21:59:33 descender Exp $
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

#include <lv_config.hpp>
#include <lv_rectangle.hpp>

#ifdef LVCPP_RECTANGLE_TEST

#include <iostream>

std::ostream& operator << (std::ostream& out, const Lv::Rectangle& rect)
{
    return (out << "Rect (" 
                << rect.get_x () << ", " << rect.get_y () << ", "
                << rect.get_width() << ", " << rect.get_height ()
                << ')');
}

int main ()
{
    std::cout << "Lv::Rectangle test\n";

    Lv::Rectangle a;
    std::cout << a << '\n';

    Lv::Rectangle b (10, 20, 100, 200);
    std::cout << b << '\n';

    Lv::Rectangle c (b);
    std::cout << c << '\n';

    // TODO: more to come

    return 0;
}

#endif // #ifdef LVCPP_RECTANGLE_TEST
