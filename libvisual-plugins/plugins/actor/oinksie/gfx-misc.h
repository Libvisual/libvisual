/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-misc.h,v 1.5 2006/01/31 16:42:47 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _OINK_GFX_MISC_H
#define _OINK_GFX_MISC_H

#include "oinksie.h"

void _oink_gfx_something_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y, int type, int pixmap);
void _oink_gfx_pixmap_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y, int type);
int _oink_gfx_pixel_get (OinksiePrivate *priv, uint8_t *buf, int x, int y);
void _oink_gfx_pixel_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y);
void _oink_gfx_line (OinksiePrivate *priv, uint8_t *buf, int color, int x0, int y0, int x1, int y1);
void _oink_gfx_hline (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y1, int y2);
void _oink_gfx_vline (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y1, int y2);
void _oink_gfx_circle_filled (OinksiePrivate *priv, uint8_t *buf, int color, int size, int x, int y);
void _oink_gfx_circle (OinksiePrivate *priv, uint8_t *buf, int color, int xsize, int ysize, int x, int y);

#endif /* _OINK_GFX_MISC_H */
