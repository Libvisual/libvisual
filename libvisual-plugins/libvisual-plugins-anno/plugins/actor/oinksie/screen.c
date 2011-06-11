/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: screen.c,v 1.4 2006/01/22 13:25:26 synap Exp $
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
#include <string.h>

#include "oinksie.h"
#include "common.h"
#include "config.h"
#include "gfx-palette.h"
#include "screen.h"

void _oink_screen_init (OinksiePrivate *priv, int width, int height)
{
	_oink_screen_size_set (priv, width, height);
}

void _oink_screen_size_set (OinksiePrivate *priv, int width, int height)
{
	priv->screen_width = width;
	priv->screen_height = height;

	priv->screen_size = priv->screen_width * priv->screen_height;

	priv->screen_halfwidth = priv->screen_width / 2;
	priv->screen_halfheight = priv->screen_height / 2;

	priv->screen_xybiggest = priv->screen_width > priv->screen_height ? priv->screen_width : priv->screen_height;
	priv->screen_xysmallest = priv->screen_width < priv->screen_height ? priv->screen_width : priv->screen_height;
}

