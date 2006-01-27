/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_finespectrum.c,v 1.2 2006-01-27 20:19:14 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <math.h>

#include <libvisual/libvisual.h>

/* Linearity of the amplitude scale (0.5 for linear, keep in [0.1, 0.9]) */
#define d 0.33

/* Time factor of the band dinamics. 3 means that the coefficient of the
 * last value is half of the current one's. (see source) */
#define tau 3

/* Factor used for the diffusion. 4 means that half of the height is
 * added to the neighbouring bars */
#define dif 4

#define BARS 256

typedef struct {
	VisPalette	pal;

	int		bar_heights[BARS];
} FinespectrumPrivate;

static void draw_bar (FinespectrumPrivate *priv, VisVideo *video, int index, int nbars, float amplitude);
static inline void draw_vline (VisVideo *video, int x1, int x2, int y, uint8_t color);

int actor_finespectrum_init (VisPluginData *plugin);
int actor_finespectrum_cleanup (VisPluginData *plugin);
int actor_finespectrum_requisition (VisPluginData *plugin, int *width, int *height);
int actor_finespectrum_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int actor_finespectrum_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *actor_finespectrum_palette (VisPluginData *plugin);
int actor_finespectrum_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = actor_finespectrum_requisition,
		.palette = actor_finespectrum_palette,
		.render = actor_finespectrum_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "finespectrum",
		.name = "libvisual finespectrum",
		.author = N_("Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "1.0",
		.about = N_("Libvisual finespectrum plugin"),
		.help = N_("A spectrum analyzer plugin."),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = actor_finespectrum_init,
		.cleanup = actor_finespectrum_cleanup,
		.events = actor_finespectrum_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int actor_finespectrum_init (VisPluginData *plugin)
{
	FinespectrumPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (FinespectrumPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

int actor_finespectrum_cleanup (VisPluginData *plugin)
{
	FinespectrumPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_mem_free (priv);

	return 0;
}

int actor_finespectrum_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw;

	reqw = *width;

	while (reqw % 2 || reqw % 4)
		reqw--;

	if (reqw < 32)
		reqw = 32;

	*width = reqw;

	return 0;
}

int actor_finespectrum_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int actor_finespectrum_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				actor_finespectrum_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *actor_finespectrum_palette (VisPluginData *plugin)
{
	FinespectrumPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = 0;
		priv->pal.colors[i].g = 0;
		priv->pal.colors[i].r = 0;
	}

	for (i = 1; i < 64; i++) {
		priv->pal.colors[i].r = i * 4;
		priv->pal.colors[i].g = 255;
		priv->pal.colors[i].b = 0;

		priv->pal.colors[i + 63].r = 255;
		priv->pal.colors[i + 63].g = (63 - i) * 4;
		priv->pal.colors[i + 63].b = 0;
	}

	return &priv->pal;
}

int actor_finespectrum_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	FinespectrumPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer buffer;
	VisBuffer pcmb;
	float freq[BARS];
	float pcm[BARS * 2];
	float scale;
	float x00;
	float y00;
	float y;
	int i;

	visual_video_fill_color (video, NULL);

	visual_buffer_set_data_pair (&buffer, freq, sizeof (freq));
	visual_buffer_set_data_pair (&pcmb, pcm, sizeof (pcm));

	visual_audio_get_sample_mixed_simple (audio, &pcmb, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (&buffer, &pcmb, FALSE);

	/* Init */
	scale = video->height / ( log((1 - d) / d) * 2 );
	x00 = d*d*32768.0/(2 * d - 1);
	y00 = -log(-x00) * scale;

	/* Work */
	for (i = 0; i < BARS; i++) {
		y = (double)(freq[i] * 3200) * (i + 1); /* Compensating the energy */
		printf ("%f ", y);
		y = ( log(y - x00) * scale + y00 ); /* Logarithmic amplitude */

		y = ( (dif-2)*y + /* FIXME: conditionals should be rolled out of the loop */
				(i==0       ? y : priv->bar_heights[i-1]) +
				(i==BARS-1 ? y : priv->bar_heights[i+1])) / dif; /* Add some diffusion */
		y = ((tau-1)*priv->bar_heights[i] + y) / tau; /* Add some dynamics */
		priv->bar_heights[i] = (int16_t)y;
//		printf ("%d ", priv->bar_heights[i]);
	}
	printf ("\n");

	/* Draw */
	for (i = 0; i < BARS; i++)
		draw_bar (priv, video, i, BARS, freq[i]);

	return 0;
}

static void draw_bar (FinespectrumPrivate *priv, VisVideo *video, int index, int nbars, float amplitude)
{
	int startx = (video->width / nbars) * index;
	int endx = ((video->width / nbars) * (index + 1));
	int height = video->height * amplitude;
	int i;
	float scale = 128.0 / video->height;



	for (i = video->height - 1; i > (video->height - height); i--) {
		draw_vline (video, index, index + 1, i, (video->height - i) * scale);
	}
}

static inline void draw_vline (VisVideo *video, int x1, int x2, int y, uint8_t color)
{
	uint8_t *pixels = visual_video_get_pixels (video);
	int i;

	if (video->depth != VISUAL_VIDEO_DEPTH_8BIT)
		return;

	pixels += (y * video->pitch) + x1;

	visual_mem_set (pixels, color, x2 - x1);
}

