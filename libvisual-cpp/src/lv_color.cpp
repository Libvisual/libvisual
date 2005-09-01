// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_color.cpp,v 1.1 2005-09-01 07:10:25 descender Exp $
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
#include <lv_color.hpp>

#ifdef LVCPP_COLOR_TEST

#include <iostream>

std::ostream& operator << (std::ostream& out, const Lv::Color& color)
{
    uint8_t r, g, b;
    color.get (r, g, b);

    return (out << int (r) << ',' << int (g) << ',' << int (b));
}

int main ()
{
    Lv::Color a;
    std::cout << a << '\n';

    a.set (128, 128, 128);
    std::cout << a << '\n';

    float h, s, v;
    a.to_hsv (h, s, v);
    std::cout << h << ',' << s << ',' << v << '\n';

    a.from_hsv (h, s, v);
    std::cout << a << '\n';

    Lv::Color b(a);
    std::cout << b << '\n';

    std::cout << "a == b: " << (a == b) << '\n';
    std::cout << "a != b: " << (a != b) << '\n';

    a.set (0, 0, 0);
    b = a;

    std::cout << a << '\n';
    std::cout << b << '\n';

    return 0;
}

#endif
