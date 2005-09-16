// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_time.hpp,v 1.1 2005-09-16 20:09:53 descender Exp $
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

#ifndef LVCPP_TIME_HPP
#define LVCPP_TIME_HPP

#include <libvisual/lv_time.h>
#include <lv_object.hpp>

namespace Lv
{

  class Time
      : public Object
  {
  public:
      
      static inline void usleep (unsigned long ms)
      {
          // TODO: throw exception if visual_time_usleep() fails
          visual_time_usleep (ms);
      }

      Time ()
          : Object (vis_time_to_object (visual_time_new ()))
      {
          set_current ();
      }

      explicit Time (long sec, long usec = 0)
          : Object (vis_time_to_object (visual_time_new ()))
      {
          set (sec, usec);
      }

      Time (const Time& other)
          : Object (vis_time_to_object (visual_time_new ()))
      {
          visual_time_copy (&vis_time (),
                             const_cast<VisTime *> (&other.vis_time ()));
      }

      inline const Time& operator = (const Time& other)
      {
          visual_time_copy (&vis_time (),
                             const_cast<VisTime *> (&other.vis_time ()));
      }

      inline void set (long sec, long usec = 0)
      {
          visual_time_set (&vis_time (), sec, usec);
      }

      inline void set_current ()
      {
          visual_time_get (&vis_time ());
      }

      inline long& sec  () { return vis_time ().tv_sec; }
      inline long& usec () { return vis_time ().tv_usec; }

      inline long get_sec  () const { return vis_time ().tv_sec; }
      inline long get_usec () const { return vis_time ().tv_usec; }

      inline Time operator - (const Time& rhs)
      {
          visual_time_difference (&vis_time (), &vis_time (),
                                  const_cast<VisTime *> (&rhs.vis_time ()));
          return *this;
      }

      inline bool operator < (const Time& rhs)
      {
          return visual_time_past (const_cast<VisTime *> (&rhs.vis_time ()),
                                   &vis_time ());
      }

      inline bool operator > (const Time& rhs)
      {
          return visual_time_past (&vis_time (),
                                   const_cast<VisTime *> (&rhs.vis_time ()));
      }

      inline const VisTime &vis_time () const
      {
          return *reinterpret_cast<const VisTime *> (&vis_object ());
      }

      inline VisTime &vis_time ()
      {
          return *reinterpret_cast<VisTime *> (&vis_object ());
      }

  private:

      static inline VisObject *vis_time_to_object (VisTime *time)
      {
          return reinterpret_cast<VisObject *> (time);
      }
  };

} // namespace Lv

#endif // #ifndef LVCPP_TIME_HPP
