/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_gfx.h,v 1.4 2006-09-19 19:05:46 synap Exp $
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

#ifndef _LV_AVS_GFX_H
#define _LV_AVS_GFX_H

#include <libvisual/libvisual.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_GFX_COLOR_CYCLER(obj)			(VISUAL_CHECK_CAST ((obj), AVSGfxColorCycler))


typedef struct _AVSGfxColorCycler AVSGfxColorCycler;

typedef enum {
	AVS_GFX_COLOR_CYCLER_TYPE_SET,
	AVS_GFX_COLOR_CYCLER_TYPE_TIME
} AVSGfxColorCyclerType;

struct _AVSGfxColorCycler {
	VisPalette		*pal;

	float			 rate;
	float			 timedrate;
	
	int			 curcolor;

	VisTime			 morphtime;
	VisTimer		 timer;

	AVSGfxColorCyclerType	 type;
};

AVSGfxColorCycler *avs_gfx_color_cycler_new (VisPalette *pal);
int avs_gfx_color_cycler_set_rate (AVSGfxColorCycler *cycler, float rate);
int avs_gfx_color_cycler_set_time (AVSGfxColorCycler *cycler, VisTime *time);
int avs_gfx_color_cycler_set_mode (AVSGfxColorCycler *cycler, AVSGfxColorCyclerType type);
VisColor *avs_gfx_color_cycler_run (AVSGfxColorCycler *cycler);


int avs_gfx_line_non_naieve_floats (VisVideo *video, float x1, float y1, float x2, float y2, VisColor *col);
int avs_gfx_line_non_naieve_ints (VisVideo *video, int x1, int y1, int x2, int y2, VisColor *col);
int avs_gfx_line_floats (VisVideo *video, float x1, float y1, float x2, float y2, VisColor *col);
int avs_gfx_line_ints (VisVideo *video, int x1, int y1, int x2, int y2, VisColor *col);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AVS_GFX_H */
