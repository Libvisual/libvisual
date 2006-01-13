// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_random.cpp,v 1.2 2006-01-13 08:09:34 descender Exp $
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

#include <lv_random.hpp>
#include <iostream>

int main ()
{
    Lv::RandomContext random (1);

    std::cout << "Lv::RandomContext test\n";

    std::cout << "Seed: " << random.get_seed () << '\n';

    random.set_seed (0);
    std::cout << "Seed: " << random.get_seed () << '\n';

    std::cout << "Seed state: " <<  random.get_seed_state () << '\n';

    int    a = random.value<int> ();
    float  b = random.value<float> ();
    double c = random.value<double> ();

    int    d = random.value_range<int> (0, 0);
    int    e = random.value_range (1, 1);

    std::cout << "Seed state: " <<  random.get_seed_state () << '\n';

    std::cout << "Random values: "
              << a << ' ' << b << ' ' << c << ' '
              << d << ' ' << e << '\n';

    std::cout << "To be or not to be: " << random.decide (0.9) << '\n';

    return 0;
}
