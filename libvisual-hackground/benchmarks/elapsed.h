#ifndef _ELAPSED_H_
#define _ELAPSED_H_

#include <libvisual/libvisual.h>

inline void show_elapsed (char *str, VisTimer *timer)
{
	VisTime time_;

	visual_timer_elapsed (timer, &time_);

	printf ("%s%d:%d\n", str, time_.tv_sec, time_.tv_usec);
}


#endif
