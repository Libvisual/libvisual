/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: struct.h,v 1.3 2005/12/20 18:49:12 synap Exp $
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

#ifndef _JESS_STRUCT_H
#define _JESS_STRUCT_H

#include <libvisual/libvisual.h>

struct conteur_struct  
{
	float angle;
	float angle2;
	float v_angle2;

	float dt;

	uint32_t blur_mode;


	int fps;

	uint32_t courbe;
	uint32_t mix_reprise;
	uint32_t last_flash;
	uint32_t draw_mode;
	uint32_t burn_mode;
	uint32_t k1;
	uint32_t k2;
	uint32_t k3;


	uint32_t general;
	int term_display;
	int fullscreen;
	int psy;
	int analyser;
	int freeze ;
	int freeze_mode ;


	int triplet;
};

#endif /* _JESS_STRUCT_H */
