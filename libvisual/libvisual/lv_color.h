#ifndef _LV_COLOR_H
#define _LV_COLOR_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_COLOR(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisColor))

typedef struct _VisColor VisColor;

/**
 * Data type to describe a palette entry, or a color. The HSV isn't kept in sync with RGB automaticly
 * but it's there especially for VisUI.
 */
struct _VisColor {
	VisObject	object;	/**< The VisObject data. */
	uint8_t		r;	/**< The red channel of this VisColor. */
	uint8_t		g;	/**< The green channel of this VisColor. */
	uint8_t		b;	/**< The blue channel of this VisColor. */
	uint8_t		unused;	/**< Unused. */
};

VisColor *visual_color_new (void);
int visual_color_compare (VisColor *src1, VisColor *src2);
int visual_color_from_hsv (VisColor *color, float h, float s, float v);
int visual_color_to_hsv (VisColor *color, float *h, float *s, float *v);
int visual_color_copy (VisColor *dest, VisColor *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_COLOR_H */
