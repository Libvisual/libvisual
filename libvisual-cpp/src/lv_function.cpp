// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_function.cpp,v 1.6 2005-09-08 03:50:28 descender Exp $
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

    std::cout << "\nLv::Function test:\n";

    {
        Function<int> functor (&print_hello_world);
        std::cout << "Result: " << functor () << '\n';
    }

    {
        Action action;

        Function<int> functor(&action, &Action::execute);
        std::cout << "Result: " << functor () << '\n';
    }

    {
        Action action;
        Function<int> functor(action);
        std::cout << "Result: " << functor () << '\n';
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

void typelist_test ()
{
#define LIST5 LVCPP_TYPELIST_5(char, int, long, std::string, Lv::Function<void>)

    namespace List = Lv::Typelist;

    std::cout << "\nLv typelist test:\n";

    std::cout << "List type: " << typeid (LIST5).name () << '\n';
    std::cout << "Car: " << typeid (List::Car<LIST5>::Result).name () << '\n';
    std::cout << "Cdr: " << typeid (List::Cdr<LIST5>::Result).name () << '\n';

    std::cout << "Type list: "
              << typeid (List::Nth<LIST5, 0>::Result).name () << ' '
              << typeid (List::Nth<LIST5, 1>::Result).name () << ' '
              << typeid (List::Nth<LIST5, 2>::Result).name () << ' '
              << typeid (List::Nth<LIST5, 3>::Result).name () << ' '
              << typeid (List::Nth<LIST5, 4>::Result).name () << '\n';

    std::cout << "Type list length: " << List::Length<LIST5>::value << '\n';

#undef LIST5
}


int main ()
{
    function_test ();
    typelist_test ();

    return 0;
}

#endif // #ifdef LVCPP_FUNCTION_TEST
