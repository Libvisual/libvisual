/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: scene.c,v 1.10 2006/01/22 13:25:26 synap Exp $
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

#include "oinksie.h"
#include "common.h"
#include "screen.h"
#include "table.h"
#include "gfx-palette.h"
#include "gfx-scope.h"
#include "gfx-blur.h"
#include "gfx-misc.h"
#include "gfx-analyzer.h"
#include "gfx-background.h"
#include "oink-config.h"
#include "misc.h"
#include "scene.h"

static void _oink_scene_scope_special (OinksiePrivate *priv, uint8_t *buf)
{
	if (visual_random_context_int_range (priv->rcontext, 0, 500) == 42)
	{
		priv->scene.scopestereo_adder = priv->screen_halfheight / 100;
		priv->scene.scopestereo_start = 1 - priv->scene.scopestereo_start;
		priv->scene.scopestereo_space = priv->screen_halfheight + 10;

		if (priv->scene.scopestereo_start == TRUE)
			priv->scene.scopestereo_enabled = TRUE;
	}

	if (priv->scene.scopestereo_enabled && priv->config.scopemode != 2 && priv->config.scopemode != 6)
	{
		if (priv->scene.scopestereo_start == TRUE)
		{
			priv->scene.scopestereo_space -= priv->scene.scopestereo_adder;
			_oink_gfx_scope_stereo (priv, buf, 235, 235 - (priv->audio.bass * 2), priv->screen_height / 6, priv->scene.scopestereo_space, 0);

			if (priv->scene.scopestereo_space < priv->screen_halfheight)
				priv->scene.scopestereo_start = FALSE;
		}
		else
		{
			_oink_gfx_scope_stereo (priv, buf, 235, 235 - (priv->audio.bass * 2), priv->screen_height / 6, priv->screen_halfheight, 0);
		}
	}
}

void _oink_scene_background_special (OinksiePrivate *priv, uint8_t *buf)
{
	if (priv->audio.beat == TRUE)
	{
		if (visual_random_context_int_range (priv->rcontext, 0, 140) == 42 && priv->scene.ball_enabled == FALSE)
		{
			priv->scene.ball_enabled = TRUE;

			priv->scene.ball_xstart = visual_random_context_int_range (priv->rcontext, 0, priv->screen_width - 1);
			priv->scene.ball_ystart = priv->screen_height;

			priv->scene.ball_distance = _oink_line_length (priv->screen_halfheight, priv->scene.ball_ystart,
						     priv->screen_halfwidth, priv->scene.ball_xstart);

			/* FIXME use fps data instead of '25' here */
			priv->scene.ball_adder = (priv->scene.ball_distance / (25 + 1)) + 1;
		}

		if (visual_random_context_int_range (priv->rcontext, 0, 5) == 4)
			priv->scene.circles_direction = 1 - priv->scene.circles_direction;
	}

	if (priv->scene.circles_direction == 0)
		priv->scene.circles_turn += priv->audio.bass * 4;
	else
		priv->scene.circles_turn -= priv->audio.bass * 4;

	if (visual_random_context_int_range (priv->rcontext, 0, 450) == 42)
	{
		priv->scene.ballsine_enabled = 1 - priv->scene.ballsine_enabled;
		priv->scene.ballsine_infade = 0;
	}

	if (visual_random_context_int_range (priv->rcontext, 0, 160) == 42)
		priv->scene.ballsine_direction = 1 - priv->scene.ballsine_direction;

	if (priv->scene.ball_enabled == TRUE)
	{
		_oink_gfx_background_ball_shooting (priv, buf, 250, priv->scene.ball_distance,
				priv->scene.ball_xstart, priv->scene.ball_ystart,
				priv->screen_halfwidth, priv->screen_halfheight);

		priv->scene.ball_distance -= priv->scene.ball_adder;

		if (priv->scene.ball_distance < 0)
		{
			priv->scene.ball_enabled = FALSE;
		}
	}

	if (priv->scene.circles_enabled == TRUE)
	{
		if (priv->audio.energy > ((priv->screen_xysmallest / 10) + 2) / 2)
		{
			if (priv->audio.energy > (priv->screen_xysmallest / 2) - ((priv->screen_xysmallest / 10) + 2))
			{
				priv->scene.ball_spread = (priv->screen_xysmallest / 2) - ((priv->screen_xysmallest / 10) + 2);
			}
			else
			{
				if (priv->screen_xysmallest / 2 > 100)
				{
					priv->scene.ball_spread = (int) ((float) ((priv->audio.energy * (float) 
						 (priv->screen_xysmallest / 100.00)) / 2))
						 - ((priv->screen_xysmallest / 10) + 2);
				}
				else
				{
					priv->scene.ball_spread = priv->audio.energy;
				}
			}
		}
		else
		{
			priv->scene.ball_spread = (priv->screen_xysmallest / 10) + 2;
		}

		_oink_gfx_background_circles_filled (priv, buf, 250, priv->screen_xysmallest / 10, 5, priv->scene.ball_spread,
					       priv->scene.circles_turn, priv->screen_halfwidth, priv->screen_halfheight);

	}
/*
	if (priv->scene.ballsine_enabled == TRUE)
	{
		if (priv->audio.beat == 1 && visual_random_context_int_range (priv->rcontext, 0, 42) == 0)
			priv->scene.ballsine_infade = 0;

		if (priv->scene.ballsine_infade < 240)
			priv->scene.ballsine_infade += 10;

		_oink_gfx_background_circles_sine (priv, priv->drawbuf, priv->scene.ballsine_infade,
				priv->scene.ballsine_rotate, priv->scene.ballsine_scroll, 80, 50);

		if (priv->scene.ballsine_direction == 0)
		{
			priv->scene.ballsine_scroll += ((priv->audio.energy >> 3) + 1);
			priv->scene.ballsine_rotate += ((priv->audio.energy >> 3) + 1);
		}
		else
		{
			priv->scene.ballsine_scroll -= ((priv->audio.energy >> 3) + 1);
			priv->scene.ballsine_rotate -= ((priv->audio.energy >> 3) + 1);
		}
	}
	*/
}

void _oink_scene_background_select (OinksiePrivate *priv, uint8_t *buf)
{
	if (visual_random_context_int_range (priv->rcontext, 0, 5) == 4 && priv->audio.beat == TRUE)
		priv->scene.floaters_direction = 1 - priv->scene.floaters_direction;

	if (priv->scene.floaters_direction == 0)
		priv->scene.floaters_turn += priv->audio.bass * 4;
	else
		priv->scene.floaters_turn -= priv->audio.bass * 4;


	switch (priv->config.backgroundmode)
	{
		case 0:
			_oink_gfx_background_floaters (priv, buf, 245, 5, 6, 0, priv->scene.floaters_turn,
					priv->screen_height - (priv->screen_height / 4), 0, priv->audio.energy); 
			break;

		case 1:
			_oink_gfx_background_floaters (priv, buf, 245, 10, 4, 0, priv->scene.floaters_turn * 2,
					priv->screen_halfheight, 0, priv->audio.energy);

			_oink_gfx_background_floaters (priv, buf, 245, 5, 6, 0, priv->scene.floaters_turn,
					priv->screen_height - (priv->screen_height / 4), 0, priv->audio.energy);
			break;

/*		case 2:
			_oink_gfx_background_circles_star (priv, priv->drawbuf, 242,
					priv->screen_xysmallest / 4,
					5, 6, priv->audio.bass * 3, priv->audio.tripple * 30,
					priv->screen_halfwidth, priv->screen_halfheight);
			break;
*/
		default:
			break;
	}

}

void _oink_scene_blur_select (OinksiePrivate *priv, uint8_t *buf)
{
	switch (priv->config.blurmode)
	{
		case 0:
			_oink_gfx_blur_midstrange (priv, buf);
			break;

		case 1:
			_oink_gfx_blur_middle (priv, buf);
			break;

		case 2:
			_oink_gfx_blur_midstrange (priv, buf);
			break;

		default:
			_oink_gfx_blur_midstrange (priv, buf);
			break;
	}
}

void _oink_scene_scope_select (OinksiePrivate *priv, uint8_t *buf, int color, int height)
{
	switch (priv->config.scopemode)
	{
		case 0:
		case 1:
			_oink_gfx_scope_normal (priv, buf, color, height);
			break;

		case 2:
			_oink_gfx_analyzer_stereo (priv, priv->drawbuf, color, priv->screen_height - 20);
			break;

		case 3:
		case 4:
			_oink_gfx_scope_bulbous (priv, buf, color, height);
			break;

		case 5:
			_oink_gfx_scope_circle (priv, buf, 250, MIN (priv->screen_width, priv->screen_height) / 4,
					priv->screen_halfwidth, priv->screen_halfheight);
			break;

		case 6:
			priv->scene.rotate += 10;
			_oink_gfx_scope_stereo (priv, buf, 250, 250,
					priv->screen_height / 6, priv->screen_halfheight + (priv->screen_halfheight / 2),
					priv->scene.rotate);
			break;

	case 7:
		priv->scene.rotate += 2;
		_oink_gfx_scope_stereo (priv, buf, 250, 250, priv->screen_height / 6, priv->screen_halfheight + (priv->screen_halfheight / 2), 
				    (int) (_oink_table_sin[priv->scene.rotate % OINK_TABLE_NORMAL_SIZE] * 150) + 600);
		break;

	default:
		_oink_gfx_scope_bulbous (priv, buf, color, height);
		break;
	}
}

void _oink_scene_randomize (OinksiePrivate *priv)
{
	_oink_config_random_scopemode (priv);
	_oink_config_random_blurmode (priv);
	_oink_config_random_backgroundmode (priv);

	_oink_gfx_palette_build (priv, priv->config.acidpalette);
}

void _oink_scene_render (OinksiePrivate *priv)
{
	time (&priv->timing);

	if (priv->drawbuf == NULL)
		return;

	if (priv->config.scenenew == TRUE)
		_oink_scene_randomize (priv);

	priv->config.scenenew = FALSE;

	if (priv->audio.beat == TRUE)
	{
		if (visual_random_context_int_range (priv->rcontext, 0, 50) == 0)
			_oink_config_random_scopemode (priv);

		if (visual_random_context_int_range (priv->rcontext, 0, 40) == 0)
			_oink_config_random_blurmode (priv);

		if (visual_random_context_int_range (priv->rcontext, 0, 20) == 0)
			_oink_gfx_palette_build (priv, priv->config.acidpalette);
	}

	_oink_gfx_blur_fade (priv, priv->drawbuf, priv->audio.bass / 2);

	_oink_scene_background_select (priv, priv->drawbuf);

	if (visual_random_context_int_range (priv->rcontext, 0, 500) == 42) 
		_oink_scene_randomize (priv);

	switch (priv->audio.musicmood)
	{
		case 0:
			_oink_scene_scope_select (priv, priv->drawbuf, 245, priv->screen_height / 4); 
			break;

		case 1:
			_oink_scene_scope_select (priv, priv->drawbuf, priv->audio.bass * 21, priv->screen_height / 4);
			break;

		case 2:
			_oink_scene_scope_select (priv, priv->drawbuf, priv->audio.bass * 14, priv->screen_height / 4);
			break;

		default:
			break;
	}

	_oink_scene_scope_special (priv, priv->drawbuf);
	_oink_scene_background_special (priv, priv->drawbuf);
	_oink_scene_blur_select (priv, priv->drawbuf);

	priv->timing_prev = priv->timing;
}

