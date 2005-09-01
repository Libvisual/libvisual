// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_object.cpp,v 1.2 2005-09-01 04:48:16 descender Exp $
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
#include <lv_object.hpp>
#include <cstdlib>


#ifdef LVCPP_OBJECT_TEST

#include <libvisual_cpp.hpp>
#include <iostream>

int test_visual_object_dtor (VisObject *object)
{
    char *privates = static_cast<char *> (visual_object_get_private (object));

    std::cout << "Stored private data: " << privates << "\n";

    return VISUAL_OK;
}

int main (int argc, char **argv)
{
    Lv::init (argc, argv);

    std::cout << "Lv::Object test\n";

    {
        Lv::Object  object;
        VisObject  *vis_object = &object.vis_object ();

        visual_object_ref (vis_object);
        visual_object_unref (vis_object);

        visual_object_set_dtor (vis_object, test_visual_object_dtor);
        visual_object_set_private (vis_object, const_cast <void *> (static_cast<const void *> ("hello world!")));
    }

    Lv::quit ();

    return 0;
}

#endif // #ifdef LV_CPP_OBJECT_TEST
