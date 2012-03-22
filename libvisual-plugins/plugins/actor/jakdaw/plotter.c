/* 
 *  Jakdaw's XMMS Plugin.
 *  Copyright (C) 1999, 2000, Christopher Wilson.
 *
 *  Email: <Jakdaw@usa.net> 
 *  Project Homepage: <http://www.jakdaw.ucam.org/xmms/>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 */
#include <stdlib.h>
#include <math.h>

#include "plotter.h"

static void vert_line(JakdawPrivate *priv, int x, int a, int b, uint32_t col, uint32_t *vscr);

void _jakdaw_plotter_draw(JakdawPrivate *priv, float *pcm_data, float *freq_data, uint32_t *vscr)
{
	int x, y;
	int oy;
	uint32_t col;

	switch (priv->plotter_colortype)
	{
		case PLOTTER_COLOUR_SOLID: col=priv->plotter_scopecolor; break;
		case PLOTTER_COLOUR_RANDOM: col=visual_random_context_int (priv->rcontext); break;
		case PLOTTER_COLOUR_MUSICTRIG:
		default:
					    {
						    float d;
						    int c;
						    d=0;
						    for(c=0;c<16;c++)
							    d=d+freq_data[c];
						    col=(int)((double) d*(256.0 * 16.0));

						    d=0;
						    for(c=16;c<108;c++)
							    d=d+freq_data[c];
						    col|=((int)((double) d*(256.0 * 64.0)))<<8;

						    d=0;
						    for(c=108;c<255;c++)
							    d=d+freq_data[c];
						    col|=((int)((double) d*(256.0 * 128.0)))<<16;

					    }
	}

	oy = (priv->yres / 2) + ((pcm_data[0] * priv->plotter_amplitude) * (priv->yres / 2));

	oy=oy<0 ? 0 : oy>=priv->yres ? priv->yres -1 : oy;

	for(x=0;x<priv->xres;x++)
	{
		y = (priv->yres / 2) + ((pcm_data[x % 512] * priv->plotter_amplitude) * (priv->yres / 2));

		if(y<0) y=0;
		if(y>=priv->yres) y=priv->yres-1;
		if(priv->plotter_scopetype==PLOTTER_SCOPE_LINES)
		{
			vert_line(priv, x, oy, y, col, vscr);
			oy=y;
		}
		else if(priv->plotter_scopetype==PLOTTER_SCOPE_DOTS)
		{
			if(x>0 && x<priv->xres && y>0 && y<priv->yres)
				vscr[(y*priv->xres)+x]=col;
		}
		else if(priv->plotter_scopetype==PLOTTER_SCOPE_SOLID)
		{
			vert_line(priv, x, (priv->yres>>1), y, col, vscr);
		}
	}
}

static void vert_line(JakdawPrivate *priv, int x,int a, int b, uint32_t col, uint32_t *vscr)
{
	int y, ptr;

	if(b<a)
	{
		y=a; a=b; b=y;
	}

	if(a<0 || a>=priv->yres || b<0 || b>=priv->yres)
		return;

	ptr=(a*priv->xres)+x;

	for(y=a;y<=b;y++)
	{
		vscr[ptr]=col;
		ptr+=priv->xres;
	}
}
