/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_rectangle.h,v 1.6 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_RECTANGLE_H
#define _LV_RECTANGLE_H

#include <libvisual/lv_object.h>

/**
 * @defgroup VisRectangle VisRectangle
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_RECTANGLE(obj)				(VISUAL_CHECK_CAST ((obj, VisRectangle))

typedef struct _VisRectangle VisRectangle;

/**
 * Using the VisRectangle structure you can define rectangular areas in for example
 * VisVideo buffers.
 */
struct _VisRectangle {
	VisObject	object;		/**< The VisObject data. */
	int		x;		/**< X Position of the upper left corner. */
	int		y;		/**< Y Position of the upper left corner. */
	int		width;		/**< The width. */
	int		height;		/**< The height. */
};

/**
 * Creates a new VisRectangle.
 *
 * @param x X Position of the upper left corner.
 * @param y Y Position of the upper left corner.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 *
 * @return A newly allocated VisRectangle.
 */
VisRectangle *visual_rectangle_new (int x, int y, int width, int height);

/**
 * Sets the values for a VisRectangle.
 *
 * @param rect Pointer to the VisRectangle in which the values are set.
 * @param x X Position of the upper left corner.
 * @param y Y Position of the upper left corner.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_set (VisRectangle *rect, int x, int y, int width, int height);

/**
 * Checks whether the src VisRectangle falls within the dest VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle in which the src should fall within.
 * @param src Pointer to the source VisRectangle that should fall within the destination.
 *
 * @return TRUE if the src partially falls within the dest, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_position_within (VisRectangle *rect, int x, int y);

/**
 * Checks whether the src VisRectangle partially falls within the dest
 * VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle in which the src should partially fall within.
 * @param src Pointer to the source VisRectangle that should partially fall within the destination.
 *
 * @return TRUE if the src partially falls within the dest, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_within_partially (VisRectangle *dest, VisRectangle *src);

/**
 * Checks if a certain point is within the defined VisRectangle.
 *
 * @param rect Pointer to the VisRectangle to which the position is checked.
 * @param x X Position of the point to be checked.
 * @param y Y Position of the point to be checked.
 *
 * @return TRUE if within rectangle, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_within (VisRectangle *dest, VisRectangle *src);

/**
 * Copies the colors from one VisRectangle to another.
 *
 * @param dest Pointer to the destination VisRectangle.
 * @param src Pointer to the source VisRectangle in which dest is copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_copy (VisRectangle *dest, VisRectangle *src);

/**
 * Clips two VisRectangles into one, This is done by using the within
 * parameter as the boundary for the src parameter, so src is adopten
 * so that is falls within the within parameter.  The final result is
 * stored in dest. It's legal to give the same VisRectangle for the
 * dest and src VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle.
 * @param within Pointer to the boundary VisRectangle.
 * @param src Pointer to the source VisRectangle which is boundary adopted to the within parameter.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL or -VISUAL_ERROR_RECTANGLE_OUT_OF_BOUNDS
 *	on failure.
 */
int visual_rectangle_clip (VisRectangle *dest, VisRectangle *within, VisRectangle *src);

/**
 * Normalizes the VisRectangle, this means that both the x and y
 * position are set to 0.
 *
 * @param rect Pointer to the VisRectangle that is to be normalized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_normalise (VisRectangle *rect);


/**
 * Normalizes the VisRectangle to another VisRectangle, this means
 * that the x and y position of the destination VisRectangle is set to
 * that of the source VisRectangle.
 *
 * @param dest Pointer to the VisRectangle that is to be normalized.
 * @param src Pointer to the VisRectangle that is used as the source for normalization.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_normalise_to (VisRectangle *dest, VisRectangle *src);

/**
 * Checks if a VisRectangle occupies an area. If either the width or
 * height is equal or lower than 0, this returns TRUE.
 *
 * @param rect Pointer to the VisRectangle that is checked for emptyness.
 *
 * @return VISUAL_OK on success, TRUE if it's empty, FALSE if not or
 *	-VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_is_empty (VisRectangle *rect);

/**
 * Denormalises a set of floating point x y coordinates that are
 * ranging from 0.0 to 1.0 to absolute locations using the
 * VisRectangle as reference. The floating point coordinates are
 * clamped to 0.0 and 1.0. Don't use this function to do many
 * transforms, instead use visual_rectangle_denormalise_many_values.
 *
 * @see visual_rectangle_denormalise_many_values
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fx Floating point X location ranging from 0.0 to 1.0.
 * @param fy Floating point Y location ranging from 0.0 to 1.0.
 * @param x Pointer to an integer in which the absolute X location is stored.
 * @param y Pointer to an integer in which the absolute Y location is stored.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_values (VisRectangle *rect, float fx, float fy, int32_t *x, int32_t *y);

/**
 * Denormalises a list of floating point x y coordinates tht are
 * ranging from 0.0 to 1.0 to absolute locations using the
 * VisRectangle as reference. WARNING: Unlike
 * visual_rectangle_denormalise_values, the floating point locations
 * are NOT clamped. This is done because of performance reasons.
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fxlist Pointer to a floating point X location ranging from 0.0 to 1.0 array.
 * @param fylist Pointer to a floating point Y location ranging from 0.0 to 1.0 array.
 * @param xlist  Pointer to an array of integers in which the absolute X location are stored.
 * @param ylist  Pointer to an array of integers in which the absolute Y location are stored.
 * @param size The size of the arrays for all X Y locations.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_many_values (VisRectangle *rect, float *fxlist, float *fylist, int32_t *xlist, int32_t *ylist, int size);

/**
 * Denormalises a set of floating point x y coordinates that are
 * ranging from -1.0 to 1.0 to absolute locations using the
 * VisRectangle as reference. The floating point coordinates are
 * clamped to -1.0 and 1.0. Don't use this function to do many
 * transforms, instead use
 * visual_rectangle_denormalise_many_values_neg.
 *
 * @see visual_rectangle_denormalise_many_values_neg
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fx Floating point X location ranging from -1.0 to 1.0.
 * @param fy Floating point Y location ranging from -1.0 to 1.0.
 * @param x Pointer to an integer in which the absolute X location is stored.
 * @param y Pointer to an integer in which the absolute Y location is stored.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_values_neg (VisRectangle *rect, float fx, float fy, int32_t *x, int32_t *y);


/**
 * Denormalises a list of floating point x y coordinates tht are
 * ranging from -1.0 to 1.0 to absolute locations using the
 * VisRectangle as reference. WARNING: Unlike
 * visual_rectangle_denormalise_values_neg, the floating point
 * locations are NOT clamped. This is done because of performance
 * reasons.
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fxlist Pointer to a floating point X location ranging from -1.0 to 1.0 array.
 * @param fylist Pointer to a floating point Y location ranging from -1.0 to 1.0 array.
 * @param xlist Pointer to an array of integers in which the absolute X location are stored.
 * @param ylist Pointer to an array of integers in which the absolute Y location are stored.
 * @param size The size of the arrays for all X Y locations.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_many_values_neg (VisRectangle *rect, float *fxlist, float *fylist, int32_t *xlist, int32_t *ylist, int size);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_RECTANGLE_H */
