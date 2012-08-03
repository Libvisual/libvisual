#ifndef _LV_UTIL_HPP
#define _LV_UTIL_HPP

#include <string>
#include <memory>

namespace LV {

  template<typename T, typename ...Args>
  std::unique_ptr<T> make_unique( Args&& ...args )
  {
      return std::unique_ptr<T> (new T (std::forward<Args> (args)... ));
  }

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

} // LV namespace

#endif // _LV_UTIL_HPP
