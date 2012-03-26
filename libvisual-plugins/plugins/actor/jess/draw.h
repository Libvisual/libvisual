/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: draw.h,v 1.4 2005/12/20 18:49:12 synap Exp $
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

void analyser(JessPrivate *priv, uint8_t * buffer);
void fusee(JessPrivate *priv, uint8_t * buffer, int new);
void super_spectral(JessPrivate *priv, uint8_t * buffer);
void super_spectral_balls(JessPrivate *priv, uint8_t * buffer);
void grille_3d(JessPrivate *priv, uint8_t *buffer, float data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam);
void l2_grilles_3d(JessPrivate *priv, uint8_t *buffer, float data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam);
void burn_3d(JessPrivate *priv, uint8_t *buffer, float data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam, int mode);
void courbes(JessPrivate *priv, uint8_t *buffer, float data[2][512],uint8_t color, int type);
void sinus_rect(JessPrivate *priv, uint8_t *buffer,int k);
void stars_create_state(JessPrivate *priv, float pos[3][STARS_MAX], int mode);
void stars_manage(JessPrivate *priv, uint8_t *buffer, int new,  float alpha, float beta, float gamma, int persp, int dist_cam);
