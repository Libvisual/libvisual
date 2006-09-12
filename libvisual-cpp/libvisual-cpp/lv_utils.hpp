// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2006 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_utils.hpp,v 1.3 2006-09-12 21:18:42 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LV_UTILS_HPP
#define LV_UTILS_HPP

#include <libvisual/lv_utils.h>

namespace Lv
{
  namespace Utils
  {
    inline bool is_power_of_2 (int n)
    {
        return visual_utils_is_power_of_2 (n);
    }

  } // namespace Utils

} // namespace Lv

#endif // #ifndef LV_UTILS_HPP

