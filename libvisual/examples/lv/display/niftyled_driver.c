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


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <niftyled.h>
#include "display.h"



#define LEDS_DIMENSIONS_FILE	"/mnt/led/dimensions"
#define LEDS_RAW_INPUT_FILE	"/mnt/led/raw_input"

#define LEDS_NATIVE(obj)					(VISUAL_CHECK_CAST ((obj), LedsNative))


typedef struct _LedsNative LedsNative;

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable);
static int native_close (SADisplay *display);
static int native_lock (SADisplay *display);
static int native_unlock (SADisplay *display);
static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale);
static int native_getvideo (SADisplay *display, VisVideo *screen);
static int native_updaterect (SADisplay *display, VisRectangle *rect);
static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue);


struct _LedsNative 
{
	VisObject object;

	int width;
	int height;

	void *area;
};

SADisplayDriver *niftyled_driver_new ()
{
	SADisplayDriver *driver;

	driver = visual_mem_new0 (SADisplayDriver, 1);

	visual_object_initialize (VISUAL_OBJECT (driver), TRUE, NULL);

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

static int native_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable)
{
	LedsNative *native;
	

	if(!(native = LEDS_NATIVE (display->native))) 
        {
		native = visual_mem_new0(LedsNative, 1);
		visual_object_initialize(VISUAL_OBJECT (native), TRUE, NULL);
	}

	if (native->area)
        {
		visual_mem_free (native->area);
        }
        
	native->area = visual_mem_malloc0(width * height * (visual_video_depth_value_from_enum (VISUAL_VIDEO_DEPTH_24BIT) / 8));

        /*FILE *dimensions;
	dimensions = fopen (LEDS_DIMENSIONS_FILE, "r");

	fscanf (dimensions, "%d\n%d\n", &native->width, &native->height);

	fclose (dimensions);*/

        native->width = 20;
        native->height = 20;
        
	display->native = VISUAL_OBJECT (native);

	return 0;
}

static int native_close (SADisplay *display)
{
	LedsNative *native = LEDS_NATIVE (display->native);

	if (native == NULL)
		return 0;

	visual_mem_free (native->area);

	visual_object_unref (VISUAL_OBJECT (native));
	display->native = NULL;

	return 0;
}

static int native_lock (SADisplay *display)
{
	return 0;
}

static int native_unlock (SADisplay *display)
{
	return 0;
}

static int native_fullscreen (SADisplay *display, int fullscreen, int autoscale)
{
	return 0;
}

static int native_getvideo (SADisplay *display, VisVideo *screen)
{
	LedsNative *native = LEDS_NATIVE (display->native);

	visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_24BIT);

	visual_video_set_dimension (screen, native->width, native->height);
	visual_video_set_buffer (screen, native->area);

	return 0;
}

static int native_updaterect(SADisplay *display, VisRectangle *rect)
{
	LedsNative *native = LEDS_NATIVE(display->native);

	

	//write (fd, native->area, native->width * native->height * (visual_video_depth_value_from_enum (VISUAL_VIDEO_DEPTH_24BIT) / 8));

	printf ("%d %d\n", native->width, native->height);

	//close (fd);

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	return 0;
}

