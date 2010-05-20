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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "lvconfig.h"

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

#include "config.h"
#include "lv_common.h"
#include "lv_thread.h"
#include "lv_time.h"

/**
 * @defgroup VisTime VisTime
 * @{
 */

/**
 * Creates a new VisTime structure.
 *
 * @return A newly allocated VisTime.
 */
VisTime *visual_time_new ()
{
	VisTime *time_;

	time_ = visual_mem_new0 (VisTime, 1);

	visual_time_init (time_);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (time_), TRUE);
	visual_object_ref (VISUAL_OBJECT (time_));

	return time_;
}

/**
 * Initializes a VisTime, this should not be used to reset a VisTime.
 * The resulting initialized VisTime is a valid VisObject even if it was not allocated.
 * Keep in mind that VisTime structures that were created by visual_time_new() should not
 * be passed to visual_time_init().
 *
 * @see visual_time_new
 *
 * @param time_ Pointer to the VisTime which needs to be initialized.
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_init (VisTime *time_)
{
	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (time_));
	visual_object_set_dtor (VISUAL_OBJECT (time_), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (time_), FALSE);

	/* Reset the VisTime data */
	visual_time_set	(time_, 0, 0);

	return VISUAL_OK;
}


/**
 * Loads the current time into the VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the current time needs to be set.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_get (VisTime *time_)
{
#if defined(VISUAL_OS_WIN32)
	SYSTEMTIME systime;

	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	GetLocalTime (&systime);

	visual_time_set (time_, (systime.wHour * 60 * 60) + (systime.wMinute * 60) + systime.wSecond,
			systime.wMilliseconds * 1000);

#else
	struct timeval tv;

	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	gettimeofday (&tv, NULL);

	visual_time_set (time_, tv.tv_sec, tv.tv_usec);
#endif
	return VISUAL_OK;
}

/**
 * Sets the time by sec, usec in a VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the time is set.
 * @param sec The seconds.
 * @param usec The microseconds.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_set (VisTime *time_, long sec, long usec)
{
	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	time_->tv_sec = sec;
	time_->tv_usec = usec;

	return VISUAL_OK;
}

/**
 * Copies ine VisTime into another.
 *
 * @param dest Pointer to the destination VisTime in which the source VisTime is copied.
 * @param src Pointer to the source VisTime that is copied to the destination VisTime.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_copy (VisTime *dest, VisTime *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_TIME_NULL);

	dest->tv_sec = src->tv_sec;
	dest->tv_usec = src->tv_usec;

	return VISUAL_OK;
}

/**
 * Calculates the difference between two VisTime structures.
 *
 * @param dest Pointer to the VisTime that contains the difference between time1 and time2.
 * @param time1 Pointer to the first VisTime.
 * @param time2 Pointer to the second VisTime from which the first is substracted to generate a result.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_difference (VisTime *dest, VisTime *time1, VisTime *time2)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_log_return_val_if_fail (time1 != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_log_return_val_if_fail (time2 != NULL, -VISUAL_ERROR_TIME_NULL);

	dest->tv_usec = time2->tv_usec - time1->tv_usec;
	dest->tv_sec = time2->tv_sec - time1->tv_sec;

	if (dest->tv_usec < 0) {
		dest->tv_usec = VISUAL_USEC_PER_SEC + dest->tv_usec;
		dest->tv_sec--;
	}

	return VISUAL_OK;
}

/**
 * Checks if a VisTime is later than another VisTime.
 *
 * @param time_ Pointer to the VisTime for which is checked whether it's later or not than the other.
 * @param past Pointer to the VisTime that acts as the past time source data.
 *
 * @return TRUE if past, FALSE if not on succes, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_past (VisTime *time_, VisTime *past)
{
	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);
	visual_log_return_val_if_fail (past != NULL, -VISUAL_ERROR_TIME_NULL);

	if (time_->tv_sec > past->tv_sec)
		return TRUE;

	if (time_->tv_sec == past->tv_sec && time_->tv_usec > past->tv_usec)
		return TRUE;

	return FALSE;
}

/**
 * Sleeps an certain amount of microseconds.
 *
 * @param microseconds The amount of microseconds we're going to sleep. To sleep a certain amount of
 *	seconds you can call this function with visual_time_usleep (VISUAL_USEC_PER_SEC * seconds).
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIME_NO_USLEEP or -1 on failure.
 */
int visual_time_usleep (unsigned long microseconds)
{
#if defined(VISUAL_OS_WIN32)
	Sleep (microseconds / 1000);

	return VISUAL_OK;
#else /* !VISAUL_OS_WIN32 */
#ifdef HAVE_NANOSLEEP
	struct timespec request, remaining;
	request.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	request.tv_nsec = 1000 * (microseconds % VISUAL_USEC_PER_SEC);
	while (nanosleep (&request, &remaining) == EINTR)
		request = remaining;
#elif HAVE_SELECT
	struct timeval tv;
	tv.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	tv.tv_usec = microseconds % VISUAL_USEC_PER_SEC;
	select (0, NULL, NULL, NULL, &tv);
#elif HAVE_USLEEP
	return usleep (microseconds);
#else
#warning visual_time_usleep() will not work!
	return -VISUAL_ERROR_TIME_NO_USLEEP;
#endif
	return VISUAL_OK;
#endif /* !VISUAL_OS_WIN32 */
}

/**
 * Creates a new VisTimer structure.
 *
 * @return A newly allocated VisTimer.
 */
VisTimer *visual_timer_new ()
{
	VisTimer *timer;

	timer = visual_mem_new0 (VisTimer, 1);

	visual_timer_init (timer);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (timer), TRUE);
	visual_object_ref (VISUAL_OBJECT (timer));

	return timer;
}

/**
 * Initializes a VisTimer, this should not be used to reset a VisTimer.
 * The resulting initialized VisTimer is a valid VisObject even if it was not allocated with visual_timer_new().
 * Keep in mind that VisTimer structures that were created by visual_timer_new() should not
 * be passed to visual_timer_init().
 *
 * @see visual_timer_new
 *
 * @param timer Pointer to the VisTimer which needs to be initialized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_init (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

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

/**
 * Resets a VisTimer.
 *
 * @param timer Pointer to the VisTimer that is to be reset.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_reset (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_set (&timer->start, 0, 0);
	visual_time_set (&timer->stop, 0, 0);

	timer->active = FALSE;

	return VISUAL_OK;
}

/**
 * Checks if the timer is active.
 *
 * @param timer Pointer to the VisTimer of which we want to know if it's active.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_is_active (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	return timer->active;
}

/**
 * Starts a timer.
 *
 * @param timer Pointer to the VisTimer in which we start the timer.
 *
 * return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_start (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_get (&timer->start);

	timer->active = TRUE;

	return VISUAL_OK;
}

/**
 * Stops a timer.
 *
 * @param timer Pointer to the VisTimer that is stopped.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_stop (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_get (&timer->stop);

	timer->active = FALSE;

	return VISUAL_OK;
}

/**
 * Continues a stopped timer. The timer needs to be stopped before continueing.
 *
 * @param timer Pointer to the VisTimer that is continued.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_continue (VisTimer *timer)
{
	VisTime elapsed;

	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_log_return_val_if_fail (timer->active != FALSE, -VISUAL_ERROR_TIMER_NULL);

	visual_time_difference (&elapsed, &timer->start, &timer->stop);

	visual_time_get (&timer->start);

	if (timer->start.tv_usec < elapsed.tv_usec) {
		timer->start.tv_usec += VISUAL_USEC_PER_SEC;
		timer->start.tv_sec--;
	}

	timer->start.tv_sec -= elapsed.tv_sec;
	timer->start.tv_usec -= elapsed.tv_usec;

	timer->active = TRUE;

	return VISUAL_OK;
}

/**
 * Retrieve the amount of time passed since the timer has started.
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of time passed.
 * @param time_ Pointer to the VisTime in which we put the amount of time passed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_TIMER_NULL or -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_timer_elapsed (VisTimer *timer, VisTime *time_)
{
	VisTime cur;

	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	visual_time_get (&cur);

	if (timer->active == TRUE)
		visual_time_difference (time_, &timer->start, &cur);
	else
		visual_time_difference (time_, &timer->start, &timer->stop);


	return VISUAL_OK;
}

/**
 * Returns the amount of milliseconds passed since the timer has started.
 * Be careful not to confuse milliseconds with microseconds.
 *
 * @see visual_timer_elapsed_usecs
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of milliseconds passed since activation.
 *
 * @return The amount of milliseconds passed, or -1 on error, this function will fail if your system goes back in time.
 */
int visual_timer_elapsed_msecs (VisTimer *timer)
{
	VisTime cur;

	visual_log_return_val_if_fail (timer != NULL, -1);

	visual_timer_elapsed (timer, &cur);

	return (cur.tv_sec * VISUAL_MSEC_PER_SEC) + (cur.tv_usec / VISUAL_MSEC_PER_SEC);
}

/**
 * Returns the amount of microseconds passed since the timer has started.
 * Be careful not to confuse milliseconds with microseconds.
 *
 * @see visual_timer_elapsed_msecs
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of microseconds passed since activation.
 *
 * @return The amount of microseconds passed, or -1 on error, this function will fail if your system goes back in time.
 */
int visual_timer_elapsed_usecs (VisTimer *timer)
{
	VisTime cur;

	visual_log_return_val_if_fail (timer != NULL, -1);

	visual_timer_elapsed (timer, &cur);

	return (cur.tv_sec * VISUAL_USEC_PER_SEC) + cur.tv_usec;
}

/**
 * Checks if the timer has passed a certain age.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param time_ Pointer to the VisTime containing the age we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -VISUAL_ERROR_TIMER_NULL or -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_timer_has_passed (VisTimer *timer, VisTime *time_)
{
	VisTime elapsed;

	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);
	visual_log_return_val_if_fail (time_ != NULL, -VISUAL_ERROR_TIME_NULL);

	visual_timer_elapsed (timer, &elapsed);

	if (time_->tv_sec == elapsed.tv_sec && time_->tv_usec <= elapsed.tv_usec)
		return TRUE;
	else if (time_->tv_sec < elapsed.tv_sec)
		return TRUE;

	return FALSE;
}

/**
 * Checks if the timer has passed a certain age by values.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param sec The number of seconds we check against.
 * @param usec The number of microseconds we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_has_passed_by_values (VisTimer *timer, long sec, long usec)
{
	VisTime passed;

	visual_log_return_val_if_fail (timer != NULL, -VISUAL_ERROR_TIMER_NULL);

	visual_time_set (&passed, sec, usec);

	return visual_timer_has_passed (timer, &passed);
}

/**
 * @}
 */

