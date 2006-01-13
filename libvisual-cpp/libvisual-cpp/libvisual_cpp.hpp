// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: libvisual_cpp.hpp,v 1.2 2006-01-13 07:44:44 descender Exp $
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

#ifndef LVCPP_LVCPP_HPP
#define LVCPP_LVCPP_HPP

#include <libvisual-cpp/lv_error.hpp>
#include <string>

namespace Lv
{
  const char *get_version ();

  const char *get_lv_version ();

  int init (int &argc, char **&argv, bool trap_exceptions = true);

  int init (bool trap_exceptions = true);

  bool is_init ();

  int quit ();

  int init_path_add (const std::string& path);

} // namespace Lv

#endif // #ifdef LVCPP_LVCPP_HPP
