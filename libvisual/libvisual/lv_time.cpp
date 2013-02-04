/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
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
#include "lv_time.h"
#include "private/lv_time_system.hpp"
#include "lv_common.h"

namespace LV {

  class Timer::Impl
  {
  public:

    Time start;
    Time end;
    bool active;
  };

  Time Time::now ()
  {
      return TimeSystem::now ();
  }

  void Time::usleep (uint64_t usecs)
  {
      TimeSystem::usleep (usecs);
  }

  Timer::Timer ()
      : m_impl (new Impl)
  {
      reset ();
  }

  Timer::~Timer ()
  {
      // empty
  }

  Timer::Timer (Timer const& rhs)
      : m_impl {new Impl (*rhs.m_impl)}
  {
      // nothing
  }

  Timer::Timer (Timer&& rhs)
      : m_impl {std::move (rhs.m_impl)}
  {
      // nothing
  }

  Timer& Timer::operator= (Timer const& rhs)
  {
      *m_impl = *rhs.m_impl;
      return *this;
  }

  Timer& Timer::operator= (Timer&& rhs)
  {
      m_impl.swap (rhs.m_impl);
      return *this;
  }

  void Timer::reset ()
  {
      m_impl->start = m_impl->end = Time ();
      m_impl->active = false;
  }

  bool Timer::is_active () const
  {
      return m_impl->active;
  }

  void Timer::start ()
  {
      m_impl->start  = Time::now ();
      m_impl->active = true;
  }

  void Timer::stop ()
  {
      m_impl->end    = Time::now ();
      m_impl->active = false;
  }

  Time Timer::get_start_time () const
  {
      return m_impl->start;
  }

  Time Timer::get_end_time () const
  {
      return m_impl->end;
  }

  Time Timer::elapsed () const
  {
      if (is_active ())
          return Time::now () - m_impl->start;
      else
          return m_impl->end - m_impl->start;
  }

  bool Timer::is_past (Time const& age) const
  {
      return elapsed () > age;
  }

} // LV namespace
