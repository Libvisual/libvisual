// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_thread.cpp,v 1.2 2006-01-13 07:44:44 descender Exp $
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

#include <lv_build_config.hpp>
#include "lv_thread.hpp"

namespace Lv
{
  void *Thread::invoke_functor (void *params)
  {
      Lv::Function<void> *m_func = static_cast<Lv::Function<void> *> (params);

      (*m_func) ();

      visual_thread_exit (NULL);

      // Used to shut GCC up
      return NULL;
  }
}
