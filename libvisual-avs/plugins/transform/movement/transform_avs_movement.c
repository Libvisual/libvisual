/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
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

/* FIXME TODO:
 *
 * config UI.
 * fix for other depths than 32bits
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <math.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs.h"

AvsNumber PI = M_PI;

typedef struct {
	uint8_t	*swapbuf;

	uint32_t	*tab;
	uint32_t	width, height;
	uint32_t	subpixel;
	unsigned char	blendtable[256][256];

	int lastWidth;
	int lastHeight;
	int lastPitch;
} MovementPrivate;

int lv_movement_init (VisPluginData *plugin);
int lv_movement_cleanup (VisPluginData *plugin);
int lv_movement_events (VisPluginData *plugin, VisEventQueue *events);
int lv_movement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_movement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void trans_generate_table(MovementPrivate *priv, char *effect, int rectangular, int wrap);
static void trans_generate_blend_table(MovementPrivate *priv);
static void trans_initialize(MovementPrivate *priv, int width, int height, char *effect);
static void trans_render(MovementPrivate *priv, uint32_t *fbin, uint32_t *fbout);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_movement_palette,
		.video = lv_movement_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = FALSE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,//".[avs]",

		.plugname = "avs_movement",
		.name = "Libvisual AVS Transform: movement element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: movement element",
		.help = "This is the movement element for the libvisual AVS system",

		.init = lv_movement_init,
		.cleanup = lv_movement_cleanup,
		.events = lv_movement_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_movement_init (VisPluginData *plugin)
{
	MovementPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (MovementPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many (paramcontainer, params);

	return 0;
}

int lv_movement_cleanup (VisPluginData *plugin)
{
	MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (priv->swapbuf != NULL)
		visual_mem_free (priv->swapbuf);

	visual_mem_free (priv);

	return 0;
}

int lv_movement_events (VisPluginData *plugin, VisEventQueue *events)
{
	MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_movement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_movement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
	uint8_t *vidbuf;
	int i;

	if (priv->lastWidth != video->width || priv->lastHeight != video->height || priv->lastPitch != video->pitch) {
		trans_initialize(priv, video->width, video->height, "r = cos(r * 3);");

		if (priv->swapbuf != NULL)
			visual_mem_free (priv->swapbuf);

		/* FIXME: would allocate way too much on subregion buffers, think about this. */
		priv->swapbuf = visual_mem_malloc0 (visual_video_get_size (video));
	}

	vidbuf = priv->swapbuf;
	for (i = 0; i < video->height; i++) {
		visual_mem_copy (vidbuf, pixels + (video->width * i), video->width * video->bpp);

		vidbuf += video->pitch;
	}


	trans_render(priv, (uint32_t *) priv->swapbuf,  (uint32_t *) pixels);

	priv->lastWidth = video->width;
	priv->lastHeight = video->height;
	priv->lastPitch = video->pitch;
}

static void trans_generate_table(MovementPrivate *priv, char *effect, int rectangular, int wrap)
{
	uint32_t *transp = priv->tab;
	AvsNumber d, r, px, py, pw, ph;

	AvsRunnableContext *ctx = avs_runnable_context_new();
	AvsRunnableVariableManager *vm = avs_runnable_variable_manager_new();
	AvsRunnable *obj = avs_runnable_new(ctx);
	avs_runnable_set_variable_manager(obj, vm);

	avs_runnable_variable_bind(vm, "d", &d);
	avs_runnable_variable_bind(vm, "r", &r);
	avs_runnable_variable_bind(vm, "x", &px);
	avs_runnable_variable_bind(vm, "y", &py);
	avs_runnable_variable_bind(vm, "sw", &pw);
	avs_runnable_variable_bind(vm, "sh", &ph);
	avs_runnable_variable_bind(vm, "$PI", &PI);
	
	avs_runnable_compile(obj, (unsigned char *)effect, strlen(effect));

	pw = priv->width;
	ph = priv->height;

	int x, y;
	double max_d = sqrt((double)(priv->width*priv->width + priv->height*priv->height)) / 2.0;
	double divmax_d = 1.0/max_d;

	double hw = pw / 2;
	double hh = ph / 2;
	double xsc = 1.0/hw, ysc = 1.0/hh;
	
	for (y=0; y < priv->height; y++) {
		for (x=0; x < priv->width; x++) {
			double xd, yd;
			int ow, oh;

			xd = x - hw;
			yd = y - hh;
			
			px = xd * xsc;
			py = yd * ysc;
			d = sqrt(xd*xd + yd*yd) * divmax_d;
			r = atan2(yd, xd) + M_PI*0.5;

			//fprintf(stderr, "PRE : d = %.2f, r = %.2f\n", d, y);
			avs_runnable_execute(obj);
			//fprintf(stderr, "POST: d = %.2f, r = %.2f\n", d, y);
			
			double privp1, privp2;
			if (!rectangular) {
				d *= max_d;
				r -= M_PI/2.0;
				privp1 = priv->height / 2 + sin(r) * d;
				privp2 = priv->width / 2 + cos(r) * d;
			} else {
				privp1 = (py+1.0) * hh;
				privp2 = (px+1.0) * hw;
			}

			if (priv->subpixel) {
				oh = privp1;
				ow = privp2;

				int xpartial = (int) (32.0 * (privp2 - ow));
				int ypartial = (int) (32.0 * (privp1 - oh));

				if (wrap) {
					ow %= (priv->width - 1);
					oh %= (priv->height - 1);

					if (ow < 0)
						ow += priv->width - 1;
					if (oh < 0)
						oh += priv->height - 1;
				} else {
					if (ow < 0) {
						xpartial = 0;
						ow = 0;
					}

					if (ow >= priv->width - 1) {
						xpartial=31;
						ow = priv->width - 2;
					}

					if (oh < 0) {
						ypartial = 0;
						oh = 0;
					}

					if (oh >= priv->height - 1) {
						ypartial = 31;
						oh = priv->height - 2;
					}
				}

				*transp++ = (ow + oh * priv->width) | (ypartial<<22) | (xpartial<<27);
			} else {
				privp1 += 0.5;
				privp2 += 0.5;
				oh = (int) privp1;
				ow = (int) privp2;
				
				if (wrap) {
					ow %= priv->width;
					oh %= priv->height;
				} else {
					if (ow < 0) 
						ow = 0;
					if (ow >= priv->width)
						ow = priv->width - 1;
					if (oh < 0)
						oh = 0;
					if (oh >= priv->height)
						oh = priv->height - 1;
				}

				*transp++ = ow + oh * priv->width;
			}
		}
	}
}

static void trans_generate_blend_table(MovementPrivate *priv)
{
	int i,j;
	
	for (j=0; j < 256; j++)
		for (i=0; i < 256; i++)
			priv->blendtable[i][j] = (unsigned char)((i / 255.0) * (float)j);

}

static void trans_initialize(MovementPrivate *priv, int width, int height, char *effect)
{
	if (priv->tab)
		free(priv->tab);
	
	priv->width = width;
	priv->height = height;
	priv->tab = malloc(width * height * sizeof(int));

	if (width * height < (1<<22))
		priv->subpixel = 1;
	else
		priv->subpixel = 0;
	
	trans_generate_table(priv, effect, 0, 0);
	trans_generate_blend_table(priv);

	if (1 /* !isBeat & 0x80000000 */) {
		/* ... */
	}
}

#define OFFSET_MASK ((1<<22)-1)

static uint32_t BLEND4(MovementPrivate *priv, uint32_t *p1, uint32_t w, int xp, int yp)
{
	register int t;
	unsigned char a1,a2,a3,a4;
	
	a1 = priv->blendtable[255-xp][255-yp];
	a2 = priv->blendtable[xp][255-yp];
	a3 = priv->blendtable[255-xp][yp];
	a4 = priv->blendtable[xp][yp];
	
	t = priv->blendtable[p1[0] & 0xff][a1] +
	    priv->blendtable[p1[1] & 0xff][a2] + 
	    priv->blendtable[p1[w] & 0xff][a3] +
	    priv->blendtable[p1[w+1] & 0xff][a4];
	
	t |= (priv->blendtable[(p1[0]>>8)&0xff][a1]+priv->blendtable[(p1[1]>>8)&0xff][a2]+priv->blendtable[(p1[w]>>8)&0xff][a3]+priv->blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
	t |= (priv->blendtable[(p1[0]>>16)&0xff][a1]+priv->blendtable[(p1[1]>>16)&0xff][a2]+priv->blendtable[(p1[w]>>16)&0xff][a3]+priv->blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
	return t;
}

static void trans_render(MovementPrivate *priv, uint32_t *fbin, uint32_t *fbout)
{
	uint32_t *inp = fbin;
	uint32_t *outp = fbout;
	int *transp = priv->tab;

	if (1 /* !sourcemapped */) {
		/* !blend */
		int x = (priv->width * priv->height) / 4;
		if (priv->subpixel) {
			int i;
			while (x--) {
				for (i=0; i < 4; i++) {
					int offs = transp[i] & OFFSET_MASK;
					outp[i] = BLEND4(priv, &fbin[offs], priv->width, ((transp[i] >> 24) & (31 << 3)),
									     	     ((transp[i] >> 19) & (31 << 3)));
				}

				transp += 4;
				outp += 4;
			}

			x = (priv->width * priv->height) & 3;
			while (x--) {
				int offs = transp[0] & OFFSET_MASK;
				*outp++ = BLEND4(priv, &fbin[offs], priv->width, ((transp[0] >> 24) & (31 << 3)),
									     ((transp[0] >> 19) & (31 << 3)));
				transp++;
			}
		} else {
			while (x--) {
				outp[0] = fbin[transp[0]];
				outp[1] = fbin[transp[1]];
				outp[2] = fbin[transp[2]];
				outp[3] = fbin[transp[3]];
				outp += 4;
				transp += 4;
			}
		}	
	}
}




