#ifndef _ACTOR_GDKPIXBUF_H
#define _ACTOR_GDKPIXBUF_H

#include <gdk/gdk.h>
#include <libvisual/libvisual.h>

typedef struct {
	GdkPixbuf	*pixbuf;
	GdkPixbuf	*scaled;
	VisVideo	 target;
	char		*filename;
	int		 width;
	int		 height;

	/* Config flags, set through the param interface */
	int		 set_scaled;
	int		 aspect;
	int		 center;
	int		 set_size;
	int		 set_width;
	int		 set_height;
	int		 x_offset;
	int		 y_offset;
	int		 interpolate;
} PixbufPrivate;

#endif /* _ACTOR_GDKPIXBUF_H */
