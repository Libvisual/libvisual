// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_function.cpp,v 1.1 2006-01-13 06:51:54 descender Exp $
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

#include <lv_function.hpp>
#include <iostream>

void print_hello_world ()
{
    std::cout << "Hello World!\n";
}

int identity (int x)
{
    return x;
}

struct Identity
{
    int operator () (int x)
    {
        return identity (x) + 1;
    }

    int execute (int x)
    {
        return identity (x) + 2;
    }
};

int add (int a, int b)
{
    return a + b;
}

struct Add
{
    int operator () (int a, int b)
    {
        return add (a, b) + 1;
    }

    int execute (int a, int b)
    {
        return add (a, b) + 2;
    }
};


void function_test ()
{
    using Lv::Function;

    std::cout << "\nLv::Function test:\n";
    
    {
        Function<void> a (&print_hello_world);
        Function<void> b;

        b = a;
        a ();
    }

    {
        Identity a_identity;

        Function<int, LVCPP_TYPELIST_1(int)> a (&identity);
        Function<int, LVCPP_TYPELIST_1(int)> b (a_identity);
        Function<int, LVCPP_TYPELIST_1(int)> c (&a_identity, &Identity::execute);

        std::cout << "identity: " 
                  << a (1) << ' ' 
                  << b (2) << ' '
                  << c (3) << '\n';
    }

    {
        Add a_add;

        Function<int, LVCPP_TYPELIST_2(int, int)> a (&add);
        Function<int, LVCPP_TYPELIST_2(int, int)> b (a_add);
        Function<int, LVCPP_TYPELIST_2(int, int)> c (&a_add, &Add::execute);

        std::cout << "add: "
                  << a(1, 2) << ' ' 
                  << b(1, 2) << ' ' 
                  << c(1, 2) << '\n';
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
