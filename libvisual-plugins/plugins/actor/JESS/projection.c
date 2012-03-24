/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: projection.c,v 1.3 2005/12/20 18:49:12 synap Exp $
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

#include <math.h>
#include "projection.h"

void rotation_3d (float *x, float *y, float *z, float alpha, float beta,
		float gamma)
{
	float nx, ny;
	float cosga = cos (gamma), singa = sin (gamma), cosal = cos (alpha), sinal =
		sin (alpha);


	nx = cosga * *x - singa * *y;
	ny = cosal * (singa * *x + cosga * *y) - sinal * *z;
	*z = sinal * (singa * *x + cosga * *y) + cosal * *z;

	*x = nx;
	*y = ny;

}

void perspective (float *x, float *y, float *z, int persp, int dist_cam)
{
	float foc_cam = 100,aux;
	aux = (foc_cam + persp) / (dist_cam + persp - *z);
	*x = *x * aux;
	*y = *y * aux;
}

