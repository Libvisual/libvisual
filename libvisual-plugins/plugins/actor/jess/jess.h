/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: jess.h,v 1.5 2005/12/20 18:49:12 synap Exp $
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

#ifndef _JESS_JESS_H
#define _JESS_JESS_H

#include <sys/time.h>

#include <libvisual/libvisual.h>

#include "struct.h"
#include "analyser_struct.h"
#include "def.h"

#define BIG_BALL_SIZE 1024

typedef struct {
	struct conteur_struct conteur;
	struct analyser_struct lys;
	float E_old1;
	float E_old2;

	struct timeval start;

	/* Randomize context from libvisual */
	VisRandomContext *rcontext;
	VisPalette *jess_pal;

	VisBuffer *pcm_data1;
	VisBuffer *pcm_data2;
	float pcm_data[2][512];

	uint32_t *table1;
	uint32_t *table2;
	uint32_t *table3;
	uint32_t *table4;
	uint32_t pitch;
	uint32_t video;

	uint8_t dim[256];
	uint8_t dimR[256];
	uint8_t dimG[256];
	uint8_t dimB[256];
	uint8_t *bits;
	uint8_t *pixel;
	uint8_t *buffer;

	int resx;
	int resy;
	int xres2;
	int yres2;

	uint8_t *big_ball;
	uint32_t *big_ball_scale[BIG_BALL_SIZE];

	int xi[FUSEE_MAX];
	int yi[FUSEE_MAX];
	float life[FUSEE_MAX];

	float lifev[256][LINE_MAX];
	float x[256][LINE_MAX];
	float y[256][LINE_MAX];
	float vx[256][LINE_MAX];
	float vy[256][LINE_MAX];

	float lifet[256][LINE_MAX];
	float ssx[256][LINE_MAX];
	float ssy[256][LINE_MAX];
	float ssvx[256][LINE_MAX];
	float ssvy[256][LINE_MAX];
	float sstheta[256][LINE_MAX];
	float ssomega[256][LINE_MAX];
	
	float smpos[2][3][STARS_MAX];
	float smmorpheur;
	int smselect;
	    
} JessPrivate;

#endif /* _JESS_JESS_H */
