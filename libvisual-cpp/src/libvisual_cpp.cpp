// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: libvisual_cpp.cpp,v 1.5 2005-09-11 12:39:33 descender Exp $
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

#include <lv_config.hpp>
#include <libvisual_cpp.hpp>
#include <libvisual/libvisual.h>
#include <iostream>

namespace
{
  bool trap_exceptions = false;
  
  void (*last_terminate_handler) () = 0;
  void (*last_unexpected_handler) () = 0;

  void handle_unexpected ()
  {
      std::cerr << "Unexpected exception caught" << std::endl;
      throw Lv::UnexpectedError ();
  }

  void handle_terminate ()
  {
      std::cerr << "Uncaught exception, aborting program" << std::endl;
      std::abort ();
  }

  void set_exception_handlers ()
  {
      last_unexpected_handler = std::set_unexpected (handle_unexpected);
      last_terminate_handler = std::set_terminate (handle_terminate);
  }

  void unset_exception_handlers ()
  {
      std::set_unexpected (last_unexpected_handler);
      last_unexpected_handler = 0;

      std::set_terminate (last_terminate_handler);
      last_terminate_handler = 0;
  }

  void init_common (bool trap_exceptions_)
  {
      trap_exceptions = trap_exceptions_;
      if (trap_exceptions)
          set_exception_handlers ();
  }
}

namespace Lv
{
  const char *get_version ()
  {
      return VERSION;
  }

  const char *get_lv_version ()
  {
      return visual_get_version ();
  }

  int init (int& argc, char **& argv, bool trap_exceptions_)
  {
      int result = visual_init (&argc, &argv);

      if (result == VISUAL_OK)
          init_common (trap_exceptions_);

      return result;
  }

  int init (bool trap_exceptions_)
  {
      int result = visual_init (NULL, NULL);

      if (result == VISUAL_OK)
          init_common (trap_exceptions_);

      return result;      
  }

  bool is_init ()
  {
      return visual_is_initialized ();
  }

  int quit ()
  {
      if (is_init () && trap_exceptions)
          unset_exception_handlers ();

      return visual_quit ();
  }

  int init_path_add (const std::string& path)
  {
      return visual_init_path_add (const_cast<char *> (path.c_str ()));
  }

} // namespace Lv


#ifdef LIBVISUAL_CPP_TEST

void throw_unexpected_error ()
    throw ()
{
    throw std::runtime_error ("throw unexpected exception");
}

int main (int argc, char **argv)
{
    std::cout << "Libvisual version: " << Lv::get_lv_version () << "\n";

    // startup test
    std::cout << "Startup test\n";

    try
    {
        if (Lv::init (argc, argv) != VISUAL_OK)
            throw std::runtime_error ("Lv::init (argc, argv) fail");

        if (!Lv::is_init ())
            throw std::runtime_error ("Lv::is_init () returns false");

        if (Lv::quit () != VISUAL_OK)
            throw std::runtime_error ("Lv::quit () fail");

        if (Lv::init () != VISUAL_OK)
            throw std::runtime_error ("Lv::init (NULL, NULL) fail");

        if (!Lv::is_init ())
            throw std::runtime_error ("Lv::is_init () returns false");
    }
    catch (std::runtime_error& error)
    {
        std::cout << error.what () << std::endl;
    }

    // uncaught exception test

    try
    {
        std::cout << "Unexpected exception test\n";
        throw_unexpected_error ();
    }
    catch (Lv::UnexpectedError& error)
    {
        std::cout << "UnexpectedError caught\n";
        std::cout << error.what () << std::endl;
    }

    std::cout << "Uncaught exception test\n";
    throw std::runtime_error ("test uncaught exception");
}

#endif // #ifdef LIBVISUAL_CPP_TEST
