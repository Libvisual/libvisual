/* 
 *  Jakdaw's XMMS Plugin.
 *  Copyright (C) 1999, 2000, Christopher Wilson.
 *
 *  Email: <Jakdaw@usa.net> 
 *  Project Homepage: <http://www.jakdaw.ucam.org/xmms/>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 */
#ifndef _PLOTTER_H
#define _PLOTTER_H

#include "actor_jakdaw.h"

void _jakdaw_plotter_draw (JakdawPrivate *priv, float *pcm_data, float *freq_data, uint32_t *vscr);

#endif /* _PLOTTER_H */
