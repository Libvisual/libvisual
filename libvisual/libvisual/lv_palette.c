/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_palette.c,v 1.21 2006/01/22 13:23:37 synap Exp $
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

#include "config.h"
#include "lv_palette.h"
#include "lv_common.h"

static int palette_dtor (VisObject *object);

static int palette_dtor (VisObject *object)
{
	VisPalette *pal = VISUAL_PALETTE (object);

	if (pal->colors != NULL)
		visual_palette_free_colors (pal);

	pal->colors = NULL;

	return VISUAL_OK;
}

VisPalette *visual_palette_new (int ncolors)
{
	VisPalette *pal;

	pal = visual_mem_new0 (VisPalette, 1);

	visual_palette_init (pal);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (pal), TRUE);
	visual_object_ref (VISUAL_OBJECT (pal));

	visual_palette_allocate_colors (pal, ncolors);

	return pal;
}

int visual_palette_init (VisPalette *pal)
{
	visual_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (pal));
	visual_object_set_dtor (VISUAL_OBJECT (pal), palette_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (pal), FALSE);

	/* Reset the VisPalette data */
	pal->ncolors = 0;
	pal->colors = NULL;

	return VISUAL_OK;
}

int visual_palette_copy (VisPalette *dest, VisPalette *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_return_val_if_fail (dest->ncolors == src->ncolors, -VISUAL_ERROR_PALETTE_SIZE);

	visual_mem_copy (dest->colors, src->colors, sizeof (VisColor) * dest->ncolors);

	return VISUAL_OK;
}

int visual_palette_allocate_colors (VisPalette *pal, int ncolors)
{
	visual_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	pal->colors = visual_mem_new0 (VisColor, ncolors);
	pal->ncolors = ncolors;

	return VISUAL_OK;
}

int visual_palette_free_colors (VisPalette *pal)
{
	visual_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);

	if (pal->colors != NULL)
		visual_mem_free (pal->colors);

	pal->colors = NULL;
	pal->ncolors = 0;

	return VISUAL_OK;
}

int visual_palette_blend (VisPalette *dest, VisPalette *src1, VisPalette *src2, float rate)
{
	int i;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_PALETTE_NULL);
	visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_PALETTE_NULL);

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

VisColor *visual_palette_color_cycle (VisPalette *pal, float rate)
{
	VisColor *color, *tmp1, *tmp2;
	int irate = (int) rate;
	unsigned char alpha;
	float rdiff = rate - irate;

	visual_return_val_if_fail (pal != NULL, NULL);

	irate = irate % pal->ncolors;
	alpha = rdiff * 255;

	color = visual_color_new ();

	/* If rate is exactly an item, return that item */
	if (rdiff == 0) {
		visual_color_copy (color, &pal->colors[irate]);

		return color;
	}

	tmp1 = &pal->colors[irate];

	if (irate == pal->ncolors - 1)
		tmp2 = &pal->colors[0];
	else
		tmp2 = &pal->colors[irate + 1];

	color->r = ((alpha * (tmp1->r - tmp2->r) >> 8) + tmp2->r);
	color->g = ((alpha * (tmp1->g - tmp2->g) >> 8) + tmp2->g);
	color->b = ((alpha * (tmp1->b - tmp2->b) >> 8) + tmp2->b);

	return color;
}

int visual_palette_find_color (VisPalette *pal, VisColor *color)
{
	int i;

	for (i = 0; i < pal->ncolors; i++) {
		if (visual_color_compare (&pal->colors[i], color) == TRUE)
			return i;
	}

	return -1;
}

