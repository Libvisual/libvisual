/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: analyser.c,v 1.6 2006/01/30 19:06:46 synap Exp $
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

#include <sys/time.h>
#include <stdlib.h>

#include "struct.h"
#include "analyser_struct.h"
#include "analyser.h"
#include "def.h"
#include "jess.h"

int detect_beat(JessPrivate *priv)
{
	if ((priv->lys.E / priv->lys.E_moyen > LEVEL_ONE))
	{
		priv->lys.beat = OUI;

	}

	return 0;
}

/* Energie discrete moyenne temporellie*/
void spectre_moyen(JessPrivate *priv, short data_freq_tmp[2][256])
{
	int i;
	float aux,N;
	N = T_AVERAGE_SPECTRAL;

	for (i=0 ; i<256 ; i++)
	{
		aux = (float) (data_freq_tmp[0][i] + data_freq_tmp[1][i]) * 0.5 / 65536;
		aux = aux * aux;
		priv->lys.Ed_moyen[i] = (N - 1.0) / N * priv->lys.Ed_moyen[i] + 1.0 / N * aux;

		if (aux / priv->lys.Ed_moyen[i] > 9) /* discret beat */
			priv->lys.dbeat[i] = OUI;
	}
}


/* Derivee temporelle de l'energie moyenne */
/* Doit etre appelle apres C_E_moyen */
void C_dEdt_moyen(JessPrivate *priv)
{
	float new, N;

	N = T_AVERAGE_DEDT;

	new = (priv->lys.E_moyen - priv->E_old1) / priv->lys.dt;

	priv->lys.dEdt_moyen = (N - 1.0) / N * priv->lys.dEdt_moyen + 1.0 / N * new; 

	priv->E_old1 = priv->lys.E_moyen;
}

void C_dEdt(JessPrivate *priv)
{
	float new, N;

	N = T_DEDT; /* on met un petit filtre qd meme */

	new = (priv->lys.E_moyen - priv->E_old2) / priv->lys.dt;

	priv->lys.dEdt = (N - 1.0) / N * priv->lys.dEdt_moyen + 1.0 / N * new; 

	priv->E_old2 = priv->lys.E_moyen;
}
  
/* Energie moyenne temporelle */
void C_E_moyen(JessPrivate *priv, short data_freq_tmp[2][256])
{
	float N;
	N = T_AVERAGE_E;

	priv->lys.E_moyen = (N - 1.0) / N * priv->lys.E_moyen + 1.0 / N * energy(priv, data_freq_tmp,1);
}

/* Energie courante */
float energy(JessPrivate *priv, short data_freq_tmp[2][256], int type_E)
{
	int i, tmp;
	float energy_ = 0;

	for (i = 0; i < 256; i++) {
		tmp = ( data_freq_tmp[1][i]  ) >> 8 ;
		energy_+= tmp * tmp ;
	}

	energy_ =  energy_ / 65536 / 256 * 256; /*ahahah*/ /* synap: Yes, indeed */

	priv->lys.E = energy_;

	return energy_;
}

/* TICK REPLACEMENT */

int start_ticks(JessPrivate *priv)
{
	gettimeofday(&priv->start, NULL);

	return 0;
}

static int get_ticks(JessPrivate *priv)
{
	struct timeval now;
	int ticks;

	gettimeofday(&now, NULL);
	ticks = (now.tv_sec - priv->start.tv_sec) * 1000 +
		(now.tv_usec - priv->start.tv_usec) / 1000;

	return ticks;
}

/* REINIT */
float time_last(JessPrivate *priv, int i, int reinit)
{
	float new_time = get_ticks(priv);
	float delta_t;

	delta_t = (new_time - priv->lys.last_time[i]) / 1000;

	if (reinit == OUI)
		priv->lys.last_time[i] = new_time;

	return delta_t;
} 



void ips(JessPrivate *priv)
{
	priv->conteur.dt = time_last(priv, FOUR, NON);
	priv->conteur.fps = (int) 1 / time_last(priv, FOUR, REINIT);
}

