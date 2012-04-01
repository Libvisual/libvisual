/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: oinksie.c,v 1.5 2006/01/22 13:25:26 synap Exp $
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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "audio.h"
#include "table.h"
#include "screen.h"
#include "gfx-palette.h"
#include "scene.h"
#include "oinksie.h"

void oinksie_init (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_init (priv, width, height);
	_oink_table_init ();
	_oink_gfx_palette_init (priv);
	_oink_scene_randomize (priv);
}

void oinksie_quit (OinksiePrivate *priv)
{

}

void oinksie_sample (OinksiePrivate *priv)
{
	_oink_audio_analyse (priv);
}

void oinksie_render (OinksiePrivate *priv)
{
	_oink_scene_render (priv);
}

void oinksie_palette_change (OinksiePrivate *priv, uint8_t funky)
{
	_oink_gfx_palette_build (priv, funky);
}

int oinksie_size_set (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_size_set (priv, width, height);

	return 0;
}

VisPalette *oinksie_palette_get (OinksiePrivate *priv)
{
	/* return values for transformation */
	if (priv->pal_data.pal_new == 1)
	{
		_oink_gfx_palette_transform (priv);

		return priv->pal_old;
	}
	else
	{
		return priv->pal_cur;
	}
}

