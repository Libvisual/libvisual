/* Libvisual-display - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_fpsctls.c,v 1.3 2005-02-22 13:33:03 vitalyvb Exp $
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

#define DEFAULT_MAX_FPS 30.0f

int get_time_delta(VisTime *ts1, VisTime *ts2)
{
	VisTime res;
	visual_object_initialize(VISUAL_OBJECT(&res), FALSE, NULL);
	visual_time_difference(&res, ts2, ts1);
	return res.tv_sec*1000000 + res.tv_usec;
}


/*
 * Linux 2.6 Sleep FPS Limiter
 */
 
typedef struct {
	LvdFPSControl ctl;

	VisTime framestart;
	int oversleep;
} sleep26_fpsctl;


static void sleep26_fps_control_frame_start(LvdFPSControl *fpsdata)
{
	sleep26_fpsctl *fps = (sleep26_fpsctl*)fpsdata;

	visual_time_get(&fps->framestart);
}

// XXX FIXME avoid floating point arith
static void sleep26_fps_control_frame_end(LvdFPSControl *fpsdata)
{
	sleep26_fpsctl *fps = (sleep26_fpsctl*)fpsdata;
	VisTime ts, ts2;
	int d;
	int frametime;
	int tosleep;

	frametime = 1000000/fps->ctl.fps_max;

	visual_time_get(&ts);
	d = get_time_delta(&ts, &fps->framestart);

	tosleep = frametime - d;

	tosleep -= fps->oversleep;
	if (tosleep > 0)
		visual_time_usleep(tosleep);

	visual_time_get(&ts2);
	d = get_time_delta(&ts2, &fps->framestart) - frametime;
	fps->oversleep += d;

	fps->oversleep = fps->oversleep % frametime;

	visual_time_get(&ts);
	d = get_time_delta(&ts, &fps->framestart);
	fps->ctl.fps_current = 1000000.0/d;

	if (fps->ctl.fps_avg == 0.0f)
		fps->ctl.fps_avg = fps->ctl.fps_current;
	else
		fps->ctl.fps_avg = fps->ctl.fps_avg*(31.0/32) + (1000000.0/d)/32;
}

LvdFPSControl *sleep26_fps_control_init()
{
	sleep26_fpsctl *fps = visual_mem_new0(sleep26_fpsctl, 1);
	visual_object_initialize(VISUAL_OBJECT(fps), TRUE, NULL);
	visual_object_initialize(VISUAL_OBJECT(&fps->framestart), FALSE, NULL);

	fps->ctl.fps_control_frame_start = sleep26_fps_control_frame_start;
	fps->ctl.fps_control_frame_end = sleep26_fps_control_frame_end;
	fps->ctl.fps_max = DEFAULT_MAX_FPS;
	fps->ctl.fps_avg = 0.0f;

	return (LvdFPSControl*)fps;
}




/*
 * NULL FPS Limiter
 */

typedef struct {
	LvdFPSControl ctl;
	VisTime framestart;
} null_fpsctl;

static void null_fps_control_frame_start(LvdFPSControl *fpsdata)
{
	null_fpsctl *fps = (null_fpsctl*)fpsdata;
	visual_time_get(&fps->framestart);
}

static void null_fps_control_frame_end(LvdFPSControl *fpsdata)
{
	null_fpsctl *fps = (null_fpsctl*)fpsdata;
	VisTime ts;
	int d;

	visual_time_get(&ts);
	d = get_time_delta(&ts, &fps->framestart);

	fps->ctl.fps_current = 1000000.0/d;

	if (fps->ctl.fps_avg == 0.0f)
		fps->ctl.fps_avg = fps->ctl.fps_current;
	else
		fps->ctl.fps_avg = fps->ctl.fps_avg*(31.0/32) + (1000000.0/d)/32;
}

LvdFPSControl *null_fps_control_init()
{
	null_fpsctl *fps = visual_mem_new0(null_fpsctl, 1);
	visual_object_initialize(VISUAL_OBJECT(fps), TRUE, NULL);
	visual_object_initialize(VISUAL_OBJECT(&fps->framestart), FALSE, NULL);

	fps->ctl.fps_control_frame_start = null_fps_control_frame_start;
	fps->ctl.fps_control_frame_end = null_fps_control_frame_end;
	fps->ctl.fps_avg = 0.0f;

	return (LvdFPSControl*)fps;
}

/*
 * GetTimeOfDay FPS Limiter
 */

typedef struct {
	LvdFPSControl ctl;
	VisTime framestart;
} tod_fpsctl;

static void tod_fps_control_frame_start(LvdFPSControl *fpsdata)
{
	tod_fpsctl *fps = (tod_fpsctl*)fpsdata;
	visual_time_get(&fps->framestart);
}

static void tod_fps_control_frame_end(LvdFPSControl *fpsdata)
{
	tod_fpsctl *fps = (tod_fpsctl*)fpsdata;
	VisTime ts;
	int d;
	int frametime;

	frametime = 1000000/fps->ctl.fps_max;

	do {
		visual_time_get(&ts);
		d = get_time_delta(&ts, &fps->framestart);
	} while (d<frametime);

	fps->ctl.fps_current = 1000000.0/d;

	if (fps->ctl.fps_avg == 0.0f)
		fps->ctl.fps_avg = fps->ctl.fps_current;
	else
		fps->ctl.fps_avg = fps->ctl.fps_avg*(31.0/32) + (1000000.0/d)/32;
}

LvdFPSControl *tod_fps_control_init()
{
	tod_fpsctl *fps = visual_mem_new0(tod_fpsctl, 1);
	visual_object_initialize(VISUAL_OBJECT(fps), TRUE, NULL);
	visual_object_initialize(VISUAL_OBJECT(&fps->framestart), FALSE, NULL);

	fps->ctl.fps_control_frame_start = tod_fps_control_frame_start;
	fps->ctl.fps_control_frame_end = tod_fps_control_frame_end;
	fps->ctl.fps_max = DEFAULT_MAX_FPS;
	fps->ctl.fps_avg = 0.0f;

	return (LvdFPSControl*)fps;
}
