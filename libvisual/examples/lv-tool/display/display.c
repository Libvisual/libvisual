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

SADisplay *display_new (SADisplayDriver *driver)
{
	SADisplay *display;

	display = visual_mem_new0 (SADisplay, 1);

	visual_object_initialize (VISUAL_OBJECT (display), TRUE, NULL);

	display->driver = driver;
	display->screen = visual_video_new ();

	return display;
}

int display_create (SADisplay *display, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions,
		int width, int height, int resizable)
{
	return display->driver->create (display, depth, vidoptions, width, height, resizable);
}

int display_close (SADisplay *display)
{
	return display->driver->close (display);
}

VisVideo *display_get_video (SADisplay *display)
{
	display->driver->getvideo (display, display->screen);

	return display->screen;
}

int display_lock (SADisplay *display)
{
	return display->driver->lock (display);
}

int display_unlock (SADisplay *display)
{
	return display->driver->unlock (display);
}

int display_update_all (SADisplay *display)
{
	VisRectangle rect;
	VisVideo *video = display_get_video (display);

	rect.x = 0;
	rect.y = 0;

	rect.width = video->width;
	rect.height = video->height;

	display->frames_drawn++;

	if (visual_timer_is_active (&display->timer) == FALSE)
		visual_timer_start (&display->timer);

	return display_update_rectangle (display, &rect);
}

int display_update_rectangle (SADisplay *display, VisRectangle *rect)
{
	return display->driver->updaterect (display, rect);
}

int display_set_fullscreen (SADisplay *display, int fullscreen, int autoscale)
{
	return display->driver->fullscreen (display, fullscreen, autoscale);
}

int display_drain_events (SADisplay *display, VisEventQueue *eventqueue)
{
	return display->driver->drainevents (display, eventqueue);
}

int display_fps_limit (SADisplay *display, int fps)
{
	return 0;
}

int display_fps_total (SADisplay *display)
{
	return display->frames_drawn;
}

float display_fps_average (SADisplay *display)
{
	return display->frames_drawn / (visual_timer_elapsed_usecs (&display->timer) / (float) VISUAL_USEC_PER_SEC);
}

