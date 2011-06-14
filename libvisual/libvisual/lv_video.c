/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Duilio J. Protti <dprotti@users.sourceforge.net>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *	    Jean-Christophe Hoelt <jeko@ios-software.com>
 *	    Jaak Randmets <jaak.ra@gmail.com>
 *
 * $Id: lv_video.c,v 1.86.2.1 2006/03/04 12:32:47 descender Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include <lvconfig.h>
#include "lv_common.h"
#include "lv_video.h"
#include "lv_cpu.h"
#include "lv_log.h"
#include "lv_mem.h"

/* FIXME put these in lv_color.h */
typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} _color24;

/* The VisVideo dtor function */
static int video_dtor (VisObject *object);

/* Precomputation functions */
static void precompute_row_table (VisVideo *video);

/* Blit overlay functions */
static int blit_overlay_noalpha (VisVideo *dest, VisVideo *src);
static int blit_overlay_alphasrc (VisVideo *dest, VisVideo *src);
static int blit_overlay_colorkey (VisVideo *dest, VisVideo *src);
static int blit_overlay_surfacealpha (VisVideo *dest, VisVideo *src);
static int blit_overlay_surfacealphacolorkey (VisVideo *dest, VisVideo *src);

/* Color fill functions */
static int fill_color8 (VisVideo *video, VisColor *color);
static int fill_color16 (VisVideo *video, VisColor *color);
static int fill_color24 (VisVideo *video, VisColor *color);
static int fill_color32 (VisVideo *video, VisColor *color);

/* Rotate functions */
static int rotate_90 (VisVideo *dest, VisVideo *src);
static int rotate_180 (VisVideo *dest, VisVideo *src);
static int rotate_270 (VisVideo *dest, VisVideo *src);

/* Mirror functions */
static int mirror_x (VisVideo *dest, VisVideo *src);
static int mirror_y (VisVideo *dest, VisVideo *src);

/* Depth conversions */
static int depth_transform_get_smallest (VisVideo *dest, VisVideo *src, int *width, int *height);

static int depth_transform_8_to_16_c (VisVideo *dest, VisVideo *src);
static int depth_transform_8_to_24_c (VisVideo *dest, VisVideo *src);
static int depth_transform_8_to_32_c (VisVideo *dest, VisVideo *src);

static int depth_transform_16_to_8_c (VisVideo *dest, VisVideo *src);
static int depth_transform_16_to_24_c (VisVideo *dest, VisVideo *src);
static int depth_transform_16_to_32_c (VisVideo *dest, VisVideo *src);

static int depth_transform_24_to_8_c (VisVideo *dest, VisVideo *src);
static int depth_transform_24_to_16_c (VisVideo *dest, VisVideo *src);
static int depth_transform_24_to_32_c (VisVideo *dest, VisVideo *src);

static int depth_transform_32_to_8_c (VisVideo *dest, VisVideo *src);
static int depth_transform_32_to_16_c (VisVideo *dest, VisVideo *src);
static int depth_transform_32_to_24_c (VisVideo *dest, VisVideo *src);

/* BGR to RGB conversions */
static int bgr_to_rgb16 (VisVideo *dest, VisVideo *src);
static int bgr_to_rgb24 (VisVideo *dest, VisVideo *src);
static int bgr_to_rgb32 (VisVideo *dest, VisVideo *src);

/* Fast double pixeler zoomer */
static int zoom_8 (VisVideo *dest, VisVideo *src);
static int zoom_16 (VisVideo *dest, VisVideo *src);
static int zoom_24 (VisVideo *dest, VisVideo *src);
static int zoom_32 (VisVideo *dest, VisVideo *src);

/* Scaling functions */
static int scale_nearest_8 (VisVideo *dest, VisVideo *src);
static int scale_nearest_16 (VisVideo *dest, VisVideo *src);
static int scale_nearest_24 (VisVideo *dest, VisVideo *src);
static int scale_nearest_32 (VisVideo *dest, VisVideo *src);

/* Bilinear filter functions */
static int scale_bilinear_8 (VisVideo *dest, VisVideo *src);
static int scale_bilinear_16 (VisVideo *dest, VisVideo *src);
static int scale_bilinear_24 (VisVideo *dest, VisVideo *src);
static int scale_bilinear_32 (VisVideo *dest, VisVideo *src);

static int video_dtor (VisObject *object)
{
	VisVideo *video = VISUAL_VIDEO (object);

	if (video->pixel_rows != NULL)
		visual_mem_free (video->pixel_rows);

	if (video->parent != NULL)
		visual_object_unref (VISUAL_OBJECT (video->parent));

	if (video->buffer != NULL)
		visual_object_unref (VISUAL_OBJECT (video->buffer));

	video->pixel_rows = NULL;
	video->parent = NULL;
	video->buffer = NULL;

	return VISUAL_OK;
}


/**
 * @defgroup VisVideo VisVideo
 * @{
 */

/**
 * Creates a new VisVideo structure, without an associated screen buffer.
 *
 * @return A newly allocated VisVideo.
 */
VisVideo *visual_video_new ()
{
	VisVideo *video;

	video = visual_mem_new0 (VisVideo, 1);

	visual_video_init (video);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (video), TRUE);
	visual_object_ref (VISUAL_OBJECT (video));

	return video;
}

/**
 * Initializes a VisVideo, this will set the allocated flag for the object to FALSE.
 * When visual_video_new() is used, this function should not be used since visual_video_new() makes
 * sure that the VisObject initialization is done right. It's best to use this function in cases where
 * the VisVideo was not being allocated. To cleanup the none allocated VisVideo you can still use
 * visual_object_unref(). When it loses all references, it will get internally cleaned up.
 * Added to that, don't use this function to reset your VisVideo.
 *
 * @see visual_video_new
 * 
 * @param video Pointer to the VisVideo that is to be initialized.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_init (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (video));
	visual_object_set_dtor (VISUAL_OBJECT (video), video_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (video), FALSE);

	/* Reset the VisVideo data */
	video->buffer = visual_buffer_new ();

	video->pixel_rows = NULL;

	visual_video_set_attributes (video, 0, 0, 0, VISUAL_VIDEO_DEPTH_NONE);
	visual_video_set_buffer (video, NULL);
	visual_video_set_palette (video, NULL);

	video->parent = NULL;
	visual_rectangle_set (&video->rect, 0, 0, 0, 0);

	/* Composite control */
	video->compositetype = VISUAL_VIDEO_COMPOSITE_TYPE_SRC;

	return VISUAL_OK;
}

/**
 * Creates a new VisVideo and also allocates a buffer.
 *
 * @param width The width for the new buffer.
 * @param height The height for the new buffer.
 * @param depth The depth being used.
 *
 * @return A newly allocates VisVideo with a buffer allocated.
 */
VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth)
{
	VisVideo *video;
	int ret;

	video = visual_video_new ();

	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);

	ret = visual_video_allocate_buffer (video);

	if (ret < 0) {
		visual_object_unref (VISUAL_OBJECT (video));

		return NULL;
	}

	return video;
}

/**
 * Frees the buffer that relates to the VisVideo.
 *
 * @param video Pointer to a VisVideo of which the buffer needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL or -VISUAL_ERROR_VIDEO_NO_ALLOCATED
 *	on failure.
 */
int visual_video_free_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (visual_video_get_pixels (video) != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);

	if (video->pixel_rows != NULL)
		visual_mem_free (video->pixel_rows);

	if (visual_buffer_get_allocated (video->buffer)) {

		visual_buffer_destroy_content (video->buffer);

	} else {
		return -VISUAL_ERROR_VIDEO_NO_ALLOCATED;
	}

	video->pixel_rows = NULL;
	visual_buffer_set_data_pair (video->buffer, NULL, 0);

	return VISUAL_OK;
}

/**
 * Allocates a buffer for the VisVideo. Allocates based on the
 * VisVideo it's information about the screen dimension and depth.
 *
 * @param video Pointer to a VisVideo that needs an allocated buffer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_HAS_PIXELS  on failure.
 */
int visual_video_allocate_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->buffer != NULL, -VISUAL_ERROR_VIDEO_BUFFER_NULL);

	if (visual_video_get_pixels (video) != NULL) {
		if (visual_buffer_get_allocated (video->buffer)) {
			visual_video_free_buffer (video);
		} else {
			visual_log (VISUAL_LOG_CRITICAL, _("Trying to allocate an screen buffer on "
					"a VisVideo structure which points to an external screen buffer"));

			return -VISUAL_ERROR_VIDEO_HAS_PIXELS;
		}
	}

	if (visual_video_get_size (video) == 0) {
		visual_buffer_set_data (video->buffer, NULL);

		return VISUAL_OK;
	}

	visual_buffer_set_destroyer (video->buffer, visual_buffer_destroyer_free);
	visual_buffer_set_size (video->buffer, visual_video_get_size (video));
	visual_buffer_allocate_data (video->buffer);

	video->pixel_rows = visual_mem_new0 (void *, video->height);
	precompute_row_table (video);

	return VISUAL_OK;
}

/**
 * Checks if the given VisVideo has a private allocated buffer.
 *
 * @param video Pointer to the VisVideo of which we want to know if it has a private allocated buffer.
 *
 * @return TRUE if the VisVideo has an allocated buffer, or FALSE if not.
 */
int visual_video_have_allocated_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, FALSE);

	return visual_buffer_get_allocated (video->buffer);
}

static void precompute_row_table (VisVideo *video)
{
	void **table, *row;
	int y;

	visual_log_return_if_fail (video->pixel_rows != NULL);

	table = video->pixel_rows;
	row = visual_video_get_pixels (video);

	for (y = 0; y < video->height; y++, row += video->pitch)
		*table++ = row;
}

/**
 * Clones the information from a VisVideo to another.
 * This will clone the depth, dimension and screen pitch into another VisVideo.
 * It doesn't clone the palette or buffer.
 *
 * @param dest Pointer to a destination VisVideo in which the information needs to
 *	be placed.
 * @param src Pointer to a source VisVideo from which the information needs to
 *	be obtained.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_clone (VisVideo *dest, VisVideo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_set_depth (dest, src->depth);
	visual_video_set_dimension (dest, src->width, src->height);
	visual_video_set_pitch (dest, src->pitch);

	return VISUAL_OK;
}

/**
 * Checks if two VisVideo objects are the same depth, pitch and dimension wise.
 *
 * @param src1 Pointer to the first VisVideo that is used in the compare.
 * @param src2 Pointer to the second VisVideo that is used in the compare.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_compare (VisVideo *src1, VisVideo *src2)
{
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (visual_video_compare_ignore_pitch (src1, src2) == FALSE)
		return FALSE;

	if (src1->pitch != src2->pitch)
		return FALSE;

	/* We made it to the end, the VisVideos are likewise in depth, pitch, dimensions */
	return TRUE;
}

/**
 * Checks if two VisVideo objects are the same depth and dimension wise.
 *
 * @param src1 Pointer to the first VisVideo that is used in the compare.
 * @param src2 Pointer to the second VisVideo that is used in the compare.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_compare_ignore_pitch (VisVideo *src1, VisVideo *src2)
{
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (src1->depth != src2->depth)
		return FALSE;

	if (src1->width != src2->width)
		return FALSE;

	if (src1->height != src2->height)
		return FALSE;

	/* We made it to the end, the VisVideos are likewise in depth, pitch, dimensions */
	return TRUE;
}

/**
 * Sets a palette to a VisVideo. Links a VisPalette to the
 * VisVideo.
 *
 * @param video Pointer to a VisVideo to which a VisPalette needs to be linked.
 * @param pal Pointer to a Vispalette that needs to be linked with the VisVideo.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_palette (VisVideo *video, VisPalette *pal)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->pal = pal;

	return VISUAL_OK;
}

/**
 * Sets a buffer to a VisVideo. Links a sreenbuffer to the
 * VisVideo.
 *
 * @warning The given @a video must be one previously created with visual_video_new(),
 * and not with visual_video_new_with_buffer().
 *
 * @param video Pointer to a VisVideo to which a buffer needs to be linked.
 * @param buffer Pointer to a buffer that needs to be linked with the VisVideo.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_HAS_ALLOCATED on failure.
 */
int visual_video_set_buffer (VisVideo *video, void *buffer)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (visual_buffer_get_allocated (video->buffer)) {
		visual_log (VISUAL_LOG_CRITICAL, _("Trying to set a screen buffer on "
				"a VisVideo structure which points to an allocated screen buffer"));

		return -VISUAL_ERROR_VIDEO_HAS_ALLOCATED;
	}

	visual_buffer_set_data (video->buffer, buffer);
	visual_buffer_set_destroyer (video->buffer, NULL);

	if (video->pixel_rows != NULL) {
		visual_mem_free (video->pixel_rows);

		video->pixel_rows = NULL;
	}

	if (visual_buffer_get_data (video->buffer) != NULL) {
		video->pixel_rows = visual_mem_new0 (void *, video->height);

		precompute_row_table (video);
	}

	return VISUAL_OK;
}

/**
 * Sets the dimension for a VisVideo. Used to set the dimension for a
 * surface.
 *
 * @param video Pointer to a VisVideo to which the dimension is set.
 * @param width The width of the surface.
 * @param height The height of the surface.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_dimension (VisVideo *video, int width, int height)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->width = width;
	video->height = height;

	video->pitch = video->width * video->bpp;

	visual_buffer_set_size (video->buffer, video->pitch * video->height);

	return VISUAL_OK;
}

/**
 * Sets the pitch for a VisVideo. Used to set the screen
 * pitch for a surface. If the pitch doesn't differ from the
 * screen width * bpp you only need to call the
 * visual_video_set_dimension method.
 *
 * @param video Pointer to a VisVideo to which the pitch is set.
 * @param pitch The screen pitch in bytes per line.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_BPP on failure.
 */
int visual_video_set_pitch (VisVideo *video, int pitch)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (video->bpp <= 0)
		return -VISUAL_ERROR_VIDEO_INVALID_BPP;

	video->pitch = pitch;
	visual_buffer_set_size (video->buffer, video->pitch * video->height);

	return VISUAL_OK;
}

/**
 * Sets the depth for a VisVideo. Used to set the depth for
 * a surface. This will also define the number of bytes per pixel.
 *
 * @param video Pointer to a VisVideo to which the depth is set.
 * @param depth The depth choosen from the VisVideoDepth enumerate.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_depth (VisVideo *video, VisVideoDepth depth)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->depth = depth;
	video->bpp = visual_video_bpp_from_depth (video->depth);

	return VISUAL_OK;
}

/**
 * Sets all attributes for a VisVideo. Used to set width, height, pitch and the depth for a VisVideo.
 *
 * @param video Pointer to a VisVideo to which the depth is set.
 * @param width The width of the surface.
 * @param height The height of the surface.
 * @param pitch The pitch or rowstride of the surface.
 * @param depth The depth coohsen from the VisVideoDepth enumerate.
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_attributes (VisVideo *video, int width, int height, int pitch, VisVideoDepth depth)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);
	visual_video_set_pitch (video, pitch);

	return VISUAL_OK;
}

int visual_video_get_size (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	return video->pitch * video->height;
}

/**
 * Retrieves the pixel buffer from a VisVideo.
 *
 * @param video Pointer to the VisVideo from which the pixel buffer is requested.
 *
 * @return The VisVideo it's pixel buffer, NULL on failure.
 */
void *visual_video_get_pixels (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, NULL);

	return visual_buffer_get_data (video->buffer);
}

/**
 * Retrieves the VisBuffer object from a VisVideo.
 *
 * @param video Pointer to the VisVideo from which the VisBuffer object is requested.
 *
 * @return The VisBuffer object, NULL on failure.
 */
VisBuffer *visual_video_get_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, NULL);

	return video->buffer;
}

/**
 * Checks if a certain depth is supported by checking against an ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth that we want to test.
 *
 * @return TRUE when supported, FALSE when unsupported and -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth)
{
	if (visual_video_depth_is_sane (depth) == 0)
		return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

	if ((depth & depthflag) > 0)
		return TRUE;

	return FALSE;
}

/**
 * Get the next depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the next supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_prev
 * 
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the next supported depth.
 *
 * @return The next supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_next (int depthflag, VisVideoDepth depth)
{
	int i = depth;

	if (visual_video_depth_is_sane (depth) == 0)
		return VISUAL_VIDEO_DEPTH_ERROR;

	if (i == VISUAL_VIDEO_DEPTH_NONE) {
		i = VISUAL_VIDEO_DEPTH_8BIT;

		if ((i & depthflag) > 0)
			return i;
	}

	while (i < VISUAL_VIDEO_DEPTH_GL) {
		i *= 2;

		if ((i & depthflag) > 0)
			return i;
	}

	return depth;
}

/**
 * Get the previous depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the previous supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_next
 * 
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the previous supported depth.
 *
 * @return The previous supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_prev (int depthflag, VisVideoDepth depth)
{
	int i = depth;

	if (visual_video_depth_is_sane (depth) == 0)
		return VISUAL_VIDEO_DEPTH_ERROR;

	if (i == VISUAL_VIDEO_DEPTH_NONE)
		return VISUAL_VIDEO_DEPTH_NONE;

	while (i > VISUAL_VIDEO_DEPTH_NONE) {
		i >>= 1;

		if ((i & depthflag) > 0)
			return i;
	}

	return depth;
}

/**
 * Return the lowest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * 
 * @return The lowest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_lowest (int depthflag)
{
	return visual_video_depth_get_next (depthflag, VISUAL_VIDEO_DEPTH_NONE);
}

/**
 * Return the highest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_highest (int depthflag)
{
	VisVideoDepth highest = VISUAL_VIDEO_DEPTH_NONE;
	VisVideoDepth i = 0;
	int firstentry = TRUE;

	while (highest != i || firstentry == TRUE) {
		highest = i;

		i = visual_video_depth_get_next (depthflag, i);

		firstentry = FALSE;
	}

	return highest;
}

/**
 * Return the highest supported depth that is NOT openGL.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth that is not openGL or
 *	VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_highest_nogl (int depthflag)
{
	VisVideoDepth depth;

	depth = visual_video_depth_get_highest (depthflag);

	/* Get previous depth if the highest is openGL */
	if (depth == VISUAL_VIDEO_DEPTH_GL) {
		depth = visual_video_depth_get_prev (depthflag, depth);

		/* Is it still on openGL ? Return an error */
		if (depth == VISUAL_VIDEO_DEPTH_GL)
			return VISUAL_VIDEO_DEPTH_ERROR;

	} else {
		return depth;
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Checks if a certain value is a sane depth.
 *
 * @param depth Depth to be checked if it's sane.
 *
 * @return TRUE if the depth is sane, FALSE if the depth is not sane.
 */
int visual_video_depth_is_sane (VisVideoDepth depth)
{
	int count = 0;
	int i = 1;

	if (depth == VISUAL_VIDEO_DEPTH_NONE)
		return TRUE;

	if (depth >= VISUAL_VIDEO_DEPTH_ENDLIST)
		return FALSE;

	while (i < VISUAL_VIDEO_DEPTH_ENDLIST) {
		if ((i & depth) > 0)
			count++;

		if (count > 1)
			return FALSE;

		i <<= 1;
	}

	return TRUE;
}

/**
 * Returns the number of bits per pixel from a VisVideoDepth enumerate value.
 *
 * @param depth The VisVideodepth enumerate value from which the bits per pixel
 *	needs to be returned.
 *
 * @return The bits per pixel or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_depth_value_from_enum (VisVideoDepth depth)
{
	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			return 8;

		case VISUAL_VIDEO_DEPTH_16BIT:
			return 16;

		case VISUAL_VIDEO_DEPTH_24BIT:
			return 24;

		case VISUAL_VIDEO_DEPTH_32BIT:
			return 32;

		default:
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}

	return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
}

/**
 * Returns a VisVideoDepth enumerate value from bits per pixel.
 *
 * @param depthvalue Integer containing the number of bits per pixel.
 *
 * @return The corespondending enumerate value or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_enum_from_value (int depthvalue)
{
	switch (depthvalue) {
		case 8:
			return VISUAL_VIDEO_DEPTH_8BIT;

		case 16:
			return VISUAL_VIDEO_DEPTH_16BIT;

		case 24:
			return VISUAL_VIDEO_DEPTH_24BIT;

		case 32:
			return VISUAL_VIDEO_DEPTH_32BIT;

		default:
			return VISUAL_VIDEO_DEPTH_ERROR;

	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Returns the number of bytes per pixel from the VisVideoDepth enumerate.
 *
 * @param depth The VisVideodepth enumerate value from which the bytes per pixel
 *	needs to be returned.
 *
 * @return The number of bytes per pixel, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_bpp_from_depth (VisVideoDepth depth)
{
	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			return 1;

		case VISUAL_VIDEO_DEPTH_16BIT:
			return 2;

		case VISUAL_VIDEO_DEPTH_24BIT:
			return 3;

		case VISUAL_VIDEO_DEPTH_32BIT:
			return 4;

		case VISUAL_VIDEO_DEPTH_GL:
			return 0;

		default:
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Converts the VisVideo it's buffer boundries to a VisRectangle. This means that the rectangle it's
 * position will be set to 0, 0 and it's width and height respectively to that of the VisVideo.
 *
 * @param video Pointer to the VisVideo for which the buffer boundries are requested.
 * @param rect Pointer to the VisRectangle in which the buffer boundries are set.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_RECTANGLE_NULL on failure.
 */
int visual_video_get_boundary (VisVideo *video, VisRectangle *rect)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_rectangle_set (rect, 0, 0, video->width, video->height);

	return VISUAL_OK;
}

/**
 * Creates a sub region of a VisVideo. An extra reference to the src VisVideo is created. The region should
 * fall completely within the src, else the region won't be created. Notice that a sub region is not a copy
 *
 * @see visual_video_region_sub_by_values
 * @see visual_video_region_copy
 *
 * @param dest Pointer to the destination VisVideo, There should not be a buffer allocated for this VisVideo.
 * @param src Pointer to the source VisVideo from which a subregion is created.
 * @param rect Pointer to the rectangle containing the position and dimension information.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_RECTANGLE_NULL
 *	or -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS on failure.
 */
int visual_video_region_sub (VisVideo *dest, VisVideo *src, VisRectangle *rect)
{
	VisRectangle vrect;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	/* FIXME make non verbose */
	visual_log_return_val_if_fail (visual_rectangle_is_empty (rect) == FALSE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	visual_video_get_boundary (src, &vrect);

	/* FIXME make non verbose */
	visual_log_return_val_if_fail (visual_rectangle_within (&vrect, rect) == TRUE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	visual_rectangle_copy (&dest->rect, rect);
	visual_object_ref (VISUAL_OBJECT (src));

	dest->parent = src;

	visual_video_set_attributes (dest, rect->width, rect->height,
			(rect->width * src->bpp) + (src->pitch - (rect->width * src->bpp)), src->depth);
	visual_video_set_buffer (dest, (uint8_t *) (visual_video_get_pixels (src)) + ((rect->y * src->pitch) + (rect->x * src->bpp)));

	/* Copy composite */
	dest->compositetype = src->compositetype;
	dest->compfunc = src->compfunc;
	visual_color_copy (&dest->colorkey, &src->colorkey);
	dest->density = src->density;

	if (src->pal != NULL)
		visual_object_ref (VISUAL_OBJECT (src->pal));

	dest->pal = src->pal;

	return VISUAL_OK;
}

/**
 * Creates a sub region of a VisVideo likewise visual_video_region_sub() however the position and dimension is given
 * by separated values instead of a VisRectangle.
 *
 * @see visual_video_region_sub
 *
 * @param dest Pointer to the destination VisVideo, There should not be a buffer allocated for this VisVideo.
 * @param src Pointer to the source VisVideo from which a subregion is created.
 * @param x X Position of the sub region.
 * @param y Y Position of the sub region.
 * @param width Width of the sub region.
 * @param height Height Height of the sub region.
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS on failure.
 */
int visual_video_region_sub_by_values (VisVideo *dest, VisVideo *src, int x, int y, int width, int height)
{
	VisRectangle rect;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_rectangle_set (&rect, x, y, width, height);

	return visual_video_region_sub (dest, src, &rect);
}

int visual_video_region_sub_all (VisVideo *dest, VisVideo *src)
{
	VisRectangle rect;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_get_boundary (dest, &rect);

	return visual_video_region_sub (dest, src, &rect);
}

int visual_video_region_sub_with_boundary (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect)
{
	VisRectangle rsrect;
	VisRectangle sbound;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_rectangle_copy (&rsrect, srect);

	visual_video_get_boundary (src, &sbound);

	/* Merge the destination and source rect, so that only the allowed parts are sub regioned */
	visual_rectangle_clip (&rsrect, &sbound, srect);
	visual_rectangle_clip (&rsrect, drect, &rsrect);

	return visual_video_region_sub (dest, src, &rsrect);
}

int visual_video_composite_set_type (VisVideo *video, VisVideoCompositeType type)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->compositetype = type;

	return VISUAL_OK;
}

int visual_video_composite_set_colorkey (VisVideo *video, VisColor *color)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (color != NULL)
		visual_color_copy (&video->colorkey, color);
	else
		visual_color_set (&video->colorkey, 0, 0, 0);

	return VISUAL_OK;
}

int visual_video_composite_set_surface (VisVideo *video, uint8_t alpha)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->density = alpha;

	return VISUAL_OK;
}

int visual_video_composite_set_function (VisVideo *video, VisVideoCustomCompositeFunc compfunc)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->compfunc = compfunc;

	return VISUAL_OK;
}

VisVideoCustomCompositeFunc visual_video_composite_get_function (VisVideo *dest, VisVideo *src, int alpha)
{
	visual_log_return_val_if_fail (dest != NULL, NULL);
	visual_log_return_val_if_fail (src != NULL, NULL);

	if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_NONE) {

		return blit_overlay_noalpha;

	} else if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_SRC) {

		if (alpha == FALSE || src->depth != VISUAL_VIDEO_DEPTH_32BIT)
			return blit_overlay_noalpha;

		if (visual_cpu_get_mmx () != 0)
			return _lv_blit_overlay_alphasrc_mmx;
		else
			return blit_overlay_alphasrc;

	} else if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_COLORKEY) {

		return blit_overlay_colorkey;

	} else if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_SURFACE) {

		return blit_overlay_surfacealpha;

	} else if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_SURFACECOLORKEY) {

		return blit_overlay_surfacealphacolorkey;

	} else if (src->compositetype == VISUAL_VIDEO_COMPOSITE_TYPE_CUSTOM) {

		return src->compfunc;
	}

	return NULL;
}

int visual_video_blit_overlay_rectangle (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect, int alpha)
{
	return visual_video_blit_overlay_rectangle_custom (dest, drect, src, srect,
			visual_video_composite_get_function (dest, src, alpha));
}

int visual_video_blit_overlay_rectangle_custom (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect,
		                VisVideoCustomCompositeFunc compfunc)
{
	VisVideo vsrc;
	VisRectangle ndrect;
	int errret = VISUAL_OK;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_init (&vsrc);

	visual_rectangle_copy (&ndrect, drect);
	visual_rectangle_normalise_to (&ndrect, srect);

	if ((errret = visual_video_region_sub_with_boundary (&vsrc, &ndrect, src, srect)) == VISUAL_OK)
		errret = visual_video_blit_overlay_custom (dest, &vsrc, drect->x, drect->y, compfunc);

	visual_object_unref (VISUAL_OBJECT (&vsrc));

	return errret;
}

int visual_video_blit_overlay_rectangle_scale (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect,
		int alpha, VisVideoScaleMethod scale_method)
{
	return visual_video_blit_overlay_rectangle_scale_custom (dest, drect, src, srect, scale_method,
			visual_video_composite_get_function (dest, src, alpha));
}

int visual_video_blit_overlay_rectangle_scale_custom (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect,
		                VisVideoScaleMethod scale_method, VisVideoCustomCompositeFunc compfunc)
{
	VisVideo svid;
	VisVideo ssrc;
	VisRectangle frect;
	VisRectangle sbound;
	int errret = VISUAL_OK;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_log_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_init (&svid);
	visual_video_init (&ssrc);

	visual_video_get_boundary (dest, &sbound);

	/* check if the rectangle is in the screen, if not, don't scale and such */
	if (visual_rectangle_within_partially (&sbound, drect) == FALSE)
		goto out;

	visual_video_region_sub (&ssrc, src, srect);
	visual_video_set_attributes (&svid, drect->width, drect->height, src->bpp * drect->width, src->depth);
	visual_video_allocate_buffer (&svid);

	/* Scale the source to the dest rectangle it's size */
	visual_video_scale (&svid, &ssrc, scale_method);

	visual_rectangle_copy (&frect, drect);
	visual_rectangle_normalise (&frect);

	/* Blit the scaled source into the dest rectangle */
	errret = visual_video_blit_overlay_rectangle_custom (dest, drect, &svid, &frect, compfunc);

out:
	visual_object_unref (VISUAL_OBJECT (&svid));
	visual_object_unref (VISUAL_OBJECT (&ssrc));

	return errret;


	return VISUAL_OK;
}

/**
 * This function blits a VisVideo into another VisVideo. Placement can be done and there
 * is support for the alpha channel.
 *
 * @param dest Pointer to the destination VisVideo in which the source is overlayed.
 * @param src Pointer to the source VisVideo which is overlayed in the destination.
 * @param x Horizontal placement offset.
 * @param y Vertical placement offset.
 * @param alpha Sets if we want to check the alpha channel. Use FALSE or TRUE here.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_INVALID_DEPTH or -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS on failure.
 */
int visual_video_blit_overlay (VisVideo *dest, VisVideo *src, int x, int y, int alpha)
{
	return visual_video_blit_overlay_custom (dest, src, x, y,
			visual_video_composite_get_function (dest, src, alpha));
}

int visual_video_blit_overlay_custom (VisVideo *dest, VisVideo *src, int x, int y, VisVideoCustomCompositeFunc compfunc)
{
	VisVideo *transform = NULL;
	VisVideo *srcp = NULL;
	VisVideo dregion;
	VisVideo sregion;
	VisVideo tempregion;
	VisRectangle redestrect;
	VisRectangle drect;
	VisRectangle srect;
	VisRectangle trect;
	int ret = VISUAL_OK;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (compfunc != NULL, -VISUAL_ERROR_NULL);

	/* We can't overlay GL surfaces so don't even try */
	visual_log_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_GL ||
			src->depth != VISUAL_VIDEO_DEPTH_GL, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	visual_video_get_boundary (dest, &drect);
	visual_video_get_boundary (src, &srect);

	if (visual_rectangle_within_partially (&drect, &srect) == FALSE)
		return -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS;

	/* We're not the same depth, converting */
	if (dest->depth != src->depth) {
		transform = visual_video_new ();

		visual_video_set_depth (transform, dest->depth);
		visual_video_set_dimension (transform, src->width, src->height);

		visual_video_allocate_buffer (transform);

		visual_video_depth_transform (transform, src);
	}

	/* Setting all the pointers right */
	if (transform != NULL)
		srcp = transform;
	else
		srcp = src;

	visual_video_init (&dregion);
	visual_video_init (&sregion);
	visual_video_init (&tempregion);

	/* Negative offset fixture */
	if (x < 0) {
		srect.x += 0 - x;
		srect.width += x;
		x = 0;
	}

	if (y < 0) {
		srect.y += 0 - y;
		srect.height += y;
		y = 0;
	}

	/* Retrieve sub regions */
	visual_rectangle_set (&trect, x, y, srect.width, srect.height);

	if ((ret = visual_video_region_sub_with_boundary (&dregion, &drect, dest, &trect)) != VISUAL_OK)
		goto out;

	visual_video_get_boundary (&dregion, &redestrect);

	if ((ret = visual_video_region_sub (&tempregion, srcp, &srect)) != VISUAL_OK)
		goto out;


	if ((ret = visual_video_region_sub_with_boundary (&sregion, &drect, &tempregion, &redestrect)) != VISUAL_OK)
		goto out;

	/* Call blitter */
	compfunc (&dregion, &sregion);

out:
	/* If we had a transform buffer, it's time to get rid of it */
	if (transform != NULL)
		visual_object_unref (VISUAL_OBJECT (transform));

	visual_object_unref (VISUAL_OBJECT (&dregion));
	visual_object_unref (VISUAL_OBJECT (&sregion));
	visual_object_unref (VISUAL_OBJECT (&tempregion));

	return ret;
}

static int blit_overlay_noalpha (VisVideo *dest, VisVideo *src)
{
	int y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);

	/* src and dest are completely equal, do one big mem copy instead of a per line mem copy.
	 * Also check if the pitch is equal to it's width * bpp, this is because of subregions. */
	if (visual_video_compare (dest, src) == TRUE && (src->pitch == (src->width * src->bpp))) {
		visual_mem_copy (destbuf, srcbuf, visual_video_get_size (dest));

		return VISUAL_OK;
	}

	for (y = 0; y < src->height; y++) {
		visual_mem_copy (destbuf, srcbuf, src->width * src->bpp);

		destbuf += dest->pitch;
		srcbuf += src->pitch;
	}

	return VISUAL_OK;
}

static int blit_overlay_alphasrc (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha;

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			alpha = *(srcbuf + 3);

			*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
			*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
			*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

			destbuf += dest->bpp;
			srcbuf += src->bpp;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf += src->pitch - (src->width * src->bpp);
	}

	return VISUAL_OK;
}

static int blit_overlay_colorkey (VisVideo *dest, VisVideo *src)
{
	int x, y;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		VisPalette *pal = src->pal;

		if (pal == NULL) {
			blit_overlay_noalpha (dest, src);

			return VISUAL_OK;
		}

		int index = visual_palette_find_color (pal, &src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (*srcbuf != index)
					*destbuf = *srcbuf;

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t *destbuf = visual_video_get_pixels (dest);
		uint16_t *srcbuf = visual_video_get_pixels (src);
		uint16_t color = visual_color_to_uint16 (&src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (color != *srcbuf)
					*destbuf = *srcbuf;

				destbuf++;
				srcbuf++;
			}

			destbuf += (dest->pitch / dest->bpp) - dest->width;
			srcbuf += (src->pitch / src->bpp) - src->width;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		uint8_t r = src->colorkey.r;
		uint8_t g = src->colorkey.g;
		uint8_t b = src->colorkey.b;

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (b != *srcbuf && g != *(srcbuf + 1) && r != *(srcbuf + 2)) {
					*destbuf = *srcbuf;
					*(destbuf + 1) = *(srcbuf + 1);
					*(destbuf + 2) = *(srcbuf + 2);
				}

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t *destbuf = visual_video_get_pixels (dest);
		uint32_t *srcbuf = visual_video_get_pixels (src);
		uint32_t color = visual_color_to_uint32 (&src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (color != *srcbuf) {
					uint8_t alpha = *destbuf >> 24;

					*destbuf = *srcbuf;
					*destbuf = (*destbuf & 0x00ffffff) | alpha << 24;
				}

				destbuf++;
				srcbuf++;
			}

			destbuf += (dest->pitch / dest->bpp) - dest->width;
			srcbuf += (src->pitch / src->bpp) - src->width;
		}
	}

	return VISUAL_OK;
}

static int blit_overlay_surfacealpha (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = src->density;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {

		for (y = 0; y < src->height; y++) {
			_color16 *destr = (_color16 *) destbuf;
			_color16 *srcr = (_color16 *) srcbuf;

			for (x = 0; x < src->width; x++) {
				destr->r = ((alpha * (srcr->r - destr->r) >> 8) + destr->r);
				destr->g = ((alpha * (srcr->g - destr->g) >> 8) + destr->g);
				destr->b = ((alpha * (srcr->b - destr->b) >> 8) + destr->b);

				destr += 1;
				srcr += 1;
			}

			destbuf += dest->pitch;
			srcbuf += src->pitch;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
				*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
				*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
				*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
				*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}
	}

	return VISUAL_OK;
}

static int blit_overlay_surfacealphacolorkey (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = src->density;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		VisPalette *pal = src->pal;

		if (pal == NULL) {
			blit_overlay_noalpha (dest, src);

			return VISUAL_OK;
		}

		int index = visual_palette_find_color (pal, &src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (*srcbuf != index)
					*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t color = visual_color_to_uint16 (&src->colorkey);

		for (y = 0; y < src->height; y++) {
			_color16 *destr = (_color16 *) destbuf;
			_color16 *srcr = (_color16 *) srcbuf;

			for (x = 0; x < src->width; x++) {
				if (color != *((uint16_t *) srcr)) {
					destr->r = ((alpha * (srcr->r - destr->r) >> 8) + destr->r);
					destr->g = ((alpha * (srcr->g - destr->g) >> 8) + destr->g);
					destr->b = ((alpha * (srcr->b - destr->b) >> 8) + destr->b);
				}

				destr++;
				srcr++;
			}

			destbuf += dest->pitch;
			srcbuf += src->pitch;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t r = src->colorkey.r;
		uint8_t g = src->colorkey.g;
		uint8_t b = src->colorkey.b;

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (b != *srcbuf && g != *(srcbuf + 1) && r != *(srcbuf + 2)) {
					*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
					*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
					*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));
				}

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t color = visual_color_to_uint32 (&src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (color == *((uint32_t *) destbuf)) {
					*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);
					*(destbuf + 1) = ((alpha * (*(srcbuf + 1) - *(destbuf + 1)) >> 8) + *(destbuf + 1));
					*(destbuf + 2) = ((alpha * (*(srcbuf + 2) - *(destbuf + 2)) >> 8) + *(destbuf + 2));
				}

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}
	}

	return VISUAL_OK;
}

/**
 * Sets a certain color as the alpha channel and the density for the non alpha channel
 * colors. This function can be only used on VISUAL_VIDEO_DEPTH_32BIT surfaces.
 *
 * @param video Pointer to the VisVideo in which the alpha channel is made.
 * @param color Pointer to the VisColor containing the color value for the alpha channel.
 * @param density The alpha density for the other colors.
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_fill_alpha_color (VisVideo *video, VisColor *color, uint8_t density)
{
	int x, y;
	int col = 0;
	uint32_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	col = (color->r << 16 | color->g << 8 | color->b);

	vidbuf = visual_video_get_pixels (video);

	/* FIXME byte order sensitive */
	for (y = 0; y < video->height; y++) {
		for (x = 0; x < video->width; x++) {
			if ((*vidbuf & 0x00ffffff) == col)
				*vidbuf = col;
			else
				*vidbuf |= density << 24;

			vidbuf++;
		}

		vidbuf += video->pitch - (video->width * video->bpp);
	}

	return VISUAL_OK;
}

/**
 * Sets a certain alpha value for the complete buffer in the VisVideo. This function
 * can be only used on VISUAL_VIDEO_DEPTH_32BIT surfaces.
 *
 * @param video Pointer to the VisVideo in which the alpha channel density is set.
 * @param density The alpha density that is to be set.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_fill_alpha (VisVideo *video, uint8_t density)
{
	int x, y;
	uint8_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	vidbuf = visual_video_get_pixels (video) + 3;

	/* FIXME byte order sensitive */
	for (y = 0; y < video->height; y++) {
		for (x = 0; x < video->width; x++)
			*(vidbuf += video->bpp) = density;

		vidbuf += video->pitch - (video->width * video->bpp);
	}

	return VISUAL_OK;
}

/**
 *
 */
int visual_video_fill_alpha_rectangle (VisVideo *video, uint8_t density, VisRectangle *rect)
{
	VisVideo rvid;
	int errret = VISUAL_OK;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_init (&rvid);

	errret = visual_video_region_sub (video, &rvid, rect);

	if (errret < 0)
		goto out;

	visual_video_fill_alpha (&rvid, density);

out:
	visual_object_unref (VISUAL_OBJECT (&rvid));

	return errret;
}

/**
 * This function is used to fill a VisVideo with one color. It's highly advice to use this function to fill
 * a VisVideo with a color instead of using visual_mem_set, the reason is that this function takes the pitch
 * of a line in consideration. When you use a visual_mem_set on sub regions the results won't be pretty.
 * 
 * @param video Pointer to the VisVideo which is filled with one color
 * @param rcolor Pointer to the VisColor that is used as color. NULL is a valid color and will be interperted
 * 	as black.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_fill_color (VisVideo *video, VisColor *rcolor)
{
	VisColor color;
	VisRectangle rect;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (rcolor == NULL)
		visual_color_set (&color, 0, 0, 0);
	else
		visual_color_copy (&color, rcolor);

	switch (video->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			fill_color8 (video, &color);

			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			fill_color16 (video, &color);

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			fill_color24 (video, &color);

			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			fill_color32 (video, &color);

			break;

		default:
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

			break;
	}

	return VISUAL_OK;
}

int visual_video_fill_color_rectangle (VisVideo *video, VisColor *color, VisRectangle *rect)
{
	VisRectangle vrect;
	VisRectangle dbound;
	VisVideo svid;
	int errret = VISUAL_OK;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);
	visual_log_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_get_boundary (video, &vrect);

	visual_log_return_val_if_fail (visual_rectangle_within_partially (&vrect, rect) != FALSE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	visual_video_init (&svid);

	visual_video_get_boundary (video, &dbound);

	visual_video_region_sub_with_boundary (&svid, &dbound, video, rect);

	errret = visual_video_fill_color (&svid, color);

out:
	visual_object_unref (VISUAL_OBJECT (&svid));

	return errret;
}

/* Color fill functions */
static int fill_color8 (VisVideo *video, VisColor *color)
{
	int y;
	uint8_t *buf = visual_video_get_pixels (video);
	int8_t col = ((color->r + color->g + color->b) / 3);

	for (y = 0; y < video->height; y++) {
		visual_mem_set (buf, col, video->width);

		buf += video->pitch;
	}

	return VISUAL_OK;
}

static int fill_color16 (VisVideo *video, VisColor *color)
{
	int y;
	uint16_t *buf = visual_video_get_pixels (video);
	int16_t col;
	_color16 *col16 = (_color16 *) &col;
	col16->r = color->r >> 3;
	col16->g = color->g >> 2;
	col16->b = color->b >> 3;

	for (y = 0; y < video->height; y++) {
		visual_mem_set16 (buf, col, video->width);

		buf += (video->pitch / video->bpp);
	}

	return VISUAL_OK;
}

static int fill_color24 (VisVideo *video, VisColor *color)
{
	int x, y;
	uint32_t *buf;
	uint8_t *rbuf = visual_video_get_pixels (video);
	uint8_t *buf8;

	int32_t cola =
		(color->b << 24) |
		(color->g << 16) |
		(color->r << 8) |
		(color->b);
	int32_t colb =
		(color->g << 24) |
		(color->r << 16) |
		(color->b << 8) |
		(color->g);
	int32_t colc =
		(color->r << 24) |
		(color->b << 16) |
		(color->g << 8) |
		(color->r);

	for (y = 0; y < video->height; y++) {
		buf = (uint32_t *) rbuf;

		for (x = video->width; x >= video->bpp; x -= video->bpp) {
			*(buf++) = cola;
			*(buf++) = colb;
			*(buf++) = colc;
		}

		buf8 = (uint8_t *) buf;
		*(buf8++) = color->b;
		*(buf8++) = color->g;
		*(buf8++) = color->r;


		rbuf += video->pitch;
	}

	return VISUAL_OK;
}

static int fill_color32 (VisVideo *video, VisColor *color)
{
	int y;
	uint32_t *buf = visual_video_get_pixels (video);
	uint32_t col =
		(color->r << 16) |
		(color->g << 8) |
		(color->b);

	for (y = 0; y < video->height; y++) {
		visual_mem_set32 (buf, col, video->width);

		buf += (video->pitch / video->bpp);
	}

	return VISUAL_OK;
}

/**
 * Video color transforms one VisVideo bgr pixel ordering into bgr pixel ordering.
 * 
 * @param dest Pointer to the destination VisVideo, which should be a clone of the source VisVideo
 *	depth, pitch, dimension wise.
 * @param src Pointer to the source VisVideo from which the bgr data is read.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NOT_INDENTICAL, -VISUAL_ERROR_VIDEO_PIXELS_NULL or
 *	-VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_color_bgr_to_rgb (VisVideo *dest, VisVideo *src)
{
	visual_log_return_val_if_fail (visual_video_compare (dest, src) == TRUE, -VISUAL_ERROR_VIDEO_NOT_INDENTICAL);
	visual_log_return_val_if_fail (visual_video_get_pixels (dest) != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);
	visual_log_return_val_if_fail (visual_video_get_pixels (src) != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);
	visual_log_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_8BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT)
		bgr_to_rgb16 (dest, src);
	else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT)
		bgr_to_rgb24 (dest, src);
	else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT)
		bgr_to_rgb32 (dest, src);

	return VISUAL_OK;
}

/**
 *
 */
int visual_video_rotate (VisVideo *dest, VisVideo *src, VisVideoRotateDegrees degrees)
{
	int ret = VISUAL_OK;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	switch (degrees) {
		case VISUAL_VIDEO_ROTATE_NONE:
			if (dest->width == src->width && dest->height == src->height)
				ret = visual_video_blit_overlay (dest, src, 0, 0, FALSE);
			else
				ret = -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS;

			break;

		case VISUAL_VIDEO_ROTATE_90:
			ret = rotate_90 (dest, src);

			break;

		case VISUAL_VIDEO_ROTATE_180:
			ret = rotate_180 (dest, src);

			break;

		case VISUAL_VIDEO_ROTATE_270:
			ret = rotate_270 (dest, src);

			break;

		default:
			ret = -VISUAL_ERROR_VIDEO_INVALID_ROTATE;

			break;
	}

	return ret;
}

/**
 *
 */
VisVideo *visual_video_rotate_new (VisVideo *src, VisVideoRotateDegrees degrees)
{
	VisVideo *dest;
	int nwidth;
	int nheight;

	visual_log_return_val_if_fail (src != NULL, NULL);

	switch (degrees) {
		case VISUAL_VIDEO_ROTATE_NONE:
		case VISUAL_VIDEO_ROTATE_180:
			nwidth = src->width;
			nheight = src->height;

			break;

		case VISUAL_VIDEO_ROTATE_90:
		case VISUAL_VIDEO_ROTATE_270:
			nwidth = src->height;
			nheight = src->width;

			break;

		default:
			return NULL;

			break;
	}

	dest = visual_video_new_with_buffer (nwidth, nheight, src->depth);

	visual_video_rotate (dest, src, degrees);

	return dest;
}

/* rotate functions, works with all depths now */
/* FIXME: do more testing with those badasses */
static int rotate_90 (VisVideo *dest, VisVideo *src)
{
	int x, y, i;

	uint8_t *tsbuf = src->pixel_rows[src->height-1];
	uint8_t *dbuf;
	uint8_t *sbuf = tsbuf;

	visual_log_return_val_if_fail (dest->width == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_log_return_val_if_fail (dest->height == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	for (y = 0; y < dest->height; y++) {
		dbuf = dest->pixel_rows[y];

		for (x = 0; x < dest->width; x++) {
			for (i = 0; i < dest->bpp; i++) {
				*(dbuf++) = *(sbuf + i);
			}

			sbuf -= src->pitch;
		}

		tsbuf += src->bpp;
		sbuf = tsbuf;
	}

	return VISUAL_OK;
}

static int rotate_180 (VisVideo *dest, VisVideo *src)
{
	int x, y, i;

	uint8_t *dbuf;
	uint8_t *sbuf;

	const int h1 = src->height - 1;
	const int w1 = (src->width - 1) * src->bpp;

	visual_log_return_val_if_fail (dest->width == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_log_return_val_if_fail (dest->height == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	for (y = 0; y < dest->height; y++) {
		dbuf = dest->pixel_rows[y];
		sbuf = src->pixel_rows[h1 - y] + w1;

		for (x = 0; x < dest->width; x++) {
			for (i = 0; i < src->bpp; i++) {
				*(dbuf++) = *(sbuf + i);
			}

			sbuf -= src->bpp;
		}
	}

	return VISUAL_OK;
}

static int rotate_270 (VisVideo *dest, VisVideo *src)
{
	int x, y, i;

	uint8_t *tsbuf = visual_video_get_pixels (src) + src->pitch - src->bpp;
	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = tsbuf;

	visual_log_return_val_if_fail (dest->width == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_log_return_val_if_fail (dest->height == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	for (y = 0; y < dest->height; y++) {
		dbuf = dest->pixel_rows[y];

		for (x = 0; x < dest->width; x++) {
			for (i = 0; i < dest->bpp; i++) {
				*(dbuf++) = *(sbuf + i);
			}

			sbuf += src->pitch;
		}

		tsbuf -= src->bpp;
		sbuf = tsbuf;
	}

	return VISUAL_OK;
}

int visual_video_mirror (VisVideo *dest, VisVideo *src, VisVideoMirrorOrient orient)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src->depth == dest->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	switch (orient) {
		case VISUAL_VIDEO_MIRROR_NONE:
			visual_video_blit_overlay (dest, src, 0, 0, FALSE);
			break;

		case VISUAL_VIDEO_MIRROR_X:
			mirror_x (dest, src);
			break;

		case VISUAL_VIDEO_MIRROR_Y:
			mirror_y (dest, src);
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

VisVideo *visual_video_mirror_new (VisVideo *src, VisVideoMirrorOrient orient)
{
	VisVideo *video;

	visual_log_return_val_if_fail (src != NULL, NULL);

	video = visual_video_new_with_buffer (src->width, src->height, src->depth);

	visual_video_mirror (video, src, orient);

	return video;
}

/* Mirror functions */
static int mirror_x (VisVideo *dest, VisVideo *src)
{
	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);
	const int step2 = dest->bpp << 1;
	const int w1b = (dest->width - 1) * dest->bpp;
	int x, y, i;

	for (y = 0; y < dest->height; y++) {
		sbuf = src->pixel_rows[y] + w1b;
		dbuf = dest->pixel_rows[y];

		for (x = 0; x < dest->width; x++) {

			for (i = 0; i < dest->bpp; i++)
				*(dbuf++) = *(sbuf++);

			sbuf -= step2;
		}
	}

	return VISUAL_OK;
}

static int mirror_y (VisVideo *dest, VisVideo *src)
{
	int y;

	for (y = 0; y < dest->height; y++) {
		visual_mem_copy (dest->pixel_rows[y],
				src->pixel_rows[dest->height - 1 - y],
				dest->width * dest->bpp);
	}

	return VISUAL_OK;
}

/**
 * Video depth transforms one VisVideo into another using the depth information
 * stored within the VisVideos. The dimension should be equal however the pitch
 * value of the destination may be set.
 *
 * @param dest Pointer to the destination VisVideo to which the source VisVideo is transformed.
 * @param src Pointer to the source VisVideo.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_PALETTE_NULL, -VISUAL_ERROR_PALETTE_SIZE,
 * 	-VISUAL_ERROR_VIDEO_NOT_TRANSFORMED or error values returned by visual_video_blit_overlay on failure.
 */
int visual_video_depth_transform (VisVideo *dest, VisVideo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	/* We blit overlay it instead of just visual_mem_copy because the pitch can still be different */
	if (dest->depth == src->depth)
		return visual_video_blit_overlay (dest, src, 0, 0, FALSE);

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT || src->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		visual_log_return_val_if_fail (src->pal != NULL, -VISUAL_ERROR_PALETTE_NULL);
		visual_log_return_val_if_fail (src->pal->ncolors == 256, -VISUAL_ERROR_PALETTE_SIZE);
	}

	if (src->depth == VISUAL_VIDEO_DEPTH_8BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_8_to_16_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_8_to_24_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_8_to_32_c (dest, src);

	} else if (src->depth == VISUAL_VIDEO_DEPTH_16BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_16_to_8_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_16_to_24_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_16_to_32_c (dest, src);

	} else if (src->depth == VISUAL_VIDEO_DEPTH_24BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_24_to_8_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_24_to_16_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_24_to_32_c (dest, src);

	} else if (src->depth == VISUAL_VIDEO_DEPTH_32BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_32_to_8_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_32_to_16_c (dest, src);

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_32_to_24_c (dest, src);
	}

	return -VISUAL_ERROR_VIDEO_NOT_TRANSFORMED;
}

/**
 * @}
 */

static int depth_transform_get_smallest (VisVideo *dest, VisVideo *src, int *width, int *height)
{
	*width = dest->width > src->width ? src->width : dest->width;
	*height = dest->height > src->height ? src->height : dest->height;

	return 0;
}

/* Depth conversion functions */
static int depth_transform_8_to_16_c (VisVideo *dest, VisVideo *src)
{
	int x, y, i;
	int w;
	int h;

	int ddiff;
	int sdiff;

	_color16* dbuf = visual_video_get_pixels (dest);
	uint8_t* sbuf = visual_video_get_pixels (src);

	_color16 colors[256];

	for(i = 0; i < 256; i++) {
		colors[i].r = src->pal->colors[i].r >> 3;
		colors[i].g = src->pal->colors[i].g >> 2;
		colors[i].b = src->pal->colors[i].b >> 3;
	}

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = colors[*(sbuf++)];
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_8_to_24_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	int ddiff;
	int sdiff;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);


	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = src->pal->colors[*(sbuf)].b;
			*(dbuf++) = src->pal->colors[*(sbuf)].g;
			*(dbuf++) = src->pal->colors[*(sbuf)].r;
			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_8_to_32_c (VisVideo *dest, VisVideo *src)
{
	int x, y, i;
	int w;
	int h;

	uint32_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	uint32_t colors[256];

	for (i = 0; i < 256; ++i) {
		colors[i] =
			src->pal->colors[i].r << 16 |
			src->pal->colors[i].g << 8 |
			src->pal->colors[i].b;
	}

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = colors[*(sbuf++)];
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_16_to_8_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	_color16 *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	uint8_t r, g, b, col;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			r = sbuf->r << 3;
			g = sbuf->g << 2;
			b = sbuf->b << 3;
			sbuf++;

			col = (r + g + b) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_16_to_24_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	_color16 *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = sbuf->b << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->r << 3;

			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_16_to_32_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	_color16 *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch  / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = sbuf->b << 3;
			*(dbuf++) = sbuf->g << 2;
			*(dbuf++) = sbuf->r << 3;
			*(dbuf++) = 0;

			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_24_to_8_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;
	uint8_t r, g, b, col;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			b = *(sbuf++);
			g = *(sbuf++);
			r = *(sbuf++);

			col = (b + g + r) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_24_to_16_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	_color16 *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = (src->pitch / src->bpp) - w;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			dbuf->b = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->r = *(sbuf++) >> 3;

			dbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_24_to_32_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = 0;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_32_to_8_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t r, g, b, col;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			b = *(sbuf++);
			g = *(sbuf++);
			r = *(sbuf++);
			sbuf++;

			col = (r + g + b) / 3;

			dest->pal->colors[col].r = r;
			dest->pal->colors[col].g = g;
			dest->pal->colors[col].b = b;

			*(dbuf++) = col;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_32_to_16_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	_color16 *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = (dest->pitch / dest->bpp) - w;
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			dbuf->b = *(sbuf++) >> 3;
			dbuf->g = *(sbuf++) >> 2;
			dbuf->r = *(sbuf++) >> 3;

			dbuf++;
			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

static int depth_transform_32_to_24_c (VisVideo *dest, VisVideo *src)
{
	int x, y;
	int w;
	int h;

	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);

	int ddiff;
	int sdiff;

	depth_transform_get_smallest (dest, src, &w, &h);

	ddiff = dest->pitch - (w * dest->bpp);
	sdiff = src->pitch - (w * src->bpp);

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);
			*(dbuf++) = *(sbuf++);

			sbuf++;
		}

		dbuf += ddiff;
		sbuf += sdiff;
	}

	return VISUAL_OK;
}

/* BGR -> RGB functions */
static int bgr_to_rgb16 (VisVideo *dest, VisVideo *src)
{
	_color16 *destbuf, *srcbuf;
	int x, y;
	int pitchdiff = (dest->pitch - (dest->width * dest->bpp)) >> 1;

	destbuf = (_color16 *) visual_video_get_pixels (dest);
	srcbuf = (_color16 *) visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf->b = srcbuf->r;
			destbuf->g = srcbuf->g;
			destbuf->r = srcbuf->b;
			destbuf++;
			srcbuf++;
		}

		destbuf += pitchdiff;
	}

	return VISUAL_OK;
}

static int bgr_to_rgb24 (VisVideo *dest, VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int pitchdiff = dest->pitch - (dest->width * dest->bpp);

	destbuf = visual_video_get_pixels (dest);
	srcbuf = visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			*(destbuf + 2) = *(srcbuf);
			*(destbuf + 1) = *(srcbuf + 1);
			*(destbuf) = *(srcbuf + 2);

			destbuf += dest->bpp;
			srcbuf += src->bpp;
		}

		destbuf += pitchdiff;
	}

	return VISUAL_OK;
}

static int bgr_to_rgb32 (VisVideo *dest, VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int i = 0;
	int pitchdiff = dest->pitch - (dest->width * dest->bpp);

	destbuf = visual_video_get_pixels (dest);
	srcbuf = visual_video_get_pixels (src);

	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			*(destbuf + 2) = *(srcbuf);
			*(destbuf + 1) = *(srcbuf + 1);
			*(destbuf) = *(srcbuf + 2);

			*(destbuf + 3) = *(srcbuf + 3);

			destbuf += dest->bpp;
			srcbuf += src->bpp;
		}

		destbuf += pitchdiff;
	}

	return VISUAL_OK;
}

/**
 * Non interpolating fast pixel doubler zoom.
 *
 * @param dest Pointer to destination VisVideo in which the pixel doubled VisVideo is stored.
 * @param src Pointer to source VisVideo that is pixel doubled.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_zoom_double (VisVideo *dest, VisVideo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (dest->depth == src->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			zoom_8 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			zoom_16 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			zoom_24 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			zoom_32 (dest, src);

			break;

		default:
			visual_log (VISUAL_LOG_CRITICAL, _("Invalid depth passed to the scaler"));

			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

			break;
	}

	return VISUAL_OK;
}

static int zoom_8 (VisVideo *dest, VisVideo *src)
{
	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);
	int x, y;

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			*(dbuf++) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += src->pitch - (src->width * src->bpp);
		dbuf += dest->pitch - (dest->width * dest->bpp);
	}

	return VISUAL_OK;
}

static int zoom_16 (VisVideo *dest, VisVideo *src)
{
	uint16_t *dbuf = visual_video_get_pixels (dest);
	uint16_t *sbuf = visual_video_get_pixels (src);
	int x, y;

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			*(dbuf++) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += src->pitch - (src->width * src->bpp);
		dbuf += dest->pitch - (dest->width * dest->bpp);
	}

	return VISUAL_OK;
}

static int zoom_24 (VisVideo *dest, VisVideo *src)
{

	return VISUAL_OK;
}

static int zoom_32 (VisVideo *dest, VisVideo *src)
{
	uint32_t *sbuf = visual_video_get_pixels (src);
	uint32_t *dbuf = visual_video_get_pixels (dest);
	int x, y;

	const int spdiff = src->pitch - src->width*src->bpp;
	for (y = 0; y < src->height; y++) {
		dbuf = dest->pixel_rows[y << 1];
		for (x = 0; x < src->width; x++) {
			*(dbuf + dest->width) = *sbuf;
			*(dbuf++) = *sbuf;
			*(dbuf + dest->width) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += spdiff;
	}

	return VISUAL_OK;
}

/**
 * Scale VisVideo.
 *
 * @param dest Pointer to VisVideo object for storing scaled image.
 * @param src Pointer to VisVideo object whose image is to be scaled.
 * @param scale_method Scaling method to use.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_scale (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (dest->depth == src->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	visual_log_return_val_if_fail (scale_method == VISUAL_VIDEO_SCALE_NEAREST ||
			scale_method == VISUAL_VIDEO_SCALE_BILINEAR, -VISUAL_ERROR_VIDEO_INVALID_SCALE_METHOD);


	/* If the dest and source are equal in dimension and scale_method is nearest, do a
	 * blit overlay */
	if (visual_video_compare_ignore_pitch (dest, src) == TRUE && scale_method == VISUAL_VIDEO_SCALE_NEAREST) {
		visual_video_blit_overlay (dest, src, 0, 0, FALSE);

		return VISUAL_OK;
	}


	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			if (scale_method == VISUAL_VIDEO_SCALE_NEAREST)
				scale_nearest_8 (dest, src);
			else if (scale_method == VISUAL_VIDEO_SCALE_BILINEAR)
				scale_bilinear_8 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			if (scale_method == VISUAL_VIDEO_SCALE_NEAREST)
				scale_nearest_16 (dest, src);
			else if (scale_method == VISUAL_VIDEO_SCALE_BILINEAR)
				scale_bilinear_16 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			if (scale_method == VISUAL_VIDEO_SCALE_NEAREST)
				scale_nearest_24 (dest, src);
			else if (scale_method == VISUAL_VIDEO_SCALE_BILINEAR)
				scale_bilinear_24 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			if (scale_method == VISUAL_VIDEO_SCALE_NEAREST)
				scale_nearest_32 (dest, src);
			else if (scale_method == VISUAL_VIDEO_SCALE_BILINEAR) {
				if (visual_cpu_get_mmx () != 0)
					_lv_scale_bilinear_32_mmx (dest, src);
				else
					scale_bilinear_32 (dest, src);
			}

			break;

		default:
			visual_log (VISUAL_LOG_CRITICAL, _("Invalid depth passed to the scaler"));

			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

			break;
	}

	return VISUAL_OK;
}

/**
 * Scale VisVideo, and return a newly allocated scaled VisVideo.
 *
 * @param src Pointer to VisVideo object whose image is to be scaled.
 * @param scale_method Scaling method to use.
 * @param width New width.
 * @param height New height.
 *
 * @return A newly allocated scaled VisVideo, NULL on failure.
 */
VisVideo *visual_video_scale_new (VisVideo *src, int width, int height, VisVideoScaleMethod scale_method)
{
	VisVideo *video;

	visual_log_return_val_if_fail (src != NULL, NULL);

	video = visual_video_new_with_buffer (width, height, src->depth);

	visual_video_scale (video, src, scale_method);

	return video;
}

/**
 * Scale VisVideo, but does an internal depth transformation when the source VisVideo is not of the
 * same depth as the destination VisVideo.
 *
 * @see visual_video_scale
 *
 * @param dest Pointer to the destination VisVideo in which the scaled version is stored
 * @param src Pointer to the source VisVideo whose image is to be scaled.
 * @param scale_method Scaling method to use.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_IMPOSSIBLE, -VISUAL_ERROR_VIDEO_NULL
 *	or error values returned by visual_video_scale() on failure.
 */
int visual_video_scale_depth (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method)
{
	VisVideo dtransform;
	int errret;

	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (dest->depth != src->depth) {
		visual_video_init (&dtransform);

		visual_video_set_attributes (&dtransform, dest->width, dest->height, dest->width * dest->bpp, dest->depth);
		visual_video_allocate_buffer (&dtransform);

		visual_video_depth_transform (&dtransform, src);

		errret = visual_video_scale (dest, &dtransform, scale_method);

		visual_object_unref (VISUAL_OBJECT (&dtransform));

		return errret;
	} else {
		return visual_video_scale (dest, src, scale_method);
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Creates a new scaled VisVideo, but does an internal depth transformation when the
 * source VisVideo is not of the same depth as the destination VisVideo.
 *
 * @see visual_video_scale_depth
 *
 * @param src Pointer to the source VisVideo whose image is to be scaled.
 * @param width The width of the new scaled VisVideo.
 * @param height The height of the new scaled VisVideo.
 * @param depth The depth of the new scaled VisVideo.
 * @param scale_method Scaling method to use.
 *
 * @return A newly allocated scaled version of the source VisVideo with the given width, height
 *	and depth, NULL on failure.
 */
VisVideo *visual_video_scale_depth_new (VisVideo *src, int width, int height, VisVideoDepth depth,
		VisVideoScaleMethod scale_method)
{
	VisVideo *video;

	visual_log_return_val_if_fail (src != NULL, NULL);

	video = visual_video_new_with_buffer (width, height, depth);

	visual_video_scale_depth (video, src, scale_method);

	return video;
}

/* Scale functions */
static int scale_nearest_8 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint8_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint8_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += dest->pitch - dest->width;
	}

	return VISUAL_OK;
}

static int scale_nearest_16 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint16_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint16_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}

	return VISUAL_OK;
}

/* FIXME this version is of course butt ugly */
/* IF color24 is allowed use it here as well */
static int scale_nearest_24 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	_color24 *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (_color24 *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}

	return VISUAL_OK;
}

static int scale_nearest_32 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint32_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint32_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}

	return VISUAL_OK;
}

static int scale_bilinear_8 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint8_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;

	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (uint8_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (uint8_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			uint8_t cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul;
			b0 += ll * cll;
			b0 += ur * cur;
			b0 += lr * clr;

			*dest_pixel++ = b0 >> 16;
		}

		dest_pixel += dest->pitch - (dest->width - 1);

	}

	return VISUAL_OK;
}

static int scale_bilinear_16 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	_color16 *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;
	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (_color16 *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (_color16 *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0.0;

		for (x = dest->width - 1; x--; u += du) {
			_color16 cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b3, b2, b1, b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.r;
			b1 = ul * cul.g;
			b2 = ul * cul.b;

			b0 += ll * cll.r;
			b1 += ll * cll.g;
			b2 += ll * cll.b;

			b0 += ur * cur.r;
			b1 += ur * cur.g;
			b2 += ur * cur.b;

			b0 += lr * clr.r;
			b1 += lr * clr.g;
			b2 += lr * clr.b;

			b.r = b0 >> 16;
			b.g = b1 >> 16;
			b.b = b2 >> 16;

			*dest_pixel++ = b;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));
	}

	return VISUAL_OK;
}

static int scale_bilinear_24 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	_color24 *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;
	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (_color24 *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (_color24 *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			_color24 cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b3, b2, b1, b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.r;
			b1 = ul * cul.g;
			b2 = ul * cul.b;

			b0 += ll * cll.r;
			b1 += ll * cll.g;
			b2 += ll * cll.b;

			b0 += ur * cur.r;
			b1 += ur * cur.g;
			b2 += ur * cur.b;

			b0 += lr * clr.r;
			b1 += lr * clr.g;
			b2 += lr * clr.b;

			b.r = b0 >> 16;
			b.g = b1 >> 16;
			b.b = b2 >> 16;

			*dest_pixel++ = b;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));
	}

	return VISUAL_OK;
}

static int scale_bilinear_32 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint32_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;

	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (uint32_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (uint32_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			union {
				uint8_t  c8[4];
				uint32_t c32;
			} cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b3, b2, b1, b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul.c32 = src_pixel_rowu[u >> 16];
			cll.c32 = src_pixel_rowl[u >> 16];
			cur.c32 = src_pixel_rowu[(u >> 16) + 1];
			clr.c32 = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.c8[0];
			b1 = ul * cul.c8[1];
			b2 = ul * cul.c8[2];
			b3 = ul * cul.c8[3];

			b0 += ll * cll.c8[0];
			b1 += ll * cll.c8[1];
			b2 += ll * cll.c8[2];
			b3 += ll * cll.c8[3];

			b0 += ur * cur.c8[0];
			b1 += ur * cur.c8[1];
			b2 += ur * cur.c8[2];
			b3 += ur * cur.c8[3];

			b0 += lr * clr.c8[0];
			b1 += lr * clr.c8[1];
			b2 += lr * clr.c8[2];
			b3 += lr * clr.c8[3];

			b.c8[0] = b0 >> 16;
			b.c8[1] = b1 >> 16;
			b.c8[2] = b2 >> 16;
			b.c8[3] = b3 >> 16;

			*dest_pixel++ = b.c32;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));

	}

	return VISUAL_OK;
}

