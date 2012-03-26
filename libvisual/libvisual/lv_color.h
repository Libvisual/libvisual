/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_color.h,v 1.17 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_COLOR_H
#define _LV_COLOR_H

#include <libvisual/lv_object.h>

/**
 * @defgroup VisColor VisColor
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_COLOR(obj)				(VISUAL_CHECK_CAST ((obj), VisColor))

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
	uint8_t		a;	/**< Alpha channel of this VisColor */
};

/**
 * Creates a new VisColor structure
 *
 * @return A newly allocated VisColor.
 */
VisColor *visual_color_new (void);

/**
 * Sets the VisColor to a certain rgb value.
 *
 * @param color Pointer to the VisColor to which the rgb value is set.
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_set (VisColor *color, uint8_t r, uint8_t g, uint8_t b);

/**
 * Sets the VisColor to a certain rgba value.
 *
 * @param color Pointer to the VisColor to which the rgb value is set.
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 * @param a The alpha value.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_set_with_alpha(VisColor *color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Compares two VisColors with each other. If they are not the same, 0 is returned, if the same 1.
 *
 * @param src1 Pointer to the first VisColor for comparison.
 * @param src2 Pointer to the second VisColor for comparison.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_compare (VisColor *src1, VisColor *src2);

/**
 * Fills the VisColor it's rgb values from hsv colorspace values.
 *
 * @param color Pointer to a VisColor which rgb values are filled.
 * @param h Hue value for the hsv colorspace, ranging from 0 to 360.
 * @param s Saturation value for the hsv colorspace, ranging from 0 to 1.
 * @param v Value value for the hsv colorspace, ranging from 0 to 1.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_from_hsv (VisColor *color, float h, float s, float v);

/**
 * Creates hsv colorspace values from a VisColor
 *
 * @param color Pointer to a VisColor from which hsv colorspace values are created.
 * @param h Float pointer to a hue value for the hsv colorspace, ranging from 0 to 360.
 * @param s Float pointer to a saturation value for the hsv colorspace, ranging from 0 to 1.
 * @param v Float pointer to a value value for the hsv colorspace, ranging from 0 to 1.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_to_hsv (VisColor *color, float *h, float *s, float *v);

/**
 * Copies the RGB data of one VisColor into another.
 *
 * @param dest Pointer to the destination VisColor in which the RGB data is copied.
 * @param src Pointer to the source VisColor from which the RGB data is copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_COLOR_NULL on failure.
 */
int visual_color_copy (VisColor *dest, VisColor *src);

int visual_color_from_uint32 (VisColor *color, uint32_t rgb);
int visual_color_from_uint16 (VisColor *color, uint16_t rgb);
uint32_t visual_color_to_uint32 (VisColor *color);
uint16_t visual_color_to_uint16 (VisColor *color);

VisColor *visual_color_black (void);
VisColor *visual_color_white (void);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_COLOR_H */
