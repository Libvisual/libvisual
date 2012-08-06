#define _SVID_SOURCE

#include "config.h"
#include "lv_util.hpp"
#include <cstdlib>
#include <dirent.h>

namespace LV {

  bool for_each_file_in_dir (std::string const&                       dir_path,
                             std::function<bool (std::string const&)> filter,
                             std::function<bool (std::string const&)> func)
  {
      // NOTE: This typecast is needed for glibc versions that define
      // alphasort() as taking const void * arguments
      typedef int (*ScandirCompareFunc) (const struct dirent **, const struct dirent **);

      struct dirent **namelist;

      auto n = scandir (dir_path.c_str (), &namelist, nullptr, ScandirCompareFunc (alphasort));
      if (n < 0)
          return {};

      // First two entries are '.' and '..'
      std::free (namelist[0]);
      std::free (namelist[1]);

      for (auto i = 2; i < n; i++) {
          auto full_path = dir_path + "/" + namelist[i]->d_name;

          if (filter (full_path) and (!func (full_path)))
              return false;

          std::free (namelist[i]);
      }

      std::free (namelist);

      return true;
  }

} // LV namespace
