/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: morph_flash.c,v 1.21 2006/01/27 20:19:18 synap Exp $
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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (void);

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

typedef struct {
	VisPalette *whitepal;
	uint8_t replacetable[256];
} FlashPrivate;

static void replacetable_generate_24 (FlashPrivate *priv, float rate);
static void flash_8 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2);
static void flash_24 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2);

static int lv_morph_flash_init (VisPluginData *plugin);
static int lv_morph_flash_cleanup (VisPluginData *plugin);
static int lv_morph_flash_palette (VisPluginData *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2);
static int lv_morph_flash_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

const VisPluginInfo *get_plugin_info (void)
{
	static VisMorphPlugin morph = {
		.palette = lv_morph_flash_palette,
		.apply = lv_morph_flash_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT  |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "flash",
		.name = "flash morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An flash in and out morph plugin",
		.help = "This morph plugin morphs between two video sources using a bright flash",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_flash_init,
		.cleanup = lv_morph_flash_cleanup,

		.plugin = VISUAL_OBJECT (&morph)
	};

	return &info;
}

static int lv_morph_flash_init (VisPluginData *plugin)
{
	int i;
	FlashPrivate *priv;
    VisColor *whitepal_colors;

	priv = visual_mem_new0 (FlashPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->whitepal = visual_palette_new (256);
	whitepal_colors = visual_palette_get_colors (priv->whitepal);

	for (i = 0; i < 256; i++) {
		whitepal_colors[i].r = 0xff;
		whitepal_colors[i].g = 0xff;
		whitepal_colors[i].b = 0xff;
	}

	return 0;
}

static int lv_morph_flash_cleanup (VisPluginData *plugin)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free (priv->whitepal);

	visual_mem_free (priv);

	return 0;
}

static int lv_morph_flash_palette (VisPluginData *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (src1->pal == NULL || src2->pal == NULL)
		return 0;

	if (rate < 0.5)
		visual_palette_blend (pal, src1->pal, priv->whitepal, rate * 2);
	else
		visual_palette_blend (pal, priv->whitepal, src2->pal, (rate - 0.5) * 2);

	return 0;
}

static int lv_morph_flash_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			flash_8 (priv, rate, dest, src1, src2);
			break;

		case VISUAL_VIDEO_DEPTH_16BIT:

			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			replacetable_generate_24 (priv, rate);
			flash_24 (priv, rate, dest, src1, src2);
			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			replacetable_generate_24 (priv, rate);
			flash_24 (priv, rate, dest, src1, src2);
			break;

		default:
			break;
	}

	return 0;
}

static void replacetable_generate_24 (FlashPrivate *priv, float rate)
{
	int i;

	for (i = 0; i < 256; i++) {
		if (rate < 0.5)
			priv->replacetable[i] = i + (((255.00 - i) / 100.00) * ((rate * 2) * 100));
		else
			priv->replacetable[i] = i + (((255.00 - i) / 100.00) * ((1.0 - ((rate - 0.5) * 2)) * 100));
	}

}

static void flash_8 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	if (rate < 0.5)
		visual_mem_copy (visual_video_get_pixels (dest), visual_video_get_pixels (src1), visual_video_get_size (src1));
	else
		visual_mem_copy (visual_video_get_pixels (dest), visual_video_get_pixels (src2), visual_video_get_size (src2));
}

static void flash_24 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	uint8_t *scrbuf;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	int size;
	int i;

	if (rate < 0.5) {
		scrbuf = visual_video_get_pixels (src1);
		size = visual_video_get_size (src1);
	} else {
		scrbuf = visual_video_get_pixels (src2);
		size = visual_video_get_size (src2);
	}

	for (i = 0; i < size; i++)
		destbuf[i] = priv->replacetable[scrbuf[i]];
}

