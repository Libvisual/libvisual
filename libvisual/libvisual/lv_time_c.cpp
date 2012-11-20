#include "config.h"
#include "lv_time.h"
#include "lv_common.h"
//#include <thread>
#include <unistd.h>

extern "C" {

  VisTime *visual_time_new ()
  {
      return new LV::Time;
  }

  VisTime *visual_time_new_now ()
  {
      return new LV::Time (LV::Time::now ());
  }

  VisTime *visual_time_new_with_values (long sec, long nsec)
  {
      return new LV::Time (sec, nsec);
  }

  VisTime *visual_time_clone (VisTime *src)
  {
      visual_return_val_if_fail (src != nullptr, nullptr);

      return new LV::Time (*src);
  }

  void visual_time_free (VisTime *self)
  {
      delete self;
  }

  void visual_time_set (VisTime *self, long sec, long nsec)
  {
      visual_return_if_fail (self != nullptr);

      self->sec  = sec;
      self->nsec = nsec;
  }

  void visual_time_copy (VisTime *self, VisTime *src)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (src  != nullptr);

      self->sec  = src->sec;
      self->nsec = src->nsec;
  }

  void visual_time_get_now (VisTime *self)
  {
      visual_return_if_fail (self != nullptr);

      *self = LV::Time::now ();
  }

  void visual_time_diff (VisTime *diff, VisTime *time1, VisTime *time2)
  {
      visual_return_if_fail (diff  != nullptr);
      visual_return_if_fail (time1 != nullptr);
      visual_return_if_fail (time2 != nullptr);

      *diff = *time1 - *time2;
  }

  int visual_time_is_past (VisTime *self, VisTime *ref)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);
      visual_return_val_if_fail (ref  != nullptr, FALSE);

      return *self > *ref;
  }

  double visual_time_to_secs  (VisTime *self)
  {
      visual_return_val_if_fail (self != nullptr, 0.0);

      return self->to_secs ();
  }

  uint64_t visual_time_to_msecs (VisTime *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->to_msecs ();
  }

  uint64_t visual_time_to_usecs (VisTime *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->to_usecs ();
  }

  void visual_usleep (uint64_t usecs)
  {
      LV::Time::usleep (usecs);
  }

  VisTimer *visual_timer_new ()
  {
      return new LV::Timer;
  }

  void visual_timer_free (VisTimer *self)
  {
      delete self;
  }

  void visual_timer_reset (VisTimer *self)
  {
      visual_return_if_fail (self != nullptr);

      self->reset ();
  }

  void visual_timer_start (VisTimer *self)
  {
      visual_return_if_fail (self != nullptr);

      self->start ();
  }

  void visual_timer_stop (VisTimer *self)
  {
      visual_return_if_fail (self != nullptr);

      self->stop ();
  }

  int visual_timer_is_active (VisTimer *self)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);

      return self->is_active ();
  }

  void visual_timer_elapsed (VisTimer *self, VisTime *time)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (time != nullptr);

      *time = self->elapsed ();
  }

  double visual_timer_elapsed_secs (VisTimer *self)
  {
      visual_return_val_if_fail (self != nullptr, 0.0);

      return self->elapsed ().to_secs ();
  }

  uint64_t visual_timer_elapsed_msecs (VisTimer *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->elapsed ().to_msecs ();
  }

  uint64_t visual_timer_elapsed_usecs (VisTimer *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->elapsed ().to_usecs ();
  }

  int visual_timer_is_past  (VisTimer *self, VisTime *time)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);

      return self->elapsed() > *time;
  }

  int visual_timer_is_past2 (VisTimer *self, long sec, long nsec)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);

      return self->elapsed () > LV::Time (sec, nsec);
  }

} // C extern
