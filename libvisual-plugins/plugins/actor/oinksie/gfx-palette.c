/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-palette.c,v 1.6 2006/01/22 13:25:26 synap Exp $
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
#include <string.h>

#include <libvisual/libvisual.h>

#include "common.h"
#include "misc.h"
#include "screen.h"
#include "gfx-palette.h"

void _oink_gfx_palette_init (OinksiePrivate *priv)
{
	priv->pal_startup = 1;
	
	priv->pal_data.fade_start = 1;
	priv->pal_data.fade_steps = 100;
	priv->pal_data.fade_poststeps = priv->pal_data.fade_steps;
	priv->pal_data.fade_poststop = 1;
}

void _oink_gfx_palette_save_old (OinksiePrivate *priv)
{
	/* If we haven't set up an fade yet, don't save the next old.
	 * this protects us from more event handles per screen draw
	 */
	if (priv->pal_data.pal_new == 1 && priv->pal_data.fade_stepsdone == priv->pal_data.fade_steps)
		visual_palette_copy (priv->pal_old, priv->pal_cur);

	priv->pal_data.fade_start = 1;
}

void _oink_gfx_palette_transform (OinksiePrivate *priv)
{
	VisColor *pal_cur_colors = visual_palette_get_colors (priv->pal_cur);
	VisColor *pal_old_colors = visual_palette_get_colors (priv->pal_old);

	int i;

	if (priv->pal_data.fade_start == 1)
	{
		priv->pal_data.fade_stepsdone = 0;
		
		for (i = 0; i < 256; i++)
		{
			priv->pal_fades[i].r = (float) (pal_cur_colors[i].r - pal_old_colors[i].r) / (float) priv->pal_data.fade_steps;
			priv->pal_fades[i].g = (float) (pal_cur_colors[i].g - pal_old_colors[i].g) / (float) priv->pal_data.fade_steps;
			priv->pal_fades[i].b = (float) (pal_cur_colors[i].b - pal_old_colors[i].b) / (float) priv->pal_data.fade_steps;

			priv->pal_fades[i].r_cur = pal_old_colors[i].r;
			priv->pal_fades[i].g_cur = pal_old_colors[i].g;
			priv->pal_fades[i].b_cur = pal_old_colors[i].b;
		}
	
		priv->pal_data.fade_start = 0;
	}

	for (i = 0; i < 256; i++)
	{
		priv->pal_fades[i].r_cur += priv->pal_fades[i].r;
		priv->pal_fades[i].g_cur += priv->pal_fades[i].g;
		priv->pal_fades[i].b_cur += priv->pal_fades[i].b;
	
		pal_old_colors[i].r = priv->pal_fades[i].r_cur;
		pal_old_colors[i].g = priv->pal_fades[i].g_cur;
		pal_old_colors[i].b = priv->pal_fades[i].b_cur;
	}

	priv->pal_data.fade_stepsdone++;

	if (priv->pal_data.fade_stepsdone >= priv->pal_data.fade_poststeps)
	{
		visual_palette_copy (priv->pal_cur, priv->pal_old);

		priv->pal_data.pal_new = 0;
		priv->pal_data.fade_start = 1;
		priv->pal_startup = 0;
	}
}

void _oink_gfx_palette_build (OinksiePrivate *priv, uint8_t funky)
{
	/* save old palette for transistion */
	_oink_gfx_palette_save_old (priv);
	_oink_gfx_palette_build_gradient (priv, funky);

	/* FIXME, changing fadesteps can fuck this up */
	if (priv->pal_data.fade_poststop == 1 && priv->pal_startup == 0) {
		switch (visual_random_context_int_range (priv->rcontext, 0, 5)) {
		case 0:
		case 1:
			priv->pal_data.fade_poststeps = visual_random_context_int_range (priv->rcontext, 60, priv->pal_data.fade_steps);
			break;

		case 2:
			priv->pal_data.fade_poststeps = visual_random_context_int_range (priv->rcontext, 40, priv->pal_data.fade_steps / 2);
			break;

		case 3:
			priv->pal_data.fade_poststeps = visual_random_context_int_range (priv->rcontext, 20, priv->pal_data.fade_steps / 3);
			break;
		case 4:
			priv->pal_data.fade_poststeps = visual_random_context_int_range (priv->rcontext, 80, priv->pal_data.fade_steps);
			break;

		default:
			priv->pal_data.fade_poststeps = priv->pal_data.fade_steps;
			break;
		}

	}

	priv->pal_data.pal_new = 1;	
}

void _oink_gfx_palette_color (OinksiePrivate *priv, int color, int red, int green, int blue)
{
	VisColor *pal_cur_colors = visual_palette_get_colors (priv->pal_cur);

	pal_cur_colors[color].r = red << 2;
	pal_cur_colors[color].g = green << 2;
	pal_cur_colors[color].b = blue << 2;
} 

uint8_t _oink_gfx_palette_gradient_gen (OinksiePrivate *priv, uint8_t i, int mode)
{
	static const float __magic = 2.0 * PI/256;

	switch (mode)
	{
	case 0:
		return (i * i * i) >> 16; 
		break;

	case 1:
		return (i * i) >> 8;
		break;

	case 2:
		return (uint8_t) i;
		break;

	case 3:
		return (uint8_t) (128 * fabs (sin ( __magic * i )));
		break;

	case 4:
		return 0;
		break;

	default:
		break;
	}

	return 0;
}

void _oink_gfx_palette_build_gradient (OinksiePrivate *priv, uint8_t funky)
{
	VisColor *pal_cur_colors = visual_palette_get_colors (priv->pal_cur);

	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
 	
	if (funky == TRUE)
		i = 5;
	else
		i = 3;
	
	do
	{
		j = visual_random_context_int_range (priv->rcontext, 0, i - 1);
		k = visual_random_context_int_range (priv->rcontext, 0, i - 1);
		l = visual_random_context_int_range (priv->rcontext, 0, i - 1);		
		
	} while (j == k || j == l || l == k);
	
	for( i = 0; i <= 255; i++)
	{
		pal_cur_colors[i].r = _oink_gfx_palette_gradient_gen (priv, i, j);
		pal_cur_colors[i].g = _oink_gfx_palette_gradient_gen (priv, i, k);
		pal_cur_colors[i].b = _oink_gfx_palette_gradient_gen (priv, i, l);
	}
}

