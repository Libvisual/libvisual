// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_time.hpp,v 1.2 2006-01-13 07:44:44 descender Exp $
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
#include <libvisual-cpp/lv_object.hpp>

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

  class Timer
      : public Object
  {
  public:

      Timer ()
          : Object (vis_timer_to_object (visual_timer_new ()))
      {}

      inline void reset ()
      {
          visual_timer_reset (&vis_timer ());
      }

      inline void start ()
      {
          visual_timer_start (&vis_timer ());
      }

      inline void stop ()
      {
          visual_timer_stop (&vis_timer ());
      }

      inline void cont ()
      {
          visual_timer_continue (&vis_timer ());
      }

      inline bool is_active () const
      {
          return visual_timer_is_active (const_cast<VisTimer *> (&vis_timer ()));
      }

      inline void elapsed (Time& time) const
      {
          visual_timer_elapsed (const_cast<VisTimer *> (&vis_timer ()),
                                &time.vis_time ());
      }

      inline int elasped_msecs () const
      {
          return visual_timer_elapsed_msecs (const_cast<VisTimer *> (&vis_timer ()));
      }

      inline bool has_passed (const Time& time) const
      {
          return visual_timer_has_passed (const_cast<VisTimer *> (&vis_timer ()),
                                          const_cast<VisTime *> (&time.vis_time ()));
      }

      inline bool has_passed (long sec, long usec) const
      {
          return visual_timer_has_passed_by_values (const_cast<VisTimer *> (&vis_timer ()),
                                                    sec, usec);
      }

      inline const VisTimer &vis_timer () const
      {
          return *reinterpret_cast<const VisTimer *> (&vis_object ());
      }

      inline VisTimer &vis_timer ()
      {
          return *reinterpret_cast<VisTimer *> (&vis_object ());
      }

  private:

      static inline VisObject *vis_timer_to_object (VisTimer *timer)
      {
          return reinterpret_cast<VisObject *> (timer);
      }

      Timer (const Timer& timer);
      const Timer& operator = (const Timer& rhs);
  };

} // namespace Lv

#endif // #ifndef LVCPP_TIME_HPP
