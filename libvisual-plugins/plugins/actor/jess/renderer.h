/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: renderer.h,v 1.3 2005/12/20 18:49:12 synap Exp $
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

int nice(int inc);  
void *renderer(JessPrivate *priv);
void fade(float variable, uint8_t * dim);
void draw_mode(JessPrivate *priv, int mode);
void on_beat(JessPrivate *priv, int beat);
void on_reprise(JessPrivate *priv);
void copy_and_fade(JessPrivate *priv, float factor);
void render_deformation(JessPrivate *priv, int defmode);
void render_blur(JessPrivate *priv, int blur);
void manage_dynamic_and_states_open(JessPrivate *priv);
void manage_states_close(JessPrivate *priv);
