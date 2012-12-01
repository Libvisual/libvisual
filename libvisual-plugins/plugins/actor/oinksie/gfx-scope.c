/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-scope.c,v 1.8 2006/01/22 13:25:26 synap Exp $
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
#include "common.h"
#include "screen.h"
#include "audio.h"
#include "table.h"
#include "misc.h"
#include "gfx-misc.h"
#include "gfx-scope.h"


void _oink_gfx_scope_stereo (OinksiePrivate *priv, uint8_t *buf, int color1, int color2, int height, int space, int rotate)
{
	int adder = priv->screen_width > 512 ? (priv->screen_width - 512) / 2 : 0;

	int x = 0;

	int xc = 0;

	int xc1r = 0;
	int xc2r = 0;

	int xc_old = 0;

	int xc1_oldr = 0;
	int xc2_oldr = 0;

	int y1;
	int y2;

	int y1r;
	int y2r;

	int y1_old;
	int y2_old;

	int y1_oldr;
	int y2_oldr;

	int base = priv->screen_halfheight - (space / 2);
	int base2 = priv->screen_halfheight + (space / 2);

	y1_old = ((base) + (priv->audio.pcm[0][0]) * height);
	y2_old = ((base2) + (priv->audio.pcm[1][0]) * height);

	if (rotate != 0)
	{
		y1_oldr = y1_old - priv->screen_halfheight;
		y2_oldr = y2_old - priv->screen_halfheight;

		_oink_pixel_rotate (&xc1_oldr, &y1_oldr, rotate);
		_oink_pixel_rotate (&xc2_oldr, &y2_oldr, rotate);
	}

	for (x = 1; x < priv->screen_width && x < 512; x++)
	{
		y1 = ((base) + (priv->audio.pcm[0][x >> 1]) * height);
		y2 = ((base2) + (priv->audio.pcm[1][x >> 1]) * height);

		if (y1 < 0)
			y1 = 0;
		else if (y1 > priv->screen_height)
			y1 = priv->screen_height - 1;

		if (y2 < 0)
			y2 = 0;
		else if (y2 > priv->screen_height)
			y2= priv->screen_height - 1;

		xc = x + adder;

		if (rotate != 0)
		{
			xc1r = xc - priv->screen_halfwidth;
			xc2r = xc - priv->screen_halfwidth;

			xc1_oldr = xc_old - priv->screen_halfwidth;
			xc2_oldr = xc_old - priv->screen_halfwidth;

			y1r = y1 - priv->screen_halfheight;
			y2r = y2 - priv->screen_halfheight;

			y1_oldr = y1_old - priv->screen_halfheight;
			y2_oldr = y2_old - priv->screen_halfheight;

			_oink_pixel_rotate (&xc1r, &y1r, rotate);
			_oink_pixel_rotate (&xc2r, &y2r, rotate);

			_oink_pixel_rotate (&xc1_oldr, &y1_oldr, rotate);
			_oink_pixel_rotate (&xc2_oldr, &y2_oldr, rotate);

			_oink_gfx_line (priv, buf, color1, xc1r + priv->screen_halfwidth,
					y1r + priv->screen_halfheight, xc1_oldr + priv->screen_halfwidth,
					y1_oldr + priv->screen_halfheight);
			_oink_gfx_line (priv, buf, color2, xc2r + priv->screen_halfwidth,
					y2r + priv->screen_halfheight, xc2_oldr + priv->screen_halfwidth,
					y2_oldr + priv->screen_halfheight);
		}
		else
		{
			_oink_gfx_vline (priv, buf, color1, xc, y1, y1_old);
			_oink_gfx_vline (priv, buf, color2, xc, y2, y2_old);
		}

		y1_old = y1;
		y2_old = y2;

		xc_old = xc;
	}
}

void _oink_gfx_scope_bulbous (OinksiePrivate *priv, uint8_t *buf, int color, int height)
{
	int adder = priv->screen_width > 512 ? (priv->screen_width - 512) / 2 : 0;
	float tabadd = priv->screen_width > 512 ? ((float) OINK_TABLE_NORMAL_SIZE / 512.00) / 2.00 :
		((float) OINK_TABLE_NORMAL_SIZE / priv->screen_width) / 2.00;
	float tab = 0;
	int x;
	int y;
	int y2;
	int y_old;

	y = ((priv->screen_halfheight) + (((priv->audio.pcm[2][0]) * height) * _oink_table_sin[(int) tab]));

	y_old = y;

	for (x = 0; x < priv->screen_width && x < 512; x++)
	{
		tab += tabadd;

		y = ((priv->screen_halfheight) + (((priv->audio.pcm[2][x >> 1]) * height) * _oink_table_sin[(int) tab]));
		y2 = ((priv->screen_halfheight) + ((((priv->audio.pcm[2][x >> 1]) * height) * _oink_table_sin[(int) tab]) * 1.4));

		if (y < 0)
			y = 0;
		else if (y > priv->screen_height)
			y = priv->screen_height - 1;

		if (y2 < 0)
			y2 = 0;
		else if (y2 > priv->screen_height)
			y2 = priv->screen_height - 1;

		_oink_gfx_vline (priv, buf, color, x + adder, y, y2);
		_oink_gfx_vline (priv, buf, color, x + adder, y, y_old);

		y_old = y;
	}
}

void _oink_gfx_scope_normal (OinksiePrivate *priv, uint8_t *buf, int color, int height)
{
	VisRectangle *rect = NULL;
	int i;
	float fx[512];
	float fy[512];
	int x[512];
	int y[512];
	int yold = priv->screen_halfheight;

	for (i = 0; i < 512; i++) {
		fx[i] = (1.0 / 512.0) * i;
		fy[i] = (1 + sin(i)) * 0.5;
		fy[i] = 0.5 + ((priv->audio.pcm[2][i]) * 0.2);
	}

	rect = visual_rectangle_new (0, 0, priv->screen_width, priv->screen_height);
	visual_rectangle_denormalize_points (rect, fx, fy, x, y, 512);
	visual_rectangle_free (rect);

	for (i = 0; i < 512; i++) {
		_oink_gfx_vline (priv, buf, color, x[i], y[i], yold);
		yold = y[i];
	}
}

void _oink_gfx_scope_circle (OinksiePrivate *priv, uint8_t *buf, int color, int size, int x, int y)
{
	int i;

	int xs1;
	int ys1;

	int xs2;
	int ys2;

	int xcon;
	int ycon;

	int adder = (OINK_TABLE_NORMAL_SIZE - 1) / 50;
	int tab = 0;

	xs2 = (_oink_table_sin[tab] * ((priv->audio.pcm[2][0]) + size)) + x;
	ys2 = (_oink_table_cos[tab] * ((priv->audio.pcm[2][0]) + size)) + y;

	xcon = xs2;
	ycon = ys2;

	for (i = 0; i < 50; i++)
	{
		xs1 = (_oink_table_sin[tab] * ((priv->audio.pcm[2][i >> 1] * 50) + size)) + x;
		ys1 = (_oink_table_cos[tab] * ((priv->audio.pcm[2][i >> 1] * 50) + size)) + y;

		tab += adder;

		_oink_gfx_line (priv, buf, color, xs1, ys1, xs2, ys2);

		xs2 = xs1;
		ys2 = ys1;
	}

	_oink_gfx_line (priv, buf, color, xcon, ycon, xs1, ys1);
}

