/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-analyzer.c,v 1.6 2006/01/22 13:25:26 synap Exp $
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

#include "oinksie.h"
#include "common.h"
#include "screen.h"
#include "audio.h"
#include "gfx-misc.h"
#include "gfx-analyzer.h"

#define ANALYZER_BANDS	32

void _oink_gfx_analyzer_stereo (OinksiePrivate *priv, uint8_t *buf, int color, int y)
{
	int adder = (priv->screen_width - ((priv->screen_halfwidth / ANALYZER_BANDS) * ANALYZER_BANDS * 2)) / 2;

	int i;

	int y1;
	int x1;

	int y2;
	int x2;

	int real_x = 0;
	int real_x_add = (priv->screen_halfwidth / ANALYZER_BANDS);

	y2 = y;
	x2 = 0;

	for (i = ANALYZER_BANDS; i >= 0; i--)
	{
		y1 = (-(priv->audio.freq[0][i] * priv->screen_height) * 2) + y;
		x1 = real_x += real_x_add;

		if (y1 < 0) y1 = 0;

		_oink_gfx_line (priv, buf, color, x1 + adder, y1, x2 + adder, y2);

		y2 = y1;
		x2 = x1;
	}

	for (i = 1; i < ANALYZER_BANDS; i++)
	{
		y1 = (-(priv->audio.freq[1][i] * priv->screen_height) * 2) + y;
		if (y1 == ANALYZER_BANDS - 1)
			y1 = y;

		x1 = real_x += real_x_add;

		if (y1 < 0) y1 = 0;

		_oink_gfx_line (priv, buf, color, x1 + adder, y1, x2 + adder, y2);

		y2 = y1;
		x2 = x1;
	}
}

