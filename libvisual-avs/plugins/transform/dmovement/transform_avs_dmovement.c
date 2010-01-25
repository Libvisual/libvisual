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
#include "avs_inlines.h"

AvsNumber PI = M_PI;

typedef enum trans_runnable TransRunnable;

enum trans_runnable {
    TRANS_RUNNABLE_INIT,
    TRANS_RUNNABLE_FRAME,
    TRANS_RUNNABLE_BEAT,
    TRANS_RUNNABLE_PIXEL,
};

typedef struct {
    Generic obj;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsNumber var_d, var_b, var_r, var_x, var_y, var_w, var_h, var_alpha;

    uint8_t *swapbuf, *renderbuf;

    char *init, *pixel, *frame, *beat;

    uint32_t *tab;
    uint32_t *wmul;

    uint32_t last_width, last_height, last_pitch;
    uint32_t last_xres, last_yres, xres, yres;
    uint32_t buffern;
    uint32_t subpixel, rectcoords, blend, wrap, nomove;
    uint32_t preset;

    uint32_t __subpixel, __rectcoords, __blend, __wrap, __nomove;
    uint32_t w_adj, h_adj;
    uint32_t XRES, YRES;
    uint32_t width, height;
    uint8_t needs_init;
} DMovementPrivate;

typedef struct
{
  char *name;
  int rect;
  int wrap;
  int grid1;
  int grid2;
  char *init;
  char *pixel;
  char *frame;
  char *beat;
} presetType;

static presetType presets[]=
{
  {"Random Rotate", 0, 1, 2, 2, "","r = r + dr;","","dr = (rand(100) / 100) * $PI;\r\nd = d * .95;"},
  {"Random Direction", 1, 1, 2, 2, "speed=.05;dr = (rand(200) / 100) * $PI;","x = x + dx;\r\ny = y + dy;","dx = cos(dr) * speed;\r\ndy = sin(dr) * speed;","dr = (rand(200) / 100) * $PI;"},
  {"In and Out", 0, 1, 2, 2, "speed=.2;c=0;","d = d * dd;","","c = c + ($PI/2);\r\ndd = 1 - (sin(c) * speed);"},
  {"Unspun Kaleida", 0, 1, 33, 33, "c=200;f=0;dt=0;dl=0;beatdiv=8","r=cos(r*dr);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 4+(cos(dt)*2);","c=f;f=0;dl=dt"},
  {"Roiling Gridley", 1, 1, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","x=x+(sin(y*dx) * .03);\r\ny=y-(cos(x*dy) * .03);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndx = 14+(cos(dt)*8);\r\ndy = 10+(sin(dt*2)*4);","c=f;f=0;dl=dt"},
  {"6-Way Outswirl", 0, 0, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","d=d*(1+(cos(r*6) * .05));\r\nr=r-(sin(d*dr) * .05);\r\nd = d * .98;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 18+(cos(dt)*12);","c=f;f=0;dl=dt"},
  {"Wavy", 1, 1, 6, 6, "c=200;f=0;dx=0;dl=0;beatdiv=16;speed=.05","y = y + ((sin((x+dx) * $PI))*speed);\r\nx = x + .025","f = f + 1;\r\nt = ( (f * 2 * 3.1415) / c ) / beatdiv;\r\ndx = dl + t;","c = f;\r\nf = 0;\r\ndl = dx;"},
  {"Smooth Rotoblitter", 0, 1, 2, 2, "c=200;f=0;dt=0;dl=0;beatdiv=4;speed=.15","r = r + dr;\r\nd = d * dd;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = cos(dt)*speed*2;\r\ndd = 1 - (sin(dt)*speed);","c=f;f=0;dl=dt"},
};

int lv_dmovement_init (VisPluginData *plugin);
int lv_dmovement_cleanup (VisPluginData *plugin);
int lv_dmovement_events (VisPluginData *plugin, VisEventQueue *events);
int lv_dmovement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_dmovement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void trans_begin(DMovementPrivate *priv, uint8_t isBeat, uint32_t *fbin, uint32_t *fbout);
static void trans_render(DMovementPrivate *priv, uint8_t isBeat, uint32_t *fb, uint32_t *fbout);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_dmovement_palette,
		.video = lv_dmovement_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_dmovement",
		.name = "Libvisual AVS Transform: dynamic movement element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: dynamic movement element",
		.help = "This is the dynamic movement element for the libvisual AVS system",

		.init = lv_dmovement_init,
		.cleanup = lv_dmovement_cleanup,
		.events = lv_dmovement_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int trans_load_runnable(DMovementPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(DMovementPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_dmovement_init (VisPluginData *plugin)
{
	DMovementPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING ("init", "", "Init expression"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("frame", "", "Frame expression"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("beat", "", "Beat expression"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("pixel", "", "Pixel expression"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("preset", -1, VISUAL_PARAM_LIMIT_INTEGER(-1, 7), "Preset choice"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("rectcoords", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Rectangular coordinates"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("xres", 20, VISUAL_PARAM_LIMIT_INTEGER(2, 256), "Grid size x axis"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("yres", 20, VISUAL_PARAM_LIMIT_INTEGER(2, 256), "Grid size y axis"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("wrap", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Wrap"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("buffern", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 8), "Source"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("nomove", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "No movement (just blend)"),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (DMovementPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();

    avs_runnable_variable_bind(priv->vm, "d", &priv->var_d);
    avs_runnable_variable_bind(priv->vm, "b", &priv->var_b);
    avs_runnable_variable_bind(priv->vm, "r", &priv->var_r);
    avs_runnable_variable_bind(priv->vm, "x", &priv->var_x);
    avs_runnable_variable_bind(priv->vm, "y", &priv->var_y);
    avs_runnable_variable_bind(priv->vm, "w", &priv->var_w);
    avs_runnable_variable_bind(priv->vm, "h", &priv->var_h);
    avs_runnable_variable_bind(priv->vm, "alpha", &priv->var_alpha);

	return 0;
}

int lv_dmovement_cleanup (VisPluginData *plugin)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_mem_free(priv->init);
    visual_mem_free(priv->pixel);
    visual_mem_free(priv->frame);
    visual_mem_free(priv->beat);

	visual_mem_free (priv);

	return 0;
}

void load_preset(VisPluginData *plugin)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    VisParamEntry *param;
    
    param = visual_param_container_get(paramcontainer, "rectcoords");
    visual_param_entry_set_integer(param, presets[priv->preset].rect);

    param = visual_param_container_get(paramcontainer, "wrap");
    visual_param_entry_set_integer(param, presets[priv->preset].wrap);

    param = visual_param_container_get(paramcontainer, "init");
    visual_param_entry_set_string(param, presets[priv->preset].init);

    param = visual_param_container_get(paramcontainer, "pixel");
    visual_param_entry_set_string(param, presets[priv->preset].pixel);

    param = visual_param_container_get(paramcontainer, "frame");
    visual_param_entry_set_string(param, presets[priv->preset].frame);

    param = visual_param_container_get(paramcontainer, "beat");
    visual_param_entry_set_string(param, presets[priv->preset].beat);

    param = visual_param_container_get(paramcontainer, "preset");
    visual_param_entry_set_integer(param, -1);
}

int lv_dmovement_events (VisPluginData *plugin, VisEventQueue *events)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
			    if (visual_param_entry_is (param, "init")) {
                    if(priv->init)
                        visual_mem_free(priv->init);
					priv->init = strdup(visual_param_entry_get_string (param));
                    trans_load_runnable(priv, TRANS_RUNNABLE_INIT, priv->init);
                    priv->needs_init = TRUE;
				} else if (visual_param_entry_is (param, "frame")) {
                    if(priv->frame)
                        visual_mem_free(priv->frame);
					priv->frame = strdup(visual_param_entry_get_string (param));
                    trans_load_runnable(priv, TRANS_RUNNABLE_FRAME, priv->frame);
				} else if (visual_param_entry_is (param, "beat")) {
                    if(priv->beat)
                        visual_mem_free(priv->beat);
					priv->beat = strdup(visual_param_entry_get_string (param));
                    trans_load_runnable(priv, TRANS_RUNNABLE_BEAT, priv->beat);
				} else if (visual_param_entry_is (param, "pixel")) {
                    if(priv->pixel)
                        visual_mem_free(priv->pixel);
					priv->pixel = strdup(visual_param_entry_get_string (param));
                    trans_load_runnable(priv, TRANS_RUNNABLE_PIXEL, priv->pixel);
				} else if (visual_param_entry_is (param, "preset"))
					priv->preset = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "subpixel"))
                    priv->subpixel = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "rectcoords"))
                    priv->rectcoords = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "xres"))
                    priv->xres = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "yres"))
                    priv->yres = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "wrap"))
                    priv->wrap = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "buffern"))
                    priv->buffern = visual_param_entry_get_integer(param);
                else if (visual_param_entry_is (param, "nomove"))
                    priv->nomove = visual_param_entry_get_integer(param);

				break;

			default:
				break;
		}
	}

    if(priv->preset >= 0)
        load_preset(plugin);

	return 0;
}

int lv_dmovement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_dmovement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t *vidbuf, vidoutbuf;
    int i;
    uint8_t isBeat = visual_audio_is_beat(audio);

    if(priv->last_width != video->width || priv->last_height != video->height || priv->last_pitch != video->pitch) {
        //trans_initalize(priv, video->width, video->height);

        if(priv->swapbuf != NULL)
            visual_mem_free(priv->swapbuf);
        if(priv->renderbuf != NULL)
            visual_mem_free(priv->renderbuf);

        priv->swapbuf = visual_mem_malloc0(visual_video_get_size(video));
        priv->renderbuf = visual_mem_malloc0(video->width * video->height * video->bpp);
    }

    vidbuf = priv->swapbuf;
    for (i = 0; i < video->height; i++) {
        visual_mem_copy (vidbuf, pixels + (video->pitch * i), video->width * video->bpp);
        vidbuf += video->width * video->bpp;
    }

    visual_mem_set(priv->renderbuf, 0, video->width * video->height * video->bpp);

    trans_begin(priv, isBeat, (uint32_t *) priv->swapbuf, (uint32_t *)priv->renderbuf);

    priv->last_width = video->width;
    priv->last_height = video->height;
    priv->last_pitch = video->pitch;

    if(!isBeat)
        return 0;

    trans_render(priv, isBeat, (uint32_t *) priv->swapbuf, (uint32_t *)priv->renderbuf);

    for(i = 0; i < video->height; i++) {
        visual_mem_copy( pixels, priv->renderbuf + (i * video->width * video->bpp), video->width * video->bpp);
        pixels += video->pitch;
    }
    return 0;
}

static void trans_begin(DMovementPrivate *priv, uint8_t isBeat, uint32_t *fbin, uint32_t *fbout)
{
    priv->__subpixel = priv->subpixel;
    priv->__rectcoords = priv->rectcoords;
    priv->__blend = priv->blend;
    priv->__wrap = priv->wrap;
    priv->__nomove = priv->nomove;

    priv->w_adj = (priv->width - 2)<<16;
    priv->h_adj = (priv->height - 2)<<16;
    
    if(priv->last_xres != priv->xres || priv->last_yres != priv->yres || 
            priv->last_width != priv->width || priv->last_height != priv->height || 
            !priv->tab || !priv->wmul ) {
        int y;
        priv->last_xres = priv->xres;
        priv->last_yres = priv->yres;
        priv->last_width = priv->width;
        priv->last_height = priv->height;

        if(priv->wmul) 
            visual_mem_free(priv->wmul);
        priv->wmul = visual_mem_malloc0(sizeof(int)*priv->height);

        for(y = 0; y < priv->height; y++) 
            priv->wmul[y] = y * priv->width;

        if(priv->tab)
            visual_mem_free(priv->tab);

        priv->tab = visual_mem_malloc0((priv->xres * priv->yres * 3 + (priv->xres * 6 + 6)) * sizeof(int));
    }

    if(!priv->__subpixel)
    {
        priv->w_adj = (priv->width-1)<<16;
        priv->w_adj = (priv->height-1)<<16;
    }

    if(isBeat&0x80000000)
        return;

    priv->var_w = (AvsNumber)priv->width;
    priv->var_h = (AvsNumber)priv->height;
    priv->var_b = isBeat?1.0:0.0;
    priv->var_alpha = 0.5;

    if(priv->needs_init) {
        priv->needs_init = FALSE;
        trans_run_runnable(priv, TRANS_RUNNABLE_INIT);
    }
    // execute code
    trans_run_runnable(priv, TRANS_RUNNABLE_FRAME);

    if(isBeat)
        trans_run_runnable(priv, TRANS_RUNNABLE_BEAT);

    {
        int32_t x, y;
        uint32_t *tabptr=priv->tab;

        double xsc = 2.0/priv->width, ysc=2.0/priv->height;
        double dw2 = ((double)priv->width*32768.0);
        double dh2 = ((double)priv->height*32768.0);
        double max_screen_d = sqrt((double)(priv->width*priv->width + priv->height*priv->height)) * 0.5;

        double divmax_d = 1.0/max_screen_d;

        int yc_pos, yc_dpos, xc_pos, xc_dpos;

        max_screen_d *= 65536.0;

        yc_pos = 0;
        xc_dpos = (priv->width << 16)/(priv->xres-1);
        yc_dpos = (priv->height << 16)/(priv->yres-1);
        for(y = 0; y < priv->yres; y++) 
        {
            xc_pos = 0;
            for(x = 0; x < priv->xres; x++)
            {
                double xd, yd;
                int tmp1, tmp2;
                double va;
                int a;

                xd = ((double)xc_pos-dw2)*(1.0/65536.0);
                yd = ((double)yc_pos-dh2)*(1.0/65536.0);

                xc_pos+=xc_dpos;

                priv->var_x = xd*xsc;
                priv->var_y = yd*ysc;
                priv->var_d = sqrt(xd*xd + yd*yd)*divmax_d;
                priv->var_r = atan2(yd, xd) + PI*0.5;

                trans_run_runnable(priv, TRANS_RUNNABLE_PIXEL);

                if(!priv->__rectcoords)
                {
                    priv->var_d *= max_screen_d;
                    priv->var_r -= PI*0.5;
                    tmp1 = (int) (dw2 + cos(priv->var_r) * priv->var_d);
                    tmp2 = (int) (dh2 + sin(priv->var_r) * priv->var_d);
                }
                else
                {
                    tmp1 = (int) ((priv->var_x+1.0)*dw2);
                    tmp2 = (int) ((priv->var_y+1.0)*dh2);
                }
                if(!priv->__wrap)
                {
                    if(tmp1 < 0)            tmp1 = 0;
                    if(tmp1 > priv->w_adj)  tmp1 = priv->w_adj;
                    if(tmp2 < 0)            tmp2 = 0;
                    if(tmp2 > priv->h_adj)  tmp2 = priv->h_adj;
                }   

                *tabptr++ = tmp1;
                *tabptr++ = tmp2;
                va = priv->var_alpha;
                if(va < 0.0) 
                    va = 0.0;
                else if (va > 1.0)
                    va = 1.0;
                a = (int)(va*255.0*65536.0);
                *tabptr++ = a;
            }
            yc_pos+=yc_dpos;
        }
    }
}

static void trans_render(DMovementPrivate *priv, uint8_t isBeat, uint32_t *fb, uint32_t *fbout)
{
    int max_threads = 1;
    int this_thread = 0;

    int start_l = (this_thread * priv->height) / max_threads;
    int end_l;
    int ypos = 0;

    if(this_thread >= max_threads - 1) end_l = priv->height;
    else end_l = ((this_thread+1) * priv->height) / max_threads;

    int outh = end_l-start_l;

    uint32_t *fbin = fb; // !buffern ? fb | (int *)getGlobalBuffer(w, h, buffern-1, 0);
{}
    if(outh<1) return;

    {
        uint32_t *interptab = priv->tab + priv->xres * priv->yres * 3 + this_thread * (priv->xres * 6 + 6);
        uint32_t *rdtab = priv->tab;
        unsigned int *in = (unsigned int *)fbin;
        unsigned int *blendin = (unsigned int *)fb;
        unsigned int *out = (unsigned int *)fbout;
        int yseek = 1;
        int xc_dpos, yc_pos=0, yc_dpos;
        xc_dpos = (priv->width<<16)/(priv->xres-1);
        yc_dpos = (priv->height<<16)/(priv->yres-1);
        int lypos = 0;
        int yl = end_l;
        while(yl>0)
        {
            yc_pos+=yc_dpos;
            yseek=(yc_pos>>16)-lypos;
            if( !yseek ) 
            {
                #ifdef HAS_MMX
                    //__asm emms;
                #endif
                return;
            }
            lypos = yc_pos >> 16;
            int l = priv->xres;
            uint32_t *stab = interptab;
            int xr3=priv->xres * 3;
            while(l--)
            {
                int tmp1, tmp2, tmp3;
                tmp1 = rdtab[0];
                tmp2 = rdtab[1];
                tmp3 = rdtab[2];
                stab[0] = tmp1;
                stab[1] = tmp2;
                stab[2] = (rdtab[xr3]-tmp1)/yseek;
                stab[3] = (rdtab[xr3+1]-tmp2)/yseek;
                stab[4] = tmp3;
                stab[5] = (rdtab[xr3+2]-tmp3)/yseek;
                rdtab+=3;
                stab+=6;
            }

            if(yseek > yl) yseek = yl;
            yl-=yseek;

            if(yseek > 0) while(yseek--)
            {
                int d_x;
                int d_y;
                int d_a;
                int ap;
                int seek;
                uint32_t *seektab = interptab;
                int xp,yp;
                int l = priv->width;
                int lpos = 0;
                int xc_pos = 0;
                ypos++;
                {
                    while(l>0)
                    {
                        xc_pos+=xc_dpos;
                        seek = (xc_pos>>16)-lpos;
                        if(!seek)
                        {
                            #ifdef HAS_MMX
                                //__asm emms;
                            #endif
                            return;
                        }
                        lpos = xc_pos >> 16;
                        xp = seektab[0];
                        yp = seektab[1];
                        ap = seektab[4];
                        d_a = (seektab[10]-ap)/seek;
                        d_x = (seektab[6]-xp)/seek;
                        d_y = (seektab[7]-yp)/seek;
                        seektab[0] += seektab[2];
                        seektab[1] += seektab[3];
                        seektab[4] += seektab[5];
                        seektab += 6;

                        if(seek > l) 
                            seek = l;

                        l -= seek;
                        if(seek > 0 && ypos <= start_l)
                        {
                            blendin+=seek;
                            if(priv->__nomove) in+=seek;
                            else out+=seek;

                            seek = 0;
                        }

                        if (seek > 0)
                        {
#define CHECK
#define NORMAL_LOOP(Z) while ((seek--)) { Z; yp+=d_x; yp+=d_y; }

#define WRAPPING_LOOPS(Z) \
    NORMAL_LOOP(if (xp < 0) xp += priv->w_adj;  \
                else if (xp >= priv->w_adj) xp-=priv->w_adj;  \
                if (yp < 0) yp += priv->h_adj;  \
                else if (yp >= priv->h_adj) yp-=priv->h_adj;  \
                Z)

#define CLAMPED_LOOPS(Z) \
    NORMAL_LOOP(if (xp < 0) xp=0; \
                else if (xp >= priv->w_adj) xp=priv->w_adj-1; \
                if (yp < 0) yp=0; \
                else if (yp >= priv->h_adj) yp=priv->h_adj-1; \
                Z)

#define LOOPS(DO)  \
                if (priv->__blend && priv->__subpixel) DO(CHECK *out++=BLEND_ADJ(&priv->obj, BLEND4_16(&priv->obj, in+(xp>>16)+(priv->wmul[yp>>16]),priv->width,xp,yp),*blendin++,ap>>16); ap+=d_a) \
                else if (priv->__blend) DO(CHECK *out++=BLEND_ADJ(&priv->obj, in[(xp>>16)+(priv->wmul[yp>>16])],*blendin++,ap>>16); ap+=d_a) \
                else if (priv->__subpixel) DO(CHECK *out++=BLEND4_16(&priv->obj, in+(xp>>16)+(priv->wmul[yp>>16]),priv->width,xp,yp)) \
                else DO(CHECK *out++=in[(xp>>16)+(priv->wmul[yp>>16])])

                            if(priv->__nomove)
                            {
                                if(fbin != fb) while(seek--)
                                {
                                    *blendin=BLEND_ADJ(&priv->obj, *in++, *blendin, ap>>16);
                                    ap+=d_a;
                                    blendin++;
                                }
                                else while( seek--)
                                {
                                    *blendin=BLEND_ADJ(&priv->obj, 0, *blendin, ap>>16);
                                    ap+=d_a;
                                    blendin++;
                                }
                            }
                            else if (!priv->__wrap)
                            {
                                if(xp < 0) xp = 0;
                                else if (xp >= priv->w_adj) xp=priv->w_adj-1;
                                if(yp < 0) yp = 0;
                                else if (yp >= priv->h_adj) yp=priv->h_adj-1;

                                LOOPS(CLAMPED_LOOPS)
                            }
                            else
                            {
                                xp %= priv->w_adj;
                                yp %= priv->h_adj;
                                if(xp < 0) xp+=priv->w_adj;
                                if(yp < 0) yp+=priv->h_adj;

                                if(d_x <= -priv->w_adj) d_x = -priv->w_adj + 1;
                                else if (d_x >= priv->w_adj) d_x = priv->w_adj - 1;

                                if(d_y <= -priv->h_adj) d_y = -priv->h_adj + 1;
                                else if (d_y >= priv->h_adj) d_y = priv->h_adj - 1;

                                LOOPS(WRAPPING_LOOPS)
                            }
                        } // if seek>0
                    }
                    seektab[0] += seektab[2];
                    seektab[1] += seektab[3];
                    seektab[4] += seektab[5];
                }
            }
        }

    }

#ifdef HAVE_MMX
    //__asm emms;
#endif
}

