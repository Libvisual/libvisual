/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: morph_tentacle.c,v 1.18 2006/01/27 20:19:18 synap Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
	float	move;
} TentaclePrivate;

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2);

static void vline_from_video_8 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);

static int lv_morph_tentacle_init (VisPluginData *plugin);
static int lv_morph_tentacle_cleanup (VisPluginData *plugin);
static int lv_morph_tentacle_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

const VisPluginInfo *get_plugin_info (void)
{
	static VisMorphPlugin morph = {
		.apply = lv_morph_tentacle_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "tentacle",
		.name = "tentacle morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An sine wave morph plugin",
		.help = "This morph plugin morphs between two video sources using some sort of wave that grows in size",
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_morph_tentacle_init,
		.cleanup = lv_morph_tentacle_cleanup,

		.plugin = VISUAL_OBJECT (&morph)
	};

	return &info;
}

static int lv_morph_tentacle_init (VisPluginData *plugin)
{
	TentaclePrivate *priv;

	priv = visual_mem_new0 (TentaclePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	return 0;
}

static int lv_morph_tentacle_cleanup (VisPluginData *plugin)
{
	TentaclePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

static int lv_morph_tentacle_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	TentaclePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *src1buf = visual_video_get_pixels (src1);

	int height1;
	int height2;
	int add1;
	int add2;

	float sinrate = priv->move;
	float multiplier = 0;
	float multiadd = 1.000 / dest->width;

	int i;

	visual_mem_copy (destbuf, src1buf, visual_video_get_size (src1));

	for (i = 0; i < dest->width; i++) {
		add1 = (dest->height / 2) - ((dest->height / 2) * (rate * 1.5));
		add2 = (dest->height / 2) + ((dest->height / 2) * (rate * 1.5));

		height1 = (sin (sinrate) * ((dest->height / 4) * multiplier)) + add1;
		height2 = (sin (sinrate) * ((dest->height / 4) * multiplier)) + add2;
		multiplier += multiadd;

		switch (dest->depth) {
			case VISUAL_VIDEO_DEPTH_8BIT:
				vline_from_video_8 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_16BIT:
				vline_from_video_16 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_24BIT:
				vline_from_video_24 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_32BIT:
				vline_from_video_32 (dest, src2, i, height1, height2);
				break;

			default:
				break;
		}

		sinrate += 0.02;
		priv->move += 0.0002;
	}

	return 0;
}

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2)
{
	if (*x > dest->width)
		*x = dest->width;
	else if (*x < 0)
		*x = 0;

	if (*y1 > dest->height)
		*y1 = dest->height;
	else if (*y1 < 0)
		*y1 = 0;

	if (*y2 > dest->height)
		*y2 = dest->height;
	else if (*y2 < 0)
		*y2 = 0;
}

static void vline_from_video_8 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	int i;

	sane_coords (dest, &x, &y1, &y2);

	for (i = y1; i < y2; i++)
		destbuf[(i * dest->pitch) + x] = srcbuf[(i * src->pitch) + x];
}

static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint16_t *destbuf = visual_video_get_pixels (dest);
	uint16_t *srcbuf = visual_video_get_pixels (src);
	int i;

	sane_coords (dest, &x, &y1, &y2);

	for (i = y1; i < y2; i++)
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];
}

static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	int i;

	sane_coords (dest, &x, &y1, &y2);

	x *= 3;

	for (i = y1; i < y2; i++) {
		destbuf[(i * dest->pitch) + x] = srcbuf[(i * src->pitch) + x];
		destbuf[(i * dest->pitch) + x + 1] = srcbuf[(i * src->pitch) + x + 1];
		destbuf[(i * dest->pitch) + x + 2] = srcbuf[(i * src->pitch) + x + 2];
	}
}

static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint32_t *destbuf = visual_video_get_pixels (dest);
	uint32_t *srcbuf = visual_video_get_pixels (src);
	int i;

	sane_coords (dest, &x, &y1, &y2);

	for (i = y1; i < y2; i++) {
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];
	}
}
