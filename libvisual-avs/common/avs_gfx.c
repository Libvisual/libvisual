/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <math.h>

#include <libvisual/libvisual.h>

#include "avs_gfx.h"

#define SGN(a) (a == 0 ? 0 : a < 0 ? -1 : 1)

int avs_gfx_line_non_naieve_floats (VisVideo *video, float x0, float y0, float x1, float y1, VisColor *col)
{
	return avs_gfx_line_non_naieve_ints (video, video->width * x0, video->height * y0, video->width * x1, video->height * y1, col);
}


int avs_gfx_line_non_naieve_ints (VisVideo *video, int x0, int y0, int x1, int y1, VisColor *col)
{
	int u, s, v, d1x, d1y, d2x, d2y, m, n;
	int  i;
	u = x1 - x0;
	v = y1 - y0;
	d1x = SGN (u);
	d1y = SGN (v);
	d2x = SGN (u);
	d2y = 0;
	m = abs (u);
	n = abs (v);
	uint32_t color = col->r << 16 | col->g << 8 | col->b;
	uint32_t *buf = video->pixels;
	int pbpp = video->pitch / 4;

	if (m <= n) {
		d2x = 0;
		d2y = SGN (v);
		m = abs (v);
		n = abs (u);
	}
	
	s = (int) (m / 2);
	for (i = 0; i < m; i++) {
	
		if (x0 > 0 && x0 < video->width && y0 > 0 && y0 < video->height)
			buf[(y0 * pbpp) + x0] = color;

		s += n;
		if (s >= m) {
			s -= m;
			x0 += d1x;
			y0 += d1y;
		} else {
			x0 += d2x;
			y0 += d2y;
		}
	}

	return 0;
}

int avs_gfx_line_floats (VisVideo *video, float x0, float y0, float x1, float y1, VisColor *col)
{
	return avs_gfx_line_ints (video, video->width * x0, video->height * y0, video->width * x1, video->height * y1, col);
}

int avs_gfx_line_ints (VisVideo *video, int x0, int y0, int x1, int y1, VisColor *col)
{
	register int dy = y1 - y0;
	register int dx = x1 - x0;
	register int stepx, stepy, stepy_;
	register int fraction;
	register int bp;
	register int x;
	register int y;
	uint32_t color = col->r << 16 | col->g << 8 | col->b;
	uint32_t *buf = video->pixels;
	int pbpp = video->pitch / 4;

	if (dy < 0) {
		dy = -dy;
		stepy_ = -pbpp;
		stepy = -1;
	} else {
		stepy = 1;
		stepy_ = pbpp;
	}

	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	} else {
		stepx = 1;
	}

	dy <<= 1;
	dx <<= 1;

	bp = x0 + y0 * (pbpp);

	*(buf + bp) = color;

	x = x0;
	y = y0;

	if (dx > dy) {
		fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				bp += stepy_;
				fraction -= dx;
			}
			x0 += stepx;
			bp += stepx;
			fraction += dy;

			buf[bp] = color;
		}
	} else {
		fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
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

	return 0;
}

