/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_time.c,v 1.29 2006/01/23 22:32:42 synap Exp $
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
#include <time.h>
#include <errno.h>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>

static uint64_t perf_counter_freq;
#endif

void visual_time_initialize (void)
{
#if defined(VISUAL_OS_WIN32)
       LARGE_INTEGER freq;
       QueryPerformanceFrequency (&freq);
       perf_counter_freq = freq.QuadPart;
#endif
}

VisTime *visual_time_new (void)
{
	return visual_mem_new0 (VisTime, 1);
}

int visual_time_init (VisTime *time_)
{
	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	time_->sec = time_->nsec = 0;

	return VISUAL_OK;
}

int visual_time_get (VisTime *time_)
{
#if defined(VISUAL_OS_WIN32)
	LARGE_INTEGER perf_counter;

	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	QueryPerformanceCounter (&perf_counter);
	time_->sec  = perf_counter.QuadPart / perf_counter_freq;
	time_->nsec = ((perf_counter.QuadPart % perf_counter_freq) * VISUAL_NSEC_PER_SEC) / perf_counter_freq;
#else
	struct timespec clock_time;

	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	clock_gettime (CLOCK_MONOTONIC, &clock_time);
	time_->sec  = clock_time.tv_sec;
	time_->nsec = clock_time.tv_nsec;
#endif

	return VISUAL_OK;
}

long visual_time_get_msecs (VisTime *time_)
{
	visual_return_val_if_fail (time_ != NULL, 0);

	return time_->sec * VISUAL_MSEC_PER_SEC + time_->nsec / VISUAL_NSEC_PER_MSEC;
}

int visual_time_set (VisTime *time_, long sec, long nsec)
{
	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	time_->sec  = sec;
	time_->nsec = nsec;

	return VISUAL_OK;
}

int visual_time_set_from_msecs (VisTime *time_, long msec)
{
	visual_return_val_if_fail(time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	long sec  = msec / VISUAL_MSEC_PER_SEC;
	long nsec = (msec % VISUAL_MSEC_PER_SEC) * VISUAL_NSEC_PER_MSEC;

	visual_time_set (time_, sec, nsec);

	return VISUAL_OK;
}

int visual_time_copy (VisTime *dest, VisTime *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_return_val_if_fail (src  != NULL, -VISUAL_ERROR_TIME_NULL);

	dest->sec  = src->sec;
	dest->nsec = src->nsec;

	return VISUAL_OK;
}

int visual_time_difference (VisTime *dest, VisTime *time1, VisTime *time2)
{
	visual_return_val_if_fail (dest  != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_return_val_if_fail (time1 != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_return_val_if_fail (time2 != NULL, -VISUAL_ERROR_TIME_NULL);

	dest->sec  = time2->sec  - time1->sec;
	dest->nsec = time2->nsec - time1->nsec;

	if (dest->nsec < 0) {
		dest->sec--;
		dest->nsec += VISUAL_NSEC_PER_SEC;
	}

	return VISUAL_OK;
}

int visual_time_past (VisTime *time_, VisTime *past)
{
	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_return_val_if_fail (past != NULL, -VISUAL_ERROR_TIME_NULL);

	if (time_->sec > past->sec)
		return TRUE;

	if (time_->sec == past->sec && time_->nsec > past->nsec)
		return TRUE;

	return FALSE;
}

int visual_time_usleep (unsigned long microseconds)
{
#if defined(VISUAL_OS_WIN32)
	Sleep (microseconds / 1000);

	return VISUAL_OK;
#else /* !VISUAL_OS_WIN32 */
#ifdef HAVE_NANOSLEEP
	struct timespec request, remaining;
	request.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	request.tv_nsec = 1000 * (microseconds % VISUAL_USEC_PER_SEC);
	while (nanosleep (&request, &remaining) == EINTR)
		request = remaining;
#elif defined(HAVE_SELECT)
	struct timeval tv;
	tv.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	tv.tv_usec = microseconds % VISUAL_USEC_PER_SEC;
	select (0, NULL, NULL, NULL, &tv);
#elif defined(HAVE_USLEEP)
	return usleep (microseconds);
#else
#warning visual_time_usleep() will not work!
	return -VISUAL_ERROR_TIME_NO_USLEEP;
#endif
	return VISUAL_OK;
#endif /* !VISUAL_OS_WIN32 */
}

VisTimer *visual_timer_new (void)
{
	VisTimer *timer;

	timer = visual_mem_new0 (VisTimer, 1);

	visual_timer_init (timer);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (timer), TRUE);
	visual_object_ref (VISUAL_OBJECT (timer));

	return timer;
}

int visual_timer_init (VisTimer *timer)
{
	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (timer));
	visual_object_set_dtor (VISUAL_OBJECT (timer), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (timer), FALSE);

	/* Reset the VisTime data */
	visual_time_init (&timer->start);
	visual_time_init (&timer->stop);

	visual_timer_reset (timer);

	return VISUAL_OK;
}

int visual_timer_reset (VisTimer *timer)
{
	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_set (&timer->start, 0, 0);
	visual_time_set (&timer->stop, 0, 0);

	timer->active = FALSE;

	return VISUAL_OK;
}

int visual_timer_is_active (VisTimer *timer)
{
	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	return timer->active;
}

int visual_timer_start (VisTimer *timer)
{
	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_get (&timer->start);

	timer->active = TRUE;

	return VISUAL_OK;
}

int visual_timer_stop (VisTimer *timer)
{
	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_get (&timer->stop);

	timer->active = FALSE;

	return VISUAL_OK;
}

int visual_timer_continue (VisTimer *timer)
{
	VisTime elapsed;

	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_return_val_if_fail (timer->active, -VISUAL_ERROR_TIMER_NULL);

	visual_time_difference (&elapsed, &timer->start, &timer->stop);

	visual_time_get (&timer->start);

	if (timer->start.nsec < elapsed.nsec) {
		timer->start.sec--;
		timer->start.nsec += VISUAL_NSEC_PER_SEC;
	}

	timer->start.sec  -= elapsed.sec;
	timer->start.nsec -= elapsed.nsec;

	timer->active = TRUE;

	return VISUAL_OK;
}

int visual_timer_elapsed (VisTimer *timer, VisTime *time_)
{
	VisTime cur;

	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	visual_time_get (&cur);

	if (timer->active)
		visual_time_difference (time_, &timer->start, &cur);
	else
		visual_time_difference (time_, &timer->start, &timer->stop);


	return VISUAL_OK;
}

int visual_timer_elapsed_msecs (VisTimer *timer)
{
	VisTime cur;

	visual_return_val_if_fail (timer != NULL, -1);

	visual_timer_elapsed (timer, &cur);

	return (cur.sec * VISUAL_MSEC_PER_SEC) + (cur.nsec / VISUAL_NSEC_PER_MSEC);
}

int visual_timer_elapsed_usecs (VisTimer *timer)
{
	VisTime cur;

	visual_return_val_if_fail (timer != NULL, -1);

	visual_timer_elapsed (timer, &cur);

	return (cur.sec * VISUAL_USEC_PER_SEC) + (cur.nsec / VISUAL_NSEC_PER_USEC);
}

int visual_timer_has_passed (VisTimer *timer, VisTime *time_)
{
	VisTime elapsed;

	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	visual_timer_elapsed (timer, &elapsed);

	if (time_->sec == elapsed.sec && time_->nsec <= elapsed.nsec)
		return TRUE;
	else if (time_->sec < elapsed.sec)
		return TRUE;

	return FALSE;
}

int visual_timer_has_passed_by_values (VisTimer *timer, long sec, long usec)
{
	VisTime passed;

	visual_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_set (&passed, sec, usec);

	return visual_timer_has_passed (timer, &passed);
}
