/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: draw.c,v 1.6 2006/01/18 19:45:23 synap Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "def.h"
#include "draw.h"
#include "struct.h"
#include "analyser_struct.h"
#include "pal.h"
#include "draw_low_level.h"
#include "projection.h"
#include "jess.h"

/*******************************************************************/
/**************** ANALYSER *****************************************/
/*******************************************************************/

void analyser(JessPrivate *priv, uint8_t * buffer)
{
	int i ;

	droite (priv, buffer, -priv->xres2, 0, -priv->xres2+10, 0, 30);

	for (i = -priv->xres2 ; i<-priv->xres2+5 ; i++)
	{
		droite (priv, buffer, i, 0, i,(int)(priv->lys.E_moyen*2000) , 250);
		droite (priv, buffer, i+5, 0, i+5, (int)(priv->lys.dEdt_moyen*25000) , 230);
		droite (priv, buffer, i+10, 0, i+10, (int)(priv->lys.dEdt*25000) , 200);
	}
}

/*******************************************************************/
/**************** FUSEES *******************************************/
/*******************************************************************/

void fusee(JessPrivate *priv, uint8_t * buffer, int new)
{
	int i;
	int x=0,y=0;
	float factor ;

	if (new == NEW) /* on creer une nouvelle fusee*/
	{
		i=0;
		while (priv->life[i] > 0)
		{
			i++;
			if (i == FUSEE_MAX)
				return;
		}
		priv->xi[i] = visual_random_context_int(priv->rcontext) % priv->resx - priv->xres2;
		priv->yi[i] = -visual_random_context_int(priv->rcontext) % priv->yres2;
		priv->life[i] = FUSEE_VIE; /* on donne la vie max*/
	}
	else  /* on gere les autres */
	{
		for(i = 0 ; i< FUSEE_MAX; i++)
		{
			if (priv->life[i] > 0) /* si elle est vivante */
			{
				factor = priv->life[i] / FUSEE_VIE;
				x = priv->xi[i] * factor;
				y = priv->yi[i] * factor;
				priv->life[i]--;
				ball (priv, buffer, x, y, (int) FUSEE_RAYON * factor, FUSEE_COLOR);
			}
		}
	}
}

/*******************************************************************/
/**************** SUPER SPECTRAL ***********************************/
/*******************************************************************/

void super_spectral_balls(JessPrivate *priv, uint8_t * buffer)
{
	int i,j;

	float dt = priv->conteur.dt;

	int resx = priv->resx;
	int resy = priv->resy;
	int xres2 = priv->xres2;
	int yres2 = priv->yres2;

	for(i=0; i < 256 ;i++) /* pour chaque frequence */
	{
		if (priv->lys.dbeat[i] == NEW) /*creation d'une nouvelle ligne */
		{
			priv->lys.dbeat[i] = MANAGE;
			j=0;
			while (priv->lifev[i][j] > 0)
			{
				j++;
				if (j == LINE_MAX+1)
					goto quit; /* pas de place desole */
			}

			/* initialisation de la ligne */
			priv->lifev[i][j] = LINE_VIE;
			priv->vx[i][j] = RESFACTXF( 0.025*((float) i - 128.0) * 32 +0*
					(1-visual_random_context_int(priv->rcontext)/(float)UINT32_MAX) );
			priv->vy[i][j] = RESFACTYF( (10+i)*i*priv->lys.Ed_moyen[i]*5000*(   (float)j+1)/4);
			priv->x[i][j] = RESFACTXF( 2*(i - 128) ) +((float)j*(i-128))/2;
			priv->y[i][j] = 0*RESFACTXF( yres2/2-(float)((i-128)*(i-128))/256) -20*j ;
		}
quit:; /* pas de nouvelle ligne */

     		for (j=0 ; j < LINE_MAX ; j++)
		{
			if (priv->lifev[i][j] > 0)
			{

				priv->vy[i][j] += -0.5*dt*1024;
				priv->x[i][j] += priv->vx[i][j] * dt;
				priv->y[i][j] += priv->vy[i][j] * dt;

				boule(priv, buffer, (int) priv->x[i][j], (int) priv->y[i][j], 5,
						250*(LINE_VIE-priv->lifev[i][j])/LINE_VIE); 

				if ((priv->y[i][j] < resy) &&(priv->y[i][j] > -resy) ) /*oui !*/
				{
					if (i>128)
						droite (priv, buffer, priv->xres2, (int) priv->y[i][j] / 32,
								(int) priv->x[i][j], (int) priv->y[i][j], 50 * 
								(LINE_VIE-priv->lifev[i][j]) / LINE_VIE);
					else
						droite (priv, buffer, -xres2, (int) priv->y[i][j] / 32,
								(int) priv->x[i][j], (int) priv->y[i][j], 50 * 
								(LINE_VIE-priv->lifev[i][j])/LINE_VIE);
				}
				priv->lifev[i][j]--;
			}
		}
	}
}

/*******************************************************************/
/**************** SUPER SPECTRAL ***********************************/
/*******************************************************************/

void super_spectral(JessPrivate *priv, uint8_t * buffer)
{
	int i,j;

	float nx,ny; 
	float taille,dt = priv->conteur.dt;

	int resx = priv->resx;
	int resy = priv->resy;
	int yres2 = priv->yres2;

	for(i=0; i < 256 ;i++) /* pour chaque frequence */
	{
		if (priv->lys.dbeat[i] == NEW) /*creation d'une nouvelle ligne */
		{
			priv->lys.dbeat[i] = MANAGE;
			j=0;
			while (priv->lifet[i][j] > 0)
			{
				j++;
				if (j == LINE_MAX+1)
					goto quit; /* pas de place desole */
			}
			/* initialisation de la ligne */
			priv->lifet[i][j] = LINE_VIE;
			priv->ssvx[i][j] = 0*RESFACTXF( 0.025*((float) i - 128.0) * 32 +
					visual_random_context_int(priv->rcontext)/(float)UINT32_MAX*60 );
			priv->ssvy[i][j] = 0*RESFACTYF( 64 +visual_random_context_int(priv->rcontext)/(float)UINT32_MAX*64);
			priv->ssx[i][j] = RESFACTXF( 2*(i - 128) ) +((float)j*(i-128))/2;
			priv->ssy[i][j] = 0*RESFACTXF( yres2/2-(float)((i-128)*(i-128))/256) -20*j+60 ;
			priv->sstheta[i][j] = 0;
			priv->ssomega[i][j] = (10+i)*i*priv->lys.Ed_moyen[i]*32;
		}
quit:; /* pas de nouvelle ligne */
     for (j=0 ; j < LINE_MAX ; j++)
	     if (priv->lifet[i][j] > 0)
	     {
		     priv->sstheta[i][j] += priv->ssomega[i][j]*dt; 
		     priv->ssvy[i][j] += -0.5*dt*1024*0;
		     priv->ssx[i][j] += priv->ssvx[i][j] * dt;
		     priv->ssy[i][j] += priv->ssvy[i][j] * dt;
		     taille =  RESFACTXF(70) * (0+2*(LINE_VIE-priv->lifet[i][j]))/LINE_VIE*(j+1)/6;
		     nx = taille*sin(priv->sstheta[i][j]);
		     ny = taille*cos(priv->sstheta[i][j]);

		     droite (priv, buffer, (int) priv->ssx[i][j]+nx , (int) priv->ssy[i][j]+ny, (int) priv->ssx[i][j], (int) priv->ssy[i][j], 50*(LINE_VIE-priv->lifet[i][j])/LINE_VIE);

		     if (priv->video ==8)
			     cercle (priv, buffer, (int) priv->ssx[i][j]+nx, (int) priv->ssy[i][j]+ny, 3*j, 150*(LINE_VIE-priv->lifet[i][j])/LINE_VIE);
		     else
			     cercle_32 (priv, buffer, (int) priv->ssx[i][j]+nx, (int) priv->ssy[i][j]+ny, 3*j, 150*(LINE_VIE-priv->lifet[i][j])/LINE_VIE);

		     priv->lifet[i][j]--;
	     }
	}
}

/*******************************************************************/
/**************** GRANDE GRILLE ************************************/
/*******************************************************************/

void grille_3d (JessPrivate *priv, uint8_t * buffer, float data[2][512], float alpha, float beta,
		float gamma, int persp, int dist_cam)
{

	float x, y, z, xres2 = (float) (priv->resx >> 1);
	short ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;
	uint8_t color;

	int resx = priv->resx;
	int resy = priv->resy;

	nb_x = 32;
	nb_y = 32;

	for (i = 0; i < nb_x; i++)
	{
		/* Optimisation par Karl Soulabaille. Factorisation et expressions communes. */
		for (j = 0; j < nb_y; j++)
		{
			x = RESFACTXF ((i - ((float) nb_x) / 2) * 10);
			y = RESFACTYF ((j - ((float) nb_y) / 2) * 10);

			if (j >= 16)
			{
				z = RESFACTXF ((float) (data[0][i + nb_x * (j - 16)]) * 256);
				color = data[0][i + nb_x * (j - 16)] * 64 + 100;
			}
			else
			{
				z = RESFACTXF ((float) (data[1][i + nb_x * j]) * 256);
				color = (data[1][i + nb_x * j]) * 64 + 100;
			}

			rotation_3d (&x, &y, &z, alpha, beta, gamma);
			perspective (&x, &y, &z, persp, dist_cam);

			if (x >= xres2)
			{
				x = xres2 - 1;
				color = 0;
			}
			if (x <= -xres2)
			{
				x = -xres2 + 1;
				color = 0;
			}
			if (y >= priv->yres2)
			{
				y = priv->yres2 - 1;
				color = 0;
			}
			if (y <= -priv->yres2)
			{
				y = -priv->yres2 + 1;
				color = 0;
			}

			ix = (int) x;
			iy = (int) y;

			if (j != 0)
				droite (priv, buffer, ix, iy, ax, ay, color);

			ax = ix;
			ay = iy;
		}
	}
}

/*******************************************************************/
/**************** 2 GRILLES JUMELLES *******************************/
/*******************************************************************/

void l2_grilles_3d (JessPrivate *priv, uint8_t * buffer, float data[2][512], float alpha, float beta,
		float gamma, int persp, int dist_cam)
{

	float x, y, z, xres4 = (float) (priv->resx >> 2);
	short ax = 0, ay = 0, ix, iy, i, j, nb_x, nb_y;
	uint8_t color[16][16];

	int resx = priv->resx;
	int resy = priv->resy;

	nb_x = 16;
	nb_y = 16;

	for (i = 0; i < nb_x; i++)
	{
		/* Optimisation par Karl Soulabaille. Factorisation de calcul & expression commune */
		x = RESFACTXF ((i - ((float) nb_x / 2)) * 15);
		for (j = 0; j < nb_y; j++)
		{
			y = RESFACTYF ((j - ((float) nb_y / 2)) * 15);

			z = fabs(RESFACTXF ((float) (data[1][i + nb_x * j]) * 256));
			color[i][j] = data[1][i + nb_x * j] * 64 + 100;

			rotation_3d (&x, &y, &z, alpha, beta, gamma);
			perspective (&x, &y, &z, persp, dist_cam);

			ix = (int) x;
			iy = (int) y;

			if (j != 0)
			{
				droite (priv, buffer, ix - xres4, iy, ax - xres4, ay, color[i][j]);
				droite (priv, buffer, ix + xres4, iy, ax + xres4, ay, color[i][j]); 
			}

			ax = ix;
			ay = iy;
		}
	}
}

/*******************************************************************/
/**************** BURN 3D ******************************************/
/*******************************************************************/

void burn_3d (JessPrivate *priv, uint8_t * buffer, float data[2][512], float alpha, float beta,
		float gamma, int persp, int dist_cam, int mode)
{
	float x,y,z,xres2 = (float) (priv->resx >> 1), yres2 = (float) (priv->resy >> 1);
	short ix, iy, i, j, nb_x, nb_y, color, taille;

	/* Variable temporaire pour la factorisation de constantes */
	float temp1, temp2, temp3, temp3b, temp4, temp4b, temp5,temp6,temp7,temp8,temp9;

	int resx = priv->resx;
	int resy = priv->resy;

	nb_x = 12;
	nb_y = 12;

	/* 4 cas:  */
	switch(mode) {
		case 0:

			temp1 = 2*PI/nb_x*cos(alpha);
			temp2 = 2*PI/nb_y;
			for (i = 0 ; i< nb_x ; i++)
			{
				temp3 = (float)((int)(i*i));
				for (j = 0 ; j<nb_y ; j++)
				{
					x = RESFACTXF ((float) 25*(i+1)*cos(temp3*temp1+ temp2*j) );
					y = RESFACTYF ((float) 25*(i+1)*sin(temp3*temp1+ temp2*j) );
					z = RESFACTXF (40*cos(5*alpha));
					rotation_3d (&x, &y, &z, alpha, beta, gamma);
					perspective (&x, &y, &z, persp, dist_cam);
					if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2))
					{
						ix = (int) x;
						iy = (int) y;

						color = ((int)(float)(100+0.4*z));
						if (color<0)
							color = 0;
						taille = color/8;

						boule (priv, buffer, ix, iy, taille, color);
					}
				}
			}
			break;
		case 1:
			temp1 = PI/(nb_x*nb_x*nb_x)*fabs(priv->lys.dEdt_moyen*5000);
			temp6 = 2*PI/nb_y;
			for (i = 0 ; i< nb_x ; i++)
			{
				temp3 = (float)(i*i*i);
				temp4 = sin((i+1)*PI/nb_x);
				temp5 = 5*alpha*2*PI/nb_x*i;
				temp3b = temp3*temp1;
				temp4b = temp4*temp1;

				for (j = 0 ; j<nb_y ; j++)
				{
					x = RESFACTXF ((float) 50*(temp3b + temp4*cos(temp5 +  temp6*j)));
					y = RESFACTYF ((float) 50*(temp4b + temp4*sin(temp5 +  temp6*j)));
					z = RESFACTXF ((float) 100*cos((float)i/nb_x*PI)*(1 + priv->lys.dEdt_moyen*1000));
					rotation_3d (&x, &y, &z, alpha, beta, gamma);
					perspective (&x, &y, &z, persp, dist_cam);
					if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2))
					{
						ix = (int) x;
						iy = (int) y;

						color = ((int)(float)(100+0.4*z));
						if (color<0)
							color = 0;
						taille = color/8;

						boule (priv, buffer, ix, iy, taille, color);
					}
				}
			}
			break;
		case 2:
			temp5 = 2*PI/(5*nb_x);
			temp6 = 2*PI/nb_y;
			for (i = 0 ; i< nb_x ; i++) 
			{
				temp1 = sin((i+1)*PI/nb_x);
				temp2 = temp5 * -i;

				for (j = 0 ; j<nb_y ; j++)
				{
					x = RESFACTXF ((float) 130*temp1*cos(temp2+ j*temp6 ));
					y = RESFACTYF ((float) 130*temp1*sin(temp2+ j*temp6 ));
					z = -RESFACTXF (130*cos((float)i/nb_x*PI)*priv->lys.dEdt_moyen*1000 );
					rotation_3d (&x, &y, &z, alpha, beta, gamma);
					perspective (&x, &y, &z, persp, dist_cam);
					if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2))
					{
						ix = (int) x;
						iy = (int) y;

						color = ((int)(float)(100+0.4*z));
						if (color<0)
							color = 0;
						taille = color/8;

						boule (priv, buffer, ix, iy, taille, color);
					}
				}
			}
			break;
		case 3: 
			temp5 = 25;
			temp6 = ((float)(-2))*PI/(10*nb_x);
			temp8 = (float)(2)*PI/nb_y;
			temp9 = 10*alpha;
			for (i = 0 ; i< nb_x ; i++)
			{
				temp5 += 25;
				temp7 = -i*temp6;
				for (j = 0 ; j<nb_y ; j++)
				{
					x = RESFACTXF ((float) temp5*cos(temp7+ temp8*j) );
					y = RESFACTYF ((float) temp5*sin(temp7+ temp8*j) );
					z = RESFACTXF (60* (cos(temp8*j+temp9)+cos(-temp7)) );
					rotation_3d (&x, &y, &z, alpha, beta, gamma);
					perspective (&x, &y, &z, persp, dist_cam);
					if (!(x >= xres2 || x <= -xres2 || y >= yres2 || y <= -yres2))
					{
						ix = (int) x;
						iy = (int) y;

						color = ((int)(float)(100+0.4*z));
						if (color<0)
							color = 0;
						taille = color/8;

						boule (priv, buffer, ix, iy, taille, color);
					}
				}
			}
			break;
	}
}

/*******************************************************************/
/**************** STARS MORPH **************************************/
/*******************************************************************/

void stars_create_state(JessPrivate *priv, float pos[3][STARS_MAX], int mode)
{
	int i,j,sq=sqrt(STARS_MAX);
	float fsq = (float) sq;


	switch(mode)
	{
		case 0:
			for(i=0 ; i< STARS_MAX; i++)
			{
				for(j=0; j<3 ; j++)
				{
					pos[j][i] = 0;
				}
			}
			break;
		case 1:
			for(i=0 ; i< STARS_MAX; i++)
			{
				for(j=0; j<3 ; j++)
				{
					pos[j][i] = (visual_random_context_int(priv->rcontext)/(float)UINT32_MAX-0.5);

				}
			}
			break;
		case 2:
			for(j=0 ; j< sq; j++)
			{
				for(i=0 ; i< sq; i++)
				{
					pos[0][i + j*sq] = (i - fsq/2)*2/fsq;
					pos[1][i + j*sq] = (j - fsq/2)*2/fsq;
					pos[2][i + j*sq] = 0;
				}
			}
			break;
		case 3:
			for(j=0 ; j< sq; j++)
			{
				for(i=0 ; i< sq; i++)
				{
					pos[0][i + j*sq] = sin((i+1)*PI/fsq);
					pos[1][i + j*sq] = sin(-i*2*PI/(10*fsq)+ j*2*PI/fsq) ;
					pos[2][i + j*sq] = cos(j*2*PI/fsq) ;
				}
			}
			break;
	}
}

void stars_manage(JessPrivate *priv, uint8_t *buffer, int new,  float alpha, float beta,
		float gamma, int persp, int dist_cam)
{
	static float x[STARS_MAX], y[STARS_MAX], z[STARS_MAX];
	float xres2 = (float) (priv->resx >> 1), yres2 = (float) (priv->resy >> 1);
	float x_t, y_t, z_t;
	float mult;
	int ix, iy, i, color, taille;

	if (new == NEW_SESSION) /* explosion */
	{
		priv->smmorpheur = 0;
		priv->smselect = 1;
		stars_create_state(priv, priv->smpos[0], 0);
		stars_create_state(priv, priv->smpos[1], 1);
	}
	else if(new == NEW) /* on creer une nouvelle forme */
	{ 
		mult = 1;
		if ((visual_random_context_int(priv->rcontext)%3)==0)
			mult = 4;

		for(i = 0 ; i< STARS_MAX; i++)
		{
			priv->smpos[priv->smselect][0][i]= mult*x[i];
			priv->smpos[priv->smselect][1][i]= mult*y[i];
			priv->smpos[priv->smselect][2][i]= mult*z[i]; 
		}
		priv->smselect = 1 - priv->smselect;
		stars_create_state(priv, priv->smpos[priv->smselect], visual_random_context_int(priv->rcontext)%2+1);
	}
	else  /* on gere */
	{
		priv->smmorpheur += ( 2 * (float)priv->smselect - 1) * MORPH_SPEED * priv->conteur.dt;

		if (priv->smmorpheur > 1)
			priv->smmorpheur = 1;
		else if (priv->smmorpheur < 0)
			priv->smmorpheur = 0;

		for(i = 0 ; i< STARS_MAX; i++)
		{
			x[i] =(priv->smpos[1][0][i] * priv->smmorpheur + (1-priv->smmorpheur) * priv->smpos[0][0][i]);
			y[i] =(priv->smpos[1][1][i] * priv->smmorpheur + (1-priv->smmorpheur) * priv->smpos[0][1][i]);
			z[i] =(priv->smpos[1][2][i] * priv->smmorpheur + (1-priv->smmorpheur) * priv->smpos[0][2][i]);

			x_t = 250 * x[i];
			y_t = 250 * y[i];
			z_t = 250 * z[i];

			rotation_3d (&x_t, &y_t, &z_t, alpha, beta, gamma);
			perspective (&x_t, &y_t, &z_t, persp, dist_cam);

			if ((int)x_t >= xres2 || (int)x_t <= -xres2 || (int)y_t >= yres2 || (int)y_t <= -yres2 || z_t >2*dist_cam)
				return;

			ix = (int) x_t;
			iy = (int) y_t;

			color = ((int)(float)(100+0.4*z_t));
			if (color<0)
				color = 0;

			taille = color/8;

			droite(priv, buffer,ix,iy, xres2/2  ,-yres2 , color/8); 
			boule (priv, buffer, ix, iy, taille, color); 
		}
	}
}

/*******************************************************************/
/**************** OSCILLO ******************************************/
/*******************************************************************/

static uint8_t couleur (JessPrivate *priv, short x)
{
	int resx = priv->resx;

	float xf = (float) x, resx2f = (float) resx / 2;
	return (uint8_t) ((float) -(xf - resx2f) * (xf + resx2f) * 150 /
			(resx2f * resx2f));
}

void courbes (JessPrivate *priv, uint8_t * buffer, float data[2][512], uint8_t color, int type)
{
	int j, i, x1, y1, x2=0, y2=0;
	int r;

	int resx = priv->resx;
	int resy = priv->resy;

#define RAYON 100

	switch (type)
	{
		case 0:
			for (i = 0; i < resx - 1 && i < 511; i++)
			{
				j = i - 256;
				droite (priv, buffer, j, data[0][i] * (128) + resy / 6, j + 1,
						data[0][i + 1] * (128) + resy / 6, couleur (priv, j));
				droite (priv, buffer, j, data[1][i] * (128) - resy / 6, j + 1,
						data[1][i + 1] * (128) - resy / 6, couleur (priv, j));
			}
			break;
		case 1:
			r = data[0][255] * 256;
			x2 = (RAYON+r) * cos (255*2*PI/256);
			y2 = (RAYON+r) * sin (255*2*PI/256);
			for (i = 0; i < 256 ;i++)
			{
				r = data[0][i] * 256;
				x1 = (RAYON+r) * cos (i*2*PI/256);
				y1 = (RAYON+r) * sin( i*2*PI/256);
				droite(priv, buffer,x1,y1,x2,y2,100);
				x2 = x1;
				y2 = y1;
			}
			break;
		default:
			break;
	}

}

/*******************************************************************/
/**************** SINUS_RECT ***************************************/
/*******************************************************************/

void sinus_rect (JessPrivate *priv, uint8_t * buffer, int k)
{
	int resx = priv->resx;
	int resy = priv->resy;
       
	float fk = k;
	
	ball (priv, buffer, RESFACTXF (250 * cos (fk / 25)),
			RESFACTYF (100 * sin (2 * fk / 25)), RESFACTXF (100), 200);
	ball (priv, buffer, RESFACTXF (-250 * cos (fk / 25)),
			RESFACTYF (100 * sin (2 * fk / 25)), RESFACTXF (100), 200);
}

