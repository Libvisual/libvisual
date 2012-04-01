/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: pal.c,v 1.5 2005/12/20 18:49:12 synap Exp $
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
#include <math.h>

#include "def.h"
#include "struct.h"
#include "analyser_struct.h"
#include "jess.h"
#include "pal.h"

const float __magic = 2.0 * PI / 256;

uint8_t courbes_palette(JessPrivate *priv, uint8_t i, int no_courbe)
{
	/* Optimisation par Karl Soulabaille */
	switch(no_courbe)
	{
		case 0:
			return (i * i * i) >> 16;
			break;
		case 1:
			return (i * i) >> 8;
			break;
		case 2:
			return (uint8_t) i ;
			break;
		case 3:
			return (uint8_t) (128 * fabs(sin(__magic * i)));
			break;
		case 4:
			return 0;
			break;
	}
	return 0;
}

void random_palette(JessPrivate *priv)
{
	VisColor *pal_colors = visual_palette_get_colors (priv->jess_pal);

	int i,j,k,l;

again_mister:
	;

	if (priv->conteur.psy == 1)
		i = 5;
	else
		i = 3;

	j=visual_random_context_int(priv->rcontext) % i;
	k=visual_random_context_int(priv->rcontext) % i;
	l=visual_random_context_int(priv->rcontext) % i;

	priv->conteur.triplet = j+10*k+100*l;

	if ((j==k) || (j==l) || (l==k))
		goto again_mister;

	for (i = 0; i < 256; i++){
		pal_colors[i].r = courbes_palette(priv, i, j);
		pal_colors[i].g = courbes_palette(priv, i, k);
		pal_colors[i].b = courbes_palette(priv, i, l);
	}
}

