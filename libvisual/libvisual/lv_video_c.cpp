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
#include "lv_common.h"
#include "gettext.h"

VisVideo *visual_video_new ()
{
    // FIXME: implement
    return NULL;
}

VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth)
{
    // FIXME: implement
    return NULL;
}

VisVideo *visual_video_new_wrap_buffer (void *buffer, int owner, int width, int height, VisVideoDepth depth)
{
    // FIXME: implement
    return NULL;
}

void visual_video_free_buffer (VisVideo *video)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

int visual_video_allocate_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, FALSE);

    // FIXME: implement
}

int visual_video_has_allocated_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, FALSE);

    // FIXME: implement
	return FALSE;
}

void visual_video_copy_attrs (VisVideo *dest, VisVideo *src)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

int visual_video_compare_attrs (VisVideo *src1, VisVideo *src2)
{
	visual_return_val_if_fail (src1 != NULL, FALSE);
	visual_return_val_if_fail (src2 != NULL, FALSE);

    // FIXME: implement
    return FALSE;
}

int visual_video_compare_attrs_ignore_pitch (VisVideo *src1, VisVideo *src2)
{
	visual_return_val_if_fail (src1 != NULL, FALSE);
	visual_return_val_if_fail (src2 != NULL, FALSE);

    // FIXME: implement
    return FALSE;
}

void visual_video_set_palette (VisVideo *video, VisPalette *pal)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

VisPalette *visual_video_get_palette (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, NULL);

    // FIXME: implement
    return NULL;
}

void visual_video_set_buffer (VisVideo *video, void *buffer)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

void visual_video_set_dimension (VisVideo *video, int width, int height)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

int visual_video_get_width (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, 0);

    // FIXME: implement
    return 0;
}

int visual_video_get_height (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, 0);

    // FIXME: implement
    return 0;
}

void visual_video_set_pitch (VisVideo *video, int pitch)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

int visual_video_get_pitch (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, 0);

    // FIXME: implement
    return 0;
}

void visual_video_set_depth (VisVideo *video, VisVideoDepth depth)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

VisVideoDepth visual_video_get_depth (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, VISUAL_VIDEO_DEPTH_NONE);

    // FIXME: implement
    return VISUAL_VIDEO_DEPTH_NONE;
}

int visual_video_get_bpp (VisVideo *video)
{
    visual_return_val_if_fail (video != NULL, 0);

    // FIXME: implement
    return 0;
}

void visual_video_set_attrs (VisVideo *video, int width, int height, int pitch, VisVideoDepth depth)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

visual_size_t visual_video_get_size (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, 0);

    // FIXME: implement
	return 0;
}

void *visual_video_get_pixels (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, NULL);

    // FIXME: implement
    return NULL;
}

VisBuffer *visual_video_get_buffer (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, NULL);

    // FIXME: implement
	return NULL;
}

VisRectangle *visual_video_get_extents (VisVideo *video)
{
	visual_return_val_if_fail (video != NULL, NULL);

    // FIXME: implement
	return NULL;
}

void *visual_video_get_pixel_ptr (VisVideo *video, int x, int y)
{
	visual_return_val_if_fail (video != NULL, NULL);

    // FIXME: implement
	return NULL;
}

void visual_video_region_sub (VisVideo *dest, VisVideo *src, VisRectangle *area)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);
	visual_return_if_fail (area != NULL);

    // FIXME: implement
}

void visual_video_region_sub_by_values (VisVideo *dest, VisVideo *src, int x, int y, int width, int height)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    LV::Rect rect (x, y, width, height);
	visual_video_region_sub (dest, src, &rect);
}

void visual_video_region_sub_all (VisVideo *dest, VisVideo *src)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

	VisRectangle *rect = visual_video_get_extents (dest);
	visual_video_region_sub (dest, src, rect);
	visual_rectangle_free (rect);
}

void visual_video_region_sub_with_boundary (VisVideo *dest, VisRectangle *drect, VisVideo *src, VisRectangle *srect)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);
	visual_return_if_fail (drect != NULL);
	visual_return_if_fail (srect != NULL);
}

void visual_video_set_compose_type (VisVideo *video, VisVideoComposeType type)
{
	visual_return_if_fail (video != NULL);
}

void visual_video_set_compose_colorkey (VisVideo *video, VisColor *color)
{
	visual_return_if_fail (video != NULL);
}

void visual_video_set_compose_surface (VisVideo *video, uint8_t alpha)
{
	visual_return_if_fail (video != NULL);
}

void visual_video_set_compose_function (VisVideo *video, VisVideoComposeFunc compose_func)
{
	visual_return_if_fail (video != NULL);
}

VisVideoComposeFunc visual_video_get_compose_function (VisVideo *dest, VisVideo *src, int alpha)
{
	visual_return_val_if_fail (dest != NULL, NULL);
	visual_return_val_if_fail (src  != NULL, NULL);

    return NULL;
}

void visual_video_blit_area (VisVideo     *dest,
                             VisRectangle *drect,
                             VisVideo     *src,
                             VisRectangle *srect,
                             int           alpha)
{
    // FIXME: implement
}

void visual_video_compose_area (VisVideo            *dest,
                                VisRectangle        *drect,
                                VisVideo            *src,
                                VisRectangle        *srect,
                                VisVideoComposeFunc  compose_func)
{
    // FIXME: implement
}

void visual_video_blit_scale_area (VisVideo           *dest,
                                   VisRectangle       *drect,
                                   VisVideo           *src,
                                   VisRectangle       *srect,
                                   int                 alpha,
                                   VisVideoScaleMethod scale_method)
{
    // FIXME: implement
}

void visual_video_compose_scale_area (VisVideo            *dest,
                                      VisRectangle        *drect,
                                      VisVideo            *src,
                                      VisRectangle        *srect,
                                      VisVideoScaleMethod  scale_method,
                                      VisVideoComposeFunc  compose_func)
{
    // FIXME: implement
}

void visual_video_blit (VisVideo *dest, VisVideo *src, int x, int y, int alpha)
{
    // FIXME: implement
}

void visual_video_compose (VisVideo *dest, VisVideo *src, int x, int y, VisVideoComposeFunc compose_func)
{
    // FIXME: implement
}


void visual_video_fill_alpha_color (VisVideo *video, VisColor *color, uint8_t alpha)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

void visual_video_fill_alpha (VisVideo *video, uint8_t alpha)
{
	visual_return_if_fail (video != NULL);

    // FIXME: implement
}

void visual_video_fill_alpha_area (VisVideo *video, uint8_t alpha, VisRectangle *area)
{
	visual_return_if_fail (video != NULL);
	visual_return_if_fail (area  != NULL);

    // FIXME: implement
}

void visual_video_fill_color (VisVideo *video, VisColor *color)
{
	visual_return_if_fail (video != NULL);
	visual_return_if_fail (color != NULL);

    // FIXME: implement
}

void visual_video_fill_color_area (VisVideo *video, VisColor *color, VisRectangle *area)
{
	visual_return_if_fail (video != NULL);
	visual_return_if_fail (color != NULL);
	visual_return_if_fail (area  != NULL);

    // FIXME: implement
}


void visual_video_flip_pixel_bytes (VisVideo *dest, VisVideo *src)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

void visual_video_rotate (VisVideo *dest, VisVideo *src, VisVideoRotateDegrees degrees)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

void visual_video_mirror (VisVideo *dest, VisVideo *src, VisVideoMirrorOrient orient)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

void visual_video_convert_depth (VisVideo *dest, VisVideo *src)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

void visual_video_scale (VisVideo *dest, VisVideo *src, VisVideoScaleMethod method)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

void visual_video_scale_depth (VisVideo *dest, VisVideo *src, VisVideoScaleMethod scale_method)
{
	visual_return_if_fail (dest != NULL);
	visual_return_if_fail (src  != NULL);

    // FIXME: implement
}

VisVideo *visual_video_scale_depth_new (VisVideo*           src,
                                        int                 width,
                                        int                 height,
                                        VisVideoDepth       depth,
                                        VisVideoScaleMethod scale_method)
{
	visual_return_val_if_fail (src != NULL, NULL);

    // FIXME: Implement

	return NULL;
}
