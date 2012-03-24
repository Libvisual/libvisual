/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: audio.c,v 1.6 2006/01/22 13:25:26 synap Exp $
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


/* audio priv->audio.freq analyze */
void _oink_audio_analyse (OinksiePrivate *priv)
{
	priv->audio.bass = (priv->audio.freqsmall[0] + priv->audio.freqsmall[1]) * 20;
	priv->audio.tripple = (priv->audio.freqsmall[2] + priv->audio.freqsmall[3]) * 100;
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

