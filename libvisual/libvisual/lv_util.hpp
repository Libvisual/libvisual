#ifndef _LV_UTIL_HPP
#define _LV_UTIL_HPP

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <filesystem>
#include <string>
#include <memory>
#include <functional>

namespace LV {

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

  LV_API bool for_each_file_in_dir (std::filesystem::path const&                       path,
                                    std::function<bool (std::filesystem::path const&)> filter,
                                    std::function<bool (std::filesystem::path const&)> func);

  /**
   * Returns a null-terminated C string from a std::string, or nullptr if string is empty
   *
   * @param string input string
   *
   * return C string, or nullptr
   */
  inline char const* string_to_c (std::string const& string)
  {
      return !string.empty () ? string.c_str () : nullptr;
  }

} // LV namespace

#endif // _LV_UTIL_HPP
