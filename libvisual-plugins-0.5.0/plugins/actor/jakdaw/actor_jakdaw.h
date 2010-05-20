/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_jakdaw.h,v 1.7 2006/01/22 13:25:25 synap Exp $
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

#ifndef _ACTOR_JAKDAW_H
#define _ACTOR_JAKDAW_H

#include <libvisual/libvisual.h>

typedef enum {
	PLOTTER_SCOPE_LINES,
	PLOTTER_SCOPE_DOTS,
	PLOTTER_SCOPE_SOLID,
	PLOTTER_SCOPE_NOTHING
} JakdawPlotterOptions;

typedef enum {
	PLOTTER_COLOUR_SOLID,
	PLOTTER_COLOUR_RANDOM,
	PLOTTER_COLOUR_MUSICTRIG
} JakdawPlotterColor;

typedef enum
{
	FEEDBACK_ZOOMRIPPLE,
	FEEDBACK_BLURONLY,
	FEEDBACK_ZOOMROTATE,
	FEEDBACK_SCROLL,
	FEEDBACK_INTOSCREEN,
	FEEDBACK_NEWRIPPLE
} JakdawFeedbackType;

typedef struct {
	int			 xres;
	int			 yres;

	int			 decay_rate;

	JakdawFeedbackType	 zoom_mode;
	double			 zoom_ripplesize;
	double			 zoom_ripplefact;
	double			 zoom_zoomfact;

	float			 plotter_amplitude;
	JakdawPlotterColor	 plotter_colortype;
	int			 plotter_scopecolor;
	JakdawPlotterOptions	 plotter_scopetype;

	/* Feedback privates */
	uint32_t		*table;
	uint32_t		*new_image;
	int			 tableptr;

	/* PCM Buffer */
	VisBuffer		*pcmbuf;

	/* Spectrum buffer */
	VisBuffer		*freqbuf;

	/* Plugin it's random context */
	VisRandomContext	*rcontext;
} JakdawPrivate;

#endif /* _ACTOR_JAKDAW_H */
