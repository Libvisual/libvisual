/* Libvisual-display - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_fpsctls.c,v 1.1 2005-02-14 22:05:15 vitalyvb Exp $
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//#if TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
//#else
//#	if HAVE_SYS_TIME_H
//#		include <sys/time.h>
//#	else
//#		include <time.h>
//#	endif
//#endif

#include "lv_display.h"

/*
 * Precise time control functions
 */

typedef struct {
	unsigned int sec;
	unsigned int usec;
} TimeStruct;

void get_time(TimeStruct *ts)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ts->sec = tv.tv_sec;
	ts->usec = tv.tv_usec;
}

int get_time_delta(TimeStruct *ts1, TimeStruct *ts2)
{
	int delta;

	if (ts1->usec < ts2->usec) {
		int nsec = (ts2->usec - ts1->usec) / 1000000 + 1;
		ts2->usec -= 1000000 * nsec;
		ts2->sec += nsec;
	}

	if (ts1->usec - ts2->usec > 1000000) {
		int nsec = (ts1->usec - ts2->usec) / 1000000;
		ts2->usec += 1000000 * nsec;
		ts2->sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   `tv_usec' is certainly positive. */
	delta = (ts1->sec - ts2->sec)*1000000 + (ts1->usec - ts2->usec);

	/* Return 1 if result is negative. */
	return delta;
}

/*
 * Linux 2.6 Sleep FPS Limiter
 */
 
typedef struct {
	LvdFPSControl ctl;

	TimeStruct framestart;
	int oversleep;
} sleep26_fpsctl;



static void sleep26_fps_control_frame_start(LvdFPSControl *fpsdata)
{
	sleep26_fpsctl *fps = (sleep26_fpsctl*)fpsdata;

	get_time(&fps->framestart);
}

static void sleep26_fps_control_frame_end(LvdFPSControl *fpsdata)
{
	sleep26_fpsctl *fps = (sleep26_fpsctl*)fpsdata;
	TimeStruct ts, ts2;
	int d;
	int frametime;
	int tosleep;

	frametime = 1000000/fps->ctl.fps_max;

	get_time(&ts);
	d = get_time_delta(&ts, &fps->framestart);

	tosleep = frametime - d;

	tosleep -= fps->oversleep;
	if (tosleep > 0)
		usleep(tosleep);

	get_time(&ts2);
	d = get_time_delta(&ts2, &fps->framestart) - frametime;
	fps->oversleep += d;

	fps->oversleep = fps->oversleep % frametime;

	get_time(&ts);
	d = get_time_delta(&ts, &fps->framestart);
	fps->ctl.fps_current = 1000000.0/d;

	if (fps->ctl.fps_avg == 0.0f)
		fps->ctl.fps_avg = fps->ctl.fps_current;
	else
		fps->ctl.fps_avg = fps->ctl.fps_avg*(31.0/32) + (1000000.0/d)/32;

	printf("FPS: %.4f\n", fps->ctl.fps_avg);
}

static int sleep26_fps_control_finit(VisObject *fpsdata)
{
	sleep26_fpsctl *fps = (sleep26_fpsctl*)fpsdata;
	visual_mem_free(fps);
	return 0;
}

LvdFPSControl *sleep26_fps_control_init()
{
	sleep26_fpsctl *fps = visual_mem_new0(sleep26_fpsctl, 1);
	visual_object_initialize(VISUAL_OBJECT(fps), TRUE, sleep26_fps_control_finit);

	fps->ctl.fps_control_frame_start = sleep26_fps_control_frame_start;
	fps->ctl.fps_control_frame_end = sleep26_fps_control_frame_end;
	fps->ctl.fps_max = 85.0f;
	fps->ctl.fps_avg = 0.0f;

	return (LvdFPSControl*)fps;
}




/*
 * NULL FPS Limiter
 */

typedef struct {
	LvdFPSControl ctl;
	TimeStruct framestart;
} null_fpsctl;

static void null_fps_control_frame_start(LvdFPSControl *fpsdata)
{
	null_fpsctl *fps = (null_fpsctl*)fpsdata;
	get_time(&fps->framestart);
}

static void null_fps_control_frame_end(LvdFPSControl *fpsdata)
{
	null_fpsctl *fps = (null_fpsctl*)fpsdata;
	TimeStruct ts;
	int d;

	get_time(&ts);
	d = get_time_delta(&ts, &fps->framestart);

	fps->ctl.fps_current = 1000000.0/d;

	if (fps->ctl.fps_avg == 0.0f)
		fps->ctl.fps_avg = fps->ctl.fps_current;
	else
		fps->ctl.fps_avg = fps->ctl.fps_avg*(31.0/32) + (1000000.0/d)/32;

	printf("FPS: %.4f\n", fps->ctl.fps_avg);
}

static int null_fps_control_finit(VisObject *fpsdata)
{
	visual_mem_free(fpsdata);
	return 0;
}

LvdFPSControl *null_fps_control_init()
{
	null_fpsctl *fps = visual_mem_new0(null_fpsctl, 1);
	visual_object_initialize(VISUAL_OBJECT(fps), TRUE, null_fps_control_finit);

	fps->ctl.fps_control_frame_start = null_fps_control_frame_start;
	fps->ctl.fps_control_frame_end = null_fps_control_frame_end;

	return (LvdFPSControl*)fps;
}
