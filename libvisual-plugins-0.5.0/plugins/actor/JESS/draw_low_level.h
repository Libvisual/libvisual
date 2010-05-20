/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: draw_low_level.h,v 1.3 2005/12/20 18:49:12 synap Exp $
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

#include "jess.h"

void boule_random (JessPrivate *priv, uint8_t * buffer, int x, int y, int r, uint8_t color);
void cercle(JessPrivate *priv, uint8_t *buffer, int h, int k, int y, uint8_t color);
void cercle_32(JessPrivate *priv, uint8_t *buffer, int h, int k, int y, uint8_t color);
void boule(JessPrivate *priv, uint8_t *buffer, int x, int y, int r, uint8_t color);
void droite(JessPrivate *priv, uint8_t *buffer,int x1, int y1, int x2, int y2, uint8_t color);
void tracer_point_add(JessPrivate *priv, uint8_t *buffer, int x, int y, uint8_t color);
void tracer_point_add_32(JessPrivate *priv, uint8_t *buffer, int x, int y, uint8_t color);

void ball_init(JessPrivate *priv);
void boule_no_add(uint8_t *buffer, int x, int y, int r, uint8_t color);
void cercle_no_add(uint8_t *buffer, int h, int k, int y, uint8_t color);
void tracer_point_no_add(uint8_t *buffer, int x, int y, uint8_t color);
void ball(JessPrivate *priv, uint8_t *buffer, int x, int y, int r, uint8_t color);
