#ifndef _LV_UTIL_HPP
#define _LV_UTIL_HPP

#include <string>

namespace LV {

  // This file should really not be existing; it's a collection of
  // utility classes and functions that exist in some form in Boost or
  // C++11

  /**
   * Checks if a string has a given suffix.
   *
   * @param str    string to check
   * @param suffix suffix to look for
   *
   * @return true if string has the given suffix, false otherwise
   */
  inline bool str_has_suffix (std::string const& str, std::string const& suffix)
  {
      if (str.length() >= suffix.length()) {
          return (str.compare (str.length() - suffix.length(), suffix.length(), suffix) == 0);
      } else {
          return false;
      }
  }

  /**
   * An conditional version of std::copy() that filters container
   * elements according to a given predicate.
   */
  // Why oh why did they have to leave this out of C++98 by accident?
  template <class InputIterator, class OutputIterator, class Predicate>
  OutputIterator copy_if (InputIterator first, InputIterator last,
                          OutputIterator result, Predicate pred)
  {
      while (first != last)
      {
          if (pred (*first))
              *result++ = *first;
          ++first;
      }
      return result;
  }

  template<class T>
  inline void checked_delete (T* x)
  {
      typedef char type_must_be_complete[ sizeof(T) ? 1 : -1 ];
      (void) sizeof (type_must_be_complete);
      delete x;
  }

  template <class Pair>
  typename Pair::first_type select1st (Pair const& pair)
  {
      return pair.first;
  }

  template <class Pair>
  typename Pair::second_type select2nd (Pair const& pair)
  {
      return pair.second;
  }

} // LV namespace

#endif // _LV_UTIL_HPP
