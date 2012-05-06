/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Pascal Brochart <p.brochart@libertysurf.fr>
 *
 * Authors: Pascal Brochart <p.brochart@libertysurf.fr>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: plazma.c,v 1.5 2005-12-20 18:49:14 synap Exp $
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
#include <stdio.h>
#include <stdlib.h>

#include "actor_plazma.h"
#include "plazma.h"

#ifndef PI
#define PI	3.14159265358979323846
#endif

#define RESFACTXF(par1) ( (float) priv->width*(par1)/video_width )
#define RESFACTYF(par1) ( (float) priv->height*(par1)/video_height )

/* New prototypes */
static void do_plasma(PlazmaPrivate *priv, double x1, double y1, double x2, double y2, unsigned char *t);
static void what_display(PlazmaPrivate *priv);
static void plazma_create(PlazmaPrivate *priv);
static void aff_pixel(PlazmaPrivate *priv, int x, int y, int colpix);
static void cercle (PlazmaPrivate *priv, int h, int k, int y, uint8_t colcer);
static void droite (PlazmaPrivate *priv, int x1, int y1, int x2, int y2, uint8_t color);
static void grille_3d (PlazmaPrivate *priv, float alpha, float beta, float gamma, int persp, int dist_cam, int col_grille_3d, int cx, int cy);
static void cercle_3d (PlazmaPrivate *priv, float alpha, float beta, float gamma, int persp, int dist_cam, int col_grille_3d, int cx, int cy);
static void do_radial_wave(PlazmaPrivate *priv);
static void do_tourni_spec(PlazmaPrivate *priv);
static void do_lines(PlazmaPrivate *priv);
static void do_grille_3d(PlazmaPrivate *priv);
static void do_spectrum(PlazmaPrivate *priv);
static void do_cercle_3d(PlazmaPrivate *priv);

void _plazma_run (PlazmaPrivate *priv)
{
	if (priv->state == priv->old_state)
		priv->state++;
	priv->old_state = priv->state;

	switch (priv->effect) {
		case 0: 
			do_plasma(priv,
					(sin(((double)priv->state)*priv->R[0])+1)/2,
					(sin(((double)priv->state)*priv->R[1])+1)/2,
					(cos(((double)priv->state)*priv->R[2])+1)/2,
					(cos(((double)priv->state)*priv->R[3])+1)/2, priv->tmem);
			break;
		case 1:
			do_plasma(priv,
					(sin(((double)priv->state)*priv->R[0])+1)/2,
					(sin(((double)priv->state)*priv->R[1])+1)/2,
					(cos(((double)priv->state)*priv->R[2])+1)/2,
					(cos(((double)priv->state)*priv->R[3])+1)/2, priv->tmem1);
			break;
	}

	what_display(priv);
}

void _plazma_init(PlazmaPrivate *priv)
{
	int c, i;
	float halfPI  = (float)PI/2;
	
	for (c=0; c<4; c++)
		priv->R[c]=((double)(cos(c)*120))/5000;

	priv->tmem = visual_mem_malloc0 (priv->tabley*priv->tablex);
	priv->tmem1 = visual_mem_malloc0 (priv->tabley*priv->tablex);
	
	priv->cosw.i = priv->width;
	priv->cosw.f = visual_mem_malloc0(sizeof(float)*priv->width);

	for (i=0; i<priv->width;i++) 
		priv->cosw.f[i] = cos((float)i/priv->width*PI+halfPI);

	priv->sinw.i = priv->width;
	priv->sinw.f = visual_mem_malloc0(sizeof(float)*priv->width);

	for (i=0; i<priv->width;i++) 
		priv->sinw.f[i] = sin((float)i/priv->width*PI+halfPI);
	
	plazma_create(priv);
	_plazma_change_effect(priv);	
}

void _plazma_cleanup(PlazmaPrivate *priv)
{
	if (priv->tmem != NULL)
		visual_mem_free (priv->tmem);

	if (priv->tmem1 != NULL)
		visual_mem_free (priv->tmem1);

	if (priv->cosw.f != NULL)
		visual_mem_free (priv->cosw.f);

	if (priv->sinw.f != NULL)
		visual_mem_free (priv->sinw.f);
}

void _plazma_change_effect(PlazmaPrivate *priv)
{
	int c;
	VisColor *colors = visual_palette_get_colors (priv->colors);

	switch (priv->effect)	{
		case 0:
			for (c=0 ; c<256; c++) {
				colors[c].r = (sin(((double)c)/256*6*PI+(sin(445)))+1)*127;
				colors[c].g = (sin(((double)c)/256*6*PI+(sin(561)))+1)*127;
				colors[c].b = (cos(((double)c)/256*6*PI+(sin(278)))+1)*127;
			}
			break;
		case 1:
			for (c=0 ; c<256; c++) {
				colors[c].r = (sin(((double)c)/256*6*PI+(sin(c/4)))+1)*127;
				colors[c].g = (sin(((double)c)/256*6*PI+(sin(561)))+1)*127;
				colors[c].b = (cos(((double)c)/256*6*PI+(sin(278)))+1)*127;
			}
			break;
	}
}

static void do_plasma(PlazmaPrivate *priv, double x1, double y1,
                                           double x2, double y2,
					   unsigned char *t)
{
	unsigned int	X1=x1*(priv->tablex/2), Y1=y1*(priv->tabley/2),
			X2=x2*(priv->tablex/2), Y2=y2*(priv->tabley/2), y;
	unsigned char 	*t1=t+X1+Y1*priv->tablex, *t2=t+X2+Y2*priv->tablex;

	int pitch = visual_video_get_pitch (priv->video);

	for (y=0; y<priv->height; y++) {
		unsigned char*tmp = priv->pixel + y * pitch;
		unsigned int t = y*priv->tablex, tmax=t+priv->width; 
		for (; t<tmax; t++, tmp++)
			tmp[0]=t1[t]+t2[t];
	}
}

static void plazma_create(PlazmaPrivate *priv)
{		
    	int y;
  	for (y=0 ; y<priv->tabley ; y++) {
   	  	int x;      		
		for (x=0 ; x<priv->tablex ; x++) {
			double tmp = (((double)((x-(priv->tablex/2))*(x-(priv->tablex/2))+(y-(priv->tablex/2))*(y-(priv->tablex/2))))*(PI/(priv->tablex*priv->tablex+priv->tabley*priv->tabley)));
			priv->tmem[y*priv->tablex+x] = tan(tan(tmp*1500))+(tan(tmp)*40)+sqrt(exp(tmp)*10)+sin(tmp*10);
			priv->tmem1[y*priv->tablex+x] = sin(exp(tmp*35)*120)+sqrt(tmp*14)+log(exp(tmp*8)+cos(tmp*tmp))+tan(exp(tmp*40)*150)/6;
		}
   	}
}

static void aff_pixel(PlazmaPrivate *priv, int x, int y, int colpix)
{
	unsigned char*point, *old_point;
	int pitch = visual_video_get_pitch (priv->video);

	if (x<0 || x>(priv->width-1) || y<0 || y>(priv->height-1))
		return;
	point = priv->pixel+y*pitch;
	old_point = point;
	point[x] = old_point[x] | colpix;
	if (!priv->use_3d)
		point[x] = colpix;
}

static void cercle (PlazmaPrivate *priv, int h, int k, int y, uint8_t colcer)
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
		aff_pixel (priv, h + x, y + k, colcer);
		aff_pixel (priv, h + y, x + k, colcer);
		aff_pixel (priv, h - y, x + k, colcer);
		aff_pixel (priv, h - x, y + k, colcer);
		aff_pixel (priv, h - x, -y + k, colcer);
		aff_pixel (priv, h - y, -x + k, colcer);
		aff_pixel (priv, h + y, -x + k, colcer);
		aff_pixel (priv, h + x, -y + k, colcer);
	}
}

static void droite (PlazmaPrivate *priv, int x1, int y1, int x2, int y2, uint8_t color)
{
	int lx, ly, dx, dy;
	int i, j, k;
	lx = abs(x1-x2);
	ly = abs(y1-y2);
	dx = (x1>x2) ? -1 : 1;
	dy = (y1>y2) ? -1 : 1;
	
	if (lx>ly) 
	{
		for (i=x1,j=y1,k=0;i!=x2;i+=dx,k+=ly) 
		{
			if (k>=lx) 
			{
				k -= lx;
				j += dy;
			}
			aff_pixel (priv, i, j, color);
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
		aff_pixel (priv, j, i, color);
		}
	}	
}

static void rotation_3d (float *x, float *y, float *z, float alpha, float beta, float gamma)
{
	float nx, ny;
	float cosga = cos (gamma), singa=sin (gamma), cosal=cos (alpha), sinal=sin (alpha);
	nx = cosga * *x - singa * *y;
	ny = cosal * (singa * *x + cosga * *y) - sinal * *z;
	*z = sinal * (singa * *x + cosga * *y) + cosal * *z;
	*x = nx;
	*y = ny;
}

static void perspective (float *x, float *y, float *z, int persp, int dist_cam)
{
	float foc_cam = 100;
	*x = *x * (foc_cam + persp) / (dist_cam + persp - *z);
	*y = *y * (foc_cam + persp) / (dist_cam + persp - *z);
}

static void grille_3d (PlazmaPrivate *priv, float alpha, float beta, float gamma, int persp, int dist_cam, int col_grille_3d, int cx, int cy)
{
	float x, y, z;
	int16_t ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;
	float ampli_grille = 20;

	int video_width	 = visual_video_get_width  (priv->video);
	int video_height = visual_video_get_height (priv->video);

	nb_x = 32;
	nb_y = 32;

	if (priv->spectrum == 0 && priv->lines == 1)
		ampli_grille = 30;

	for (i = 0; i < nb_x; i++)
	{
		for (j = 0; j < nb_y; j++)
		{
			x = RESFACTXF ((i - ((float) nb_x) / 2) * 10);
			y = RESFACTYF ((j - ((float) nb_y) / 2) * 10);
			z = RESFACTXF ((float) (priv->pcm_buffer[(i + (nb_x/3)*j) % 1024]) * (priv->height / ampli_grille));
			rotation_3d (&x, &y, &z, alpha, beta, gamma);
			perspective (&x, &y, &z, persp, dist_cam);
			ix = (int) x;
			iy = (int) y;
			if (j != 0)	{
				droite (priv, ix+cx, iy+cy, ax+cx, ay+cy, col_grille_3d);
			}
			ax = ix;
			ay = iy;
		}
	}
}

static void cercle_3d (PlazmaPrivate *priv, float alpha, float beta, float gamma, int persp, int dist_cam, int col_grille_3d, int cx, int cy)
{
	float x, y, z;
	int16_t ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;

	int video_width	 = visual_video_get_width  (priv->video);
	int video_height = visual_video_get_height (priv->video);

	nb_x = 16;
	nb_y = 16;

	for (i = 0; i < nb_x; i++)
	{
		for (j = 0; j < nb_y; j++)
		{
			x = RESFACTXF ((i - ((float) nb_x) / 2) * 30);
			y = RESFACTYF ((j - ((float) nb_y) / 2) * 30);
			z = RESFACTXF (-((float) (priv->render_buffer[i + (nb_x/32)*j])*32000));

			rotation_3d (&x, &y, &z, alpha, beta, gamma);
			perspective (&x, &y, &z, persp, dist_cam);
			ix = (int) x;
			iy = (int) y;
			if (j != 0)	{
				cercle (priv, ix+cx, iy+cy, 3, col_grille_3d);
				cercle (priv, ax+cx, ay+cy, 3, col_grille_3d);
			}
			ax = ix;
			ay = iy;
		}
	}
}

static void do_radial_wave(PlazmaPrivate *priv)
{
	int i, halfheight, halfwidth, shift, col_fleur;
	float y1, old_y1, k, opt, opt_old;

	y1 = priv->pcm_buffer[0]*82;
	col_fleur = 74;
	shift = priv->height/3.1;

	if (priv->effect)
		col_fleur = 72;

	halfheight = priv->height >> 1;
	halfwidth  = priv->width >> 1;
	for (i=1 ; i<priv->width ; i++) {
		old_y1 = y1;
		y1 = priv->pcm_buffer[i % 1024]*82;
		if (y1<-50)
			y1 = y1/2;
		for (k=1 ; k<2.6 ; k+=0.4)	{
			opt = shift+(y1/k);
			opt_old = shift+(old_y1/k);
			droite (priv,
				halfwidth + priv->cosw.f[i-1] * opt_old,
				halfheight + priv->sinw.f[i-1] * opt_old,
				halfwidth + priv->cosw.f[i] * opt,
				halfheight + priv->sinw.f[i] * opt,
				col_fleur);
			droite (priv,
				halfwidth - priv->cosw.f[i-1] * opt_old,
				halfheight + priv->sinw.f[i-1] * opt_old,
				halfwidth - priv->cosw.f[i] * opt,
				halfheight + priv->sinw.f[i] * opt,
				col_fleur);
		}
	}
	for (k=1 ; k<2.6 ; k+=0.4)	{
			opt = shift+(y1/k);
		droite (priv,
			halfwidth  + priv->cosw.f[1] * opt,
			halfheight - priv->sinw.f[1] * opt,
			halfwidth  - priv->cosw.f[1] * opt,
			halfheight - priv->sinw.f[1] * opt,
			col_fleur);
		droite (priv,
			halfwidth  - priv->cosw.f[1] * opt,
			halfheight - priv->sinw.f[1] * opt,
			halfwidth  + priv->cosw.f[1] * opt,
			halfheight - priv->sinw.f[1] * opt,
			col_fleur);
	}
}

static void do_tourni_spec(PlazmaPrivate *priv)
{
	int i, j, k, l, halfheight, halfwidth, col_tourni, amplitude;
	float v, vr, x, y;
	v = 0.1;
	vr = 0.00001;
	col_tourni = 52;
	amplitude = 44;
	j = 20;
	k = priv->k_put;
	l = 1000;
	halfheight = priv->height >> 1;
	halfwidth  = priv->width >> 1;
	if (priv->effect)
		col_tourni = 99;
	if (priv->lines == 0 && priv->spectrum == 1)	{
		j = 4;
		l = 2500;
		amplitude = 88;
	}
	for (i=0 ; i<l ; i++ , k++) {
		x = cos(k/(v*priv->rot_tourni))*amplitude+(priv->render_buffer[i>>2]/j);
		y = sin(k/(v*0.6))*amplitude+(priv->render_buffer[i>>2]);
		aff_pixel (priv, x*cos(k*vr)+y*sin(k*vr)+halfwidth, x*sin(k*vr)-y*cos(k*vr)+halfheight, col_tourni);
	}
	priv->k_put = k;
}

static void do_lines(PlazmaPrivate *priv)
{
	int lc, lc2lc, reg_lines;
	float larg_lines;
	float pluseffect, coul_lines;
	if (priv->lines == 1 && priv->spectrum == 0)	{
		reg_lines = (priv->height/2)-3;
		larg_lines = 154;
	}
	else	{
		reg_lines = 77;
		larg_lines = 88;
	}
	for (lc=0 ; lc<priv->width ; lc++)	{
		for (lc2lc=0 ; lc2lc<7 ; lc2lc++)	{
			coul_lines = 55;
			pluseffect = 0;	
			if (priv->effect)	{
				coul_lines = 227.4;
				pluseffect = 0.5;
			}
			if (lc2lc==0 || lc2lc==1 || lc2lc==4 || lc2lc==5 || lc2lc==6)
				coul_lines = 400/(3.4+pluseffect);
			aff_pixel(priv, lc, reg_lines+(priv->pcm_buffer[lc % 1024]*larg_lines)+lc2lc, coul_lines);
		}
	}
}

static void do_spectrum(PlazmaPrivate *priv)
{
	int lc, lc2lc, lc2lc2lc, l_spect, toujours=0, coul_lines;
	int i=0, too_hard=64, first_decr=FALSE;
	float precision = 71;
	if (priv->lines == 0 && priv->spectrum == 1)	{
		priv->val_maxi = priv->height - 150;
		l_spect = 14;
	}
	else	{
		priv->val_maxi = priv->height - 280;
		l_spect = 32;
	}
	for (lc=1 ; lc<priv->width ; lc+=10 , toujours++, too_hard-=i)	{
		for (lc2lc=0; lc2lc<7 ; lc2lc++, precision+=0.08)	{
			for (lc2lc2lc=0 ; lc2lc2lc<7 ; lc2lc2lc++)	{
				coul_lines=7.4+priv->chcol0+precision+(lc2lc*2);
				if (lc2lc2lc==0 || lc2lc2lc==1 || lc2lc2lc==5 || lc2lc2lc==6)
					coul_lines = priv->chcol0+(lc2lc*2);
				aff_pixel(priv, lc+lc2lc, (priv->height - 10) -((int)(((priv->render_buffer[toujours] * 32000)/(l_spect+too_hard)))&priv->val_maxi)+lc2lc2lc, coul_lines);
			}
		}
		if (!too_hard)
			too_hard = 2;
		if (lc<33)	{
			i = 6;
			first_decr = TRUE;
		}
		else	{
			if (first_decr)	{
				first_decr = FALSE;
				too_hard -= 10;
			}
			i = 2;
		}
	}
}

static void do_grille_3d(PlazmaPrivate *priv)
{
	int dis_col = 322;
	if (priv->effect)
		dis_col = 72;
	if (priv->spectrum)
		grille_3d (priv, 1.45, 0, priv->compt_grille_3d/30, 200, 110, dis_col, priv->width / 2, priv->height / 10);
	else
		grille_3d (priv, 1.1, 0, priv->compt_grille_3d/30, 200, 20, dis_col, priv->width / 2, priv->height / 5);
	priv->compt_grille_3d += 0.1;
}

static void do_cercle_3d(PlazmaPrivate *priv)
{
	int dis_col = 382;
	if (priv->effect)
		dis_col = 99;
	if (priv->lines)
		cercle_3d (priv, 3.6, 0, priv->compt_cercle_3d/30, 200, 80, dis_col, priv->width / 2, priv->height);
	else
		cercle_3d (priv, 3.2, 0, priv->compt_cercle_3d/30, 200, 80, dis_col, priv->width / 2, priv->height / 2);
	priv->compt_cercle_3d -= 0.1;
}

static void what_display(PlazmaPrivate *priv)
{
	if (priv->use_3d)	{
		if (priv->options)	{
			if (priv->lines)
				do_radial_wave(priv);
			if (priv->spectrum)
				do_tourni_spec(priv);
		}
		else	{
			if (priv->lines)
				do_grille_3d(priv);
			if (priv->spectrum)
				do_cercle_3d(priv);
		}
	}
	else	{
		if (priv->lines)
			do_lines(priv);
		if (priv->spectrum)
			do_spectrum(priv);
	}
}

