// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_object.hpp,v 1.1 2005-09-01 03:22:05 descender Exp $
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

#ifndef LVCPP_OBJECT_HPP
#define LVCPP_OBJECT_HPP

#include <libvisual/lv_object.h>

namespace Lv
{

  class Object
  {
  public:

      Object ()
      {
          visual_object_initialize (&m_object, FALSE, NULL);
      }
  
      ~Object ()
      {
          visual_object_destroy (&m_object);
      }

      const VisObject &vis_object () const
      {
          return m_object;
      }

      VisObject &vis_object ()
      {
          return m_object;
      }

  private:

      VisObject m_object;
  };

} // namespace Lv

#endif // #ifdef LVCPP_OBJECT_HPP
