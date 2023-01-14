/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Duilio J. Protti <dprotti@users.sourceforge.net>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Jean-Christophe Hoelt <jeko@ios-software.com>
 *          Jaak Randmets <jaak.ra@gmail.com>
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
    auto self = LV::Video::create ();
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth)
{
    auto self = LV::Video::create (width, height, depth);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisVideo *visual_video_new_wrap_buffer (void *buffer, int owner, int width, int height, VisVideoDepth depth, int pitch)
{
    auto self = LV::Video::wrap (buffer, owner, width, height, depth, pitch);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisVideo *visual_video_load_from_file (const char *path)
{
    auto self = LV::Video::create_from_file (path);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

int visual_video_has_allocated_buffer (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, FALSE);

    return self->has_allocated_buffer ();
}

int visual_video_compare_attrs (VisVideo *self, VisVideo *src)
{
    visual_return_val_if_fail (self != nullptr, FALSE);
    visual_return_val_if_fail (src  != nullptr, FALSE);

    return self->compare_attrs (LV::VideoPtr (src));
}

int visual_video_compare_attrs_ignore_pitch (VisVideo *self, VisVideo *src)
{
    visual_return_val_if_fail (self != nullptr, FALSE);
    visual_return_val_if_fail (src  != nullptr, FALSE);

    return self->compare_attrs_ignore_pitch (LV::VideoPtr (src));
}

void visual_video_set_palette (VisVideo *self, VisPalette *pal)
{
    visual_return_if_fail (self != nullptr);

    if (pal) {
        self->set_palette (*pal);
    } else {
        self->set_palette (LV::Palette ());
    }
}

VisPalette *visual_video_get_palette (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    auto& palette = self->get_palette ();
    if (!palette.empty ()) {
        return &palette;
    } else {
        return nullptr;
    }
}

int visual_video_get_width (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_width ();
}

int visual_video_get_height (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_height ();
}

int visual_video_get_pitch (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_pitch ();
}

VisVideoDepth visual_video_get_depth (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, VISUAL_VIDEO_DEPTH_NONE);

    return self->get_depth ();
}

int visual_video_get_bpp (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_bpp ();
}

visual_size_t visual_video_get_size (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, 0);

    return self->get_size ();
}

void *visual_video_get_pixels (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_pixels ();
}

VisBuffer *visual_video_get_buffer (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    LV::BufferPtr buffer = self->get_buffer ();
    if (buffer) {
        LV::intrusive_ptr_add_ref (buffer.get ());
    }

    return buffer.get ();
}

VisRectangle *visual_video_get_extents (VisVideo *self)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return new LV::Rect (self->get_extents ());
}

void *visual_video_get_pixel_ptr (VisVideo *self, int x, int y)
{
    visual_return_val_if_fail (self != nullptr, nullptr);

    return self->get_pixel_ptr (x, y);
}

VisVideo* visual_video_new_sub (VisVideo *src, VisRectangle *area)
{
    visual_return_val_if_fail (src  != nullptr, nullptr);
    visual_return_val_if_fail (area != nullptr, nullptr);

    LV::VideoPtr self = LV::Video::create_sub (LV::VideoPtr (src), *area);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisVideo* visual_video_new_sub_by_values (VisVideo *src, int x, int y, int width, int height)
{
    visual_return_val_if_fail (src != nullptr, nullptr);

    LV::VideoPtr self = LV::Video::create_sub (LV::VideoPtr (src), LV::Rect (x, y, width, height));
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

VisVideo* visual_video_new_sub_with_boundary (VisRectangle *drect, VisVideo *src, VisRectangle *srect)
{
    visual_return_val_if_fail (src   != nullptr, nullptr);
    visual_return_val_if_fail (drect != nullptr, nullptr);
    visual_return_val_if_fail (srect != nullptr, nullptr);

    LV::VideoPtr self = LV::Video::create_sub (*drect, LV::VideoPtr (src), *srect);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

void visual_video_set_compose_type (VisVideo *self, VisVideoComposeType type)
{
    visual_return_if_fail (self != nullptr);

    self->set_compose_type (type);
}

void visual_video_set_compose_colorkey (VisVideo *self, VisColor *color)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (color != nullptr);

    self->set_compose_colorkey (*color);
}

void visual_video_set_compose_surface (VisVideo *self, uint8_t alpha)
{
    visual_return_if_fail (self != nullptr);

    self->set_compose_surface (alpha);
}

void visual_video_set_compose_function (VisVideo *self, VisVideoComposeFunc compose_func)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (compose_func != nullptr);

    self->set_compose_function (compose_func);
}

VisVideoComposeFunc visual_video_get_compose_function (VisVideo *self, VisVideo *src, int alpha)
{
    visual_return_val_if_fail (self != nullptr, nullptr);
    visual_return_val_if_fail (src  != nullptr, nullptr);

    return self->get_compose_function (src, alpha);
}

void visual_video_blit_area (VisVideo     *self,
                             VisRectangle *drect,
                             VisVideo     *src,
                             VisRectangle *srect,
                             int           alpha)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (drect != nullptr);
    visual_return_if_fail (src   != nullptr);
    visual_return_if_fail (srect != nullptr);

    self->blit (*drect, LV::VideoPtr (src), *srect, alpha);
}

void visual_video_compose_area (VisVideo            *self,
                                VisRectangle        *drect,
                                VisVideo            *src,
                                VisRectangle        *srect,
                                VisVideoComposeFunc  compose_func)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (drect != nullptr);
    visual_return_if_fail (src   != nullptr);
    visual_return_if_fail (srect != nullptr);
    visual_return_if_fail (compose_func != nullptr);

    self->compose (*drect, LV::VideoPtr (src), *srect, compose_func);
}

void visual_video_blit_scale_area (VisVideo           *self,
                                   VisRectangle       *drect,
                                   VisVideo           *src,
                                   VisRectangle       *srect,
                                   int                 alpha,
                                   VisVideoScaleMethod scale_method)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (drect != nullptr);
    visual_return_if_fail (src   != nullptr);
    visual_return_if_fail (srect != nullptr);

    self->blit_scale (*drect, LV::VideoPtr (src), *srect, alpha, scale_method);
}

void visual_video_compose_scale_area (VisVideo            *self,
                                      VisRectangle        *drect,
                                      VisVideo            *src,
                                      VisRectangle        *srect,
                                      VisVideoScaleMethod  scale_method,
                                      VisVideoComposeFunc  compose_func)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (drect != nullptr);
    visual_return_if_fail (src   != nullptr);
    visual_return_if_fail (srect != nullptr);
    visual_return_if_fail (compose_func != nullptr);

    self->compose_scale (*drect, LV::VideoPtr (src), *srect, scale_method, compose_func);
}

void visual_video_blit (VisVideo *self, VisVideo *src, int x, int y, int alpha)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (src   != nullptr);

    self->blit (LV::VideoPtr (src), x, y, alpha);
}

void visual_video_compose (VisVideo *self, VisVideo *src, int x, int y, VisVideoComposeFunc compose_func)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (src   != nullptr);
    visual_return_if_fail (compose_func != nullptr);

    self->compose (LV::VideoPtr (src), x, y, compose_func);
}

void visual_video_fill_alpha (VisVideo *self, uint8_t alpha)
{
    visual_return_if_fail (self != nullptr);

    self->fill_alpha (alpha);
}

void visual_video_fill_alpha_area (VisVideo *self, uint8_t alpha, VisRectangle *area)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (area != nullptr);

    self->fill_alpha (alpha, *area);
}

void visual_video_fill_color (VisVideo *self, VisColor *color)
{
    visual_return_if_fail (self  != nullptr);

    if (color) {
        self->fill_color (*color);
    } else {
        self->fill_color (LV::Color::black ());
    }
}

void visual_video_fill_color_area (VisVideo *self, VisColor *color, VisRectangle *area)
{
    visual_return_if_fail (self  != nullptr);
    visual_return_if_fail (color != nullptr);
    visual_return_if_fail (area  != nullptr);

    self->fill_color (*color, *area);
}


void visual_video_flip_pixel_bytes (VisVideo *self, VisVideo *src)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->flip_pixel_bytes (LV::VideoPtr (src));
}

void visual_video_rotate (VisVideo *self, VisVideo *src, VisVideoRotateDegrees degrees)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->rotate (LV::VideoPtr (src), degrees);
}

void visual_video_mirror (VisVideo *self, VisVideo *src, VisVideoMirrorOrient orient)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->mirror (LV::VideoPtr (src), orient);
}

void visual_video_convert_depth (VisVideo *self, VisVideo *src)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->convert_depth (LV::VideoPtr (src));
}

void visual_video_scale (VisVideo *self, VisVideo *src, VisVideoScaleMethod method)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->scale (LV::VideoPtr (src), method);
}

void visual_video_scale_depth (VisVideo *self, VisVideo *src, VisVideoScaleMethod scale_method)
{
    visual_return_if_fail (self != nullptr);
    visual_return_if_fail (src  != nullptr);

    self->scale_depth (LV::VideoPtr (src), scale_method);
}

VisVideo *visual_video_scale_depth_new (VisVideo*           src,
                                        int                 width,
                                        int                 height,
                                        VisVideoDepth       depth,
                                        VisVideoScaleMethod scale_method)
{
    visual_return_val_if_fail (src != nullptr, nullptr);

    auto self = LV::Video::create_scale_depth (LV::VideoPtr (src), width, height, depth, scale_method);
    if (self) {
        LV::intrusive_ptr_add_ref (self.get ());
    }

    return self.get ();
}

void visual_video_ref (VisVideo *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_add_ref (self);
}

void visual_video_unref (VisVideo *self)
{
    visual_return_if_fail (self != nullptr);

    LV::intrusive_ptr_release (self);
}
