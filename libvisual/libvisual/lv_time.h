/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_time.h,v 1.21 2006/02/05 18:45:57 synap Exp $
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

#include <libvisual/lv_object.h>
#include <time.h>

/**
 * @defgroup VisTime VisTime
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_USEC_PER_SEC	1000000
#define VISUAL_MSEC_PER_SEC	1000

#define VISUAL_TIME(obj)				(VISUAL_CHECK_CAST ((obj), VisTime))
#define VISUAL_TIMER(obj)				(VISUAL_CHECK_CAST ((obj), VisTimer))

typedef struct _VisTime VisTime;
typedef struct _VisTimer VisTimer;

/**
 * The VisTime structure can contain seconds and microseconds for timing purpose.
 */
struct _VisTime {
	VisObject object;     /**< The VisObject data. */
	long      sec;        /**< seconds. */
	long      usec;       /**< microseconds. */
};

/**
 * The VisTimer structure is used for timing using the visual_timer_* methods.
 */
struct _VisTimer {
	VisObject	object;		/**< The VisObject data. */
	VisTime		start;		/**< Private entry to indicate the starting point,
					 * Shouldn't be read for reliable starting point because
					 * the visual_timer_continue method changes it's value. */
	VisTime		stop;		/**< Private entry to indicate the stopping point. */

	int		active;		/**< Private entry to indicate if the timer is currently active or inactive. */
};


/**
 * Creates a new VisTime structure.
 *
 * @return A newly allocated VisTime.
 */
VisTime *visual_time_new (void);

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
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_init (VisTime *time_);

int visual_time_get (VisTime *time_);

/**
 * Loads the current time into the VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the current time needs to be set.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure.
 */

/**
 * Sets the time by sec, usec in a VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the time is set.
 * @param sec The seconds.
 * @param usec The microseconds.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_set (VisTime *time_, long sec, long usec);

/**
 * Calculates the difference between two VisTime structures.
 *
 * @param dest Pointer to the VisTime that contains the difference between time1 and time2.
 * @param time1 Pointer to the first VisTime.
 * @param time2 Pointer to the second VisTime from which the first is substracted to generate a result.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_difference (VisTime *dest, VisTime *time1, VisTime *time2);

/**
 * Checks if a VisTime is later than another VisTime.
 *
 * @param time_ Pointer to the VisTime for which is checked whether it's later or not than the other.
 * @param past Pointer to the VisTime that acts as the past time source data.
 *
 * @return TRUE if past, FALSE if not on success, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_past (VisTime *time_, VisTime *past);

/**
 * Copies ine VisTime into another.
 *
 * @param dest Pointer to the destination VisTime in which the source VisTime is copied.
 * @param src Pointer to the source VisTime that is copied to the destination VisTime.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_time_copy (VisTime *dest, VisTime *src);

/**
 * Sleeps an certain amount of microseconds.
 *
 * @param microseconds The amount of microseconds we're going to sleep. To sleep a certain amount of
 *	seconds you can call this function with visual_time_usleep (VISUAL_USEC_PER_SEC * seconds).
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NO_USLEEP or -1 on failure.
 */
int visual_time_usleep (unsigned long microseconds);

/**
 * Extract the milliseconds from a VisTime
 *
 * @param The VisTime from which milliseconds are to be extracted from
 *
 * @return Milliseconds on success, -1 on failure
 */
long visual_time_get_msecs (VisTime *time_);

/**
 * Sets the time by msecs in a VisTime structure.
 *
 * @param time_ Pointer to the VisTime in which the time is set.
 * @param msecs The milliseconds.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIME_NULL on failure
 */
int visual_time_set_from_msecs (VisTime *time_, long msec);

/**
 * Creates a new VisTimer structure.
 *
 * @return A newly allocated VisTimer.
 */
VisTimer *visual_timer_new (void);

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
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_init (VisTimer *timer);

/**
 * Resets a VisTimer.
 *
 * @param timer Pointer to the VisTimer that is to be reset.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_reset (VisTimer *timer);

/**
 * Checks if the timer is active.
 *
 * @param timer Pointer to the VisTimer of which we want to know if it's active.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_is_active (VisTimer *timer);

/**
 * Starts a timer.
 *
 * @param timer Pointer to the VisTimer in which we start the timer.
 *
 * return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_start (VisTimer *timer);

/**
 * Stops a timer.
 *
 * @param timer Pointer to the VisTimer that is stopped.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_stop (VisTimer *timer);

/**
 * Continues a stopped timer. The timer needs to be stopped before continueing.
 *
 * @param timer Pointer to the VisTimer that is continued.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_continue (VisTimer *timer);

/**
 * Retrieve the amount of time passed since the timer has started.
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of time passed.
 * @param time_ Pointer to the VisTime in which we put the amount of time passed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_TIMER_NULL or -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_timer_elapsed (VisTimer *timer, VisTime *time_);

/**
 * Returns the amount of milliseconds passed since the timer has
 * started.  Be careful not to confuse milliseconds with microseconds.
 *
 * @see visual_timer_elapsed_usecs
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of milliseconds passed since activation.
 *
 * @return The amount of milliseconds passed, or -1 on error, this function will fail if your system goes back in time.
 */
int visual_timer_elapsed_msecs (VisTimer *timer);

/**
 * Returns the amount of microseconds passed since the timer has
 * started.  Be careful not to confuse milliseconds with microseconds.
 *
 * @see visual_timer_elapsed_msecs
 *
 * @param timer Pointer to the VisTimer from which we want to know the amount of microseconds passed since activation.
 *
 * @return The amount of microseconds passed, or -1 on error, this function will fail if your system goes back in time.
 */
int visual_timer_elapsed_usecs (VisTimer *timer);

/**
 * Checks if the timer has passed a certain age.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param time_ Pointer to the VisTime containing the age we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -VISUAL_ERROR_TIMER_NULL or -VISUAL_ERROR_TIME_NULL on failure.
 */
int visual_timer_has_passed (VisTimer *timer, VisTime *time_);

/**
 * Checks if the timer has passed a certain age by values.
 *
 * @param timer Pointer to the VisTimer which we check for age.
 * @param sec The number of seconds we check against.
 * @param usec The number of microseconds we check against.
 *
 * @return TRUE on passed, FALSE if not passed, -VISUAL_ERROR_TIMER_NULL on failure.
 */
int visual_timer_has_passed_by_values (VisTimer *timer, long sec, long usec);

/* FIXME: does this work everywhere (x86) ? Check the cycle.h that can be found in FFTW,
 * also check liboil it's profile header: add powerpc support */

/**
 * This function can be used to retrieve the real time stamp counter. This function
 * will not check if timestamping is around, the reason for this is because it will make
 * the timing less reliable since checking for timestamping gives overhead. You
 * must make arrangments for this.
 *
 * @see visual_cpu_get_tsc
 *
 * @param lo The lower 32 bits of the timestmap.
 * @param hi The higher 32 bits of the timestamp.
 *
 * @return Nothing.
 */
static inline void visual_timer_tsc_get (uint32_t *lo, uint32_t *hi)
{
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	__asm __volatile
		("\n\t cpuid"
		 "\n\t rdtsc"
		 "\n\t movl %%edx, %0"
		 "\n\t movl %%eax, %1"
		 : "=r" (*hi), "=r" (*lo)
		 :: "memory");
#endif
}

/* FIXME use uint64_t here, make sure type exists */
static inline unsigned long long visual_timer_tsc_get_returned (void)
{
	uint32_t lo, hi;

	visual_timer_tsc_get (&lo, &hi);

	return ((unsigned long long) hi << 32) | lo;
}

#define visual_time_get_now() (clock() / (float)CLOCKS_PER_SEC * 1000)

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_TIME_H */
