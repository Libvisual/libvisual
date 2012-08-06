#include "config.h"
#include "lv_util.hpp"
#include <windows.h>

namespace LV {

  void for_each_file_in_dir (std::string const&                       dir_path,
                             std::function<bool (std::string const&)> filter,
                             std::function<bool (std::string const&)> func)
  {
      auto pattern = dir_path + "/*";

      WIN32_FIND_DATA file_data;
      auto hList = FindFirstFile (pattern.c_str (), &file_data);

      if (hList == INVALID_HANDLE_VALUE) {
          FindClose (hList);
          return;
      }

      auto finished = false;

      while (!finished) {
          if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
              auto full_path = dir_path + "/" + file_data.cFileName;

              if (filter (full_path) && !func (full_path))
                  return false;
          }

          if (!FindNextFile (hList, &file_data)) {
              if (GetLastError () == ERROR_NO_MORE_FILES) {
                  finished = true;
              }
          }
      }

      FindClose (hList);

      return true;
  }

} // LV namespace
