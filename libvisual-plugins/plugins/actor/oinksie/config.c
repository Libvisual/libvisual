/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2002, 2003, 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: config.c,v 1.7 2006/01/31 16:42:47 synap Exp $
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

#include "common.h"
#include "audio.h"
#include "config-defines.h"
#include "gfx-misc.h"
#include "screen.h"
#include "misc.h"
#include "oink-config.h"

/* random configs */
void _oink_config_random_blurmode (OinksiePrivate *priv)
{
	priv->config.blurmode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_BLURMODES);
}

void _oink_config_random_scopemode (OinksiePrivate *priv)
{
	priv->config.scopemode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_SCOPEMODES);
}

void _oink_config_random_backgroundmode (OinksiePrivate *priv)
{
	priv->config.backgroundmode = visual_random_context_int_range (priv->rcontext, 0, CONFIG_BACKGROUNDMODES);
}

void _oink_config_random_palfunky (OinksiePrivate *priv)
{
	priv->config.palfunky = visual_random_context_int_range (priv->rcontext, 0, 1);
}

