/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *      Duilio J. Protti <dprotti@users.sourceforge.net>
 *      Chong Kai Xiong <kaixiong@codeleft.sg>
 *      Jean-Christophe Hoelt <jeko@ios-software.com>
 *      Jaak Randmets <jaak.ra@gmail.com>
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

VisVideo *visual_video_new ()
{
    LV::VideoPtr self = LV::Video::create ();
    self->ref ();

    return self.get ();
}

VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth)
{
    LV::VideoPtr self = LV::Video::create (width, height, depth);
    self->ref ();

    return self.get ();
}

VisVideo *visual_video_new_wrap_buffer (void *buffer, int owner, int width, int height, VisVideoDepth depth)
{
    LV::VideoPtr self = LV::Video::wrap (buffer, owner, width, height, depth);
    self->ref ();

    return self.get ();
}

void visual_video_free_buffer (VisVideo *self)
{
    visual_return_if_fail (self != NULL);

    self->free_buffer ();
}

int visual_video_allocate_buffer (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    return self->allocate_buffer ();
}

int visual_video_has_allocated_buffer (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, FALSE);

    return self->has_allocated_buffer ();
}

void visual_video_copy_attrs (VisVideo *self, VisVideo *src)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->copy_attrs (LV::VideoPtr (src));
}

int visual_video_compare_attrs (VisVideo *self, VisVideo *src)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (src  != NULL, FALSE);

    return self->compare_attrs (LV::VideoPtr (src));
}

int visual_video_compare_attrs_ignore_pitch (VisVideo *self, VisVideo *src)
{
    visual_return_val_if_fail (self != NULL, FALSE);
    visual_return_val_if_fail (src  != NULL, FALSE);

    return self->compare_attrs_ignore_pitch (LV::VideoPtr (src));
}

void visual_video_set_palette (VisVideo *self, VisPalette *pal)
{
    visual_return_if_fail (self != NULL);

    if (pal) {
        self->set_palette (*pal);
    } else {
        self->set_palette (LV::Palette ());
    }
}

VisPalette *visual_video_get_palette (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    LV::Palette& palette = self->get_palette ();
    if (!palette.empty ()) {
        return &palette;
    } else {
        return NULL;
    }
}

void visual_video_set_buffer (VisVideo *self, void *buffer)
{
    visual_return_if_fail (self != NULL);

    self->set_buffer (buffer);
}

void visual_video_set_dimension (VisVideo *self, int width, int height)
{
    visual_return_if_fail (self != NULL);

    self->set_dimension (width, height);
}

int visual_video_get_width (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_width ();
}

int visual_video_get_height (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_height ();
}

void visual_video_set_pitch (VisVideo *self, int pitch)
{
    visual_return_if_fail (self != NULL);

    self->set_pitch (pitch);
}

int visual_video_get_pitch (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_pitch ();
}

void visual_video_set_depth (VisVideo *self, VisVideoDepth depth)
{
    visual_return_if_fail (self != NULL);

    self->set_depth (depth);
}

VisVideoDepth visual_video_get_depth (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, VISUAL_VIDEO_DEPTH_NONE);

    return self->get_depth ();
}

int visual_video_get_bpp (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_bpp ();
}

void visual_video_set_attrs (VisVideo *self, int width, int height, int pitch, VisVideoDepth depth)
{
    visual_return_if_fail (self != NULL);

    self->set_attrs (width, height, pitch, depth);
}

visual_size_t visual_video_get_size (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, 0);

    return self->get_size ();
}

void *visual_video_get_pixels (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->get_pixels ();
}

VisBuffer *visual_video_get_buffer (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    LV::BufferPtr buffer = self->get_buffer ();
    if (buffer) {
        buffer->ref ();
    }

    return buffer.get ();
}

VisRectangle *visual_video_get_extents (VisVideo *self)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return new LV::Rect (self->get_extents ());
}

void *visual_video_get_pixel_ptr (VisVideo *self, int x, int y)
{
    visual_return_val_if_fail (self != NULL, NULL);

    return self->get_pixel_ptr (x, y);
}

VisVideo* visual_video_new_sub (VisVideo *src, VisRectangle *area)
{
    visual_return_val_if_fail (src  != NULL, NULL);
    visual_return_val_if_fail (area != NULL, NULL);

    LV::VideoPtr self = LV::Video::create_sub (LV::VideoPtr (src), *area);
    self->ref ();

    return self.get ();
}

VisVideo* visual_video_new_sub_by_values (VisVideo *src, int x, int y, int width, int height)
{
    visual_return_val_if_fail (src != NULL, NULL);

    LV::VideoPtr self = LV::Video::create_sub (LV::VideoPtr (src), LV::Rect (x, y, width, height));
    self->ref ();

    return self.get ();
}

VisVideo* visual_video_new_sub_with_boundary (VisRectangle *drect, VisVideo *src, VisRectangle *srect)
{
    visual_return_val_if_fail (src   != NULL, NULL);
    visual_return_val_if_fail (drect != NULL, NULL);
    visual_return_val_if_fail (srect != NULL, NULL);

    LV::VideoPtr self = LV::Video::create_sub (*drect, LV::VideoPtr (src), *srect);
    self->ref ();

    return self.get ();
}

void visual_video_set_compose_type (VisVideo *self, VisVideoComposeType type)
{
    visual_return_if_fail (self != NULL);

    self->set_compose_type (type);
}

void visual_video_set_compose_colorkey (VisVideo *self, VisColor *color)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (color != NULL);

    self->set_compose_colorkey (*color);
}

void visual_video_set_compose_surface (VisVideo *self, uint8_t alpha)
{
    visual_return_if_fail (self != NULL);

    self->set_compose_surface (alpha);
}

void visual_video_set_compose_function (VisVideo *self, VisVideoComposeFunc compose_func)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (compose_func != NULL);

    self->set_compose_function (compose_func);
}

VisVideoComposeFunc visual_video_get_compose_function (VisVideo *self, VisVideo *src, int alpha)
{
    visual_return_val_if_fail (self != NULL, NULL);
    visual_return_val_if_fail (src  != NULL, NULL);

    return self->get_compose_function (src, alpha);
}

void visual_video_blit_area (VisVideo     *self,
                             VisRectangle *drect,
                             VisVideo     *src,
                             VisRectangle *srect,
                             int           alpha)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (drect != NULL);
    visual_return_if_fail (src   != NULL);
    visual_return_if_fail (srect != NULL);

    self->blit (*drect, LV::VideoPtr (src), *srect, alpha);
}

void visual_video_compose_area (VisVideo            *self,
                                VisRectangle        *drect,
                                VisVideo            *src,
                                VisRectangle        *srect,
                                VisVideoComposeFunc  compose_func)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (drect != NULL);
    visual_return_if_fail (src   != NULL);
    visual_return_if_fail (srect != NULL);
    visual_return_if_fail (compose_func != NULL);

    self->compose (*drect, LV::VideoPtr (src), *srect, compose_func);
}

void visual_video_blit_scale_area (VisVideo           *self,
                                   VisRectangle       *drect,
                                   VisVideo           *src,
                                   VisRectangle       *srect,
                                   int                 alpha,
                                   VisVideoScaleMethod scale_method)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (drect != NULL);
    visual_return_if_fail (src   != NULL);
    visual_return_if_fail (srect != NULL);

    self->blit_scale (*drect, LV::VideoPtr (src), *srect, alpha, scale_method);
}

void visual_video_compose_scale_area (VisVideo            *self,
                                      VisRectangle        *drect,
                                      VisVideo            *src,
                                      VisRectangle        *srect,
                                      VisVideoScaleMethod  scale_method,
                                      VisVideoComposeFunc  compose_func)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (drect != NULL);
    visual_return_if_fail (src   != NULL);
    visual_return_if_fail (srect != NULL);
    visual_return_if_fail (compose_func != NULL);

    self->compose_scale (*drect, LV::VideoPtr (src), *srect, scale_method, compose_func);
}

void visual_video_blit (VisVideo *self, VisVideo *src, int x, int y, int alpha)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (src   != NULL);

    self->blit (LV::VideoPtr (src), x, y, alpha);
}

void visual_video_compose (VisVideo *self, VisVideo *src, int x, int y, VisVideoComposeFunc compose_func)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (src   != NULL);
    visual_return_if_fail (compose_func != NULL);

    self->compose (LV::VideoPtr (src), x, y, compose_func);
}

void visual_video_fill_alpha (VisVideo *self, uint8_t alpha)
{
    visual_return_if_fail (self != NULL);

    self->fill_alpha (alpha);
}

void visual_video_fill_alpha_area (VisVideo *self, uint8_t alpha, VisRectangle *area)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (area != NULL);

    self->fill_alpha (alpha, *area);
}

void visual_video_fill_color (VisVideo *self, VisColor *color)
{
    visual_return_if_fail (self  != NULL);

    if (color) {
        self->fill_color (*color);
    } else {
        self->fill_color (LV::Color::black ());
    }
}

void visual_video_fill_color_area (VisVideo *self, VisColor *color, VisRectangle *area)
{
    visual_return_if_fail (self  != NULL);
    visual_return_if_fail (color != NULL);
    visual_return_if_fail (area  != NULL);

    self->fill_color (*color, *area);
}


void visual_video_flip_pixel_bytes (VisVideo *self, VisVideo *src)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->flip_pixel_bytes (LV::VideoPtr (src));
}

void visual_video_rotate (VisVideo *self, VisVideo *src, VisVideoRotateDegrees degrees)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->rotate (LV::VideoPtr (src), degrees);
}

void visual_video_mirror (VisVideo *self, VisVideo *src, VisVideoMirrorOrient orient)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->mirror (LV::VideoPtr (src), orient);
}

void visual_video_convert_depth (VisVideo *self, VisVideo *src)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->convert_depth (LV::VideoPtr (src));
}

void visual_video_scale (VisVideo *self, VisVideo *src, VisVideoScaleMethod method)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->scale (LV::VideoPtr (src), method);
}

void visual_video_scale_depth (VisVideo *self, VisVideo *src, VisVideoScaleMethod scale_method)
{
    visual_return_if_fail (self != NULL);
    visual_return_if_fail (src  != NULL);

    self->scale_depth (LV::VideoPtr (src), scale_method);
}

VisVideo *visual_video_scale_depth_new (VisVideo*           src,
                                        int                 width,
                                        int                 height,
                                        VisVideoDepth       depth,
                                        VisVideoScaleMethod scale_method)
{
    visual_return_val_if_fail (src != NULL, NULL);

    LV::VideoPtr self = LV::Video::create_scale_depth (LV::VideoPtr (src), width, height, depth, scale_method);
    self->ref ();

    return self.get ();
}

void visual_video_ref (VisVideo *self)
{
    self->ref ();
}

void visual_video_unref (VisVideo *self)
{
    self->unref ();
}
