/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-background.c,v 1.7 2006/01/22 13:25:26 synap Exp $
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

#include <math.h>
#include <string.h>

#include "common.h"
#include "screen.h"
#include "audio.h"
#include "table.h"
#include "gfx-misc.h"
#include "misc.h"
#include "gfx-background.h"

void _oink_gfx_background_circles_sine (OinksiePrivate *priv,
		uint8_t *buf, int color, int rotate, int scroll, int stretch, int size)
{
	int i;
	int y;
	int y1;
	int x;
	int x1;
	int tab;
	int sadd = priv->screen_xysmallest / 2;
	float sden;

	rotate = abs (rotate);
	scroll = abs (scroll);
	stretch = abs (stretch);

	tab = scroll;

	for (i = 0; i < priv->screen_width; i+= 20)
	{
		sden = _oink_table_sin[abs (tab % OINK_TABLE_NORMAL_SIZE)];
		y = (sden * size) + sadd;
		
		sden = _oink_table_sin[abs ((tab + OINK_TABLE_NORMAL_SIZE / 2) % OINK_TABLE_NORMAL_SIZE)];
		y1 = (sden * size) + sadd;
		
		x = i - priv->screen_halfwidth;
		x1 = x;

		y = y - priv->screen_halfheight;
		y1 = y1 - priv->screen_halfheight;

		_oink_pixel_rotate (&x, &y, rotate);
		_oink_pixel_rotate (&x1, &y1, rotate);

		_oink_gfx_circle_filled (priv, buf, color - fabs (sden * 20), 15 - fabs (sden * 10),
				x + priv->screen_halfwidth, y + priv->screen_halfheight);
		_oink_gfx_circle_filled (priv, buf, color - fabs (sden * 20), 15 - fabs (sden * 10),
				x1 + priv->screen_halfwidth, y1 + priv->screen_halfheight);
		
		tab += stretch;
	}
}

void _oink_gfx_background_circles_filled (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int distance, int turn, int x, int y)
{
	int i;
	int xi;
	int yi;
	int adder = OINK_TABLE_NORMAL_SIZE / number;
	int tab = turn % OINK_TABLE_NORMAL_SIZE;

	if (tab < 0)
	{
		tab = OINK_TABLE_NORMAL_SIZE - +tab;
		tab = tab % OINK_TABLE_NORMAL_SIZE;
	}

	for (i = 0; i < number; i++)
	{
		xi = (_oink_table_sin[tab % OINK_TABLE_NORMAL_SIZE] * distance) + x;
		yi = (_oink_table_cos[tab % OINK_TABLE_NORMAL_SIZE] * distance) + y;
		
		tab += adder;
	
		_oink_gfx_circle_filled (priv, buf, color, size, xi, yi); 
	}
}

// In C, `-8 % 5` yields -3 (and `-8 % 5u` yields +3) rather than 2. This wrapper corrects for it.
static int mod(int a, int n) {
	const int original = a % n;
	return (original >= 0) ? original : (original + n);
}

void _oink_gfx_background_floaters (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int xturn, int yturn, int x, int badd1, int badd2)
{
	int i;
	int xi;
	int yi;
	int xb;
	int yb;
	int add1 = 0;
	int add2 = 0;
		
	int mul = ((priv->screen_width - 20) / number);
	
	for (i = 0; i < number; i++)
	{
		xi = (_oink_table_sin[mod(xturn + add1, OINK_TABLE_NORMAL_SIZE)] * (float) (priv->screen_width / (number + 1)));
		yi = (_oink_table_cos[mod(yturn + add2, OINK_TABLE_NORMAL_SIZE)] * (float) (priv->screen_height / 5));

		xb = xi + (i * mul) + 20;
		yb = yi + x;

		if (xb > 0 + size || xb < priv->screen_width - size || yb > 0 + size || yb < priv->screen_height - size)
			_oink_gfx_circle_filled (priv, buf, color, size, xb, yb);

		add1 += badd1;
		add2 += badd2;
	}
}

void _oink_gfx_background_ball_shooting (OinksiePrivate *priv,
		uint8_t *buf, int color, int distance, int xb, int yb, int x, int y)
{
	int xi = _oink_line_xory_next_xy (0, distance, x, y, xb, yb);
	int yi = _oink_line_xory_next_xy (1, distance, x, y, xb, yb);

	_oink_gfx_circle_filled (priv, buf, color, distance >> 1, xi, yi);
}

void _oink_gfx_background_ball_whirling (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int dia, int rot, int x, int y)
{
	int xd;
	int yd;

	xd = (_oink_table_sin[rot % OINK_TABLE_NORMAL_SIZE] * dia) + x;
	yd = (_oink_table_cos[rot % OINK_TABLE_NORMAL_SIZE] * dia) + y;

	_oink_gfx_circle_filled (priv, buf, color, size, xd, yd);
}

void _oink_gfx_background_fill (OinksiePrivate *priv,
		uint8_t *buf, int color)
{
	visual_mem_set (buf, color, priv->screen_size);
}

void _oink_gfx_background_circles_star (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int tentnr, int ballnr, int badd, int turn, int x, int y)
{
	int i;
	int i2;
	int sizedef = 0;
	int sizedefadd = size / ballnr;
	int sinadd = OINK_TABLE_NORMAL_SIZE / tentnr;
	int sindef = turn;
	int bdef = 0;
	int xd;
	int yd;

	for (i = 0; i < tentnr; i++)
	{
		sizedef = 0;

		bdef = 0;
		for (i2 = 0; i2 < ballnr; i2++)
		{
			xd = (_oink_table_sin[sindef % OINK_TABLE_NORMAL_SIZE] * bdef) + x;
			yd = (_oink_table_cos[sindef % OINK_TABLE_NORMAL_SIZE] * bdef) + y;

			_oink_gfx_circle_filled (priv, buf, color, size - sizedef, xd, yd);

			bdef += badd;
			sizedef += sizedefadd;
		}
		sindef += sinadd;
	}
}

