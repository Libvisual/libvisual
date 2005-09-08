// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_function.cpp,v 1.5 2005-09-08 03:27:09 descender Exp $
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
#include <lv_function.hpp>

#ifdef LVCPP_FUNCTION_TEST

#include <iostream>

int print_hello_world ()
{
    std::cout << "Free function" << std::endl;
    return 0;
}

struct Action
{
    int execute ()
    {
        std::cout << "Member function" << std::endl;
        return 1;
    }

    int operator () ()
    {
        std::cout << "Function" << std::endl;
        return 2;
    }
};

void function_test ()
{
    using Lv::Function;

    {
        Function<int> functor (&print_hello_world);
        std::cout << "Result: " << functor () << std::endl;
    }

    {
        Action action;

        Function<int> functor(&action, &Action::execute);
        std::cout << "Result: " << functor () << std::endl;
    }

    {
        Action action;
        Function<int> functor(action);
        std::cout << "Result: " << functor () << std::endl;
    }
    
    {
        Function<int> a (&print_hello_world);
        Function<int> b;

        b = a;
        a ();
    }

    {
        Function<int> a (&print_hello_world);
        Function<int> b;

        b = a;
        a ();
    }
}

#define LIST_NTH(list,n) Lv::Typelist::Nth<list,n>::Result
#define LIST_3 LVCPP_TYPELIST_3(char, int, long)

void typelist_test ()
{
    std::cout << typeid (LIST_NTH(LIST_3, 0)).name () << ' '
              << typeid (LIST_NTH(LIST_3, 1)).name () << ' '
              << typeid (LIST_NTH(LIST_3, 2)).name () << std::endl;
}

int main ()
{
    function_test ();
    typelist_test ();

    return 0;
}

#endif // #ifdef LVCPP_FUNCTION_TEST
