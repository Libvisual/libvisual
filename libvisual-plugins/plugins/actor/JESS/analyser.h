/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: analyser.h,v 1.4 2006/01/30 19:06:46 synap Exp $
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

int detect_beat(JessPrivate *priv);
void spectre_moyen(JessPrivate *priv, short data_freq_tmp[2][256]);
void C_dEdt_moyen(JessPrivate *priv);
void C_dEdt(JessPrivate *priv);
void C_E_moyen(JessPrivate *priv, short data_freq_tmp[2][256]);
float energy(JessPrivate *priv, short data_freq_tmp[2][256], int type_E);
int start_ticks(JessPrivate *priv);
float time_last(JessPrivate *priv, int i, int reinit);
void ips(JessPrivate *priv);
