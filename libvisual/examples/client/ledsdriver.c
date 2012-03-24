#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ledsdriver.h"

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


struct _LedsNative {
	VisObject object;

	int width;
	int height;

	void *area;
};

SADisplayDriver *leds_driver_new ()
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
	LedsNative *native = LEDS_NATIVE (display->native);
	FILE *dimensions;

	if (native == NULL) {
		native = visual_mem_new0 (LedsNative, 1);

		visual_object_initialize (VISUAL_OBJECT (native), TRUE, NULL);
	}

	if (native->area != NULL)
		visual_mem_free (native->area);

	native->area = visual_mem_malloc0 (width * height * (visual_video_depth_value_from_enum (VISUAL_VIDEO_DEPTH_24BIT) / 8));

	dimensions = fopen (LEDS_DIMENSIONS_FILE, "r");

	fscanf (dimensions, "%d\n%d\n", &native->width, &native->height);

	fclose (dimensions);

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

static int native_updaterect (SADisplay *display, VisRectangle *rect)
{
	LedsNative *native = LEDS_NATIVE (display->native);

	int fd = open (LEDS_RAW_INPUT_FILE, O_WRONLY);

	write (fd, native->area, native->width * native->height * (visual_video_depth_value_from_enum (VISUAL_VIDEO_DEPTH_24BIT) / 8));

	printf ("%d %d\n", native->width, native->height);

	close (fd);

	return 0;
}

static int native_drainevents (SADisplay *display, VisEventQueue *eventqueue)
{
	return 0;
}

