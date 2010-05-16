/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: distorsion.h,v 1.4 2005/12/20 18:49:12 synap Exp $
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

#include "jess.h"

void create_tables(JessPrivate *priv);
void rot_hyperbolic_radial(float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy);
void rot_cos_radial( float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy);
void homothetie_hyperbolic(float *n_fx,float *n_fy, float rad_factor, float cx, float cy);
void homothetie_cos_radial(float *n_fx,float *n_fy, float rad_factor, float cx, float cy);
void noize(JessPrivate *priv, float *n_fx,float *n_fy, float intensity);
