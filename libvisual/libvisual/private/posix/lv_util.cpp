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

#define _DEFAULT_SOURCE

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
