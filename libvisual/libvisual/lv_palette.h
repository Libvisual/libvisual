#ifndef _LV_PALETTE_H
#define _LV_PALETTE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _VisPalette VisPalette;

/**
 * Data type to describe the palette for an 8 bits screen depth.
 * 
 * To access the RGB value of a certain indexed color simply do:
 *
 * pal.r[index]; pal.g[index]; pal.b[index];
 *
 * @see visual_palette_new
 */
struct _VisPalette {
	unsigned char r[256];	/**< Red, 256 entries */
	unsigned char g[256];	/**< Green, 256 entries */
	unsigned char b[256];	/**< Blue, 256 entries */
};

VisPalette *visual_palette_new ();
int visual_palette_free (VisPalette *pal);
int visual_palette_blend (VisPalette *dest, VisPalette *src1, VisPalette *src2, float rate);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PALETTE_H */
