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

	int			 plotter_amplitude;
	JakdawPlotterColor	 plotter_colortype;
	int			 plotter_scopecolor;
	JakdawPlotterOptions	 plotter_scopetype;

	/* Plotter privates */
	int			*xlat_table;
	int			*amplitude_table;
	int			 shifts;

	/* Feedback privates */
	uint32_t		*table;
	uint32_t		*new_image;
	int			 tableptr;

	/* Plugin it's random context */
	VisRandomContext	*rcontext;
} JakdawPrivate;

#endif /* _ACTOR_JAKDAW_H */
