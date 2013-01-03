/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: oinksie.h,v 1.10 2006/01/22 13:25:26 synap Exp $
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

#ifndef _OINK_OINKSIE_H
#define _OINK_OINKSIE_H

#include <time.h>

#include <libvisual/libvisual.h>

/* FIXME indent all likewise eventually */
typedef struct {
	float r;
	float g;
	float b;
	float r_cur;
	float g_cur;
	float b_cur;
} OinksiePalFade;

typedef struct {
	int pal_new;

	int fade_start;
	int fade_steps;
	int fade_stepsdone;
	int fade_poststeps;
	int fade_poststop;
} OinksiePalData;

typedef struct {
	int blurmode;
	int scopemode;
	int backgroundmode;

	int audiodebug;

	int palfunky;

	int scenenew;

	int acidpalette;
} OinksieConfig;

typedef struct {
	int bass;

	int tripple;

	int highest;

	float pcm[3][512];
	float freq[2][256];
	float freqsmall[4];

	int musicmood;
	char beat;
	int energy;

	int volume;
} OinksieAudio;

typedef struct {
	int scopestereo_enabled;
	int scopestereo_start;
	int scopestereo_space;
	float scopestereo_adder;

	/* shooting balls vars */
	int ball_enabled;
	int ball_xstart;
	int ball_ystart;
	int ball_distance;
	int ball_adder;
	int ball_spread;

	/* turning circles vars */
	int circles_enabled;
	int circles_direction;
	int circles_nr;
	int circles_turn;

	/* ball sine */
	int ballsine_enabled;
	int ballsine_scroll;
	int ballsine_rotate;
	int ballsine_infade;
	int ballsine_direction;

        /* floaters vars */
	int floaters_direction;
	int floaters_turn;

	int rotate;
} OinksieScene;

typedef struct {
	/* Major vars */
	uint8_t			*drawbuf;

	/* Palette vars */
	OinksiePalFade		 pal_fades[256];
	OinksiePalData		 pal_data;
	VisPalette		*pal_cur;
	VisPalette		*pal_old;
	int			 pal_startup;

	/* Screen vars */
	int			 screen_size;
	int			 screen_width;
	int			 screen_height;
	int			 screen_halfwidth;
	int			 screen_halfheight;
	int			 screen_xybiggest;
	int			 screen_xysmallest;

	time_t			 timing;
	time_t			 timing_prev;

	/* Config vars */
	OinksieConfig		 config;
	OinksieAudio		 audio;
	OinksieScene		 scene;

	/* Random context */
	VisRandomContext	*rcontext;
} OinksiePrivate;

void oinksie_init (OinksiePrivate *priv, int width, int height);
void oinksie_quit (OinksiePrivate *priv);

void oinksie_sample (OinksiePrivate *priv);

void oinksie_palette_change (OinksiePrivate *priv, uint8_t funky);

void oinksie_render (OinksiePrivate *priv);
int oinksie_size_set (OinksiePrivate *priv, int width, int height);

VisPalette *oinksie_palette_get (OinksiePrivate *priv);

#endif /* _OINK_OINKSIE_H */
