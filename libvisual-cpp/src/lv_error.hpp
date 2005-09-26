// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_error.hpp,v 1.1 2005-09-26 14:06:06 descender Exp $
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

#ifndef LVCPP_ERROR_HPP
#define LVCPP_ERROR_HPP

#include <stdexcept>

namespace Lv
{

  class UnexpectedError
      : public std::runtime_error
  {
  public:

      UnexpectedError ()
          : std::runtime_error ("Unexpected exception")
      {}
  };

  class NullObjectError
      : public std::runtime_error
  {
  public:

      NullObjectError ()
          : std::runtime_error ("Attempt to use null object")
      {}
  };

  class FileError
      : public std::runtime_error
  {
  public:

      FileError (const std::string& error)
          : std::runtime_error ("File error: " + error)
      {}
  };

} // namespace Lv

#endif // #ifndef LVCPP_ERROR_HPP
