/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: misc.c,v 1.5 2006/01/22 13:25:26 synap Exp $
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
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>

#include <common.h>
#include <table.h>
#include <misc.h>

/* FIXME when using tables check radian sanity (!>= 2 PI | % TAB SIZE DUS ?); */
int _oink_line_x_next_rad (int x, int length, float rad)
{
/*	return (_oink_table_sinlarge[(int) (rad * (OINK_TABLE_LARGE_SIZE / (PI * 2)))] * length) + x; */
	return (sin (rad) * length) + x;
}

int _oink_line_y_next_rad (int y, int length, float rad)
{
/*	return (_oink_table_coslarge[(int) (rad * (OINK_TABLE_LARGE_SIZE / (PI * 2)))] * length) + y; */
	return (cos (rad + PI) * length) + y;
}

int _oink_line_length (int x0, int y0, int x1, int y1)
{
	return sqrt (pow (y0 - y1, 2) + pow (x0 - x1, 2));
}

/* FIXME .. this.. could.. be.. a.. smaller.. and especially.. faster.. function :) */
int _oink_line_xory_next_xy (int xory, int length, int x0, int y0, int x1, int y1)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	int fraction;
	int pixdrawn = 0;

	if(dy < 0)
	{
		dy = -dy;
		stepy = -1;
	}
	else
	{
		stepy = 1;
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

	if (length == 0 && xory == 0)
		return x0;
	
	if (length == 0 && xory == 1)
		return y0;

	if (dx > dy)
	{
		fraction = dy - (dx >> 1);

		while (x0 != x1)
		{
			if (fraction >= 0) 
			{
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy; 
			
			pixdrawn++;
			
			if (pixdrawn >= length)
			{
				if (xory == 0)
					return x0;
				else
					return y0;
			}
		}
	} 
	else 
	{
		fraction = dx - (dy >> 1);
		while (y0 != y1) 
		{
			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;

			pixdrawn++;
			
			if (pixdrawn >= length)
			{
				if (xory == 0)
					return x0;
				else
					return y0;
			}
		}
	}

	if (xory == 0)
		return x0;
	else
		return y0;
}

void _oink_pixel_rotate (int *x, int *y, int rot)
{
	int tx = *x;                                                                                                       

	*x = (tx * _oink_table_cos[rot % OINK_TABLE_NORMAL_SIZE]) + (*y * _oink_table_sin[rot % OINK_TABLE_NORMAL_SIZE]);
	*y = (tx * _oink_table_sin[rot % OINK_TABLE_NORMAL_SIZE]) - (*y * _oink_table_cos[rot % OINK_TABLE_NORMAL_SIZE]);
}

