/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Antti Silvast <asilvast@iki.fi>
 *
 * Authors: Antti Silvast <asilvast@iki.fi>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_flower.c,v 1.11 2006/02/25 18:45:16 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <gettext.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <libvisual/libvisual.h>

#include "main.h"
#include "notch.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (void);


#define NOTCH_BANDS	32

typedef struct {
	VisTimer		*t;
	FlowerInternal		 flower;
	int			 nof_bands;
	NOTCH_FILTER		*notch[NOTCH_BANDS];
	VisRandomContext	*rcxt;
} FlowerPrivate;

static int lv_flower_init (VisPluginData *plugin);
static int lv_flower_cleanup (VisPluginData *plugin);
static int lv_flower_requisition (VisPluginData *plugin, int *width, int *height);
static int lv_flower_resize (VisPluginData *plugin, int width, int height);
static int lv_flower_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *lv_flower_palette (VisPluginData *plugin);
static int lv_flower_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

/* Main plugin stuff */
const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_flower_requisition,
		.palette = lv_flower_palette,
		.render = lv_flower_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "flower",
		.name = "libvisual Pseudotoad flower, yellow rose of texas",
		.author = N_("Original by: Antti Silvast <asilvast@iki.fi>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.1",
		.about = N_("Libvisual yellow rose of texas port"),
		.help =  N_("This renders an awesome responsive flower"),
		.license = VISUAL_PLUGIN_LICENSE_GPL,

		.init = lv_flower_init,
		.cleanup = lv_flower_cleanup,
		.events = lv_flower_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return &info;
}

static int lv_flower_init (VisPluginData *plugin)
{
	FlowerPrivate *priv;
	float b;
	int i;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (FlowerPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->rcxt = visual_plugin_get_random_context (plugin);

	visual_random_context_float (priv->rcxt);
	init_flower (&priv->flower);

	priv->flower.rotx = visual_random_context_float (priv->rcxt) * 360.0;
	priv->flower.roty = visual_random_context_float (priv->rcxt) * 360.0;

	priv->flower.tension = (visual_random_context_float (priv->rcxt) - 0.5) * 8.0;
	priv->flower.continuity = (visual_random_context_float (priv->rcxt) - 0.5) * 16.0;

	priv->flower.timer = visual_timer_new ();

	priv->nof_bands = NOTCH_BANDS;

	for (i = 0; i < priv->nof_bands; i++) {
		b = 80.0 + i * (22000.0 - 80.0) / priv->nof_bands;

		priv->notch[i] = init_notch (b);
	}

	priv->t = visual_timer_new ();

	return 0;
}

static int lv_flower_cleanup (VisPluginData *plugin)
{
	FlowerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_timer_free (priv->flower.timer);
	visual_timer_free (priv->t);
	visual_mem_free (priv);

	return 0;
}

static int lv_flower_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;

	return 0;
}

static int lv_flower_resize (VisPluginData *plugin, int width, int height)
{
	FlowerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	GLfloat ratio;

	ratio = (GLfloat) width / (GLfloat) height;

	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (45.0, ratio, 0.1, 100.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	priv->flower.width = width;
	priv->flower.height = height;

	return 0;
}

static int lv_flower_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_flower_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *lv_flower_palette (VisPluginData *plugin)
{
	return NULL;
}

static int lv_flower_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	FlowerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer pcmbuf;
	VisBuffer freqbuf;
	float pcm[512];
	float freqnorm[256];
	float temp_bars[NOTCH_BANDS];
	float f;
	int b;
	int i;

	visual_buffer_set_data_pair (&pcmbuf, pcm, sizeof (pcm));
	visual_buffer_set_data_pair (&freqbuf, freqnorm, sizeof (freqnorm));

	visual_audio_get_sample_mixed_simple (audio, &pcmbuf, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (&freqbuf, &pcmbuf, TRUE);

	/* Activate the effect change timer */
	if (!visual_timer_is_active (priv->t))
		visual_timer_start (priv->t);

	/* At 15 secs, do with new settings, reset timer */
	if (visual_timer_is_past2 (priv->t, 15, 0)) {
		priv->flower.tension_new = (-visual_random_context_float (priv->rcxt)) * 12.0;
		priv->flower.continuity_new = (visual_random_context_float (priv->rcxt) - 0.5) * 32.0;

		visual_timer_start (priv->t);
	}

	/* Activate global timer */
	if (!visual_timer_is_active (priv->flower.timer))
		visual_timer_start (priv->flower.timer);

	for (b=0; b<priv->nof_bands; b++)
		temp_bars[b]=0.0;

	for (i=0; i<256; i++) {
		for (b=0; b<priv->nof_bands; b++) {
			f=process_notch (priv->notch[b], freqnorm[i] * 15);
			if (fabs(f)>temp_bars[b])
				temp_bars[b]=fabs(f);
		}
	}


	/* Not part of the if !!! */
	{
#define HEIGHT 1.0
#define D 0.45
#define TAU 0.25
#define DIF 5.0
		float lk=2.0;
		float l0=2.0;

		float scale = HEIGHT / ( log((1 - D) / D) * 2 );
		float x00 = D*D*1.0/(2 * D - 1);
		float y00 = -log(-x00) * scale;
		float y;
		int i;
		for (i=0; i<priv->nof_bands; i++) {
			y=temp_bars[i * 8];
			y=y*(i*lk+l0);

			y = ( log(y - x00) * scale + y00 ); /* Logarithmic amplitude */

			y = ( (DIF-2.0)*y +
					(i==0  ? 0 : temp_bars[i - 1]) +
					(i==31 ? 0 : temp_bars[i + 1])) / DIF;

			y=((1.0-TAU)*priv->flower.audio_bars[i]+TAU*y) * 1.00;
			priv->flower.audio_bars[i]=y;
		}
	}


	priv->flower.roty += (priv->flower.audio_bars[15]) * 0.6;
	priv->flower.rotx += (priv->flower.audio_bars[1]) * 0.7;
	priv->flower.posz = +1;

	render_flower_effect (&priv->flower);

	return 0;
}

