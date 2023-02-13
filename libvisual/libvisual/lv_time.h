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

#ifndef _LV_TIME_H
#define _LV_TIME_H

#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>
#include <time.h>

/**
 * @defgroup VisTime VisTime
 * @{
 */

#define VISUAL_NSECS_PER_SEC    1000000000
#define VISUAL_USECS_PER_SEC    1000000
#define VISUAL_MSECS_PER_SEC    1000
#define VISUAL_USECS_PER_MSEC   1000
#define VISUAL_NSECS_PER_MSEC   1000000
#define VISUAL_NSECS_PER_USEC   1000

#ifdef __cplusplus

#include <memory>
#include <cmath>

namespace LV {

  //! Encodes time.
  class LV_API Time
  {
  public:

      //! seconds
      long sec;

      //! nanoseconds
      long nsec;

      explicit Time (long sec_ = 0, long nsec_ = 0)
          : sec  (sec_)
          , nsec (nsec_)
      {}

      static Time from_secs (double secs)
      {
          double int_part, frac_part;
          frac_part = std::modf (secs, &int_part);

          return Time (int_part, frac_part * VISUAL_NSECS_PER_SEC);
      }

      static Time from_msecs (uint64_t msecs)
      {
          return Time (msecs / VISUAL_MSECS_PER_SEC,
                       (msecs % VISUAL_MSECS_PER_SEC) * VISUAL_NSECS_PER_MSEC);
      }

      static Time from_usecs (uint64_t usecs)
      {
          return Time (usecs / VISUAL_USECS_PER_SEC,
                       (usecs % VISUAL_USECS_PER_SEC) * VISUAL_NSECS_PER_USEC);
      }

      static Time now ();

      friend Time operator- (Time const& lhs, Time const& rhs)
      {
          Time diff (lhs);
          diff -= rhs;

          return diff;
      }

      Time& operator-= (Time const& rhs)
      {
          sec  -= rhs.sec;
          nsec -= rhs.nsec;

          if (nsec < 0) {
              sec--;
              nsec += VISUAL_NSECS_PER_SEC;
          }

          return *this;
      }

      friend bool operator== (Time const& lhs, Time const& rhs)
      {
          return lhs.sec == rhs.sec && lhs.nsec == rhs.nsec;
      }

      friend bool operator!= (Time const& lhs, Time const& rhs)
      {
          return !(lhs == rhs);
      }

      friend bool operator>= (Time const& lhs, Time const& rhs)
      {
          return lhs.sec >= rhs.sec;
      }

      friend bool operator<= (Time const& lhs, Time const& rhs)
      {
          return rhs >= lhs;
      }

      friend bool operator> (Time const& lhs, Time const& rhs)
      {
          return (lhs.sec > rhs.sec) || (lhs.sec == rhs.sec && lhs.nsec > rhs.nsec);
      }

      friend bool operator< (Time const& lhs, Time const& rhs)
      {
          return rhs > lhs;
      }

      //! Converts the time to seconds
      double to_secs () const
      {
          return sec + nsec * (1.0 / VISUAL_NSECS_PER_SEC);
      }

      //! Converts the time to milliseconds
      uint64_t to_msecs () const
      {
          return uint64_t (sec) * VISUAL_MSECS_PER_SEC + nsec / VISUAL_NSECS_PER_MSEC;
      }

      //! Converts the time to microseconds
      uint64_t to_usecs () const
      {
          return uint64_t (sec) * VISUAL_USECS_PER_SEC + nsec / VISUAL_NSECS_PER_USEC;
      }

      //! Sleeps for a period of time. This will yield the calling thread.
      static void usleep (uint64_t usecs);
  };

  class LV_API Timer
  {
  public:

      //! Creates a new Timer
      Timer ();

      //! Copy constructor
      Timer (Timer const& timer);

      //! Move constructor
      Timer (Timer&& rhs);

      //! Destructor
      ~Timer ();

      //! Copy assignment operator
      Timer& operator= (Timer const& rhs);

      //! Move assignment operator
      Timer& operator= (Timer&& rhs);

      //! Checks if the timer is active.
      bool is_active () const;

      //! Resets the timer
      void reset ();

      //! Starts the timer.
      void start ();

      //! Stops the timer.
      void stop ();

      Time get_start_time () const;

      Time get_end_time () const;

      //! Returns the length of time since the timer was started
      Time elapsed () const;

      bool is_past (Time const& age) const;

  private:

      class Impl;
      std::unique_ptr<Impl> m_impl;
  };

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus

typedef LV::Time  VisTime;
typedef LV::Timer VisTimer;

#else

typedef struct _VisTime VisTime;
struct _VisTime;

typedef struct _VisTimer VisTimer;
struct _VisTimer;

#endif /* __cplusplus */

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisTime *visual_time_new             (void);
LV_NODISCARD LV_API VisTime *visual_time_new_now         (void);
LV_NODISCARD LV_API VisTime *visual_time_new_with_values (long sec, long nsec);
LV_NODISCARD LV_API VisTime *visual_time_clone           (VisTime *src);

LV_API void visual_time_free (VisTime *time_);

LV_API void visual_time_set     (VisTime *time_, long sec, long usec);
LV_API void visual_time_copy    (VisTime *dest, VisTime *src);
LV_API void visual_time_get_now (VisTime *time_);

LV_API void visual_time_diff    (VisTime *diff, VisTime *time1, VisTime *time2);
LV_API int  visual_time_is_past (VisTime *time_, VisTime *ref);

LV_API double   visual_time_to_secs  (VisTime *time_);
LV_API uint64_t visual_time_to_msecs (VisTime *time_);
LV_API uint64_t visual_time_to_usecs (VisTime *time_);

LV_API void visual_usleep (uint64_t usecs);

LV_API void visual_time_set_from_msecs (VisTime *time_, uint64_t msecs);

LV_NODISCARD LV_API VisTimer *visual_timer_new (void);

LV_API void visual_timer_free (VisTimer *timer);

LV_API void visual_timer_reset     (VisTimer *timer);
LV_API void visual_timer_start     (VisTimer *timer);
LV_API void visual_timer_stop      (VisTimer *timer);
LV_API void visual_timer_resume    (VisTimer *timer);
LV_API int  visual_timer_is_active (VisTimer *timer);

LV_API void     visual_timer_elapsed (VisTimer *timer, VisTime *time_);
LV_API uint64_t visual_timer_elapsed_msecs (VisTimer *timer);
LV_API uint64_t visual_timer_elapsed_usecs (VisTimer *timer);
LV_API double   visual_timer_elapsed_secs  (VisTimer *timer);

LV_API int visual_timer_is_past  (VisTimer *timer, VisTime *time_);
LV_API int visual_timer_is_past2 (VisTimer *timer, long sec, long nsec);

// FIXME: Remove this
//#define visual_time_get_now() (clock() / (float)CLOCKS_PER_SEC * 1000)

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_TIME_H */
