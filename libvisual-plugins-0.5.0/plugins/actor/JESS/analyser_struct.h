/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: analyser_struct.h,v 1.3 2005/12/20 18:49:12 synap Exp $
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

#ifndef _JESS_ANALYSER_STRUCT_H
#define _JESS_ANALYSER_STRUCT_H

#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5

#define OUI 1
#define NON 0

#define BPM_MAX 400
#define LEVEL_ONE 3 
#define TOTAL_E 0
#define BASS_E 1
#define MEDIUM_E 2
#define HIGH_E 3

#define T_AVERAGE_SPECTRAL 100
#define T_AVERAGE_DEDT 100
#define T_DEDT 10
#define T_AVERAGE_E 130

struct analyser_struct
{
	float E;
	float E_moyen;
	float dEdt;
	float dEdt_moyen;

	float Ed_moyen[256];
	float dt;

	char dbeat[256]; 

	int reprise;
	int montee;
	int beat;
	int last_time[10];
	int conteur[100]; /* ZERO : sert pour l'affichage de appel JESS */
};

#endif /* _JESS_ANALYSER_STRUCT_H */
