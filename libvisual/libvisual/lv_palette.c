#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
VisPalette *visual_palette_new ()
{
	VisPalette *pal;

	pal = malloc (sizeof (VisPalette));
	memset (pal, 0, sizeof (VisPalette));

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
	if (pal == NULL)
		return -1;

	free (pal);

	return 0;
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
int visual_palette_blend (VisPalette *dest, VisPalette *src1, VisPalette *src2, float rate)
{
	int i;

	if (dest == NULL || src1 == NULL || src2 == NULL)
		return -1;

	for (i = 0; i < 256; i++) {
		dest->r[i] = src1->r[i] + ((src2->r[i] - src1->r[i]) * rate);
		dest->g[i] = src1->g[i] + ((src2->g[i] - src1->g[i]) * rate);
		dest->b[i] = src1->b[i] + ((src2->b[i] - src1->b[i]) * rate);
	}

	return 0;
}

/**
 * @}
 */

