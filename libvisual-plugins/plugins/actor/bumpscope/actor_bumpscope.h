/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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

#ifndef _LV_ACTOR_BUMPSCOPE_H
#define _LV_ACTOR_BUMPSCOPE_H

#include <libvisual/libvisual.h>

typedef struct {
	int			 height;
	int			 width;

	VisPalette		*pal;

	float			 intense1[256];
	float			 intense2[256];

	int			 phongres;
	uint8_t			*phongdat;

	uint8_t			*rgb_buf;
	uint8_t			*rgb_buf2;

	VisVideo		*video;

	float			 h;
	float			 s;
	float			 v;

	int			 colorchanged;
	int			 colorupdate;

	/* Configs */
	VisColor		 color;
	VisColor		 old_color;
	int			 color_cycle;
	int			 moving_light;
	int			 diamond;
	int			 light_x;
	int			 light_y;

	VisBuffer		*pcmbuf;

	/* Random context for the plugin */
	VisRandomContext	*rcontext;

} BumpscopePrivate;

#endif /* _LV_ACTOR_BUMPSCOPE_H */
