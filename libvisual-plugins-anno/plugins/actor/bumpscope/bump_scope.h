/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 1999, 2001 Zinx Verituse <zinx@xmms.org>
 *
 * Authors: Zinx Verituse <zinx@xmms.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * Notes: Bumpscope plugin originally from XMMS.
 * 
 * $Id: bump_scope.h,v 1.5 2005/12/20 18:49:13 synap Exp $
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

#include "actor_bumpscope.h"

void __bumpscope_generate_palette (BumpscopePrivate *priv, VisColor *col);
void __bumpscope_generate_phongdat (BumpscopePrivate *priv);
void __bumpscope_render_pcm (BumpscopePrivate *priv, float *data);
void __bumpscope_init (BumpscopePrivate *priv);
void __bumpscope_cleanup (BumpscopePrivate *priv);

