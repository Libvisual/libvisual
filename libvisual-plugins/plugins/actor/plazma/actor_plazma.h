/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_plazma.h,v 1.6 2006-01-22 13:25:26 synap Exp $
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

#ifndef _ACTOR_PLAZMA_H
#define _ACTOR_PLAZMA_H

#include <libvisual/libvisual.h>

typedef struct {
	int	 i;
	float	*f;
} SinCos;

typedef struct {
	float		 pcm_buffer[1024];
	float		 render_buffer[512];

	int		 bass;
	int		 state;
	int		 old_state;

	int		 width;
	int		 height;

	int		 tablex;
	int		 tabley;

	/* General */
	int		 bass_sensibility;
	int		 effect;
	int		 options;
	int		 lines;
	int		 spectrum;
	int		 use_3d;

	/* Data */
	float		 k_put;
	float		 rot_tourni;
	uint16_t	 val_maxi;
	int		 chcol0;
	double		 R[4];
	float		 compt_grille_3d;
	float		 compt_cercle_3d;

	/* Plazma tables */
	uint8_t		*tmem;
	uint8_t		*tmem1;

	/* Sin tables */
	SinCos		 cosw;
	SinCos		 sinw;

	/* Libvisual data */
	VisVideo	*video;
	uint8_t		*pixel;
	VisPalette  *colors;

} PlazmaPrivate;

#endif /* _ACTOR_PLAZMA_H */
