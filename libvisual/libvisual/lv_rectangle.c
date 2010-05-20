/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_rectangle.c,v 1.10 2006/02/13 20:54:08 synap Exp $
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

#include "lv_math.h"
#include "lv_rectangle.h"

/**
 * @defgroup VisRectangle VisRectangle
 * @{
 */

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
VisRectangle *visual_rectangle_new (int x, int y, int width, int height)
{
	VisRectangle *rect;

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (rect), TRUE, NULL);

	visual_rectangle_set (rect, x, y, width, height);

	return rect;
}

/**
 * Sets the values for a VisRectangle.
 * 
 * @param rect Pointer to the VisRectangle in which the values are set.
 * @param x X Position of the upper left corner.
 * @param y Y Position of the upper left corner.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_set (VisRectangle *rect, int x, int y, int width, int height)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	rect->x = x;
	rect->y = y;
	rect->width = width;
	rect->height = height;

	return VISUAL_OK;
}

/**
 * Checks if a certain point is within the defined VisRectangle.
 *
 * @param rect Pointer to the VisRectangle to which the position is checked.
 * @param x X Position of the point to be checked.
 * @param y Y Position of the point to be checked.
 *
 * @return TRUE if within rectangle, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_position_within (VisRectangle *rect, int x, int y)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if ((x >= rect->x) &&
		(x <= rect->x + rect->width) &&
		(y >= rect->y) &&
		(y <= rect->y + rect->height)) {

		return TRUE;
	}

	return FALSE;
}

/**
 * Checks whether the src VisRectangle partially falls within the dest VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle in which the src should partially fall within.
 * @param src Pointer to the source VisRectangle that should partially fall within the destination.
 *
 * @return TRUE if the src partially falls within the dest, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_within_partially (VisRectangle *dest, VisRectangle *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if ((src->x + src->width) < dest->x)
		return FALSE;

	if ((src->y + src->height) < dest->y)
		return FALSE;

	if (src->x > (dest->x + dest->width))
		return FALSE;

	if (src->y > (dest->y + dest->height))
		return FALSE;

	return TRUE;
}

/**
 * Checks whether the src VisRectangle falls within the dest VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle in which the src should fall within.
 * @param src Pointer to the source VisRectangle that should fall within the destination.
 *
 * @return TRUE if the src partially falls within the dest, FALSE if not, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_within (VisRectangle *dest, VisRectangle *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if (src->x < dest->x)
		return FALSE;

	if (src->y < dest->y)
		return FALSE;

	if ((src->x + src->width) > (dest->x + dest->width))
		return FALSE;

	if ((src->y + src->height) > (dest->y + dest->height))
		return FALSE;

	return TRUE;
}

/**
 * Copies the colors from one VisRectangle to another.
 *
 * @param dest Pointer to the destination VisRectangle.
 * @param src Pointer to the source VisRectangle in which dest is copied.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_copy (VisRectangle *dest, VisRectangle *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	dest->x = src->x;
	dest->y = src->y;
	dest->width = src->width;
	dest->height = src->height;

	return VISUAL_OK;
}

/**
 * Clips two VisRectangles into one, This is done by using the within parameter as the
 * boundary for the src parameter, so src is adopten so that is falls within the within parameter.
 * The final result is stored in dest. It's legal to give the same VisRectangle for the dest
 * and src VisRectangle.
 *
 * @param dest Pointer to the destination VisRectangle.
 * @param within Pointer to the boundary VisRectangle.
 * @param src Pointer to the source VisRectangle which is boundary adopted to the within parameter.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL or -VISUAL_ERROR_RECTANGLE_OUT_OF_BOUNDS
 *	on failure.
 */
int visual_rectangle_clip (VisRectangle *dest, VisRectangle *within, VisRectangle *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (within != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	/* If not partially within, don't even try to clip */
	if (visual_rectangle_within_partially (within, src) == FALSE) {
		visual_rectangle_set (dest, 0, 0, 0, 0);

		return -VISUAL_ERROR_RECTANGLE_OUT_OF_BOUNDS;
	}

	visual_rectangle_copy (dest, src);

	/* Left, Upper boundries */
	if (src->x < within->x) {
		dest->width = src->width - (within->x - src->x);
		dest->x = within->x;
	}

	if (src->y < within->y) {
		dest->height = src->height - (within->y - src->y);
		dest->y = within->y;
	}

	/* Right, Lower boundries */
	if (dest->x + dest->width > within->width)
		dest->width = within->width - dest->x;

	if (dest->y + dest->height > within->height)
		dest->height = within->height - dest->y;

	return VISUAL_OK;
}

/**
 * Normalizes the VisRectangle, this means that both the x and y position are set to 0.
 *
 * @param rect Pointer to the VisRectangle that is to be normalized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_normalise (VisRectangle *rect)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	rect->x = 0;
	rect->y = 0;

	return VISUAL_OK;
}

/**
 * Normalizes the VisRectangle to another VisRectangle, this means that the x and y position
 * of the destination VisRectangle is set to that of the source VisRectangle.
 *
 * @param dest Pointer to the VisRectangle that is to be normalized.
 * @param src Pointer to the VisRectangle that is used as the source for normalization.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_normalise_to (VisRectangle *dest, VisRectangle *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	dest->x = src->x;
	dest->y = src->y;

	return VISUAL_OK;
}

/**
 * Checks if a VisRectangle occupies an area. If either the width or height is equal
 * or lower than 0, this returns TRUE.
 *
 * @param rect Pointer to the VisRectangle that is checked for emptyness.
 *
 * @return VISUAL_OK on succes, TRUE if it's empty, FALSE if not or
 *	-VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_is_empty (VisRectangle *rect)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if (rect->width <= 0 || rect->height <= 0)
		return TRUE;

	return FALSE;
}

/**
 * Denormalises a set of floating point x y coordinates that are ranging from 0.0 to 1.0 to absolute
 * locations using the VisRectangle as reference. The floating point coordinates are clamped
 * to 0.0 and 1.0. Don't use this function to do many transforms, instead use 
 * visual_rectangle_denormalise_many_values.
 *
 * @see visual_rectangle_denormalise_many_values
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fx Floating point X location ranging from 0.0 to 1.0.
 * @param fy Floating point Y location ranging from 0.0 to 1.0.
 * @param x Pointer to an integer in which the absolute X location is stored.
 * @param y Pointer to an integer in which the absolute Y location is stored.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_values (VisRectangle *rect, float fx, float fy, int32_t *x, int32_t *y)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if (fx < 0)
		fx = 0;

	if (fx > 1)
		fx = 1;

	if (fy < 0)
		fy = 0;

	if (fy > 1)
		fy = 1;

	*x = rect->width * fx;
	*y = rect->height * fy;

	return VISUAL_OK;
}

/**
 * Denormalises a list of floating point x y coordinates tht are ranging from 0.0 to 1.0 to absolute
 * locations using the VisRectangle as reference. WARNING: Unlike visual_rectangle_denormalise_values, the
 * floating point locations are NOT clamped. This is done because of performance reasons.
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fxlist Pointer to a floating point X location ranging from 0.0 to 1.0 array.
 * @param fylist Pointer to a floating point Y location ranging from 0.0 to 1.0 array.
 * @param xlist  Pointer to an array of integers in which the absolute X location are stored.
 * @param ylist  Pointer to an array of integers in which the absolute Y location are stored.
 * @param size The size of the arrays for all X Y locations.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_many_values (VisRectangle *rect, float *fxlist, float *fylist, int32_t *xlist, int32_t *ylist, int size)
{
	int i;

	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_math_vectorized_floats_to_int32s_multiply (xlist, fxlist, size, rect->width);
	visual_math_vectorized_floats_to_int32s_multiply (ylist, fylist, size, rect->height);

	return VISUAL_OK;
}

/**
 * Denormalises a set of floating point x y coordinates that are ranging from -1.0 to 1.0 to absolute
 * locations using the VisRectangle as reference. The floating point coordinates are clamped
 * to -1.0 and 1.0. Don't use this function to do many transforms, instead use 
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_values_neg (VisRectangle *rect, float fx, float fy, int32_t *x, int32_t *y)
{
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	if (fx < -1)
		fx = -1;

	if (fx > 1)
		fx = 1;

	if (fy < -1)
		fy = -1;

	if (fy > 1)
		fy = 1;

	fx = (fx + 1) * 0.5;
	fy = (fy + 1) * 0.5;

	*x = rect->width * fx;
	*y = rect->height * fy;

	return VISUAL_OK;
}

/**
 * Denormalises a list of floating point x y coordinates tht are ranging from -1.0 to 1.0 to absolute
 * locations using the VisRectangle as reference. WARNING: Unlike visual_rectangle_denormalise_values_neg, the
 * floating point locations are NOT clamped. This is done because of performance reasons.
 *
 * @param rect Pointer to the VisRectangle used as the reference.
 * @param fxlist Pointer to a floating point X location ranging from -1.0 to 1.0 array.
 * @param fylist Pointer to a floating point Y location ranging from -1.0 to 1.0 array.
 * @param xlist Pointer to an array of integers in which the absolute X location are stored.
 * @param ylist Pointer to an array of integers in which the absolute Y location are stored.
 * @param size The size of the arrays for all X Y locations.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_rectangle_denormalise_many_values_neg (VisRectangle *rect, float *fxlist, float *fylist, int32_t *xlist, int32_t *ylist, int size)
{
	visual_math_vectorized_floats_to_int32s_multiply_denormalise (xlist, fxlist, size, rect->width);
	visual_math_vectorized_floats_to_int32s_multiply_denormalise (ylist, fylist, size, rect->height);

	return VISUAL_OK;
}

/**
 * @}
 */

