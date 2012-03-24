/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: draw_low_level.c,v 1.5 2005/12/20 18:49:12 synap Exp $
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
#include <stdlib.h>
#include <math.h>

#include "draw_low_level.h"
#include "jess.h"

#define MAXE(a,b) ( (a) > (b) ? (b) : (a)) 
/*#define MAXE(a,b) (((int)(a>b))&1)*b+(((int)(!(a>b))&1)*a) */
#define MAX255(a) ((a<=255)*a+(!(a<=255))*255)

void tracer_point_add (JessPrivate *priv, uint8_t * buffer, int x, int y, uint8_t color)
{
	uint8_t *point;
	int calcul;

	/*   goto non;  */

	if (x >= priv->xres2 || x <= -priv->xres2 || y >= priv->yres2 || y <= -priv->yres2)
		return;

	point = buffer + (-y + priv->yres2) * priv->resx + x + priv->xres2;
	calcul = (int)(color) + (int)(*point);
	*point = MAX255(calcul);
	/*  *point = MAXE ((int)color + *point, 255);
	*/
}

void tracer_point_add_32 (JessPrivate *priv, uint8_t * buffer, int x, int y, uint8_t color)
{
	uint8_t *point;
	int calcul;

	if (x >= priv->xres2 || x <= -priv->xres2 || y >= priv->yres2 || y <= -priv->yres2)
		return;

	point = buffer + (-y + priv->yres2) * priv->pitch + ((x + priv->xres2) << 2);
	/* *point = MAXE ((int)(color) + *point, 255); */
	calcul = (int)(color) + (int)(*point);
	*point = MAX255(calcul);

	point++;

	calcul = (int)(color) + (int)(*point);
	*point = MAX255(calcul);

	/*  *point = MAXE ((int)color + *point, 255); */
	point++;
	calcul = (int)(color) + (int)(*point);
	*point = MAX255(calcul);

	/*  *point = MAXE ((int)color + *point, 255); */
}

void cercle (JessPrivate *priv, uint8_t * buffer, int h, int k, int y, uint8_t color)
{
	int x = -1, d = 3 - 2 * y;

	while (x <= y)
	{
		if (d < 0)
		{
			d += 4 * x + 6;
			x++;
		}
		else
		{
			d += 4 * (x - y) + 10;
			x++;
			y--;
		}
		tracer_point_add (priv, buffer, h + x, y + k, color);
		tracer_point_add (priv, buffer, h + y, x + k, color);
		tracer_point_add (priv, buffer, h - y, x + k, color);
		tracer_point_add (priv, buffer, h - x, y + k, color);
		tracer_point_add (priv, buffer, h - x, -y + k, color);
		tracer_point_add (priv, buffer, h - y, -x + k, color);
		tracer_point_add (priv, buffer, h + y, -x + k, color);
		tracer_point_add (priv, buffer, h + x, -y + k, color);
	}
}

void cercle_32 (JessPrivate *priv, uint8_t * buffer, int h, int k, int y, uint8_t color)
{
	int x = -1, d = 3 - 2 * y;

	while (x <= y)
	{
		if (d < 0)
		{
			d += 2* x+6 ;
			x++;
		}
		else
		{
			d += 4 * (x - y)+10 ;
			x++;
			y--;
		}
		tracer_point_add_32 (priv, buffer, h + x, y + k, color);
		tracer_point_add_32 (priv, buffer, h + y, x + k, color);
		tracer_point_add_32 (priv, buffer, h - y, x + k, color);
		tracer_point_add_32 (priv, buffer, h - x, y + k, color);
		tracer_point_add_32 (priv, buffer, h - x, -y + k, color);
		tracer_point_add_32 (priv, buffer, h - y, -x + k, color);
		tracer_point_add_32 (priv, buffer, h + y, -x + k, color);
		tracer_point_add_32 (priv, buffer, h + x, -y + k, color);
	}
}

void boule_random (JessPrivate *priv, uint8_t * buffer, int x, int y, int r, uint8_t color)
{
	int i, j, ecart = (int) visual_random_context_int(priv->rcontext)%5+1;
	j = color;
	if (priv->video == 8)
	{
		for (i = 0; i <= r; i+=ecart)
		{

			cercle (priv, buffer, x, y, i, (int) ((float) (j * j) / (256)));
			j = color - color * ((float) i) / r;
		}
	}
	else
	{
		for (i = 0; i <= r; i+=ecart)
		{

			cercle_32 (priv, buffer, x, y, i, (int) ((float) (j * j) / (256)));
			j = color - color * ((float) i) / r;
		}
	}
}

void boule (JessPrivate *priv, uint8_t * buffer, int x, int y, int r, uint8_t color)
{
	int i, j;

	if (priv->video == 8)
	{
		for (i = r; i >= 0; i--)
		{
			j = color - color * ((float) i) / r;
			/*	cercle (buffer, x, y, i, (int) ((float) (j * j) / (256))); */
			/* Optimisation by Karl Soulabaille. 
			   Remplace un flottant par un entier */
			cercle (priv, buffer, x, y, i, (j*j)>>8);
		}

	}
	else
	{
		for (i = 0; i < r; i++)
		{
			j = color - color * ((float) i) / r;
			/*	cercle_32 (buffer, x, y, i, (int) ((float) (j * j) / (256))); */
			cercle_32 (priv, buffer, x, y, i, (j*j)>>8);
		}
	}
}

void droite (JessPrivate *priv, uint8_t * buffer, int x1, int y1, int x2, int y2, uint8_t color)
{

	int lx, ly, dx, dy;
	int i, j, k;

	lx = abs(x1-x2);
	ly = abs(y1-y2);
	dx = (x1>x2) ? -1 : 1;
	dy = (y1>y2) ? -1 : 1;

	if (priv->video == 8)
	{
		if (lx>ly) 
		{
			for (i=x1,j=y1,k=0;i!=x2;i+=dx,k+=ly) 
			{
				if (k>=lx) 
				{
					k -= lx;
					j += dy;
				}
				tracer_point_add (priv, buffer, i, j, color);
			}
		} 
		else 
		{
			for (i=y1,j=x1,k=0;i!=y2;i+=dy,k+=lx) 
			{
				if (k>=ly) 
				{
					k -= ly;
					j += dx;
				}
				tracer_point_add (priv, buffer, j, i, color);
			}
		}	
	}
	else
	{
		if (lx>ly) 
		{
			for (i=x1,j=y1,k=0;i!=x2;i+=dx,k+=ly) 
			{
				if (k>=lx) 
				{
					k -= lx;
					j += dy;
				}
				tracer_point_add_32 (priv, buffer, i, j, color);
			}
		} 
		else 
		{
			for (i=y1,j=x1,k=0;i!=y2;i+=dy,k+=lx) 
			{
				if (k>=ly) 
				{
					k -= ly;
					j += dx;
				}
				tracer_point_add_32 (priv, buffer, j, i, color);
			}
		}	
	}
}

void ball_init(JessPrivate *priv)
{
	int i,j,k;
	int x,y;
	int ssize;

	if (priv->big_ball != NULL)
		visual_mem_free (priv->big_ball);
	
	ssize = BIG_BALL_SIZE * BIG_BALL_SIZE;
	priv->big_ball = (uint8_t *) visual_mem_malloc0(ssize);

	for (i=0 ; i < BIG_BALL_SIZE ;i++)
	{
		if (priv->big_ball_scale[i] != NULL)
			visual_mem_free (priv->big_ball_scale[i]);

		ssize = (i + 1) * sizeof (int);
		priv->big_ball_scale[i] = (uint32_t *) visual_mem_malloc0(ssize);
	}
	
	for (i=0 ; i < BIG_BALL_SIZE ;i++)
		for (j=0 ; j<i ;j++)
			priv->big_ball_scale[i][j] = (int) floor((float)j*BIG_BALL_SIZE/(i+1));

	/*creation de la grande boulle */
	for (i =0; i< BIG_BALL_SIZE/2; i++) /*pour chaque rayon*/
	{
		for (j=0; j< 2000; j++)
		{
			x = (int) ((float) (BIG_BALL_SIZE/2 + i*0.5*cos((float)j/2000*2*3.1416)) );
			y = (int) ((float) (BIG_BALL_SIZE/2 + i*0.5*sin((float)j/2000*2*3.1416)) );

			k = 255-255 * ((float) (i) / (BIG_BALL_SIZE/2));

			priv->big_ball[ y * BIG_BALL_SIZE + x] = MAX255(3*((k*k)>>9));  
		}
	}
}

void ball(JessPrivate *priv, uint8_t *buffer, int x, int y, int r, uint8_t color)
{
	int i,j,k,d= 2*r;
	uint32_t *pt=priv->big_ball_scale[d];
	int colorc;
	float fcolor = (float) color;

	int a,b,c,e,f,g,h,l;
	if (d>=BIG_BALL_SIZE)
		r = BIG_BALL_SIZE/ 2 - 1;

	if (priv->video == 8) 
	{
		for (j=-r+1; j <= 0; j++)
		{
			k = pt[j+r-1] * BIG_BALL_SIZE;
			a = j + y;
			b = j + x ;
			c = -j + x ;
			e = -j + y  ;
			for (i=-r+1; i <= j; i++)
			{	
				f = i + x ;
				g = -i + x;
				h = i + y;
				l = -i + y;

				colorc = (int) (fcolor/256* (float) priv->big_ball[k + pt[i+r-1]]);

				tracer_point_add(priv, buffer, f   ,a        ,	colorc );
				tracer_point_add(priv, buffer, g     ,a        ,	colorc );  
				tracer_point_add(priv, buffer, f     ,e        ,	colorc ); 
				tracer_point_add(priv, buffer, g     ,e        ,	colorc ); 

				tracer_point_add(priv, buffer, b         ,h     ,	colorc );
				tracer_point_add(priv, buffer, b         ,l     , 	colorc );  
				tracer_point_add(priv, buffer, c         ,h     ,	colorc ); 
				tracer_point_add(priv, buffer, c         ,l     ,	colorc ); 
			}
		}
	}
	else
	{
		for (j=-r+1; j <= 0; j++)
		{
			k = pt[j+r-1] * BIG_BALL_SIZE;
			a = j + y;
			b = j + x ;
			c = -j + x ;
			e = -j + y  ;
			for (i=-r+1; i <= j; i++)
			{	
				f = i + x ;
				g = -i + x;
				h = i + y;
				l = -i + y;

				colorc = (int) (fcolor/256* (float)priv->big_ball[k + pt[i+r-1]]);

				tracer_point_add_32 (priv, buffer, f   ,a        ,	colorc );
				tracer_point_add_32 (priv, buffer, g     ,a        ,	colorc );  
				tracer_point_add_32 (priv, buffer, f     ,e        ,	colorc ); 
				tracer_point_add_32 (priv, buffer, g     ,e        ,	colorc ); 

				tracer_point_add_32 (priv, buffer, b         ,h     ,	colorc );
				tracer_point_add_32 (priv, buffer, b         ,l     , 	colorc );  
				tracer_point_add_32 (priv, buffer, c         ,h     ,	colorc ); 
				tracer_point_add_32 (priv, buffer, c         ,l     ,	colorc ); 
			}
		}
	}
}

void
boule_no_add (uint8_t * buffer, int x, int y, int r, uint8_t color)
{
	int i, j;
	for (i = r; i >= 0; i--)
	{
		j = color - color * ((float) i) / r;
		cercle_no_add (buffer, x, y, i, (j*j)>>8);
	}
}

void cercle_no_add (uint8_t * buffer, int h, int k, int y, uint8_t color)
{
	int x = -1, d = 3 - 2 * y;

	while (x <= y)
	{
		if (d < 0)
		{
			d += 4 * x + 6;
			x++;
		}
		else
		{
			d += 4 * (x - y) + 10;
			x++;
			y--;
		}
		tracer_point_no_add (buffer, h + x, y + k, color);
		tracer_point_no_add (buffer, h + y, x + k, color);
		tracer_point_no_add (buffer, h - y, x + k, color);
		tracer_point_no_add (buffer, h - x, y + k, color);
		tracer_point_no_add (buffer, h - x, -y + k, color);
		tracer_point_no_add (buffer, h - y, -x + k, color);
		tracer_point_no_add (buffer, h + y, -x + k, color);
		tracer_point_no_add (buffer, h + x, -y + k, color);
	}
}

void tracer_point_no_add (uint8_t * buffer, int x, int y, uint8_t color)
{
	uint8_t *point;

	point = buffer + (-y + BIG_BALL_SIZE/2) * BIG_BALL_SIZE + x + BIG_BALL_SIZE/2;
	*point = (int)(color); 
}

