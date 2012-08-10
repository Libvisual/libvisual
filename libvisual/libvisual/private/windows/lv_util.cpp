/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_util.hpp"
#include <windows.h>

namespace LV {

  bool for_each_file_in_dir (std::string const&                       dir_path,
                             std::function<bool (std::string const&)> filter,
                             std::function<bool (std::string const&)> func)
  {
      auto pattern = dir_path + "/*";

      WIN32_FIND_DATA file_data;
      auto hList = FindFirstFile (pattern.c_str (), &file_data);

      if (hList == INVALID_HANDLE_VALUE) {
          FindClose (hList);
          return false;
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
