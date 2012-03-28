/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>,
 * Copyright (C) 2012 Daniel Hiepler <daniel@niftylight.de>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel@niftylight.de>
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


#include "display.h"

#include <unistd.h>

/* MinGW unistd.h doesn't have *_FILENO or SEEK_* defined */
#ifdef VISUAL_WITH_MINGW
#  define STDOUT_FILENO 1
#endif


#define STDOUT_NATIVE(obj)  (VISUAL_CHECK_CAST ((obj), StdoutNative))

/** our main driver object */
typedef struct _StdoutNative StdoutNative;

/** private descriptor */
struct _StdoutNative
{
	VisObject       object;
    int             width,height;
    VisVideoDepth   depth;
	void *          area;
};

/** create display */
static int native_create(SADisplay *display,
                         VisVideoDepth depth,
                         VisVideoAttributeOptions *vidoptions,
                         int width, int height, int resizable)
{
	StdoutNative *native;

    /* allocate new private descriptor */
	if(!(native = STDOUT_NATIVE (display->native)))
    {
		native = visual_mem_new0(StdoutNative, 1);
		visual_object_initialize(VISUAL_OBJECT (native), TRUE, NULL);
	}

    /* create buffer */
    if(native->area != NULL)
        visual_mem_free(native->area);
    native->area = visual_mem_malloc0(width * height * (visual_video_depth_value_from_enum(VISUAL_VIDEO_DEPTH_24BIT) / 8));

    /* save dimensions */
    native->width = width;
    native->height = height;
    native->depth = depth;

	display->native = VISUAL_OBJECT(native);

	return 0;
}

/** close display */
static int native_close (SADisplay *display)
{
	StdoutNative *native = STDOUT_NATIVE (display->native);

	if(!native)
		return 0;

	visual_mem_free (native->area);
	visual_object_unref (VISUAL_OBJECT(native));
	display->native = NULL;

	return 0;
}

/** lock display for drawing */
static int native_lock (SADisplay *display)
{
	return 0;
}

/** unlock display after drawing */
static int native_unlock (SADisplay *display)
{
	return 0;
}

/** switch to/from fullscreen */
static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale)
{
	return 0;
}

/** fill VisVideo descriptor */
static int native_getvideo (SADisplay *display, VisVideo *screen)
{
	StdoutNative *native = STDOUT_NATIVE (display->native);

	visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_24BIT);
	visual_video_set_dimension (screen, native->width, native->height);
	visual_video_set_buffer(screen, native->area);

	return 0;
}

/** update rectangular portion of display */
static int native_updaterect(SADisplay *display, VisRectangle *rect)
{
	StdoutNative *native = STDOUT_NATIVE(display->native);

    /* write data */
	write(STDOUT_FILENO,
          native->area,
          native->width *
            native->height *
            (visual_video_depth_value_from_enum(VISUAL_VIDEO_DEPTH_24BIT) / 8));
	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	return 0;
}




/** creator */
SADisplayDriver *stdout_driver_new ()
{
	SADisplayDriver *driver;

        /* allocate new driver object */
	driver = visual_mem_new0 (SADisplayDriver, 1);

        /* initialize */
	visual_object_initialize (VISUAL_OBJECT (driver), TRUE, NULL);

        /* register methods */
	driver->create = native_create;
	driver->close = native_close;
	driver->lock = native_lock;
	driver->unlock = native_unlock;
	driver->fullscreen = native_fullscreen;
	driver->getvideo = native_getvideo;
	driver->updaterect = native_updaterect;
	driver->drainevents = native_drainevents;

	return driver;
}



