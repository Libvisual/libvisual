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

#include "common.h"
#include "gfx-blur.h"
#include "gfx-misc.h"
#include "screen.h"

void _oink_gfx_blur_fade (OinksiePrivate *priv, uint8_t *buf, int fade)
{
	int i;
	uint8_t valuetab[256];

	for (i = 0; i < 256; i++)
		valuetab[i] = (i - fade) > 0 ? i - fade : 0;

	for (i = 0; i < priv->screen_size; i++)
		buf[i] = valuetab[buf[i]];
}

void _oink_gfx_blur_simple (OinksiePrivate *priv, uint8_t *buf)
{
	int i;

	for (i = 0; i < (priv->screen_size - priv->screen_width - 1); i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2] + buf[i + priv->screen_width]
			  + buf[i + 1 + (priv->screen_width)]) >> 2;
	}

	for (i = (priv->screen_size - priv->screen_width - 1); i < priv->screen_size - 2; i++)
	{
		buf[i] = (buf[i + 1] + buf[i + 2]) >> 1;
	}
}

void _oink_gfx_blur_middle (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;

	for (i = 0; i < scrsh; i++)
	{
		buf[i] = (buf[i]
		         + buf[i + priv->screen_width]
			 + buf[i + priv->screen_width + 1]
			 + buf[i + priv->screen_width - 1]) >> 2;
	}

	for (i = priv->screen_size - 1; i > scrsh; i--)
	{
		buf[i] = (buf[i] 
			 + buf[i - priv->screen_width]
			 + buf[i - priv->screen_width + 1]
			 + buf[i - priv->screen_width - 1]) >> 2;
	}
}

void _oink_gfx_blur_midstrange (OinksiePrivate *priv, uint8_t *buf)
{
	int i;
	int scrsh = priv->screen_size / 2;
	
	for (i = scrsh; i > 0; i--)
	{
		buf[i] = (buf[i] 
		   	  + buf[i + priv->screen_width]
			  + buf[i + priv->screen_width + 1]
			  + buf[i + priv->screen_width - 1]) >> 2;
	}
	
	for (i = scrsh; i < priv->screen_size - 2; i++)
	{
		buf[i] = (buf[i]
			  + buf[i - priv->screen_width]
			  + buf[i - priv->screen_width + 1]
			  + buf[i - priv->screen_width - 1]) >> 2;
	}
}

