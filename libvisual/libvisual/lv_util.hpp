#ifndef _LV_UTIL_HPP
#define _LV_UTIL_HPP

#include <lvconfig.h>
#include <lv_defines.h>
#include <string>
#include <memory>
#include <functional>

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

  LV_API bool for_each_file_in_dir (std::string const&                       path,
                                    std::function<bool (std::string const&)> filter,
                                    std::function<bool (std::string const&)> func);

} // LV namespace

#endif // _LV_UTIL_HPP
