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
static int no_of_shifts(int val);
static int p2(int val);

void _jakdaw_plotter_init(JakdawPrivate *priv, int x, int y)
{
	double delta, acc, scalefact;
	int a;
	
	priv->xlat_table=visual_mem_malloc0(priv->xres*sizeof(int));

	delta=512.0 / (double) priv->xres;
	acc=0;

	for(a=0;a<priv->xres;a++)
	{
		priv->xlat_table[a]=(int) acc;
		acc+=delta;
	}
	
	priv->shifts=no_of_shifts((priv->yres*priv->plotter_amplitude)/100);
	scalefact=((double)(priv->yres*priv->plotter_amplitude)/100.0)/(double)p2(16-priv->shifts);

	priv->amplitude_table=visual_mem_malloc0(p2(16-priv->shifts)*sizeof(int));

	for(a=0;a<p2(16-priv->shifts);a++)
	{
		priv->amplitude_table[a]=(a*scalefact)+((int)(priv->yres-(p2(16-priv->shifts)*scalefact))>>1);
	}
			
}

void _jakdaw_plotter_reset(JakdawPrivate *priv, int x, int y)
{
	_jakdaw_plotter_close(priv);
	_jakdaw_plotter_init(priv, x,y);
}

void _jakdaw_plotter_close(JakdawPrivate *priv)
{
	visual_mem_free(priv->xlat_table);
	visual_mem_free(priv->amplitude_table);
}

void _jakdaw_plotter_draw(JakdawPrivate *priv, int16_t pcm_data[3][512], int16_t freq_data[3][256], uint32_t *vscr)
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
				int c,d;					/* PLAY WITH THESE VALUES */
				d=0;
				for(c=0;c<16;c++)
					d=d+freq_data[2][c];
					d>>=8;
				col=(int)((double) d*(255.0/16.0));
		
				d=0;
				for(c=16;c<108;c++)
					d=d+freq_data[2][c];
				d>>=8;
				col|=((int)((double) d*(255.0/72.0)))<<8;
		
				d=0;
				for(c=108;c<255;c++)
					d=d+freq_data[2][c];
				d>>=8;
				col|=((int)((double) d*(255.0/144.0)))<<16;
		
			}		
	}

	oy=priv->amplitude_table[(p2(16-priv->shifts)>>1) + (pcm_data[2][priv->xlat_table[0]] >> priv->shifts)];
	oy=oy<0 ? 0 : oy>=priv->yres ? priv->yres -1 : oy;

	for(x=0;x<priv->xres;x++)
	{
		y=priv->amplitude_table[(p2(16-priv->shifts)>>1) + (pcm_data[2][priv->xlat_table[x]] >> priv->shifts)];
		
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

// Calculate maximum no of right shifts that can be applied to a sample so
// that val can still be represented.

static int no_of_shifts(int val)
{

	int a=1, b=0;
	
	while(a<val)
	{
		b++;
		a*=2;
	}

	return 16-b;
}

// Calc a power of 2

static int p2(int val)
{
	int a=1;
	
	for(;val>0;val--)
		a*=2;
		
	return a;
}
