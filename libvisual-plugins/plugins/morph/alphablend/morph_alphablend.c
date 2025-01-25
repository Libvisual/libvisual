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

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <stdlib.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int  lv_morph_alpha_init    (VisPluginData *plugin);
static void lv_morph_alpha_cleanup (VisPluginData *plugin);
static void lv_morph_alpha_apply   (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

typedef void (*BlendFunc) (uint8_t *, const uint8_t *, const uint8_t *, visual_size_t, uint8_t);

static BlendFunc get_blend_func (VisVideoDepth depth);

const VisPluginInfo *get_plugin_info (void)
{
	static VisMorphPlugin morph = {
		.apply = lv_morph_alpha_apply,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT  |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "alphablend",
		.name     = "alphablend morph",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>",
		.version  = "0.1",
		.about    = N_("An alphablend morph plugin"),
		.help     = N_("This morph plugin morphs between two video sources using the alphablend method"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = lv_morph_alpha_init,
		.cleanup  = lv_morph_alpha_cleanup,

		.plugin   = &morph
	};

	return &info;
}

static int lv_morph_alpha_init (VisPluginData *plugin)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	return TRUE;
}

static void lv_morph_alpha_cleanup (VisPluginData *plugin)
{
    /* nothing to do */
}

static void lv_morph_alpha_apply (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	int width  = visual_video_get_width (dest);
	int height = visual_video_get_height (dest);
	int depth  = visual_video_get_depth (dest);
	int pitch  = visual_video_get_pitch (dest);

	uint8_t *src1_row_ptr = visual_video_get_pixels (src1);
	uint8_t *src2_row_ptr = visual_video_get_pixels (src2);
	uint8_t *dest_row_ptr = visual_video_get_pixels (dest);

	uint8_t alpha = progress * 255;
	BlendFunc blend_func = get_blend_func (depth);

	for (int y = 0; y < height; y++) {
		blend_func (dest_row_ptr, src1_row_ptr, src2_row_ptr, width, alpha);
		src1_row_ptr += pitch;
		src2_row_ptr += pitch;
		dest_row_ptr += pitch;
	}
}

static BlendFunc get_blend_func (VisVideoDepth depth)
{
	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			return visual_alpha_blend_8;
		case VISUAL_VIDEO_DEPTH_16BIT:
			return visual_alpha_blend_16;
		case VISUAL_VIDEO_DEPTH_24BIT:
			return visual_alpha_blend_24;
		case VISUAL_VIDEO_DEPTH_32BIT:
			return visual_alpha_blend_32;
		default:
            visual_log (VISUAL_LOG_CRITICAL, "Unsupported depth for blending (%d)", depth);
			abort ();
	}
}
