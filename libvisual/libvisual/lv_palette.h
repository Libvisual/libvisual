/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_palette.h,v 1.15 2006/01/22 13:23:37 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_PALETTE_H
#define _LV_PALETTE_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_color.h>

/**
 * @defgroup VisPalette VisPalette
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_PALETTE(obj)				(VISUAL_CHECK_CAST ((obj), VisPalette))

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
	int			 ncolors;	/**< Number of color entries in palette. */
	VisColor	*colors;	/**< Pointer to the colors. */
};

/**
 * Creates a new VisPalette.
 *
 * @return A newly allocated VisPalette.
 */
VisPalette *visual_palette_new (int ncolors);


/**
 * Initializes a VisPalette, this should not be used to reset a
 * VisPalette.  The resulting initialized VisPalette is a valid
 * VisObject even if it was not allocated.  Keep in mind that
 * VisPalette structures that were created by visual_palette_new()
 * should not be passed to visual_palette_init().
 *
 * @see visual_palette_new
 *
 * @param pal Pointer to the VisPalette which needs to be initialized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PALETTE_NULL on failure.
 */
int visual_palette_init (VisPalette *pal);

/**
 * Copies the colors from one VisPalette to another.
 *
 * @param dest Pointer to the destination VisPalette.
 * @param src Pointer to the source VisPalette from which colors are copied into the destination VisPalette.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PALETTE_NULL or -VISUAL_ERROR_PALETTE_SIZE on failure.
 */
int visual_palette_copy (VisPalette *dest, VisPalette *src);

/**
 * Allocate an amount of colors for a VisPalette.
 *
 * @param pal Pointer to the VisPalette for which colors are allocated.
 * @param ncolors The number of colors allocated for the VisPalette.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PALETTE_NULL on failure.
 */
int visual_palette_allocate_colors (VisPalette *pal, int ncolors);

/**
 * Frees allocated colors from a VisPalette.
 *
 * @param pal Pointer to the VisPalette from which colors need to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PALETTE_NULL on failure.
 */
int visual_palette_free_colors (VisPalette *pal);

/**
 * This function is capable of morphing from one palette to another.
 *
 * @param dest Pointer to the destination VisPalette, this is where the result of the morph
 * 	  is put.
 * @param src1 Pointer to a VisPalette that acts as the first source for the morph.
 * @param src2 Pointer to a VisPalette that acts as the second source for the morph.
 * @param rate Value that sets the rate of the morph, which is valid between 0 and 1.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PALETTE_NULL or -VISUAL_ERROR_PALETTE_SIZE on failure.
 */
int visual_palette_blend (VisPalette *dest, VisPalette *src1, VisPalette *src2, float rate);

/**
 * Can be used to cycle through the colors of a VisPalette and blend between elements. The rate is from 0.0 to number of
 * VisColors in the VisPalette. The VisColor is newly allocated so you have to unref it. The last VisColor in the VisPalette is
 * morphed with the first.
 *
 * @param pal Pointer to the VisPalette in which the VisColors are cycled.
 * @param rate Selection of the VisColor from the VisPalette, goes from 0.0 to number of VisColors in the VisPalette
 * 	and morphs between colors if needed.
 *
 * @return A new VisColor, possibly a morph between two VisColors, NULL on failure.
 */
VisColor *visual_palette_color_cycle (VisPalette *pal, float rate);

int visual_palette_find_color (VisPalette *pal, VisColor *color);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_PALETTE_H */
