#ifndef _LV_ENABLE_IF_HPP
#define _LV_ENABLE_IF_HPP

namespace LV {

  template <bool cond, typename T = void>
  struct enable_if_c { typedef T type; };

  template <typename T>
  struct enable_if_c<false, T> {};

  template <class Cond, typename T = void>
  struct enable_if : public enable_if_c<Cond::value, T> {};

  template <bool cond, typename T = void>
  struct disable_if_c { typedef T type; };

  template <typename T>
  struct disable_if_c<true, T> {};

  template <class Cond, typename T = void>
  struct disable_if : public disable_if_c<Cond::value, T> {};

} // LV namespace

#endif // _LV_ENABLE_IF_HPP
