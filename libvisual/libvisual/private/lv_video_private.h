#ifndef _LV_VIDEO_PRIVATE_H
#define _LV_VIDEO_PRIVATE_H

#include "lv_video.h"

struct _VisVideo {
	int                  width;	    /**< Surface it's width. */
	int                  height;    /**< Surface it's height. */
	VisVideoDepth        depth;     /**< Surface it's depth. */
	int                  bpp;       /**< Surface it's bytes per pixel. */
	int                  pitch;     /**< Surface it's pitch value. Value contains
	                                   * the number of bytes per line. */
	VisBuffer           *buffer;	/**< The video buffer. */
	void               **pixel_rows;/**< Pixel row start pointer table. */
	VisPalette          *pal;       /**< Optional pointer to the palette. */

	/* Sub region */
	VisVideo            *parent;    /**< The surface it's parent, ONLY when it is a subregion. */
	VisRectangle        *rect;      /**< The rectangle over the parent surface. */

	/* Compose control */
	VisVideoComposeType  compose_type; /**< The surface it's compose type. */
	VisVideoComposeFunc	 compose_func; /**< The surface it's custom compose function. */
	VisColor            *colorkey;     /**< The surface it's alpha colorkey. */
	uint8_t              alpha;        /**< The surface it's alpha. */

	unsigned int         refcount;
};

#endif /* _LV_VIDEO_PRIVATE_H */
