/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of Blurks-libvisual.
 *
 * Blurks-libvisual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blurks-libvisual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blurks-libvisual.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LV_ACTOR_BLURSK_H
#define _LV_ACTOR_BLURSK_H

#include <libvisual/libvisual.h>

typedef struct {
        int                      height;
        int                      width;
        /* true if colormap should be regenerated */
        int                      update_colmap;
        VisPalette              *pal;
        uint8_t                 *rgb_buf;
        VisVideo                *video;
        VisBuffer               *pcmbuf;
        VisRandomContext        *rcontext;
        VisColor                color;
        VisPluginData           *plugin;
        int                     update_config_string;
} BlurskPrivate;

#endif /* _LV_ACTOR_BLURSK_H */
