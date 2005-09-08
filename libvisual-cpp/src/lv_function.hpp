// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_function.hpp,v 1.7 2005-09-08 03:27:09 descender Exp $
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

#ifndef LVCPP_FUNCTION_HPP
#define LVCPP_FUNCTION_HPP

#include <memory>
#include <lv_typelist.hpp>

// This design is largely inspired by (read: taken from) the callback
// framework in Modern C++ Design.

// Usage:
//
// Definition:
//   Lv::Function<T> a (free_function);
//   Lv::Function<T> b (&object_instance, &Object::method);
//   Lv::Function<T> c (functor);
// 
// Invoking:
//   a (); // calls free_function()
//   b (); // calls object_instance->method()
//   c (); // calls functor()
//
// Assignment/Copying
//   a = b // assign b to a
//   a ()  // calls free_function()

namespace Lv
{
  // TODO (highest priority first)
  // * allow variable number of arguments
  // * add comparators == and !=
  // * eliminate virtual calls
  // * allow custom allocators for FunctionImpl

  // Forward declarations

  template <typename Result>
  class Function;

  template <typename Result>
  class FunctionImpl;

  template <typename Object, typename MemFunction, typename Result>
  class FunctionImplMem;

  template <typename Functor, typename Result>
  class FunctionImplFun;

  // Class definitions

  template <typename Result>
  class Function
  {
      typedef FunctionImpl<Result> Impl;

  public:

      Function ()
          : m_impl (0)
      {}

      Function (const Function& functor)
          : m_impl (functor.m_impl->clone ())
      {}

      explicit Function (std::auto_ptr<Impl> impl)
          : m_impl (impl)
      {}

      template <typename Functor>
      Function (const Functor& func)
          : m_impl (new FunctionImplFun<Functor, Result> (func))
      {}

      template <typename Object, typename MemFunction>
      Function (const Object& object, MemFunction func)
          : m_impl (new FunctionImplMem<Object, MemFunction, Result> (object, func))
      {}

      inline const Function& operator = (const Function& other)
      {
          m_impl = std::auto_ptr<Impl> (other.m_impl->clone ());
          return *this;
      }

      inline Result operator () ()
      {
          return (*m_impl) ();
      }

  private:

      std::auto_ptr<Impl> m_impl;
  };

  template <typename Result>
  class FunctionImpl
  {
  public:

      FunctionImpl ()
      {}

      virtual ~FunctionImpl ()
      {}

      virtual FunctionImpl *clone () = 0;

      virtual Result operator () () = 0;
  };

  template <typename Object, typename MemFunction, typename Result>
  class FunctionImplMem
      : public FunctionImpl<Result>
  {
  public:

      FunctionImplMem (const Object& object, MemFunction func)
          : m_object (object),
            m_func (func)
      {}

      virtual ~FunctionImplMem ()
      {}

      virtual FunctionImpl<Result> *clone ()
      {
          return new FunctionImplMem (*this);
      }

      virtual Result operator () () 
      { 
          return (m_object->*m_func) ();
      }
    
  private:

      Object m_object;
      MemFunction m_func;
  };

  template <typename Functor, typename Result>
  class FunctionImplFun
      : public FunctionImpl<Result>
  {
  public:

      explicit FunctionImplFun (const Functor& func)
          : m_func (func)
      {}

      virtual ~FunctionImplFun ()
      {}

      virtual FunctionImpl<Result> *clone ()
      {
          return new FunctionImplFun (*this);
      }

      virtual Result operator () () 
      {
          return m_func ();
      }

  private:

      Functor m_func;
  };

} // namespace Lv

#endif // #ifndef LVCPP_FUNCTION_HPP
