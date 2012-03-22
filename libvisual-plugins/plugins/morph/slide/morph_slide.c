/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: morph_slide.c,v 1.16 2006/01/27 20:19:18 synap Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

const VisPluginInfo *get_plugin_info (int *count);

typedef enum {
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_BOTTOM,
	SLIDE_UPPER
} SlideType;

typedef struct {
	SlideType	slide_type;
} SlidePrivate;

static int lv_morph_slide_init_left (VisPluginData *plugin);
static int lv_morph_slide_init_right (VisPluginData *plugin);
static int lv_morph_slide_init_bottom (VisPluginData *plugin);
static int lv_morph_slide_init_upper (VisPluginData *plugin);
static int lv_morph_slide_cleanup (VisPluginData *plugin);
static int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisMorphPlugin morph[] = {{
		.apply = lv_morph_slide_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_left",
		.name = "Slide left morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_slide_init_left,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_right",
		.name = "Slide right morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_slide_init_right,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_bottom",
		.name = "Slide bottom morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_slide_init_bottom,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_upper",
		.name = "Slide upper morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_slide_init_upper,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

static int lv_morph_slide_init_left (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_LEFT;

	return 0;
}

static int lv_morph_slide_init_right (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_RIGHT;

	return 0;
}

static int lv_morph_slide_init_bottom (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_BOTTOM;

	return 0;
}

static int lv_morph_slide_init_upper (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_UPPER;

	return 0;
}

static int lv_morph_slide_cleanup (VisPluginData *plugin)
{
	SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

static int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf1 = visual_video_get_pixels (src1);
	uint8_t *srcbuf2 = visual_video_get_pixels (src2);
	int i;
	int diff1;
	int diff2;
	int hadd;

	visual_mem_set (destbuf, 0, visual_video_get_size (dest));

	if (priv->slide_type == SLIDE_RIGHT || priv->slide_type == SLIDE_UPPER)
		rate = 1.0 - rate;

	diff1 = dest->pitch * rate;
	diff1 -= diff1 % dest->bpp;

	if (diff1 > dest->pitch)
		diff1 = dest->pitch;

	diff2 = dest->pitch - diff1;

	hadd = dest->height * rate;

	switch (priv->slide_type) {
		case SLIDE_LEFT:
			for (i = 0; i < dest->height; i++) {
				visual_mem_copy (destbuf + (i * dest->pitch), srcbuf2 + (i * dest->pitch) + diff2, diff1);
				visual_mem_copy (destbuf + (i * dest->pitch) + (diff1), srcbuf1 + (i * dest->pitch), diff2);
			}

			break;

		case SLIDE_RIGHT:
			for (i = 0; i < dest->height; i++) {
				visual_mem_copy (destbuf + (i * dest->pitch), srcbuf1 + (i * dest->pitch) + diff2, diff1);
				visual_mem_copy (destbuf + (i * dest->pitch) + (diff1), srcbuf2 + (i * dest->pitch), diff2);
			}

			break;

		case SLIDE_BOTTOM:
			visual_mem_copy (destbuf, srcbuf1 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
			visual_mem_copy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf2, hadd * dest->pitch);

			break;

		case SLIDE_UPPER:
			visual_mem_copy (destbuf, srcbuf2 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
			visual_mem_copy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf1, hadd * dest->pitch);

			break;

		default:
			visual_log (VISUAL_LOG_WARNING,
					"Slide plugin is initialized with an impossible slide direction, this should never happen");

			break;
	}

	return 0;
}

