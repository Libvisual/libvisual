/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_multiplier.c,v 1.5 2006-09-19 19:05:47 synap Exp $
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

/* FIXME TODO:
 *
 * config UI.
 * fix for other depths than 32bits
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
	int multiply;
} MultiplierPrivate;

int lv_multiplier_init (VisPluginData *plugin);
int lv_multiplier_cleanup (VisPluginData *plugin);
int lv_multiplier_events (VisPluginData *plugin, VisEventQueue *events);
int lv_multiplier_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_multiplier_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void multiply_video_root (VisPluginData *plugin, VisVideo *video);
static void multiply_video_shift (VisPluginData *plugin, VisVideo *video, int shift);
static void multiply_video_square (VisPluginData *plugin, VisVideo *video);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_multiplier_palette,
		.video = lv_multiplier_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_8BIT,
		.requests_audio = FALSE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM".[avs]",

		.plugname = "avs_multiplier",
		.name = "Libvisual AVS Transform: multiplier element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: multiplier element",
		.help = "This is the multiplier element for the libvisual AVS system",

		.init = lv_multiplier_init,
		.cleanup = lv_multiplier_cleanup,
		.events = lv_multiplier_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_multiplier_init (VisPluginData *plugin)
{
	MultiplierPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("multiply", 3, VISUAL_PARAM_LIMIT_INTEGER(0, 1000), ""),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (MultiplierPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_multiplier_cleanup (VisPluginData *plugin)
{
	MultiplierPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_multiplier_events (VisPluginData *plugin, VisEventQueue *events)
{
	MultiplierPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, VIS_BSTR ("multiply")))
					priv->multiply = visual_param_entry_get_integer (param);

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_multiplier_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_multiplier_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	MultiplierPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	switch (priv->multiply) {
		case 0:
			multiply_video_root (plugin, video);

			break;

		case 1:
			multiply_video_shift (plugin, video, 4);

			break;

		case 2:
			multiply_video_shift (plugin, video, 2);

			break;

		case 3:
			multiply_video_shift (plugin, video, 1);

			break;

		case 4:
			multiply_video_shift (plugin, video, -1);

			break;

		case 5:
			multiply_video_shift (plugin, video, -2);

			break;

		case 6:
			multiply_video_shift (plugin, video, -4);

			break;

		case 7:
			multiply_video_square (plugin, video);

			break;

		default:
			
			break;

	}
}

static void multiply_video_root (VisPluginData *plugin, VisVideo *video)
{
	uint32_t *ibuf = visual_video_get_pixels (video);
	uint8_t *buf = visual_video_get_pixels (video);
	int size = visual_video_get_size (video);
	uint8_t bytetable[256];
	int i;

	bytetable[0] = 0;

	for (i = 1; i < 256; i++)
		bytetable[i] = 0xff;
	
	if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		for (i = 0; i < size / 4; i++, ++ibuf) {
			if ((*(ibuf) & 0x00ffffff) > 0)
				*(ibuf) |= 0x00ffffff;
		}
	} else if (video->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		for (i = 0; i < size; i++)
			*(buf++) = 0;
	} else {
		for (i = 0; i < size; i++)
			*(buf++) = 0;
	}

}

static void multiply_video_shift (VisPluginData *plugin, VisVideo *video, int shift)
{
	uint8_t *buf = visual_video_get_pixels (video);
	int size = visual_video_get_size (video);
	int shiftr = abs (shift);
	int i;
	
	/* FIXME endianess.. */
	if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		if (shift > 0) {
			for (i = 0; i < size / 4; i++) {
				*(buf++) <<= shiftr;
				*(buf++) <<= shiftr;
				*(buf++) <<= shiftr;
				buf++;
			}
		} else {
			for (i = 0; i < size / 4; i++) {
				*(buf++) >>= shiftr;
				*(buf++) >>= shiftr;
				*(buf++) >>= shiftr;
				buf++;
			}
		}
	} else {
		if (shift > 0) {
			for (i = 0; i < size; i++)
				*(buf++) <<= shiftr;
		} else {
			for (i = 0; i < size; i++)
				*(buf++) >>= shiftr;
		}
	}
}

static void multiply_video_square (VisPluginData *plugin, VisVideo *video)
{
	uint8_t *buf = visual_video_get_pixels (video);
	int size = visual_video_get_size (video);
	int i;

	if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		for (i = 0; i < size / 4; i++) {
			*(buf++) = 0;
			*(buf++) = 0;
			*(buf++) = 0;
			buf++;
		}
	} else {
		memset (visual_video_get_pixels (video), 0, size);
	}
}

