/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: gfx-blur.h,v 1.4 2006/01/22 13:25:26 synap Exp $
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

#ifndef _OINK_GFX_BLUR_H
#define _OINK_GFX_BLUR_H

#include "oinksie.h"

void _oink_gfx_blur_fade (OinksiePrivate *priv, uint8_t *buf, int fade);
void _oink_gfx_blur_simple (OinksiePrivate *priv, uint8_t *buf);
void _oink_gfx_blur_middle (OinksiePrivate *priv, uint8_t *buf);
void _oink_gfx_blur_midstrange (OinksiePrivate *priv, uint8_t *buf);

#endif /* _OINK_GFX_BLUR_H */
