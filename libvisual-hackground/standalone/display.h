#ifndef _LV_STANDALONE_DISPLAY_H
#define _LV_STANDALONE_DISPLAY_H

#include <libvisual/libvisual.h>

typedef struct _SADisplayDriver SADisplayDriver;
typedef struct _SADisplay SADisplay;


typedef int (*SADisplayDriverCreateFunc)(SADisplay *display, VisVideoDepth depth, int width, int height, int resizable);
typedef int (*SADisplayDriverLockFunc)(SADisplay *display);
typedef int (*SADisplayDriverUnlockFunc)(SADisplay *display);
typedef int (*SADisplayDriverFullScreenFunc)(SADisplay *display, int fullscreen);
typedef int (*SADisplayDriverGetVideoFunc)(SADisplay *display, VisVideo *video);
typedef int (*SADisplayDriverUpdateRectFunc)(SADisplay *display, VisRectangle *rect);
typedef int (*SADisplayDriverDrainEventsFunc)(SADisplay *display, VisEventQueue *eventqueue);

struct _SADisplayDriver {
	VisObject			object;

	SADisplayDriverCreateFunc	create;
	SADisplayDriverLockFunc		lock;
	SADisplayDriverUnlockFunc	unlock;
	SADisplayDriverFullScreenFunc	fullscreen;
	SADisplayDriverGetVideoFunc	getvideo;
	SADisplayDriverUpdateRectFunc	updaterect;
	SADisplayDriverDrainEventsFunc	drainevents;
};

struct _SADisplay {
	VisObject	 object;

	SADisplayDriver	*driver;
	VisVideo	*screen;

	VisObject	*native;
};

/* prototypes */
SADisplay *display_new (SADisplayDriver *driver);

int display_create (SADisplay *display, VisVideoDepth depth, int width, int height, int resizable);

VisVideo *display_get_video (SADisplay *display);

int display_lock (SADisplay *display);
int display_unlock (SADisplay *display);

int display_update_all (SADisplay *display);
int display_update_rectangle (SADisplay *display, VisRectangle *rect);

int display_set_fullscreen (SADisplay *display, int fullscreen);

int display_drain_events (SADisplay *display, VisEventQueue *eventqueue);

#endif /* _LV_STANDALONE_DISPLAY_H */
