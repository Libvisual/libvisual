/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Antti Silvast <asilvast@iki.fi>
 *
 * Authors: Antti Silvast <asilvast@iki.fi>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: main.h,v 1.3 2006/01/22 13:25:26 synap Exp $
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

#ifndef _FLOWER_MAIN_H
#define _FLOWER_MAIN_H

#include <libvisual/libvisual.h>

typedef float Vect[3];

typedef struct _FlowerInternal FlowerInternal;

struct _FlowerInternal {
	float tension;
	float continuity;
	float bias;
	float tension_new;
	float continuity_new;
	float bias_new;

	float rotx;
	float roty;

	float spd;

	float posz;
	float posz_new;

	float audio_strength;

	int width;
	int height;

	Vect kukka[7];

	Vect kukka_morph[7];

	float audio_bars[256];

	VisTimer *timer;
};

/* Prototypes */
int init_flower (FlowerInternal *flower);
void render_flower_effect (FlowerInternal *flower);

#endif /* _FLOWER_MAIN_H */
