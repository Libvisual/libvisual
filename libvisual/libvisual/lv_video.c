/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Duilio J. Protti <dprotti@users.sourceforge.net>
 *	    Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include "lv_video.h"
#include "lv_color.h"
#include "lv_common.h"
#include "lv_cpu.h"
#include "private/lv_video_convert.h"
#include "private/lv_video_fill.h"
#include "private/lv_video_scale.h"
#include "gettext.h"

#pragma pack(1)
typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;
#pragma pack()

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

/* Rotate functions */
static int rotate_90 (VisVideo *dest, VisVideo *src);
static int rotate_180 (VisVideo *dest, VisVideo *src);
static int rotate_270 (VisVideo *dest, VisVideo *src);

/* Mirror functions */
static int mirror_x (VisVideo *dest, VisVideo *src);
static int mirror_y (VisVideo *dest, VisVideo *src);

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

int visual_video_init (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

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

int visual_video_free_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (visual_video_get_pixels (video) != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);

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

int visual_video_allocate_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (video->buffer != NULL, -VISUAL_ERROR_VIDEO_BUFFER_NULL);

	if (visual_video_get_pixels (video) != NULL) {
		if (visual_buffer_get_allocated (video->buffer)) {
			visual_video_free_buffer (video);
		} else {
			visual_log (VISUAL_LOG_ERROR, _("Trying to allocate an screen buffer on "
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

int visual_video_have_allocated_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, FALSE);

	return visual_buffer_get_allocated (video->buffer);
}

static void precompute_row_table (VisVideo *video)
{
	void **table, *row;
	int y;

	visual_return_if_fail (video->pixel_rows != NULL);

	table = video->pixel_rows;
	row = visual_video_get_pixels (video);

	for (y = 0; y < video->height; y++, row += video->pitch)
		*table++ = row;
}

int visual_video_clone (VisVideo *dest, VisVideo *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_set_depth (dest, src->depth);
	visual_video_set_dimension (dest, src->width, src->height);
	visual_video_set_pitch (dest, src->pitch);

	return VISUAL_OK;
}

int visual_video_compare (VisVideo *src1, VisVideo *src2)
{
	visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (visual_video_compare_ignore_pitch (src1, src2) == FALSE)
		return FALSE;

	if (src1->pitch != src2->pitch)
		return FALSE;

	/* We made it to the end, the VisVideos are likewise in depth, pitch, dimensions */
	return TRUE;
}

int visual_video_compare_ignore_pitch (VisVideo *src1, VisVideo *src2)
{
	visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (src1->depth != src2->depth)
		return FALSE;

	if (src1->width != src2->width)
		return FALSE;

	if (src1->height != src2->height)
		return FALSE;

	/* We made it to the end, the VisVideos are likewise in depth, pitch, dimensions */
	return TRUE;
}

int visual_video_set_palette (VisVideo *video, VisPalette *pal)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->pal = pal;

	return VISUAL_OK;
}

int visual_video_set_buffer (VisVideo *video, void *buffer)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (visual_buffer_get_allocated (video->buffer)) {
		visual_log (VISUAL_LOG_ERROR, _("Trying to set a screen buffer on "
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

int visual_video_set_dimension (VisVideo *video, int width, int height)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->width = width;
	video->height = height;

	video->pitch = video->width * video->bpp;

	visual_buffer_set_size (video->buffer, video->pitch * video->height);

	return VISUAL_OK;
}

int visual_video_set_pitch (VisVideo *video, int pitch)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (video->bpp <= 0)
		return -VISUAL_ERROR_VIDEO_INVALID_BPP;

	video->pitch = pitch;
	visual_buffer_set_size (video->buffer, video->pitch * video->height);

	return VISUAL_OK;
}

int visual_video_set_depth (VisVideo *video, VisVideoDepth depth)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->depth = depth;
	video->bpp = visual_video_bpp_from_depth (video->depth);

	return VISUAL_OK;
}

int visual_video_set_attributes (VisVideo *video, int width, int height, int pitch, VisVideoDepth depth)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);
	visual_video_set_pitch (video, pitch);

	return VISUAL_OK;
}

int visual_video_get_size (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	return video->pitch * video->height;
}

void *visual_video_get_pixels (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, NULL);

	return visual_buffer_get_data (video->buffer);
}

VisBuffer *visual_video_get_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, NULL);

	return video->buffer;
}

int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth)
{
	if (visual_video_depth_is_sane (depth) == 0)
		return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

	if ((depth & depthflag) > 0)
		return TRUE;

	return FALSE;
}

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

VisVideoDepth visual_video_depth_get_lowest (int depthflag)
{
	return visual_video_depth_get_next (depthflag, VISUAL_VIDEO_DEPTH_NONE);
}

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

int visual_video_get_boundary (VisVideo *video, VisRectangle *rect)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_rectangle_set (rect, 0, 0, video->width, video->height);

	return VISUAL_OK;
}

int visual_video_region_sub (VisVideo *dest, VisVideo *src, VisRectangle *rect)
{
	VisRectangle vrect;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	/* FIXME make non verbose */
	visual_return_val_if_fail (visual_rectangle_is_empty (rect) == FALSE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	visual_video_get_boundary (src, &vrect);

	/* FIXME make non verbose */
	visual_return_val_if_fail (visual_rectangle_within (&vrect, rect) == TRUE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

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

int visual_video_region_sub_by_values (VisVideo *dest, VisVideo *src, int x, int y, int width, int height)
{
	VisRectangle rect;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_rectangle_set (&rect, x, y, width, height);

	return visual_video_region_sub (dest, src, &rect);
}

int visual_video_region_sub_all (VisVideo *dest, VisVideo *src)
{
	VisRectangle rect;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_get_boundary (dest, &rect);

	return visual_video_region_sub (dest, src, &rect);
}

int visual_video_region_sub_with_boundary (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect)
{
	VisRectangle rsrect;
	VisRectangle sbound;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_rectangle_copy (&rsrect, srect);

	visual_video_get_boundary (src, &sbound);

	/* Merge the destination and source rect, so that only the allowed parts are sub regioned */
	visual_rectangle_clip (&rsrect, &sbound, srect);
	visual_rectangle_clip (&rsrect, drect, &rsrect);

	return visual_video_region_sub (dest, src, &rsrect);
}

int visual_video_composite_set_type (VisVideo *video, VisVideoCompositeType type)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->compositetype = type;

	return VISUAL_OK;
}

int visual_video_composite_set_colorkey (VisVideo *video, VisColor *color)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (color != NULL)
		visual_color_copy (&video->colorkey, color);
	else
		visual_color_set (&video->colorkey, 0, 0, 0);

	return VISUAL_OK;
}

int visual_video_composite_set_surface (VisVideo *video, uint8_t alpha)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->density = alpha;

	return VISUAL_OK;
}

int visual_video_composite_set_function (VisVideo *video, VisVideoCustomCompositeFunc compfunc)
{
	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->compfunc = compfunc;

	return VISUAL_OK;
}

VisVideoCustomCompositeFunc visual_video_composite_get_function (VisVideo *dest, VisVideo *src, int alpha)
{
	visual_return_val_if_fail (dest != NULL, NULL);
	visual_return_val_if_fail (src != NULL, NULL);

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

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

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

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (drect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);
	visual_return_val_if_fail (srect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

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

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (compfunc != NULL, -VISUAL_ERROR_NULL);

	/* We can't overlay GL surfaces so don't even try */
	visual_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_GL ||
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
	unsigned int i;
	unsigned int pixel_count = dest->width * dest->height;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		VisPalette *pal = src->pal;

		if (pal == NULL) {
			blit_overlay_noalpha (dest, src);

			return VISUAL_OK;
		}

		int index = visual_palette_find_color (pal, &src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (*srcbuf != index)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t *destbuf = visual_video_get_pixels (dest);
		uint16_t *srcbuf = visual_video_get_pixels (src);
		uint16_t color = visual_color_to_uint16 (&src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (color != *srcbuf)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		uint8_t r = src->colorkey.r;
		uint8_t g = src->colorkey.g;
		uint8_t b = src->colorkey.b;

		for (i = 0; i < pixel_count; i++) {
			if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
				destbuf[0] = srcbuf[0];
				destbuf[1] = srcbuf[1];
				destbuf[2] = srcbuf[2];
			}

			destbuf += 3;
			srcbuf  += 3;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t *destbuf = visual_video_get_pixels (dest);
		uint32_t *srcbuf = visual_video_get_pixels (src);
		uint32_t color = visual_color_to_uint32 (&src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (color != *srcbuf)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
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

int visual_video_fill_alpha_color (VisVideo *video, VisColor *color, uint8_t density)
{
	int x, y;
	int col = 0;
	uint32_t *vidbuf;

	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

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

int visual_video_fill_alpha (VisVideo *video, uint8_t density)
{
	int x, y;
	uint8_t *vidbuf;

	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	vidbuf = visual_video_get_pixels (video) + 3;

	/* FIXME byte order sensitive */
	for (y = 0; y < video->height; y++) {
		for (x = 0; x < video->width; x++)
			*(vidbuf += video->bpp) = density;

		vidbuf += video->pitch - (video->width * video->bpp);
	}

	return VISUAL_OK;
}

int visual_video_fill_alpha_rectangle (VisVideo *video, uint8_t density, VisRectangle *rect)
{
	VisVideo rvid;
	int errret = VISUAL_OK;

	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	visual_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_init (&rvid);

	errret = visual_video_region_sub (video, &rvid, rect);

	if (errret < 0)
		goto out;

	visual_video_fill_alpha (&rvid, density);

out:
	visual_object_unref (VISUAL_OBJECT (&rvid));

	return errret;
}

int visual_video_fill_color (VisVideo *video, VisColor *rcolor)
{
	VisColor color;

	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (rcolor == NULL)
		visual_color_set (&color, 0, 0, 0);
	else
		visual_color_copy (&color, rcolor);

	switch (video->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			visual_video_fill_color_index8 (video, &color);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_16BIT:
			visual_video_fill_color_rgb16 (video, &color);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_24BIT:
			visual_video_fill_color_rgb24 (video, &color);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_32BIT:
			visual_video_fill_color_argb32 (video, &color);
			return VISUAL_OK;


		default:
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}
}

int visual_video_fill_color_rectangle (VisVideo *video, VisColor *color, VisRectangle *rect)
{
	VisRectangle vrect;
	VisRectangle dbound;
	VisVideo svid;
	int errret = VISUAL_OK;

	visual_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);
	visual_return_val_if_fail (rect != NULL, -VISUAL_ERROR_RECTANGLE_NULL);

	visual_video_get_boundary (video, &vrect);

	visual_return_val_if_fail (visual_rectangle_within_partially (&vrect, rect) != FALSE, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

	visual_video_init (&svid);

	visual_video_get_boundary (video, &dbound);

	visual_video_region_sub_with_boundary (&svid, &dbound, video, rect);

	errret = visual_video_fill_color (&svid, color);

	visual_object_unref (VISUAL_OBJECT (&svid));

	return errret;
}


int visual_video_color_bgr_to_rgb (VisVideo *dest, VisVideo *src)
{
	visual_return_val_if_fail (visual_video_compare (dest, src) == TRUE, -VISUAL_ERROR_VIDEO_NOT_INDENTICAL);
	visual_return_val_if_fail (visual_video_get_pixels (dest) != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);
	visual_return_val_if_fail (visual_video_get_pixels (src)  != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_16BIT:
			visual_video_flip_bytes_color16 (dest, src);
			return VISUAL_OK;
		case VISUAL_VIDEO_DEPTH_24BIT:
			visual_video_flip_bytes_color24 (dest, src);
			return VISUAL_OK;
		case VISUAL_VIDEO_DEPTH_32BIT:
			visual_video_flip_bytes_color32 (dest, src);
			return VISUAL_OK;
		default:
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}
}

int visual_video_rotate (VisVideo *dest, VisVideo *src, VisVideoRotateDegrees degrees)
{
	int ret = VISUAL_OK;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

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

VisVideo *visual_video_rotate_new (VisVideo *src, VisVideoRotateDegrees degrees)
{
	VisVideo *dest;
	int nwidth;
	int nheight;

	visual_return_val_if_fail (src != NULL, NULL);

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

	visual_return_val_if_fail (dest->width == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_return_val_if_fail (dest->height == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

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

	visual_return_val_if_fail (dest->width == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_return_val_if_fail (dest->height == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

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

	visual_return_val_if_fail (dest->width == src->height, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);
	visual_return_val_if_fail (dest->height == src->width, -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS);

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
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src->depth == dest->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

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

	visual_return_val_if_fail (src != NULL, NULL);

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

int visual_video_depth_transform (VisVideo *dest, VisVideo *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL,  -VISUAL_ERROR_VIDEO_NULL);

	/* We blit overlay it instead of just visual_mem_copy because the pitch can still be different */
	if (dest->depth == src->depth)
		return visual_video_blit_overlay (dest, src, 0, 0, FALSE);

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT || src->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		visual_return_val_if_fail (src->pal != NULL, -VISUAL_ERROR_PALETTE_NULL);
		visual_return_val_if_fail (src->pal->ncolors == 256, -VISUAL_ERROR_PALETTE_SIZE);
	}

	if (src->depth == VISUAL_VIDEO_DEPTH_8BIT) {

	    if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
			visual_video_index8_to_rgb16 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
			visual_video_index8_to_rgb24 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
			visual_video_index8_to_argb32 (dest, src);
			return VISUAL_OK;
		}

	} else if (src->depth == VISUAL_VIDEO_DEPTH_16BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
			visual_video_rgb16_to_index8 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
			visual_video_rgb16_to_rgb24 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
			visual_video_rgb16_to_argb32 (dest, src);
			return VISUAL_OK;
		}

	} else if (src->depth == VISUAL_VIDEO_DEPTH_24BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
			visual_video_rgb24_to_index8 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
			visual_video_rgb24_to_rgb16 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
			visual_video_rgb24_to_argb32 (dest, src);
			return VISUAL_OK;
		}

	} else if (src->depth == VISUAL_VIDEO_DEPTH_32BIT) {

		if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
			visual_video_argb32_to_index8 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
			visual_video_argb32_to_rgb16 (dest, src);
			return VISUAL_OK;
		}

		if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
			visual_video_argb32_to_rgb24 (dest, src);
			return VISUAL_OK;
		}
	}

	return -VISUAL_ERROR_VIDEO_NOT_TRANSFORMED;
}

int visual_video_zoom_double (VisVideo *dest, VisVideo *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (dest->depth == src->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			visual_video_zoom_color8 (dest, src);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_16BIT:
			visual_video_zoom_color16 (dest, src);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_24BIT:
			visual_video_zoom_color24 (dest, src);
			return VISUAL_OK;

		case VISUAL_VIDEO_DEPTH_32BIT:
			visual_video_zoom_color32 (dest, src);
			return VISUAL_OK;

		default:
			visual_log (VISUAL_LOG_ERROR, _("Invalid depth passed to the scaler"));
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}
}

static inline int is_valid_scale_method (VisVideoScaleMethod scale_method)
{
    return scale_method == VISUAL_VIDEO_SCALE_NEAREST
	    || scale_method == VISUAL_VIDEO_SCALE_BILINEAR;
}

int visual_video_scale (VisVideo *dest, VisVideo *src, VisVideoScaleMethod method)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (dest->depth == src->depth, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	visual_return_val_if_fail (is_valid_scale_method (method), -VISUAL_ERROR_VIDEO_INVALID_SCALE_METHOD);

	/* If the dest and source are equal in dimension and scale_method is nearest, do a
	 * blit overlay */
	if (visual_video_compare_ignore_pitch (dest, src) == TRUE && method == VISUAL_VIDEO_SCALE_NEAREST) {
		visual_video_blit_overlay (dest, src, 0, 0, FALSE);

		return VISUAL_OK;
	}

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			if (method == VISUAL_VIDEO_SCALE_NEAREST)
				visual_video_scale_nearest_color8 (dest, src);
			else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
				visual_video_scale_bilinear_color8 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			if (method == VISUAL_VIDEO_SCALE_NEAREST)
				visual_video_scale_nearest_color16 (dest, src);
			else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
				visual_video_scale_bilinear_color16 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			if (method == VISUAL_VIDEO_SCALE_NEAREST)
				visual_video_scale_nearest_color24 (dest, src);
			else if (method == VISUAL_VIDEO_SCALE_BILINEAR)
				visual_video_scale_bilinear_color24 (dest, src);

			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			if (method == VISUAL_VIDEO_SCALE_NEAREST)
				visual_video_scale_nearest_color32 (dest, src);
			else if (method == VISUAL_VIDEO_SCALE_BILINEAR) {
				if (visual_cpu_get_mmx ())
					_lv_scale_bilinear_32_mmx (dest, src);
				else
					visual_video_scale_bilinear_color32 (dest, src);
			}

			break;

		default:
			visual_log (VISUAL_LOG_ERROR, _("Invalid depth passed to the scaler"));

			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

			break;
	}

	return VISUAL_OK;
}

VisVideo *visual_video_scale_new (VisVideo *src, int width, int height, VisVideoScaleMethod scale_method)
{
	VisVideo *video;

	visual_return_val_if_fail (src != NULL, NULL);

	video = visual_video_new_with_buffer (width, height, src->depth);

	visual_video_scale (video, src, scale_method);

	return video;
}

int visual_video_scale_depth (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method)
{
	VisVideo dtransform;
	int errret;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

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

VisVideo *visual_video_scale_depth_new (VisVideo *src, int width, int height, VisVideoDepth depth,
		VisVideoScaleMethod scale_method)
{
	VisVideo *video;

	visual_return_val_if_fail (src != NULL, NULL);

	video = visual_video_new_with_buffer (width, height, depth);

	visual_video_scale_depth (video, src, scale_method);

	return video;
}
