/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-background.h,v 1.5 2006/01/22 13:25:26 synap Exp $
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

#ifndef _OINK_GFX_BACKGROUND_H
#define _OINK_GFX_BACKGROUND_H

void _oink_gfx_background_circles_sine (OinksiePrivate *priv,
		uint8_t *buf, int color, int rotate, int scroll, int stretch, int size);
void _oink_gfx_background_circles_filled (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int distance, int turn, int x, int y);
void _oink_gfx_background_floaters (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int xturn, int yturn, int x, int badd1, int badd2);
void _oink_gfx_background_ball_shooting (OinksiePrivate *priv,
		uint8_t *buf, int color, int distance, int xb, int yb, int x, int y);
void _oink_gfx_background_ball_whirling (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int dia, int rot, int x, int y);
void _oink_gfx_background_fill (OinksiePrivate *priv,
		uint8_t *buf, int color);
void _oink_gfx_background_circles_star (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int tentnr, int ballnr, int badd, int turn, int x, int y);

#endif /* _OINK_GFX_BACKGROUND_H */
