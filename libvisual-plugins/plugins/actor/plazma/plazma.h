/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Pascal Brochart <p.brochart@libertysurf.fr>
 *
 * Authors: Pascal Brochart <p.brochart@libertysurf.fr>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: plazma.h,v 1.3 2005-12-20 18:49:14 synap Exp $
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

#ifndef _PLAZMA_H
#define _PLAZMA_H

#include "actor_plazma.h"

void _plazma_init (PlazmaPrivate *priv);
void _plazma_run (PlazmaPrivate *priv);
void _plazma_cleanup (PlazmaPrivate *priv);
void _plazma_change_effect (PlazmaPrivate *priv);

#endif /* _PLAZMA_H */
