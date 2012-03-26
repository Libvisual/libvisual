/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: morph_alphablend.c,v 1.19 2006/01/27 20:19:18 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

const VisPluginInfo *get_plugin_info (int *count);

static inline void alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha);

static int lv_morph_alpha_init (VisPluginData *plugin);
static int lv_morph_alpha_cleanup (VisPluginData *plugin);
static int lv_morph_alpha_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisMorphPlugin morph[] = {{
		.apply = lv_morph_alpha_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT  |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "alphablend",
		.name = "alphablend morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An alphablend morph plugin",
		.help = "This morph plugin morphs between two video sources using the alphablend method",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_alpha_init,
		.cleanup = lv_morph_alpha_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

static int lv_morph_alpha_init (VisPluginData *plugin)
{
	return 0;
}

static int lv_morph_alpha_cleanup (VisPluginData *plugin)
{
	return 0;
}

static int lv_morph_alpha_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	visual_return_val_if_fail (dest != NULL, -1);
	visual_return_val_if_fail (src1 != NULL, -1);
	visual_return_val_if_fail (src2 != NULL, -1);

	alpha_blend_buffer (visual_video_get_pixels (dest),
			visual_video_get_pixels (src1),
			visual_video_get_pixels (src2),
			visual_video_get_size (dest), dest->depth, rate);

	return 0;
}

static inline void alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha)
{
	uint8_t a = alpha * (1/255.0);

	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			visual_alpha_blend_8 (dest, src1, src2, size, a);
			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			visual_alpha_blend_16 (dest, src1, src2, size, a);
			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			visual_alpha_blend_24 (dest, src1, src2, size, a);
			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			visual_alpha_blend_32 (dest, src1, src2, size, a);
			break;
	}
}
