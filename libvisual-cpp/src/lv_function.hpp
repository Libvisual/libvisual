#ifndef LVCPP_FUNCTION_HPP
#define LVCPP_FUNCTION_HPP

#include <memory>

// This design is largely inspired (read: taken) from the callback
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
  // TODO: 
  // * allow variable number of arguments
  // * add comparators == and !=

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

      Function& operator = (const Function& other)
      {
          m_impl = std::auto_ptr<Impl> (other.m_impl->clone ());
          return *this;
      }

      Result operator () ()
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

      FunctionImplMem (const Object& object, MemFunction func) : 
          m_object (object),
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

#endif
