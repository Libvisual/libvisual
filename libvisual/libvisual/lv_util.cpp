/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2023 Libvisual team
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
#include <filesystem>
#include <set>

namespace LV {

  namespace fs = std::filesystem;

  bool for_each_file_in_dir (fs::path const&                       dir_path,
                             std::function<bool (fs::path const&)> filter,
                             std::function<bool (fs::path const&)> func)
  {
      std::set<fs::path> sorted_paths;

      try {
          for (auto const& entry : fs::directory_iterator (dir_path)) {
              sorted_paths.insert (entry.path());
          }

          for (auto const& entry : sorted_paths) {
              if (filter (entry) and (!func (entry))) {
                  return false;
              }
          }

          return true;
      }
      catch (fs::filesystem_error& error) {
          return false;
      }
  }

} // LV namespace
