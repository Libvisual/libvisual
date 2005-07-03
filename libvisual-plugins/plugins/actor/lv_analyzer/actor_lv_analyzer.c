/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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
#include <gettext.h>

#include <libvisual/libvisual.h>

#define BARS 16

typedef struct {
	VisPalette pal;
} AnalyzerPrivate;

static int xranges[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

int lv_analyzer_init (VisPluginData *plugin);
int lv_analyzer_cleanup (VisPluginData *plugin);
int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height);
int lv_analyzer_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_analyzer_palette (VisPluginData *plugin);
int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = lv_analyzer_requisition,
		.palette = lv_analyzer_palette,
		.render = lv_analyzer_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_analyzer",
		.name = "libvisual analyzer",
		.author = N_("Original by: Andy Lo A Foe <andy@alsaplayer.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.1",
		.about = N_("Libvisual analyzer plugin"),
		.help = N_("This is a test plugin that'll display a simple analyzer"),

		.init = lv_analyzer_init,
		.cleanup = lv_analyzer_cleanup,
		.events = lv_analyzer_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_analyzer_init (VisPluginData *plugin)
{
	AnalyzerPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (AnalyzerPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

int lv_analyzer_cleanup (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_mem_free (priv);

	return 0;
}

int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height)
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

int lv_analyzer_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_analyzer_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_analyzer_palette (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
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

int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	int maxbar[BARS];
	unsigned char *bits = visual_video_get_pixels (video);
	unsigned int val;
	int j;
	int k = 0;
	int i, h;
	uint8_t *loc;
	float scale;
	float colscale;

	visual_mem_set (bits, 0, visual_video_get_size (video));
	visual_mem_set (maxbar, 0, sizeof (maxbar));
	
	scale = (float) video->height / 127;

	colscale = 1 / scale;
	
	for (i=0; i < BARS; i++) {
		val = 0;
		for (j = xranges[i]; j < xranges[i + 1]; j++) {
			k = (audio->freq[2][j]) / 128;
			val += k;
		}

		if(val > 127)
			val = 127;

		val *= scale;
		
		if (val >= video->height)
			val = video->height - 1;


		if (val > (unsigned int)maxbar[ i ])
			maxbar[ i ] = val;
		
		else {
			k = maxbar[ i ] - (4 + (8 / (128 - maxbar[ i ])));
			val = k > 0 ? k : 0;
			maxbar[ i ] = val;
		}
	
		
		loc = bits + video->pitch * (video->height - 1);
		for (h = val; h > 0; h--) {
			for (j = (video->pitch / BARS) * i + 0; j < (video->pitch / BARS) * i + ((video->pitch / BARS) - 1); j++) {
				*(loc + j) = (float) (val-h) * colscale;
			}
			loc -= video->pitch;
		}
	}

	return 0;
}

