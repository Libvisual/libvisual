#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_common.h"
#include "lv_palette.h"

/**
 * @defgroup VisPalette VisPalette
 * @{
 */

/**
 * Creates a new VisPalette.
 *
 * @return A newly allocated VisPalette.
 */
VisPalette *visual_palette_new (int ncolors)
{
	VisPalette *pal;

	pal = visual_mem_new0 (VisPalette, 1);

	visual_palette_allocate_colors (pal, ncolors);

	return pal;
}

/**
 * Frees the VisPalette, This frees the VisPalette data structure.
 *
 * @param pal Pointer to the VisPalette that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_palette_free (VisPalette *pal)
{
	visual_log_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	if (pal->colors != NULL)
		visual_palette_free_colors (pal);
	
	return visual_mem_free (pal);
}

/**
 * Copies the colors from one VisPalette to another.
 *
 * @param dest Pointer to the destination VisPalette.
 * @param src Pointer to the source VisPalette from which colors are copied into the destination VisPalette.
 *
 * @return 0 on succes -1 on error.
 */
int visual_palette_copy (VisPalette *dest, const VisPalette *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_log_return_val_if_fail (dest->ncolors == src->ncolors, -1);

	memcpy (dest->colors, src->colors, sizeof (VisColor) * dest->ncolors);

	return VISUAL_OK;
}

/**
 * Allocate an amount of colors for a VisPalette.
 *
 * @param pal Pointer to the VisPalette for which colors are allocated.
 * @param ncolors The number of colors allocated for the VisPalette.
 *
 * @return 0 on succes -1 on error.
 */
int visual_palette_allocate_colors (VisPalette *pal, int ncolors)
{
	visual_log_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	pal->colors = visual_mem_new0 (VisColor, ncolors);
	pal->ncolors = ncolors;

	return VISUAL_OK;
}

/**
 * Frees allocated colors from a VisPalette.
 * 
 * @param pal Pointer to the VisPalette from which colors need to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_palette_free_colors (VisPalette *pal)
{
	visual_log_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	if (pal->colors != NULL)
		visual_mem_free (pal->colors);

	return VISUAL_OK;
}

/**
 * This function is capable of morphing from one palette to another.
 *
 * @param dest Pointer to the destination VisPalette, this is where the result of the morph
 * 	  is put.
 * @param src1 Pointer to a VisPalette that acts as the first source for the morph.
 * @param src2 Pointer to a VisPalette that acts as the second source for the morph.
 * @param rate Value that sets the rate of the morph, which is valid between 0 and 1.
 *
 * @return 0 on succes -1 on error.
 */
int visual_palette_blend (VisPalette *dest, const VisPalette *src1, const VisPalette *src2, float rate)
{
	int i;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_PALETTE_NULL);
	
	if (src1->ncolors != src2->ncolors)
		return -VISUAL_ERROR_PALETTE_SIZE;

	if (dest->ncolors != src1->ncolors)
		return -VISUAL_ERROR_PALETTE_SIZE;

	for (i = 0; i < dest->ncolors; i++) {
		dest->colors[i].r = src1->colors[i].r + ((src2->colors[i].r - src1->colors[i].r) * rate);
		dest->colors[i].g = src1->colors[i].g + ((src2->colors[i].g - src1->colors[i].g) * rate);
		dest->colors[i].b = src1->colors[i].b + ((src2->colors[i].b - src1->colors[i].b) * rate);
	}

	return VISUAL_OK;
}

/**
 * @}
 */

