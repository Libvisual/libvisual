#ifndef _LV_TIME_H
#define _LV_TIME_H

#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
#define VISUAL_USEC_PER_SEC	1000000

typedef struct _VisTime VisTime;
typedef struct _VisTimer VisTimer;

/**
 * The VisTime structure can contain seconds and microseconds for timing purpose.
 */
struct _VisTime {
	long tv_sec;	/**< seconds. */
	long tv_usec;	/**< microseconds. */
};

/**
 * The VisTimer structure is used for timing using the visual_timer_* methods.
 */
struct _VisTimer {
	VisTime	start;	/**< Private entry to indicate the starting point,
			 * Shouldn't be read for reliable starting point because
			 * the visual_timer_continue method changes it's value. */
	VisTime	stop;	/**< Private entry to indicate the stopping point. */

	int	active;	/**< Private entry to indicate if the timer is currently active or inactive. */
};

VisTime *visual_time_new (void);
int visual_time_free (VisTime *time_);
int visual_time_get (VisTime *time_);
int visual_time_set (VisTime *time_, long sec, long usec);
int visual_time_difference (VisTime *dest, const VisTime *time1, const VisTime *time2);
int visual_time_copy (VisTime *dest, const VisTime *src);
int visual_time_usleep (unsigned long microseconds);

VisTimer *visual_timer_new (void);
int visual_timer_free (VisTimer *timer);
int visual_timer_is_active (const VisTimer *timer);
int visual_timer_start (VisTimer *timer);
int visual_timer_stop (VisTimer *timer);
int visual_timer_continue (VisTimer *timer);
int visual_timer_elapsed (const VisTimer *timer, VisTime *time_);
int visual_timer_has_past (const VisTimer *timer, VisTime *time_);
int visual_timer_has_past_by_values (const VisTimer *timer, long sec, long usec);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_TIME_H */
