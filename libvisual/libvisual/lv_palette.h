#ifndef _LV_PALETTE_H
#define _LV_PALETTE_H

#include <libvisual/lv_common.h>
#include <libvisual/lv_color.h>
	
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_PALETTE(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisPalette))
	
typedef struct _VisPalette VisPalette;

/**
 * Data type to describe the palette for an 8 bits screen depth.
 * 
 * To access the RGB value of a certain indexed color simply do:
 * pal->colors[index].(r,g,b)
 *
 * @see visual_palette_new
 */
struct _VisPalette {
	VisObject	 object;	/**< The VisObject data. */
	int		 ncolors;	/**< Number of color entries in palette. */
	VisColor	*colors;	/**< Pointer to the colors. */
};

VisPalette *visual_palette_new (int ncolors);
int visual_palette_copy (VisPalette *dest, const VisPalette *src);
int visual_palette_allocate_colors (VisPalette *pal, int ncolors);
int visual_palette_free_colors (VisPalette *pal);
int visual_palette_blend (VisPalette *dest, const VisPalette *src1, const VisPalette *src2, float rate);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PALETTE_H */
