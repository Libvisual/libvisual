// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_function.hpp,v 1.10 2005-09-26 13:28:07 descender Exp $
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

// TODO (highest priority first)
// * wrap parameter types so that structs and classes are
//   passed by reference
// * add comparators == and !=
// * eliminate virtual calls
// * allow custom allocators for FunctionImpl

// Implementation notes:
// * the 'virtual' keyword must be omitted for the () operators in
//   FunctionImplMem and FunctionImplFun because it will force the
//   compiler to instantiate them

namespace Lv
{
  // Forward declarations

  template <typename Result, class ParamList>
  class Function;

  template <typename Result, class ParamList>
  class FunctionImpl;

  template <class Object, typename MemFunction, class BaseFunctor>
  class FunctionImplMem;

  template <typename Functor, class BaseFunctor>
  class FunctionImplFun;


  // Class definitions

  template <typename R, class PList = Typelist::Null>
  class Function
  {
      typedef FunctionImpl<R, PList> Impl;

  public:

      typedef R Result;
      typedef PList ParamList;

      typedef typename Typelist::Nth<ParamList, 0>::Result Param1;
      typedef typename Typelist::Nth<ParamList, 1>::Result Param2;
      typedef typename Typelist::Nth<ParamList, 2>::Result Param3;
      typedef typename Typelist::Nth<ParamList, 3>::Result Param4;
      typedef typename Typelist::Nth<ParamList, 4>::Result Param5;

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
          : m_impl (new FunctionImplFun<Functor, Function> (func))
      {}

      template <class Object, typename MemFunction>
      Function (const Object& object, MemFunction func)
          : m_impl (new FunctionImplMem<Object, MemFunction, Function> (object, func))
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

      inline Result operator () (Param1 arg1)
      {
          return (*m_impl) (arg1);
      }

      inline Result operator () (Param1 arg1, Param2 arg2)
      {
          return (*m_impl) (arg1, arg2);
      }

      inline Result operator () (Param1 arg1, Param2 arg2, Param3 arg3)
      {
          return (*m_impl) (arg1, arg2, arg3);
      }

      inline Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                                 Param4 arg4)
      {
          return (*m_impl) (arg1, arg2, arg3, arg4);
      }

      inline Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                                 Param4 arg4, Param5 arg5)
      {
          return (*m_impl) (arg1, arg2, arg3, arg4, arg5);
      }

  private:

      std::auto_ptr<Impl> m_impl;
  };


  template <typename Result>
  class FunctionImpl<Result, LVCPP_TYPELIST_0>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () const = 0;
      virtual Result operator () () = 0;
  };

  template <typename Result, typename Param1>
  class FunctionImpl<Result, LVCPP_TYPELIST_1(Param1)>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () const = 0;
      virtual Result operator () (Param1) = 0;
  };

  template <typename Result, typename Param1, typename Param2>
  class FunctionImpl<Result, LVCPP_TYPELIST_2(Param1, Param2)>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () const = 0;
      virtual Result operator () (Param1, Param2) = 0;
  };

  template <typename Result, typename Param1, typename Param2,
            typename Param3>
  class FunctionImpl<Result, LVCPP_TYPELIST_3(Param1, Param2, Param3)>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () = 0;
      virtual Result operator () (Param1, Param2, Param3) = 0;
  };

  template <typename Result, typename Param1, typename Param2,
            typename Param3, typename Param4>
  class FunctionImpl<Result, LVCPP_TYPELIST_4(Param1, Param2, Param3,
                                              Param4)>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () const = 0;
      virtual Result operator () (Param1, Param2, Param3, Param4) = 0;
  };

  template <typename Result, typename Param1, typename Param2,
            typename Param3, typename Param4, typename Param5>
  class FunctionImpl<Result, LVCPP_TYPELIST_5(Param1, Param2, Param3,
                                              Param4, Param5)>
  {
  public:

      virtual ~FunctionImpl () {}
      virtual FunctionImpl *clone () const = 0;
      virtual Result operator () (Param1, Param2, Param3, Param4, Param5) = 0;
  };


  template <class Object, typename MemFunction, class BaseFunctor>
  class FunctionImplMem
      : public FunctionImpl<typename BaseFunctor::Result,
                            typename BaseFunctor::ParamList>
  {
  public:

      typedef typename BaseFunctor::Result Result;
      typedef typename BaseFunctor::Param1 Param1;
      typedef typename BaseFunctor::Param2 Param2;
      typedef typename BaseFunctor::Param3 Param3;
      typedef typename BaseFunctor::Param4 Param4;
      typedef typename BaseFunctor::Param5 Param5;

      FunctionImplMem (const Object& object, MemFunction func)
          : m_object (object),
            m_func (func)
      {}

      virtual ~FunctionImplMem ()
      {}

      virtual FunctionImplMem *clone () const
      {
          return new FunctionImplMem (*this);
      }

      Result operator () ()
      {
          return (m_object->*m_func) ();
      }

      Result operator () (Param1 arg1)
      {
          return (m_object->*m_func) (arg1);
      }

      Result operator () (Param1 arg1, Param2 arg2)
      {
          return (m_object->*m_func) (arg1, arg2);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3)
      {
          return (m_object->*m_func) (arg1, arg2, arg3);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                          Param4 arg4)
      {
          return (m_object->*m_func) (arg1, arg2, arg3, arg4);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                          Param4 arg4, Param5 arg5)
      {
          return (m_object->*m_func) (arg1, arg2, arg3, arg4, arg5);
      }

  private:

      Object m_object;
      MemFunction m_func;
  };


  template <typename Functor, class BaseFunctor>
  class FunctionImplFun
      : public FunctionImpl<typename BaseFunctor::Result,
                            typename BaseFunctor::ParamList>
  {
  public:

      typedef typename BaseFunctor::Result Result;
      typedef typename BaseFunctor::Param1 Param1;
      typedef typename BaseFunctor::Param2 Param2;
      typedef typename BaseFunctor::Param3 Param3;
      typedef typename BaseFunctor::Param4 Param4;
      typedef typename BaseFunctor::Param5 Param5;

      explicit FunctionImplFun (const Functor& func)
          : m_func (func)
      {}

      virtual ~FunctionImplFun ()
      {}

      virtual FunctionImplFun *clone () const
      {
          return new FunctionImplFun (*this);
      }

      Result operator () ()
      {
          return m_func ();
      }

      Result operator () (Param1 arg1)
      {
          return m_func (arg1);
      }

      Result operator () (Param1 arg1, Param2 arg2)
      {
          return m_func (arg1, arg2);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3)
      {
          return m_func (arg1, arg2, arg3);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                          Param4 arg4)
      {
          return m_func (arg1, arg2, arg3, arg4);
      }

      Result operator () (Param1 arg1, Param2 arg2, Param3 arg3,
                          Param4 arg4, Param5 arg5)
      {
          return m_func (arg1, arg2, arg3, arg4, arg5);
      }

  private:

      Functor m_func;
  };

} // namespace Lv

#endif // #ifndef LVCPP_FUNCTION_HPP
