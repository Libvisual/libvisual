// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_typelist.hpp,v 1.1 2006-01-13 07:00:33 descender Exp $
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

#ifndef LVCPP_TYPELIST_HPP
#define LVCPP_TYPELIST_HPP

// So this is why they say those who do not know Lisp are doomed to
// reinvent it poorly. >:)

// C++ does not support variable number of template parameters, so
// we're forced to define the type list recursively, in Lisp style.

// The Car and Cdr functions defined here are probably not very
// useful. We can easily decompose a Cons since it's parameterised by
// by a Head type (= Car), and a Tail type (= Cdr).

// TODO:
// * add template parameter checks


// Here we use macros instead of template typedefs, because C++ does
// not support them. The well-known workaround by wrapping typedefs in
// structs requires the annoying typename keyword because the alias is
// a dependent name. E.g. typename Lv::Typelist<T0, T1, T2>::Type

#define LVCPP_TYPELIST_0                 Lv::Typelist::Null
#define LVCPP_TYPELIST_1(T0)             Lv::Typelist::Cons<T0,Lv::Typelist::Null>
#define LVCPP_TYPELIST_2(T0,T1)          Lv::Typelist::Cons<T0,LVCPP_TYPELIST_1(T1)>
#define LVCPP_TYPELIST_3(T0,T1,T2)       Lv::Typelist::Cons<T0,LVCPP_TYPELIST_2(T1,T2)>
#define LVCPP_TYPELIST_4(T0,T1,T2,T3)    Lv::Typelist::Cons<T0,LVCPP_TYPELIST_3(T1,T2,T3)>
#define LVCPP_TYPELIST_5(T0,T1,T2,T3,T4) Lv::Typelist::Cons<T0,LVCPP_TYPELIST_4(T1,T2,T3,T4)>

namespace Lv
{

  namespace Typelist
  {
    // nil / ()
    struct Null {};

    // (cons head tail)
    template <typename Head, typename Tail>
    struct Cons
    { typedef Cons Result; };

    // (car list)
    template <typename List>
    struct Car;

    // (cdr list)
    template <typename List>
    struct Cdr;

    // (length list)
    template <typename List>
    struct Length;

    // (nth n list)
    template <typename List, int n>
    struct Nth;

    // (car ()) -> ()
    template <>
    struct Car<Null>
    { typedef Null Result; };

    // (car (cons head tail)) -> head
    template <typename Head, typename Tail>
    struct Car<Cons<Head,Tail> >
    { typedef Head Result; };

    // (cdr ()) -> ()
    template <>
    struct Cdr<Null>
    { typedef Null Result; };

    // (cdr (cons head tail)) -> tail
    template <typename Head, typename Tail>
    struct Cdr<Cons<Head,Tail> >
    { typedef Tail Result; };

    // (length ()) -> 0
    template <>
    struct Length<Null>
    { static const unsigned int value = 0; };

    // (length list) -> (1+ (length (cdr list)))
    template <typename Head, typename Tail>
    struct Length<Cons<Head,Tail> >
    { static const unsigned int value = 1 + Length<Tail>::value; };

    // (nth n ()) -> ()
    // where n is a non-negative integer
    template <int n>
    struct Nth<Null, n>
    { typedef Null Result; };

    // (nth 0 list) -> (car list)
    template <typename Head, typename Tail>
    struct Nth<Cons<Head,Tail>, 0>
    { typedef Head Result; };

    // (nth n list) -> (nth (1- n) (cdr list))
    // where n is a postive integer
    template <typename Head, typename Tail, int n>
    struct Nth<Cons<Head,Tail>, n>
    { typedef typename Nth<Tail, n-1>::Result Result; };

  } // namespace Type

} // namespace Lv

#endif // #ifndef LVCPP_TYPELIST_HPP
