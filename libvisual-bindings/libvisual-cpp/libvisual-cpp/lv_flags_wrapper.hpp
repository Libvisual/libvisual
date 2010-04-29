// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2006 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_flags_wrapper.hpp,v 1.1 2006-09-12 04:00:20 descender Exp $
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

#ifndef LV_FLAGS_WRAPPER_HPP
#define LV_FLAGS_WRAPPER_HPP

#define LV_FLAGS_WRAP_OP(Type, op) \
  inline Type operator op (Type lhs, Type rhs)                                               \
  {                                                                                          \
      return static_cast<Type> (static_cast<unsigned> (lhs) op static_cast<unsigned> (rhs)); \
  }                                                                                          \

#define LV_FLAGS_WRAP_ASSIGN_OP(Type, op) \
  inline Type& operator op##= (Type &lhs, Type rhs)                                                  \
  {                                                                                                  \
      return (lhs = static_cast<Type> (static_cast<unsigned> (lhs) op static_cast<unsigned> (rhs))); \
  }                                                                                                  \

#define LV_FLAGS_WRAP(Type)           \
    LV_FLAGS_WRAP_OP (Type, |)        \
    LV_FLAGS_WRAP_OP (Type, &)        \
    LV_FLAGS_WRAP_OP (Type, ^)        \
    LV_FLAGS_WRAP_ASSIGN_OP (Type, |) \
    LV_FLAGS_WRAP_ASSIGN_OP (Type, &) \
    LV_FLAGS_WRAP_ASSIGN_OP (Type, ^)

#endif // #ifndef LV_FLAGS_WRAPPER_HPP
