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

int display_create (SADisplay *display, VisVideoDepth depth, int width, int height)
{
	return display->driver->create (display, depth, width, height);
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

	return display_update_rectangle (display, &rect);
}

int display_update_rectangle (SADisplay *display, VisRectangle *rect)
{
	return display->driver->updaterect (display, rect);
}

int display_set_fullscreen (SADisplay *display, int fullscreen)
{
	return display->driver->fullscreen (display, fullscreen);
}

