/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: transform_avs_movement.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
	uint8_t	*swapbuf, *renderbuf;

	uint32_t	*tab;
	uint32_t	width, height;
	uint32_t	subpixel;
	unsigned char	blendtable[256][256];

	int effect;
	int rectangular;
	int blend;
	int sourcemapped;
	int wrap;

	char *code;

	int lastWidth;
	int lastHeight;
	int lastPitch;
} MovementPrivate;

typedef struct {
	char *list_desc; // The string to show in the listbox.
	char *eval_desc; // The optional string to display in the evaluation editor.
	char uses_eval;   // If this is true, the preset engages the eval library and there is NULL in the radial_effects array for its entry
	char uses_rect;   // This value sets the checkbox for rectangular calculation
} MovementTemplateDescription;

static MovementTemplateDescription __movement_descriptions[] =
{
	{/* 0,*/ "none", "", 0, 0},
	{/* 1,*/ "slight fuzzify", "", 0, 0},
	{/* 2,*/ "shift rotate left", "x=x+1/32; // use wrap for this one", 0, 1},
	{/* 3,*/ "big swirl out", "r = r + (0.1 - (0.2 * d));\r\nd = d * 0.96;", 0, 0},
	{/* 4,*/ "medium swirl", "d = d * (0.99 * (1.0 - sin(r-$PI*0.5) / 32.0));\r\nr = r + (0.03 * sin(d * $PI * 4));", 0, 0},
	{/* 5,*/ "sunburster", "d = d * (0.94 + (cos((r-$PI*0.5) * 32.0) * 0.06));", 0, 0},
	{/* 6,*/ "swirl to center", "d = d * (1.01 + (cos((r-$PI*0.5) * 4) * 0.04));\r\nr = r + (0.03 * sin(d * $PI * 4));", 0, 0},
	{/* 7,*/ "blocky partial out", "", 0, 0},
	{/* 8,*/ "swirling around both ways at once", "r = r + (0.1 * sin(d * $PI * 5));", 0, 0},
	{/* 9,*/ "bubbling outward", "t = sin(d * $PI);\r\nd = d - (8*t*t*t*t*t)/sqrt((sw*sw+sh*sh)/4);", 0, 0},
	{/*10,*/ "bubbling outward with swirl", "t = sin(d * $PI);\r\nd = d - (8*t*t*t*t*t)/sqrt((sw*sw+sh*sh)/4);\r\nt=cos(d*$PI/2.0);\r\nr= r + 0.1*t*t*t;", 0, 0},
	{/*11,*/ "5 pointed distro", "d = d * (0.95 + (cos(((r-$PI*0.5) * 5.0) - ($PI / 2.50)) * 0.03));", 0, 0},
	{/*12,*/ "tunneling", "r = r + 0.04;\r\nd = d * (0.96 + cos(d * $PI) * 0.05);", 0, 0},
	{/*13,*/ "bleedin'", "t = cos(d * $PI);\r\nr = r + (0.07 * t);\r\nd = d * (0.98 + t * 0.10);", 0, 0},
	{/*14,*/ "shifted big swirl out", "// this is a very bad approximation in script. fixme.\r\nd=sqrt(x*x+y*y); r=atan2(y,x);\r\nr=r+0.1-0.2*d; d=d*0.96;\r\nx=cos(r)*d + 8/128; y=sin(r)*d;", 0, 1},
	{/*15,*/ "psychotic beaming outward", "d = 0.15", 0, 0},
	{/*16,*/ "cosine radial 3-way", "r = cos(r * 3)", 0, 0},
	{/*17,*/ "spinny tube", "d = d * (1 - ((d - .35) * .5));\r\nr = r + .1;", 0, 0},
	{/*18,*/ "radial swirlies", "d = d * (1 - (sin((r-$PI*0.5) * 7) * .03));\r\nr = r + (cos(d * 12) * .03);", 1, 0},
	{/*19,*/ "swill", "d = d * (1 - (sin((r - $PI*0.5) * 12) * .05));\r\nr = r + (cos(d * 18) * .05);\r\nd = d * (1-((d - .4) * .03));\r\nr = r + ((d - .4) * .13)", 1, 0},
	{/*20,*/ "gridley", "x = x + (cos(y * 18) * .02);\r\ny = y + (sin(x * 14) * .03);", 1, 1},
	{/*21,*/ "grapevine", "x = x + (cos(abs(y-.5) * 8) * .02);\r\ny = y + (sin(abs(x-.5) * 8) * .05);\r\nx = x * .95;\r\ny = y * .95;", 1, 1},
	{/*22,*/ "quadrant", "y = y * ( 1 + (sin(r + $PI/2) * .3) );\r\nx = x * ( 1 + (cos(r + $PI/2) * .3) );\r\nx = x * .995;\r\ny = y * .995;", 1, 1},
	{/*23,*/ "6-way kaleida (use wrap!)", "y = (r*6)/($PI); x = d;", 1, 1},
};


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

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", 32767),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("rectangular", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("sourcemapped", 0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 1),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("wrap", 0),
		VISUAL_PARAM_LIST_ENTRY_STRING ("code",
				"d = d * (1.01 + (cos((r-$PI*0.5) * 4) * 0.04)); r = r + (0.03 * sin(d * $PI * 4));"),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (MovementPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

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

				if (visual_param_entry_is (param, VIS_BSTR ("effect"))) {
					priv->effect = visual_param_entry_get_integer (param);

					if (priv->effect != 32767) {
						if (priv->effect >= 0 && priv->effect < 23) {
							visual_param_entry_set_string (
									visual_param_container_get (param->parent, VIS_BSTR ("code")),
									__movement_descriptions[priv->effect].eval_desc);
						}
					}

				} else if (visual_param_entry_is (param, VIS_BSTR ("rectangular")))
					priv->rectangular = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("blend")))
					priv->blend = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("sourcemapped")))
					priv->sourcemapped = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("subpixel")))
					priv->subpixel = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("wrap")))
					priv->wrap = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("code")))
					priv->code = visual_param_entry_get_string (param);

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
	uint8_t *vidbuf, *vidoutbuf;
	int i;

	if (priv->lastWidth != video->width || priv->lastHeight != video->height || priv->lastPitch != video->pitch) {

		//trans_initialize(priv, video->width, video->height, "r = cos(r * 3);");
		trans_initialize(priv, video->width, video->height, priv->code);

		if (priv->swapbuf != NULL)
			visual_mem_free (priv->swapbuf);

		/* FIXME: would allocate way too much on subregion buffers, think about this. */
		priv->swapbuf = visual_mem_malloc0 (visual_video_get_size (video));
		priv->renderbuf = visual_mem_malloc0(video->width * video->height * video->bpp);
	}

	vidbuf = priv->swapbuf;
	for (i = 0; i < video->height; i++) {
		visual_mem_copy (vidbuf, pixels + (video->pitch * i), video->width * video->bpp);
		vidbuf += video->width * video->bpp;
	}

	visual_mem_set(priv->renderbuf, 0, video->width * video->height * video->bpp);
	trans_render(priv, (uint32_t *) priv->swapbuf,  (uint32_t *) priv->renderbuf);

	for (i =0; i < video->height; i++) {
		visual_mem_copy (pixels, priv->renderbuf + (i * video->width * video->bpp), video->width * video->bpp);
		pixels += video->pitch;
	}

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




