/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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
#include <math.h>

#include "oinksie.h"
#include "common.h"
#include "gfx-palette.h"
#include "config.h"
#include "audio.h"

void _oink_audio_init (OinksiePrivate *priv)
{
	priv->audio.basssens = 20;
	priv->audio.tripplesens = 20;
}

/* audio priv->audio.freq analyze */
void _oink_audio_analyse (OinksiePrivate *priv)
{
	int i, j, total, mean;
	
	/* Tripple left */
	for (i = 50, total = 0; i < 190; i++)
		total += priv->audio.freq[0][i];
	
	mean = (total / 140);

	priv->audio.trippleleft = 1;
	for (i = 9; i > 1; i--)
	{
		if (mean > priv->audio.tripplesens * i)
		{
			priv->audio.trippleleft = i;
			break;
		}

	}

	/* Tripple Right */
	for (i = 50, total = 0; i < 190; i++)
		total += priv->audio.freq[1][i];
	
	mean = (total / 140);

	priv->audio.trippleright = 1;
	for (i = 9; i > 1; i--)
	{
		if (mean > priv->audio.tripplesens * i)
		{
			priv->audio.trippleright = i;
			break;
		}

	}

	/* Bass left */
	for (i = 0, total = 0; i < 35; i++)
		total += priv->audio.freq[0][i];

	mean = (total / 35);

	priv->audio.bassleft = 1;
	for (i = 9, j = 350; i > 1; i--, j -= 35)
	{
		if (mean > priv->audio.basssens * j)
		{
			priv->audio.bassleft = i;
			break;
		}
	}

	/* Bass Right */
	for (i = 0, total = 0; i < 35; i++)
		total += priv->audio.freq[1][i];

	mean = (total / 35);

	priv->audio.bassright = 1;
	for (i = 9, j = 350; i > 1; i--, j -= 35)
	{
		if (mean > priv->audio.basssens * j)
		{
			priv->audio.bassright = i;
			break;
		}
	}

	priv->audio.bass = (priv->audio.bassleft + priv->audio.bassright) / 2;
	priv->audio.tripple = (priv->audio.trippleleft + priv->audio.trippleright) / 2;
	priv->audio.highest = MAX (priv->audio.bass, priv->audio.tripple);

	if (priv->audio.bass >= 0 && priv->audio.bass <= 2)
		priv->audio.musicmood = 0;
	else if (priv->audio.bass >= 3 && priv->audio.bass <= 6)
		priv->audio.musicmood = 1;
	else if (priv->audio.bass >= 7 && priv->audio.bass <= 10)
		priv->audio.musicmood = 2;
	else
		priv->audio.musicmood = 0;

	priv->audio.beat = FALSE;
	
	if (priv->audio.bass >= 9)
		priv->audio.beat = TRUE;
}

