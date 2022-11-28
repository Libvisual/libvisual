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
#include <math.h>

#include "feedback.h"

typedef uint32_t (*transform_function) (JakdawPrivate *priv, int x, int y);

static void init_table(JakdawPrivate *priv);
static void table_store(JakdawPrivate *priv, int x);
static void blur_then(JakdawPrivate *priv, int x, int y, transform_function func);

/* Transforms */
static uint32_t nothing(JakdawPrivate *priv, int x, int y);
static uint32_t scroll(JakdawPrivate *priv, int x, int y);
static uint32_t zoom_rotate(JakdawPrivate *priv, int x, int y);
static uint32_t zoom_ripple(JakdawPrivate *priv, int x, int y);
static uint32_t into_screen(JakdawPrivate *priv, int x, int y);
static uint32_t zoom_ripplenew(JakdawPrivate *priv, int x, int y);

void _jakdaw_feedback_init(JakdawPrivate *priv, int x, int y)
{
	int a, b;

	init_table(priv);
	priv->new_image= visual_mem_malloc0 (priv->xres*priv->yres*4);

	for(b=0;b<priv->yres;b++)
		for(a=0;a<priv->xres;a++)
		{
			switch(priv->zoom_mode)
			{
				case FEEDBACK_ZOOMRIPPLE: blur_then(priv, a,b,zoom_ripple); break;
				case FEEDBACK_BLURONLY: blur_then(priv, a,b,nothing); break;
				case FEEDBACK_ZOOMROTATE: blur_then(priv, a,b,zoom_rotate); break;
				case FEEDBACK_SCROLL: blur_then(priv, a,b,scroll); break;
				case FEEDBACK_INTOSCREEN: blur_then(priv, a,b,into_screen); break;
				case FEEDBACK_NEWRIPPLE: blur_then(priv, a,b,zoom_ripplenew); break;
				default: blur_then(priv, a,b,nothing);
			}
		}
}

void _jakdaw_feedback_reset(JakdawPrivate *priv, int x, int y)
{
	// Check whether res or feedback type have changed here
	_jakdaw_feedback_close(priv);
	_jakdaw_feedback_init(priv, x,y);
}

void _jakdaw_feedback_close(JakdawPrivate *priv)
{
	if (priv->new_image != NULL)
		visual_mem_free (priv->new_image);

	if (priv->table != NULL)
		visual_mem_free (priv->table);
}

void _jakdaw_feedback_render(JakdawPrivate *priv, uint32_t *vscr)
{
	int x,tptr,niptr,a;
	int r,g,b;
	int np, rdr, gdr, bdr;
	
	/* Most feedback effects don't take well to the middle pixel becoming
	 * a bright colour - so we just blank it here. Most effects now rely on
	 * this as a black pixel to be used instead of those that fall off the
	 * edges of the screen. */

	vscr[((priv->yres>>1)*priv->xres)+(priv->xres>>1)]=0;

	tptr=niptr=0;

	np=priv->xres*priv->yres;
	rdr=priv->decay_rate<<2;
	gdr=priv->decay_rate<<10;
	bdr=priv->decay_rate<<18;

	for(x=0;x<np;x++)
	{
		/* I dunno how good a job the C compiler makes at optimizing this
		 * loop. Since this is what the plugin spends pretty much all of
		 * it's time doing it might be worth looking into the possiblilty
		 * of optimizing it further. */

		a=vscr[priv->table[tptr++]];
		r=a&0xff;
		g=a&0xff00;
		b=a&0xff0000;

		a=vscr[priv->table[tptr++]];
		r+=a&0xff;
		g+=a&0xff00;
		b+=a&0xff0000;

		a=vscr[priv->table[tptr++]];
		r+=a&0xff;
		g+=a&0xff00;
		b+=a&0xff0000;

		a=vscr[priv->table[tptr++]];
		r+=a&0xff;
		g+=a&0xff00;
		b+=a&0xff0000;

		r=r>rdr ? r-rdr : 0;
		g=g>gdr ? g-gdr : 0;
		b=b>bdr ? b-bdr : 0;
	
		a=(r&0x3fc)|(g&0x3fc00)|(b&0x3fc0000);
		a>>=2;
		
		priv->new_image[niptr++]=a;

	}
	
	visual_mem_copy(vscr, priv->new_image, priv->xres*priv->yres*4);
}

static void init_table(JakdawPrivate *priv)
{
	priv->table=visual_mem_malloc0(priv->xres*priv->yres*sizeof(uint32_t)*4); // 4 points / pixel
	priv->tableptr=0;
}

static void table_store(JakdawPrivate *priv, int x)
{
	priv->table[priv->tableptr++]=x;
}

static void blur_then(JakdawPrivate *priv, int x, int y, transform_function func)
{
	uint32_t a;
	
	a=x+1<priv->xres ? x+1 : x;
	table_store(priv, func(priv,a,y));
	a=x-1<0 ? 0 : x-1;
	table_store(priv, func(priv,a,y));
	a=y+1<priv->yres ? y+1 : y;
	table_store(priv, func(priv,x,a));
	a=y-1<0 ? 0 : y-1;
	table_store(priv, func(priv,x,a));
	
	return;
}

// Transform functions ---------------------------------------------------------

static uint32_t nothing(JakdawPrivate *priv, int x, int y)
{
	return (y*priv->xres)+x;
}

static uint32_t scroll(JakdawPrivate *priv, int x, int y)
{
	int retval;

	if(y+6<=priv->yres)
		retval = ((y+6)*priv->xres)+x;
	else
		retval = ((priv->yres>>1)*priv->xres)+(priv->xres>>1);

	if (retval > priv->xres * priv->yres)
		return priv->xres * priv->yres;
	else if (retval < 0)
		return 0;
	else
		return retval;
}

static uint32_t zoom_rotate(JakdawPrivate *priv, int x, int y)
{
	double ang=5.0*(3.14/180.0);
	int nx, ny;
	
	x-=(priv->xres>>1);
	y-=(priv->yres>>1);
	
	nx=x*cos(ang) + y*sin(ang);
	ny=y*cos(ang) - x*sin(ang);
	nx/=1.2;
	ny/=1.2;

	nx+=(priv->xres>>1);
	ny+=(priv->yres>>1);
		
	if(nx>=priv->xres || nx<0 || ny>=priv->yres || ny<0)
	{
		nx=0; ny=0;
	}

	if(nx<0 || nx>=priv->xres || ny<0 || ny>=priv->yres) { nx=priv->xres>>1; ny=priv->yres>>1; }
	
	return (ny*priv->xres)+nx;
	
}

static uint32_t zoom_ripple(JakdawPrivate *priv, int x, int y)
{
	double dist;
	int nx, ny;
	
	x-=(priv->xres>>1);
	y-=(priv->yres>>1);
		
	dist=sqrt((x*x)+(y*y));
	
	dist*=3.14*priv->zoom_ripplesize/sqrt((priv->xres*priv->xres)+(priv->yres*priv->yres));
	
	nx=x*(priv->zoom_zoomfact+(priv->zoom_ripplefact*sin(dist)));
	ny=y*(priv->zoom_zoomfact+(priv->zoom_ripplefact*sin(dist)));
	
	nx+=(priv->xres>>1);
	ny+=(priv->yres>>1);

	if(nx<0 || nx>=priv->xres || ny<0 || ny>=priv->yres) { nx=priv->xres>>1; ny=priv->yres>>1; }

	return (ny*priv->xres)+nx;
}

static uint32_t zoom_ripplenew(JakdawPrivate *priv, int x, int y)
{
	double dist;
	int nx, ny;
	
	x-=(priv->xres>>1);
	y-=(priv->yres>>1);
	
	dist=sqrt((x*x)+(y*y));
	dist*=3.14*priv->zoom_ripplesize/sqrt((priv->xres*priv->xres)+(priv->yres*priv->yres));
	
	// FIXME - implement the rest of this!
	(void)dist;  // just for -Wunused-but-set-variable

	nx=x;
	ny=y;
	
	nx+=(priv->xres>>1);
	ny+=(priv->yres>>1);
	
	if(nx<0 || nx>=priv->xres || ny<0 || ny>=priv->yres) { nx=priv->xres>>1; ny=priv->yres>>1; }

	return (ny*priv->xres)+nx;
}

static uint32_t into_screen(JakdawPrivate *priv, int x, int y)
{
	double xfact, yfact;
	int nx,ny;
	
	x-=(priv->xres>>1);
	xfact=1.0+(0.05*((priv->yres-y)/(double)priv->yres));
	yfact=1.05+(0.05*(y/(double)priv->yres));
	
	nx=x*xfact;
	ny=y*yfact;
	
	nx+=(priv->xres>>1);

	if(nx<0 || nx>=priv->xres || ny<0 || ny>=priv->yres) { nx=priv->xres>>1; ny=priv->yres>>1; }

	return (ny*priv->xres)+nx;
}

