// Copyright (c) 2009 by Alexander Demin

#ifndef _LUASCRIPT_H
#define _LUASCRIPT_H

#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <memory>
#include <cstdio>

#include <luajit-2.1/lua.hpp>

class lua {
 public:
  lua();
  ~lua();

  class arg_t {
   public:
    virtual ~arg_t() {}
    virtual void unpack(lua_State* L, int nparam) = 0;
    virtual void pack(lua_State* L) = 0;
    virtual std::string asString() = 0;
    virtual arg_t* clone() const = 0;
  };

  class bool_arg_t: public arg_t {
   public:
    typedef bool value_type;

    bool_arg_t() : value_(0) {}
    explicit bool_arg_t(bool value) : value_(value) {}

    virtual arg_t* clone() const { return new bool_arg_t(value_); }
    virtual void unpack(lua_State* L, int nparam);
    virtual void pack(lua_State* L);
    std::string asString();
    bool& value() { return value_; }

   private:
    bool value_;
  };

  class int_arg_t: public arg_t {
   public:
    typedef double value_type;

    int_arg_t() : value_(0) {}
    explicit int_arg_t(double value) : value_(value) {}

    virtual arg_t* clone() const { return new int_arg_t(value_); }
    virtual void unpack(lua_State* L, int nparam);
    virtual void pack(lua_State* L);
    std::string asString();
    double& value() { return value_; }

   private:
    value_type value_;
  };

  class string_arg_t: public arg_t {
   private:
    std::string value_;
   public:
    typedef std::string value_type;

    string_arg_t() : value_() {}
    explicit string_arg_t(const std::string& value) : value_(value) {}

    virtual arg_t* clone() const { return new string_arg_t(value_); }
    virtual void unpack(lua_State* L, int nparam);
    virtual void pack(lua_State* L);
    std::string asString();
    std::string& value() { return value_; }
  };

  class args_t: public std::vector< arg_t* > {
   public:
    args_t() { clear(); }
    args_t(const args_t& rhs);
    virtual ~args_t();

    args_t* clone() const;
    void unpack(lua_State* L);
    void pack(lua_State* L);
    args_t& add(arg_t* arg);
  };

  template< class T >
  static int lua_callback(lua_State* L);

  class exception : public std::exception {
   public:
    explicit exception(const std::string& msg);
    virtual ~exception() throw() {}
    const char* what() const throw() { return msg_.c_str(); }
    int line() const throw() { return line_; }
    const std::string& error() const throw() { return error_; }
   private:
    std::string msg_;
    int line_;
    std::string error_;
  };

  void exec(const std::string& script);

  template< class T >
  T get_variable(const std::string& name);

  template< class T >
  void set_variable(const std::string& name,
                    const typename T::value_type& value);

  template< class T >
  void register_function();

  class deleter {
   public:
    template <typename T> void operator() (const T* p) const {
      delete p;
    }
  };

 protected:
  lua(const lua&);
  void operator=(const lua&);

 private:
  lua_State* L_;
};

template< class impl_t >
class lua_func_t {
 public:
  static const lua::args_t* in_args() {
    static lua::args_t* args = 0;
    if (!args)
      args = const_cast<lua::args_t *>(impl_t::in_args());
    return args;
  }

  static const lua::args_t* out_args() {
    static lua::args_t* args = 0;
    if (!args)
      args = const_cast<lua::args_t *>(impl_t::out_args());
    return args;
  }

  static const std::string ns() { return impl_t::ns(); }
  static const std::string name() { return impl_t::name(); }

  static void calc(const lua::args_t& in, lua::args_t& out) {
    impl_t::calc(in, out);
  }
};

template< class T >
int lua::lua_callback(lua_State* L) {
  int argc = lua_gettop(L);

  if (static_cast<int>(lua_func_t<T>::in_args()->size()) != argc) {
    std::stringstream fmt;
    fmt << "function '" << lua_func_t<T>::name() << "'"
        << " requires " << lua_func_t<T>::in_args()->size()
        << " arguments, but " << argc << " given";
    throw lua::exception(fmt.str());
  }

  std::unique_ptr<lua::args_t> in_args(lua_func_t<T>::in_args()->clone());
  in_args->unpack(L);

  std::unique_ptr<lua::args_t> out_args(lua_func_t<T>::out_args()->clone());
  lua_func_t<T>::calc(*in_args, *out_args);
  out_args->pack(L);
  return out_args->size();
}

template< class T >
void lua::register_function() {
  if (lua_func_t<T>::ns().length()) {
    exec(std::string("if ") + lua_func_t<T>::ns() + " == nil then " +
         lua_func_t<T>::ns() + " = {}; "
         "end");
    lua_register(L_, "dummy", lua_callback< lua_func_t<T> >);
    exec(lua_func_t<T>::ns() + "." +
         lua_func_t<T>::name().c_str() + " = dummy; dummy = nil");
  } else {
    lua_register(L_, lua_func_t<T>::name().c_str(),
                 lua_callback< lua_func_t<T> >);
  }
}

template< class T >
T lua::get_variable(const std::string& name) {
  lua_getglobal(L_, name.c_str());
  T value;
  int index = lua_gettop(L_);
  value.unpack(L_, index);
  lua_pop(L_, index);
  return value;
}

template< class T >
void lua::set_variable(const std::string& name,
                       const typename T::value_type& value) {
  T var(value);
  var.pack(L_);
  lua_setglobal(L_, name.c_str());
}

#endif
