/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_color.c,v 1.18.2.1 2006/03/04 12:32:47 descender Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_common.h"
#include "lv_color.h"

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;


/**
 * @defgroup VisColor VisColor
 * @{
 */

/**
 * Creates a new VisColor structure
 *
 * @return A newly allocated VisColor.
 */
VisColor *visual_color_new ()
{
	VisColor *color;

	color = visual_mem_new0 (VisColor, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (color), TRUE, NULL);

	return color;
}

/**
 * Sets the VisColor to a certain rgb value.
 *
 * @param color Pointer to the VisColor to which the rgb value is set.
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_set (VisColor *color, uint8_t r, uint8_t g, uint8_t b)
{
	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

	color->r = r;
	color->g = g;
	color->b = b;

	return VISUAL_OK;
}

/**
 * Compares two VisColors with each other. If they are not the same, 0 is returned, if the same 1.
 *
 * @param src1 Pointer to the first VisColor for comparison.
 * @param src2 Pointer to the second VisColor for comparison.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_compare (VisColor *src1, VisColor *src2)
{
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_COLOR_NULL)
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_COLOR_NULL)

	if (src1->r != src2->r || src1->g != src2->g || src1->b != src2->b)
		return FALSE;

	return TRUE;
}

/**
 * Fills the VisColor it's rgb values from hsv colorspace values.
 *
 * @param color Pointer to a VisColor which rgb values are filled.
 * @param h Hue value for the hsv colorspace, ranging from 0 to 360.
 * @param s Saturation value for the hsv colorspace, ranging from 0 to 1.
 * @param v Value value for the hsv colorspace, ranging from 0 to 1.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_from_hsv (VisColor *color, float h, float s, float v)
{
	int i;
	float f, w, q, t, r = 0, g = 0, b = 0;

	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

	if (s == 0.0)
		s = 0.000001;

	if (h == 360.0)
		h = 0.0;

	h = h / 60.0;
	i = (int) h;
	f = h - i;
	w = v * (1.0 - s);
	q = v * (1.0 - (s * f));
	t = v * (1.0 - (s * (1.0 - f)));

	switch (i) {
		case 0: r = v; g = t; b = w; break;
		case 1: r = q; g = v; b = w; break;
		case 2: r = w; g = v; b = t; break;
		case 3: r = w; g = q; b = v; break;
		case 4: r = t; g = w; b = v; break;
		case 5: r = v; g = w; b = q; break;

		default:
			break;
	}

	visual_color_set (color, (float) r * 255, (float) g * 255, (float) b * 255);

	return VISUAL_OK;
}

/**
 * Creates hsv colorspace values from a VisColor
 *
 * @param color Pointer to a VisColor from which hsv colorspace values are created.
 * @param h Float pointer to a hue value for the hsv colorspace, ranging from 0 to 360.
 * @param s Float pointer to a saturation value for the hsv colorspace, ranging from 0 to 1.
 * @param v Float pointer to a value value for the hsv colorspace, ranging from 0 to 1.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_to_hsv (VisColor *color, float *h, float *s, float *v)
{

        float max, min, delta, r, g, b;

	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

	r = (float) color->r / 255.0;
	g = (float) color->g / 255.0;
	b = (float) color->b / 255.0;

	max = r;
	if (g > max)
		max = g;

	if (b > max)
		max = b;

	min = r;
	if (g < min)
		min = g;

	if (b < min)
		min = b;

	*v = max;

	if (max != 0.0)
		*s = (max - min) / max;
	else
		*s = 0.0;

	if (*s == 0.0) {
		*h = 0.0;
	} else {
		delta = max - min;

		if (r == max)
			*h = (g - b) / delta;
		else if (g == max)
			*h = 2.0 + (b - r) / delta;
		else if (b == max)
			*h = 4.0 + (r - g) / delta;

		*h = *h * 60.0;

		if (*h < 0.0)
			*h = *h + 360;
	}


	return VISUAL_OK;
}

/**
 * Copies the RGB data of one VisColor into another.
 *
 * @param dest Pointer to the destination VisColor in which the RGB data is copied.
 * @param src Pointer to the source VisColor from which the RGB data is copied.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_copy (VisColor *dest, VisColor *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_COLOR_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_COLOR_NULL);

	visual_color_set (dest, src->r, src->g, src->b);

	/* You never know ;) */
	dest->unused = src->unused;

	return VISUAL_OK;
}

int visual_color_from_uint32 (VisColor *color, uint32_t rgb)
{
	uint8_t *colors = (uint8_t *) &rgb;

	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

	color->r = colors[0];
	color->g = colors[1];
	color->b = colors[2];

	return VISUAL_OK;
}

int visual_color_from_uint16 (VisColor *color, uint16_t rgb)
{
	_color16 *colors = (_color16 *) &rgb;

	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

	color->r = colors->r << 2;
	color->g = colors->g << 3;
	color->b = colors->b << 2;

	return VISUAL_OK;
}

uint32_t visual_color_to_uint32 (VisColor *color)
{
	uint32_t colors;

	visual_log_return_val_if_fail (color != NULL, 0);

	colors = (256 << 24) |
		(color->r << 16) |
		(color->g << 8) |
		(color->b);

	return colors;
}

uint16_t visual_color_to_uint16 (VisColor *color)
{
	_color16 colors;

	visual_log_return_val_if_fail (color != NULL, 0);

	colors.r = color->r >> 2;
	colors.g = color->g >> 3;
	colors.b = color->b >> 2;

	return *((uint16_t *) &colors);
}

VisColor *visual_color_black ()
{
	static VisColor black;
	static int black_initialized = FALSE;

	if (black_initialized == FALSE) {
		visual_object_initialize (VISUAL_OBJECT (&black), FALSE, NULL);
		visual_color_set (&black, 0, 0, 0);

		black_initialized = TRUE;
	}

	return &black;
}

VisColor *visual_color_white ()
{
	static VisColor white;
	static int white_initialized = FALSE;

	if (white_initialized == FALSE) {
		visual_object_initialize (VISUAL_OBJECT (&white), FALSE, NULL);
		visual_color_set (&white, 255, 255, 255);

		white_initialized = TRUE;
	}

	return &white;
}

/**
 * @}
 */

