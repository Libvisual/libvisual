#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "lv_common.h"
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

	return time_;
}

/**
 * Frees the VisTime. This frees the VisTime data structure.
 *
 * @param time_ Pointer to the VisTime that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_time_free (VisTime *time_)
{
	visual_log_return_val_if_fail (time_ != NULL, -1);

	visual_mem_free (time_);

	return 0;
}

/**
 * Loads the current time into the VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the current time needs to be set.
 *
 * @return 0 on succes -1 on error.
 */
int visual_time_get (VisTime *time_)
{
	struct timeval tv;
	
	visual_log_return_val_if_fail (time_ != NULL, -1);

	gettimeofday (&tv, NULL);

	time_->tv_sec = tv.tv_sec;
	time_->tv_usec = tv.tv_usec;

	return 0;
}

/**
 * Calculates the difference between two VisTime structures.
 *
 * @param dest Pointer to the VisTime that contains the difference between time1 and time2.
 * @param time1 Pointer to the first VisTime.
 * @param time2 Pointer to the second VisTime from which the first is substracted to generate a result.
 *
 * @return 0 on succes -1 on error.
 */
int visual_time_difference (VisTime *dest, VisTime *time1, VisTime *time2)
{
	visual_log_return_val_if_fail (dest != NULL, -1);
	visual_log_return_val_if_fail (time1 != NULL, -1);
	visual_log_return_val_if_fail (time2 != NULL, -1);

	dest->tv_usec = time2->tv_usec - time1->tv_usec;
	dest->tv_sec = time2->tv_sec - time1->tv_sec;

	if (dest->tv_usec < 0) {
		dest->tv_usec = VISUAL_USEC_PER_SEC + dest->tv_usec;
		dest->tv_sec--;
	}

	return 0;
}

/**
 * Sleeps an certain amount of microseconds.
 *
 * @param microseconds The amount of microseconds we're going to sleep. To sleep a certain amount of
 *	seconds you can call this function with visual_time_usleep (VISUAL_USEC_PER_SEC * seconds).
 * 
 * @return 0 on succes -1 on error.
 */
int visual_time_usleep (unsigned long microseconds)
{
#ifdef HAVE_NANOSLEEP
	struct timespec request, remaining;
	request.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	request.tv_nsec = 1000 * (microseconds % VISUAL_USEC_PER_SEC);
	while (nanosleep (&request, &remaining) == EINTR)
		request = remaining;
#elif HAVE_USLEEP
	return usleep (microseconds);
#elif HAVE_SELECT
	struct timeval tv;
	tv.tv_sec = microseconds / VISUAL_USEC_PER_SEC;
	tv.tv_usec = microseconds % VISUAL_USEC_PER_SEC;
	select(0, NULL, NULL, NULL, &tv);
#else
#warning visual_time_usleep() will does not work!
#endif
	return 0;
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

	return timer;
}

/**
 * Frees the VisTimer. This frees the VisTimer data structure.
 *
 * @param timer Pointer to the VisTimer that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_timer_free (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -1);

	visual_mem_free (timer);

	return 0;
}

/**
 * Starts a timer.
 *
 * @param timer Pointer to the VisTimer in which we start the timer.
 *
 * return 0 on succes -1 on error.
 */ 
int visual_timer_start (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -1);

	visual_time_get (&timer->start);

	timer->active = TRUE;

	return 0;
}

/**
 * Stops a timer.
 *
 * @param timer Pointer to the VisTimer that is stopped.
 *
 * @return 0 on succes -1 on error.
 */
int visual_timer_stop (VisTimer *timer)
{
	visual_log_return_val_if_fail (timer != NULL, -1);

	visual_time_get (&timer->stop);

	timer->active = FALSE;

	return 0;
}

/**
 * Continues a stopped timer. The timer needs to be stopped before continueing.
 *
 * @param timer Pointer to the VisTimer that is continued.
 *
 * @return 0 on succes -1 on error.
 */
int visual_timer_continue (VisTimer *timer)
{
	VisTime elapsed;

	visual_log_return_val_if_fail (timer != NULL, -1);
	visual_log_return_val_if_fail (timer->active != FALSE, -1);

	visual_time_difference (&elapsed, &timer->start, &timer->stop);

	visual_time_get (&timer->start);

	if (timer->start.tv_usec < elapsed.tv_usec) {
		timer->start.tv_usec += VISUAL_USEC_PER_SEC;
		timer->start.tv_sec--;
	}

	timer->start.tv_sec -= elapsed.tv_sec;
	timer->start.tv_usec -= elapsed.tv_usec;

	timer->active = TRUE;

	return 0;
}

/**
 * Retrieve the amount of time past since the timer started.
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of time past.
 * @param time_ Pointer to the VisTime in which we put the amount of time past.
 *
 * @return 0 on succes -1 on error.
 */
int visual_timer_elapsed (VisTimer *timer, VisTime *time_)
{
	VisTime cur;

	visual_log_return_val_if_fail (timer != NULL, -1);
	visual_log_return_val_if_fail (time_ != NULL, -1);

	visual_time_get (&cur);

	if (timer->active == TRUE)
		visual_time_difference (time_, &timer->start, &cur);
	else
		visual_time_difference (time_, &timer->start, &timer->stop);


	return 0;
}

/**
 * Checks if the timer has past a certain age.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param time_ Pointer to the VisTime containing the age we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -1 on error.
 */
int visual_timer_has_past (VisTimer *timer, VisTime *time_)
{
	VisTime elapsed;

	visual_log_return_val_if_fail (timer != NULL, -1);
	visual_log_return_val_if_fail (time_ != NULL, -1);

	visual_timer_elapsed (timer, &elapsed);

	if (time_->tv_sec == elapsed.tv_sec && time_->tv_usec <= elapsed.tv_usec)
		return TRUE;
	else if (time_->tv_sec < elapsed.tv_sec)
		return TRUE;

	return FALSE;
}

/**
 * Checks if the timer has past a certain age by values.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param sec The number of seconds we check against.
 * @param usec The number of microseconds we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -1 on error.
 */
int visual_timer_has_past_by_values (VisTimer *timer, long sec, long usec)
{
	VisTime past;

	past.tv_sec = sec;
	past.tv_usec = usec;

	return visual_timer_has_past (timer, &past);
}

/**
 * @}
 */

