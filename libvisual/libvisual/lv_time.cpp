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

#define _POSIX_C_SOURCE 200112L

#include "config.h"
#include "lv_time.h"
#include "lv_common.h"
#include <thread>

#if defined(VISUAL_OS_WIN32)
// Performance counters, timer resolution, and sleep function
#include <windows.h>
#else
// High-resolution monotonic clock, and nanosleep (Android)
#include <time.h>
#endif

namespace LV {

#if defined(VISUAL_OS_WIN32)
  namespace {
    uint64_t perf_counter_freq;
  }
#endif

  class Timer::Impl
  {
  public:

    Time start;
    Time end;
    bool active;
  };

  void Time::init ()
  {
#if defined(VISUAL_OS_WIN32)
      LARGE_INTEGER freq;
      QueryPerformanceFrequency (&freq);
      perf_counter_freq = freq.QuadPart;
#endif

      // Increase timer resolution for fine-grained sleeps
#if defined(VISUAL_WITH_MINGW) && defined(ENABLE_WIN32_HIGH_RES_SLEEP)
      timeBeginPeriod (1);
#endif
  }

  void Time::deinit ()
  {
#if defined(VISUAL_WITH_MINGW) && defined(ENABLE_WIN32_HIGH_RES_SLEEP)
      timeEndPeriod (1);
#endif
  }

  Time Time::now ()
  {
#if defined(VISUAL_OS_WIN32)
      LARGE_INTEGER perf_counter;
      QueryPerformanceCounter (&perf_counter);

      return Time (perf_counter.QuadPart / perf_counter_freq,
                   ((perf_counter.QuadPart % perf_counter_freq) * VISUAL_NSECS_PER_SEC) / perf_counter_freq);
#else
      struct timespec clock_time;
      clock_gettime (CLOCK_MONOTONIC, &clock_time);

      return Time (clock_time.tv_sec, clock_time.tv_nsec);
#endif
  }

  void Time::usleep (uint64_t usecs)
  {
      // libstdc++'s sleep_for() requires the POSIX function
      // nanosleep() to work. This is a workaround using the Windows
      // Sleep() function.
  #if defined(VISUAL_WITH_MINGW)
      Sleep (usecs / VISUAL_USECS_PER_MSEC);
  #elif defined(VISUAL_OS_ANDROID)
      timespec request;
      request.tv_sec  = usecs / VISUAL_USECS_PER_SEC;
      request.tv_nsec = VISUAL_NSECS_PER_USEC * (usecs % VISUAL_USECS_PER_SEC);
      nanosleep (&request, nullptr);
  #else
      std::this_thread::sleep_for (std::chrono::microseconds (usecs));
  #endif
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

  Timer::Timer (Timer const& timer)
      : m_impl (new Impl (*timer.m_impl))
  {
      // empty
  }

  Timer& Timer::operator= (Timer const& rhs)
  {
      *m_impl = *rhs.m_impl;
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
