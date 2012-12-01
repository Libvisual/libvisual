/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-misc.c,v 1.9 2006/01/31 16:42:47 synap Exp $
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

#include <string.h>

#include <math.h>

#include "common.h"
#include "config.h"
#include "audio.h"
#include "table.h"
#include "screen.h"
#include "gfx-misc.h"

int _oink_gfx_pixel_get (OinksiePrivate *priv, uint8_t *buf, int x, int y)
{
	int pos = (y * priv->screen_width) + x;

	if (pos < priv->screen_size && pos > 0)
		return buf[pos];

	return 0;
}

void _oink_gfx_pixel_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y)
{
	int pos = (y * priv->screen_width) + x;

	if (pos < priv->screen_size && pos > 0)
		buf[pos] = color;

}

/* FIXME this routine shows errors */
void _oink_gfx_line (OinksiePrivate *priv, uint8_t *buf, int color, int x0, int y0, int x1, int y1)
{
	register int dy = y1 - y0;
	register int dx = x1 - x0;
	register int stepx, stepy, stepy_;
	register int fraction;
	register int bp;

	if (x0 < 0 || x0 > priv->screen_width - 1 || x1 < 0 || x1 > priv->screen_width - 1||
	    y0 < 0 || y0 > priv->screen_height - 1 || y1 < 0 || y1 > priv->screen_height - 1)
	{
		if (x0 < 0)
			x0 = 0;
		else if (x0 > priv->screen_width - 1)
			x0 = priv->screen_width - 1;

		if (x1 < 0)
			x1 = 0;
		else if (x1 > priv->screen_width - 1)
			x1 = priv->screen_width - 1;

		if (y0 < 0)
			y0 = 0;
		else if (y0 > priv->screen_height - 1)
			y0 = priv->screen_height - 1;

		if (y1 < 0)
			y1 = 0;
		else if (y1 > priv->screen_height - 1)
			y1 = priv->screen_height - 1;

		return; /* FIXME */
	}

	if (dy < 0)
	{
		dy = -dy;
		stepy_ = -priv->screen_width;
		stepy = -1;
	}
	else
	{
		stepy = 1;
		stepy_ = priv->screen_width;
	}

	if (dx < 0)
	{
		dx = -dx;
		stepx = -1;
	}
	else
	{
		stepx = 1;
	}

	dy <<= 1;
	dx <<= 1;

	bp = x0 + y0 * priv->screen_width;

	_oink_gfx_pixel_set (priv, buf, color, x0, y0);

	if (dx > dy)
	{
		fraction = dy - (dx >> 1);
		while (x0 != x1)
		{
			if (fraction >= 0)
			{
				bp += stepy_;
				fraction -= dx;
			}
			x0 += stepx;
			bp += stepx;
			fraction += dy;

			buf[bp] = color;
		}
	}
	else
	{
		fraction = dx - (dy >> 1);
		while (y0 != y1)
		{
			if (fraction >= 0)
			{
				bp += stepx;
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			bp += stepy_;
			fraction += dx;

			buf[bp] = color;
		}
	}
}

void _oink_gfx_hline (OinksiePrivate *priv, uint8_t *buf, int color, int y, int x1, int x2)
{
	int firstx = x1 < x2 ? x1 : x2;
	int endx = x1 > x2 ? x1 : x2;
	int begin;

	if (firstx > priv->screen_width - 1)
		firstx = priv->screen_width - 1;
	else if (firstx < 0)
		firstx = 0;

	if (endx > priv->screen_width - 1)
		endx = priv->screen_width - 1;
	else if (endx < 0)
		endx = 0;

	if (y < 0 || y > priv->screen_height - 1)
		return;
		
	if (firstx == endx)
	{
		_oink_gfx_pixel_set (priv, buf, color, firstx, y);
		return;
	}

	begin = (y * priv->screen_width) + firstx;
	visual_mem_set (buf + begin, color, endx - firstx);
}

void _oink_gfx_vline (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y1, int y2)
{
	int y;
	
	if (y1 < y2)
	{
		for (y = y1; y <= y2; y++)
			_oink_gfx_pixel_set (priv, buf, color, x, y);
	}
	else if (y2 < y1)
	{
		for (y = y2; y <= y1; y++)
			_oink_gfx_pixel_set (priv, buf, color, x, y);
	}
	else
	{
		_oink_gfx_pixel_set (priv, buf, color, x, y1);
	}
}

void _oink_gfx_circle_filled (OinksiePrivate *priv, uint8_t *buf, int color, int size, int x, int y)
{
	int i;
	int sizei = (size * PI) > 0 ? (size * PI) : 1;
	int sini;
	int cosi;
	
	float adder = (float) ((float) OINK_TABLE_LARGE_SIZE / (float) sizei) / 4.00000;
	float tab = 0;

	for (i = 0; i < sizei; i++)
	{
		cosi = _oink_table_sinlarge[(int) tab] * size;
		sini = _oink_table_coslarge[(int) tab] * size;
		
		_oink_gfx_hline (priv, buf, color, cosi + y, -sini + x, sini + x); 
		_oink_gfx_hline (priv, buf, color, -cosi + y, -sini + x, sini + x);
	
		tab += adder;
	}
}

void _oink_gfx_circle (OinksiePrivate *priv, uint8_t *buf, int color, int xsize, int ysize, int x, int y)
{
	int i;
	int size = 300;

	int tabx = OINK_TABLE_NORMAL_SIZE / size;
	int taby = OINK_TABLE_NORMAL_SIZE / size;

	for (i = 0; i < size; i++)
	{
		_oink_gfx_pixel_set (priv, buf, color, (int) (_oink_table_sin[i * tabx] * xsize) + x,
			       (int) (_oink_table_cos[i * taby] * ysize) + y);
	}
}

