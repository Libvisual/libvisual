/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: renderer.c,v 1.11 2006/02/05 18:47:26 synap Exp $
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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "def.h"
#include "struct.h"
#include "distorsion.h"
#include "draw.h"
#include "jess.h"
#include "analyser.h"
#include "analyser_struct.h"
#include "renderer.h"
#include "pal.h"

void draw_mode(JessPrivate *priv, int mode)
{
	switch (priv->lys.montee)
	{
		case NON: /* bruit calme */
			if (priv->conteur.courbe <= 255 - 35) /* le bruit calme revient */
				priv->conteur.courbe += 32; /* on fait re-apparaitre la courbe */

			if (mode == 0)
				courbes (priv, priv->pixel, priv->pcm_data, priv->conteur.courbe,0);
			else if (mode == 1)
				l2_grilles_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle2 / 200, 0,
						priv->conteur.angle2 / 30, 200, 130);
			else if (mode == 2)
				burn_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle2 / 400, 0,
						priv->conteur.angle2 / 60, 200, 130, priv->conteur.burn_mode);
			else if ((mode == 3) && (priv->conteur.k3 > 700)) /* mode 3 */
				burn_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle / 200, 0,
						priv->conteur.angle / 30, 200, 130, priv->conteur.burn_mode);
			else if (mode == 4) /* mode ligne */
			{
				super_spectral_balls(priv, priv->pixel);
				courbes (priv, priv->pixel, priv->pcm_data, priv->conteur.courbe,1);
			}
			else if (mode == 6) 
				super_spectral(priv, priv->pixel);

			else if (mode == 5) /* mode stars */
				stars_manage(priv, priv->pixel, MANAGE, priv->conteur.angle2 / 400, 0,
						priv->conteur.angle2 / 60, 200, 130);
			break;

		case OUI: /* bruit modere */
			priv->conteur.courbe = 0;
			if (mode == 0)
				grille_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle / 200, 0, priv->conteur.angle / 30,
						100, -priv->lys.E_moyen*20 + 130);
			else if (mode == 1)
				l2_grilles_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle2 / 200, 0,
						priv->conteur.angle2 / 30, 200, -priv->lys.E_moyen * 20 + 130);
			else if (mode == 2)
				burn_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle2 / 400, 0,
						priv->conteur.angle2 / 60, 200, 130, priv->conteur.burn_mode);
			else if ((mode == 3) && (priv->conteur.k3 > 700)) /* mode 3 */
				burn_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle / 200, 0,
						priv->conteur.angle / 30, 200, 130, priv->conteur.burn_mode);
			else if (mode == 4) /* mode ligne */
			{
				super_spectral_balls (priv, priv->pixel);
				courbes (priv, priv->pixel, priv->pcm_data, priv->conteur.courbe,1);
			}
			else if (mode == 6) 
				super_spectral (priv, priv->pixel);
			else if (mode == 5) /* mode stars */
				stars_manage (priv, priv->pixel, MANAGE, priv->conteur.angle2 / 400, 0,
						priv->conteur.angle2 / 60, 200, 130);
			break;
	}

	priv->conteur.k3 += 10;
	if (priv->conteur.k3 < 300)  /* Ici c'est les boules qui se barrent */
		burn_3d (priv, priv->pixel, priv->pcm_data, priv->conteur.angle2 / 200, 0,
				priv->conteur.angle2 / 200, 200, -50 + 3 * priv->conteur.k3, priv->conteur.burn_mode);

	fusee(priv, priv->pixel, MANAGE);

	on_beat(priv, priv->lys.beat);

	on_reprise(priv);
}

void *renderer (JessPrivate *priv)
{
	ips (priv);

	manage_dynamic_and_states_open(priv);

	render_deformation(priv, priv->conteur.blur_mode);

	render_blur(priv, 0); 

	draw_mode(priv, priv->conteur.draw_mode);

	copy_and_fade(priv, DEDT_FACTOR * priv->lys.dEdt_moyen);

	if (priv->conteur.analyser == 1) {
		analyser (priv, priv->pixel);
	}

	manage_states_close(priv);

	return NULL;
}

void manage_dynamic_and_states_open(JessPrivate *priv)
{
	priv->conteur.general++;
	priv->conteur.k2++;
	priv->conteur.last_flash++;

	priv->conteur.angle += priv->conteur.dt * 50;

	priv->conteur.v_angle2 = 0.97 * priv->conteur.v_angle2 ;
	priv->conteur.angle2 += priv->conteur.v_angle2 * priv->conteur.dt ;

	detect_beat(priv);

	if (priv->lys.dEdt_moyen > 0)
		priv->lys.montee = OUI;

	if ((priv->lys.montee == OUI) && (priv->lys.beat == OUI))
		priv->lys.reprise = OUI ;

}

void manage_states_close(JessPrivate *priv)
{
	priv->lys.beat = NON ;
	priv->lys.montee = NON;
	priv->lys.reprise = NON;
}

void on_beat(JessPrivate *priv, int beat)
{
	if (priv->lys.beat == OUI)
	{

		fusee(priv, priv->pixel,NEW);

		/* on fou des etoiles */
		priv->conteur.k1 += 4;

		/* vitesse a l angle 2 */
		priv->conteur.v_angle2 += (visual_random_context_int (priv->rcontext) % 2 - 0.5) * 16 * 32; 

		if (priv->conteur.draw_mode == 3)
			priv->conteur.k3 = 0;

		if (priv->conteur.draw_mode == 5)
			stars_manage(priv, priv->pixel, NEW, priv->conteur.angle2 / 400, 0,
					priv->conteur.angle2 / 60, 200, 130);
	}
}

void on_reprise(JessPrivate *priv)
{
	uint32_t j;
	uint8_t *pix = priv->pixel;

	if (priv->lys.reprise == OUI) {
		if (priv->conteur.last_flash > 5 * priv->conteur.fps) {
			if (priv->conteur.draw_mode == 5)
				stars_manage(priv, priv->pixel, NEW_SESSION, priv->conteur.angle2 / 400, 0,
						priv->conteur.angle2 / 60, 200, 130);	

			pix = priv->pixel;
			for (j = 0; j < priv->resy * priv->pitch; j++)
				*(pix++) = 250;  

			if (priv->conteur.freeze_mode == NON) {      
				priv->conteur.burn_mode = visual_random_context_int(priv->rcontext) % 4;      

				priv->conteur.draw_mode = visual_random_context_int(priv->rcontext) % 7;    

				priv->conteur.blur_mode = visual_random_context_int(priv->rcontext) % 5 ; 
				if (priv->conteur.draw_mode==2)
					priv->conteur.blur_mode=0;

				random_palette(priv);
			}
			priv->conteur.last_flash = 0;
		} else {
			/* il y a eu un flash y a pas longtemps, donc on fait juste des etoiles */
			/* on change de mode blur */
			if ((priv->conteur.freeze_mode == 0) && (priv->conteur.mix_reprise >5) && (priv->conteur.draw_mode!=2)) {
				priv->conteur.blur_mode = visual_random_context_int(priv->rcontext) % 5 ; 
			}
		}
	}
}

void copy_and_fade(JessPrivate *priv, float factor)
{
	uint32_t j;
	uint8_t *pix, *buf;

	buf = priv->buffer;
	pix = priv->pixel;

	if(priv->video == 8)
	{
		fade(factor, priv->dim);

		for (j = 0; j <  priv->resy * priv->resx; j++)
		{
			*(buf++) = priv->dim[*(pix++)];
		}
	}
	else
	{
		fade(cos(0.125*factor)*factor*2, priv->dimR);
		fade(cos(0.25*factor)*factor*2, priv->dimG);
		fade(cos(0.5*factor)*factor*2, priv->dimB);

		for (j = 0; j <  priv->resy * priv->resx; j++)
		{
			*(buf++) = priv->dimR[*(pix++)];
			*(buf++) = priv->dimG[*(pix++)];
			*(buf++) = priv->dimB[*(pix++)];

			buf++;
			pix++;
		}
	}
}


void fade(float variable, uint8_t * dim)
{
	uint32_t aux2,j ;
	float aux;

	aux = 1-exp(-fabs(variable));

	if (aux>1)
		aux=1;
	if (aux<0)
		aux=0;

	for (j= 0; j < 256; j++)
	{
		aux2 = (uint8_t) ((float) j * 0.245 * aux);

		if (aux2>255)
			aux2=255;
		if (aux2<0)
			aux2=0;

		dim[j]= aux2;
	}
}

void render_deformation(JessPrivate *priv, int defmode)
{
	intptr_t bmax;
	uint32_t *tab1 = NULL, *tab2, *tab3, *tab4, i;
	uint8_t *pix = priv->pixel, *aux;

	/**************** BUFFER DEFORMATION ****************/
	if (priv->video == 8)
	{
		tab1 = priv->table1;
		tab2 = priv->table2;
		tab3 = priv->table3;
		tab4 = priv->table4;
		bmax = priv->resx * priv->resy + (intptr_t) priv->pixel;

		switch(defmode)
		{
			case 0:
				visual_mem_copy(priv->pixel, priv->buffer, priv->resx * priv->resy);
				break;
			case 1:
				for (pix = priv->pixel; pix < (uint8_t *) bmax ; pix++)
					*pix = *(priv->buffer + *(tab1++)) ;
				break;
			case 2:
				for (pix = priv->pixel; pix < (uint8_t *) bmax; pix++)
					*pix = *(priv->buffer + *(tab2++)) ;
				break;
			case 3:
				for (pix = priv->pixel; pix < (uint8_t *) bmax; pix++)
					*pix = *(priv->buffer + *(tab3++)) ;
				break;
			case 4:
				for (pix = priv->pixel; pix < (uint8_t *) bmax; pix++)
					*pix = *(priv->buffer + *(tab4++)) ;
				break;   
			default:
		
				break;
		}
	}
	else
	{
		pix = priv->pixel;

		bmax = priv->resx * priv->resy;
		switch(defmode)
		{
			case 0:
				visual_mem_copy(priv->pixel, priv->buffer, priv->pitch * priv->resy);
				return;
				break;
			case 1:
				tab1 = priv->table1;
				break;
			case 2:
				tab1 = priv->table2;
				break;
			case 3:
				tab1 = priv->table3;
				break;
			case 4:
				tab1 = priv->table4;
				break;

			default:
		
				break;
		}
		for (i = 0; i < priv->resy * priv->resx; i++)
		{
			aux  =  (uint8_t *) ((*(tab1) << 2 ) + (intptr_t) priv->buffer);
			*(pix++) = *(aux++) ;
			*(pix++) = *(aux++);  
			*(pix++) = *(aux);  
			
			pix++;

			tab1++;
		}
	}
}

void render_blur(JessPrivate *priv, int blur)
{

	/***************** Annotation par Karl Soulabaille:     ***/
	/* Quel est la valeur d'initialisation de pix ? */
	/* (d'ou le segfault) */
	/* j'ai mis pixel par defaut... */

	uint8_t *pix = priv->pixel;
	intptr_t bmax;
	uint32_t pitch_4;
	
	pix = priv->pixel;
	if (priv->pixel == NULL)
		return;


	/* FIXME MMX 8 BIT VERSION */
	
	/* Annotation par Karl Soulabaille: */
	/* Il y avait des overflows sur les boucles (indice supérieur trop élevé de 1) */
	if (priv->video == 8)
	{
		bmax = priv->resx * (priv->resy-1) + (intptr_t) priv->pixel;

		if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
			__asm __volatile
				("\n\t pxor %%mm6, %%mm6"
				 ::);

			for (pix = priv->pixel; pix < (uint8_t *) bmax-9; pix += 8)
			{
				__asm __volatile
					("\n\t movq %[pix1], %%mm0"
					 "\n\t movq %[pix2], %%mm1"
					 "\n\t movq %[pix3], %%mm2"
					 "\n\t paddb %%mm0, %%mm1"
					 "\n\t movq %[pix4], %%mm3"
					 "\n\t paddb %%mm2, %%mm1"
					 "\n\t paddb %%mm3, %%mm1"
					 "\n\t movq %%mm1, %[pix1]"
					 :: [pix1] "m" (*pix)
					 , [pix2] "m" (*(pix + 1))
					 , [pix3] "m" (*(pix + priv->resx))
					 , [pix4] "m" (*(pix + priv->resx + 1)));

			}

			__asm __volatile
				("\n\t emms");

#endif
		} else {
			for (pix = priv->pixel; pix < (uint8_t *) bmax-1; pix++)
				*pix += *(pix+1) + *(pix+ priv->resx) + *(pix+ priv->resx+1); 
		}
	}
	else
	{
		pitch_4 = priv->pitch+4;
		bmax = priv->pitch*(priv->resy-1) + (intptr_t) priv->pixel;

		if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
			__asm __volatile
				("\n\t pxor %%mm6, %%mm6"
				 ::);

			for (pix = priv->pixel; pix < (uint8_t *) bmax-12; )
			{
				__asm __volatile
					("\n\t movq %[pix1], %%mm0"
					 "\n\t movq %[pix2], %%mm1"
					 "\n\t movq %[pix3], %%mm2"
					 "\n\t paddb %%mm0, %%mm1"
					 "\n\t movq %[pix4], %%mm3"
					 "\n\t paddb %%mm2, %%mm1"
					 "\n\t paddb %%mm3, %%mm1"
					 "\n\t movq %%mm1, %[pix1]"
					 :: [pix1] "m" (*pix)
					 , [pix2] "m" (*(pix + 4))
					 , [pix3] "m" (*(pix + priv->pitch))
					 , [pix4] "m" (*(pix + pitch_4)));
				
				pix += 8;
			}

			__asm __volatile
				("\n\t emms");

#endif
		} else {
			for (pix = priv->pixel; pix < (uint8_t *) bmax-4; )
			{
				*pix += *(pix + 4) + *(pix + priv->pitch) + *(pix + pitch_4); 
				pix++;
				*pix += *(pix + 4) + *(pix + priv->pitch) + *(pix + pitch_4);   
				pix++;
				*pix += *(pix + 4) + *(pix + priv->pitch) + *(pix + pitch_4);  
				pix += 2;
			}
		}
	}
}

