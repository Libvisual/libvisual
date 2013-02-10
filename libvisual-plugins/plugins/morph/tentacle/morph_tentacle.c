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

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <math.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

typedef struct {
	float	move;
} TentaclePrivate;

static int  lv_morph_tentacle_init (VisPluginData *plugin);
static void lv_morph_tentacle_cleanup (VisPluginData *plugin);
static void lv_morph_tentacle_apply (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2);

static void vline_from_video_8  (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);

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
		.name     = "tentacle morph",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>",
		.version  = "0.1",
		.about    = N_("An sine wave morph plugin"),
		.help     = N_("This morph plugin morphs between two video sources using some sort of wave that grows in size"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = lv_morph_tentacle_init,
		.cleanup  = lv_morph_tentacle_cleanup,
		.plugin   = &morph
	};

	return &info;
}

static int lv_morph_tentacle_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	TentaclePrivate *priv = visual_mem_new0 (TentaclePrivate, 1);
	visual_plugin_set_private (plugin, priv);

	return TRUE;
}

static void lv_morph_tentacle_cleanup (VisPluginData *plugin)
{
	TentaclePrivate *priv = visual_plugin_get_private (plugin);

	visual_mem_free (priv);
}

static void lv_morph_tentacle_apply (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	TentaclePrivate *priv = visual_plugin_get_private (plugin);

	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *src1buf = visual_video_get_pixels (src1);

    int dest_width  = visual_video_get_width  (dest);
    int dest_height = visual_video_get_height (dest);

    VisVideoDepth dest_depth = visual_video_get_depth (dest);

	int height1;
	int height2;
	int add1;
	int add2;

	float sinrate = priv->move;
	float multiplier = 0;
	float multiadd = 1.000 / dest_width;

	int i;

	visual_mem_copy (destbuf, src1buf, visual_video_get_size (src1));

	for (i = 0; i < dest_width; i++) {
		add1 = (dest_height / 2) - ((dest_height / 2) * (progress * 1.5));
		add2 = (dest_height / 2) + ((dest_height / 2) * (progress * 1.5));

		height1 = (sin (sinrate) * ((dest_height / 4) * multiplier)) + add1;
		height2 = (sin (sinrate) * ((dest_height / 4) * multiplier)) + add2;
		multiplier += multiadd;

		switch (dest_depth) {
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
}

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2)
{
    int width  = visual_video_get_width  (dest);
    int height = visual_video_get_height (dest);

	if (*x > width)
		*x = width;
	else if (*x < 0)
		*x = 0;

	if (*y1 > height)
		*y1 = height;
	else if (*y1 < 0)
		*y1 = 0;

	if (*y2 > height)
		*y2 = height;
	else if (*y2 < 0)
		*y2 = 0;
}

static void vline_from_video_8 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf;
	uint8_t *srcbuf;
    int dest_step;
    int src_step;
	int i;

	sane_coords (dest, &x, &y1, &y2);

    destbuf = visual_video_get_pixel_ptr (dest, x, y1);
    srcbuf  = visual_video_get_pixel_ptr (src, x, y1);

    dest_step = visual_video_get_width (dest);
    src_step  = visual_video_get_width (src);

	for (i = y1; i < y2; i++) {
		*destbuf = *srcbuf;

        destbuf += dest_step;
        srcbuf  += src_step;
    }
}

static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint16_t *destbuf;
	uint16_t *srcbuf;
    int dest_step;
    int src_step;
	int i;

	sane_coords (dest, &x, &y1, &y2);

    destbuf = visual_video_get_pixel_ptr (dest, x, y1);
    srcbuf  = visual_video_get_pixel_ptr (src, x, y1);

    dest_step = visual_video_get_width (dest);
    src_step  = visual_video_get_width (src);

	for (i = y1; i < y2; i++) {
		*destbuf = *srcbuf;

        destbuf += dest_step;
        srcbuf  += src_step;
    }
}

static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf;
	uint8_t *srcbuf;
    int dest_step;
    int src_step;
	int i;

	sane_coords (dest, &x, &y1, &y2);

    destbuf = visual_video_get_pixel_ptr (dest, x, y1);
    srcbuf  = visual_video_get_pixel_ptr (src, x, y1);

    dest_step = visual_video_get_pitch (dest);
    src_step  = visual_video_get_pitch (src);

	for (i = y1; i < y2; i++) {
		destbuf[0] = srcbuf[0];
		destbuf[1] = srcbuf[1];
		destbuf[2] = srcbuf[2];

        destbuf += dest_step;
        srcbuf  += src_step;
	}
}

static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint32_t *destbuf;
	uint32_t *srcbuf;
    int dest_step;
    int src_step;
	int i;

	sane_coords (dest, &x, &y1, &y2);

    destbuf = visual_video_get_pixel_ptr (dest, x, y1);
    srcbuf  = visual_video_get_pixel_ptr (src, x, y1);

    dest_step = visual_video_get_width (dest);
    src_step  = visual_video_get_width (src);

	for (i = y1; i < y2; i++) {
		*destbuf = *srcbuf;

        destbuf += dest_step;
        srcbuf  += src_step;
	}
}
