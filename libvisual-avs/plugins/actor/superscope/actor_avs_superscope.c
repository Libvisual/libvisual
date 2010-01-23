/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_superscope.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
 */
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs.h"

AvsNumber PI = M_PI;

typedef enum scope_runnable ScopeRunnable;

enum scope_runnable {
	SCOPE_RUNNABLE_INIT,
	SCOPE_RUNNABLE_FRAME,
	SCOPE_RUNNABLE_BEAT,
	SCOPE_RUNNABLE_POINT,
};

typedef struct {
	AvsRunnableContext		*ctx;
	AvsRunnableVariableManager	*vm;
	AvsRunnable			*runnable[4];
	AvsNumber			n, b, x, y, i, v, w, h, red, green, blue, linesize, skip, drawmode; 


	char			*point;
	char			*frame;
	char			*beat;
	char			*init;
	int			 channel_source;
	int			 draw_type;
	VisPalette		 pal;

    int             needs_init;

	AVSGfxColorCycler	*cycler;
} SuperScopePrivate;

int lv_superscope_init (VisPluginData *plugin);
int lv_superscope_cleanup (VisPluginData *plugin);
int lv_superscope_requisition (VisPluginData *plugin, int *width, int *height);
int lv_superscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_superscope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_superscope_palette (VisPluginData *plugin);
int lv_superscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (SuperScopePrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_superscope_requisition,
		.palette = lv_superscope_palette,
		.render = lv_superscope_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR, //".[avs]",

		.plugname = "avs_superscope",
		.name = "Libvisual AVS Render: superscope element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: superscope element",
		.help = "This is the superscope element for the libvisual AVS system",

		.init = lv_superscope_init,
		.cleanup = lv_superscope_cleanup,
		.events = lv_superscope_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int scope_load_runnable(SuperScopePrivate *priv, ScopeRunnable runnable, char *buf)
{
	AvsRunnable *obj = avs_runnable_new(priv->ctx);
	avs_runnable_set_variable_manager(obj, priv->vm);
	priv->runnable[runnable] = obj;
	avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
	return 0;
}

int scope_run(SuperScopePrivate *priv, ScopeRunnable runnable)
{
	avs_runnable_execute(priv->runnable[runnable]);
	return 0;
}


int lv_superscope_init (VisPluginData *plugin)
{
	SuperScopePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING ("point", "d=i+v*0.2; r=t+i*$PI*4; x = cos(r)*d; y = sin(r) * d;", ""),
		VISUAL_PARAM_LIST_ENTRY_STRING ("frame", "t=t-0.01;", ""),
		VISUAL_PARAM_LIST_ENTRY_STRING ("beat", "", ""),
		VISUAL_PARAM_LIST_ENTRY_STRING ("init", "n=800;", ""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("channel source", 0, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("draw type", 0, VISUAL_PARAM_LIMIT_NONE, ""),
		VISUAL_PARAM_LIST_END
	};

	visual_param_container_add_many_proxy (paramcontainer, params);

    VisString string;

    visual_string_init(&string);

    visual_string_set(&string, "init");

    VisParamEntry *param = visual_param_container_get(paramcontainer, &string);
    visual_param_entry_set_string(param, "n=8;g=23;ig=1/(g-1);mhx=getosc(.41,0,0)*2;mfx=sin((getosc(0.41,0,0)+getosc(0.78,0,0))*5);mfy=sin((getosc(0.71,0,0)+getosc(.13,0,0))*5);d=.4;pi=acos(-1); \
    md=bnot(md);d=-d;rfz=rfz+sin(getosc(.41,0,0)+getosc(.74,0,0)+getosc(.55,0,0));mfx=sin((getosc(0.41,0,0)+getosc(0.78,0,0))*5)*2.2;mfy=sin((getosc(0.71,0,0)+getosc(.13,0,0))*5)*1.2-.25; \
    mgx=mfx;mgy=mfy;mhx=mfx;mhy=mfy;");

    visual_string_set(&string, "frame");
    param = visual_param_container_get(paramcontainer, &string);
    visual_param_entry_set_string(param, "ti=ti*.8;ph=0; \
    gx=3;gy=2;t=t+d;tm=gettime(0); \
    dec=pow(0.95,30*(tm-lasttime)); \
    lasttime=tm; \
    rgz=rfz+(rgz-rfz)*dec; \
    rhz=rgz+(rhz-rgz)*dec; \
    cr=cos(rhz);sr=sin(rhz); \
    mgx=mfx+(mgx-mfx)*dec; \
    mgy=mfy+(mgy-mfy)*dec; \
    mhx=mgx+(mhx-mgx)*dec; \
    mhy=mgy+(mhy-mgy)*dec; \
    jk=jk+.1;af=w/h; \
    £XXXXXXXXXXXXXXXXXXXX;px=.5;py=.5;x=px*px-py*py+sin(px)+mhx;y=2*px*py+mhy;px=x;py=y;x=px*px-py*py+sin(px)+mhx;y=2*px*py-cos(py)*.5+mhy;px=x;py=y;x=px*px-py*py+sin(px);y=2*px*py;esx=x;esy=y;px=.3;py=.5;x=px*px-py*py+sin(px)+mhx;y=2*px*py+mhy;px=x;py=y;x=px*px-py*py+sin(px)+mhx;y=2*px*py-cos(py)*.5+mhy;px=x;py=y;x=px*px-py*py+sin(px);y=2*px*py;esx=esx+x;esy=esy+y;px=.5;py=.14;x=px*px-py*py+sin(px)+mhx;y=2*px*py+mhy;px=x;py=y;x=px*px-py*py+sin(px)+mhx;y=2*px*py-cos(py)*.5+mhy;px=x;py=y;x=px*px-py*py+sin(px);y=2*px*py;esx=esx+x;esy=esy+y;px=.3;py=.14;x=px*px-py*py+sin(px)+mhx;y=2*px*py+mhy;px=x;py=y;x=px*px-py*py+sin(px)+mhx;y=2*px*py-cos(py)*.5+mhy;px=x;py=y;x=px*px-py*py+sin(px);y=2*px*py;esx=(esx+x)*.0125;esy=(esy+y)*.0125;");

    visual_string_set(&string, "beat");
    param = visual_param_container_get(paramcontainer, &string);
    visual_param_entry_set_string(param, "md=bnot(md);d=-d;rfz=rfz+sin(getosc(.41,0,0)+getosc(.74,0,0)+getosc(.55,0,0));mfx=sin((getosc(0.41,0,0)+getosc(0.78,0,0))*5)*2.2;mfy=sin((getosc(0.71,0,0)+getosc(.13,0,0))*5)*1.2-.25;ti=1;");

    visual_string_set(&string, "point");
    param = visual_param_container_get(paramcontainer, &string);
    visual_param_entry_set_string(param, "px=(gy*ig*2-1)*.8;py=(gx*ig*1.2-.2)*.8; \
    x=px*px-py*py+sin(px)+mhx;y=2*px*py+mhy;px=x;py=y; \
    x=px*px-py*py+sin(px)+mhx;y=2*px*py-cos(py)*.5+mhy;px=x;py=y; \
    x=px*px-py*py+sin(px);py=.3*px*py-esy;px=x*.15-esx; \
    x=px*cr-py*sr;y=px*sr+py*cr; \
    px=(ti*.05+.1)*4; \
    i=equal(ph,0)+if(equal(ph,1),px,0)-equal(ph,4)-if(equal(ph,5),px,0); \
    px=equal(ph,2)+if(equal(ph,3),px,0)-equal(ph,6)-if(equal(ph,7),px,0); \
    x=x+i*.23;y=y+px*af*.23; \
    red=if(bnot(equal(ph,2)+equal(ph,4)+equal(ph,6)),sin(jk)*.125+.875,0);green=red;blue=red; \
    ph=ph+1;");

	priv = visual_mem_new0 (SuperScopePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 1);

	for (i = 0; i < priv->pal.ncolors; i++) {
		priv->pal.colors[i].r = 0xff;
		priv->pal.colors[i].g = 0xff;
		priv->pal.colors[i].b = 0xff;
	}

	visual_param_entry_set_palette (visual_param_container_get (paramcontainer, VIS_BSTR ("palette")), &priv->pal);

	visual_palette_free_colors (&priv->pal);

	/* Init super scope */
	priv->ctx = avs_runnable_context_new();
	priv->vm = avs_runnable_variable_manager_new();

	/* Bind variables to context */
	avs_runnable_variable_bind(priv->vm, "n", &priv->n);
	avs_runnable_variable_bind(priv->vm, "b", &priv->b);
	avs_runnable_variable_bind(priv->vm, "x", &priv->x);
	avs_runnable_variable_bind(priv->vm, "y", &priv->y);
	avs_runnable_variable_bind(priv->vm, "i", &priv->i);
	avs_runnable_variable_bind(priv->vm, "v", &priv->v);
	avs_runnable_variable_bind(priv->vm, "w", &priv->w);
	avs_runnable_variable_bind(priv->vm, "h", &priv->h);
	avs_runnable_variable_bind(priv->vm, "$PI", &PI);
	avs_runnable_variable_bind(priv->vm, "red", &priv->red);
	avs_runnable_variable_bind(priv->vm, "green", &priv->green);
	avs_runnable_variable_bind(priv->vm, "blue", &priv->blue);
	avs_runnable_variable_bind(priv->vm, "linesize", &priv->linesize);
	avs_runnable_variable_bind(priv->vm, "skip", &priv->skip);
	avs_runnable_variable_bind(priv->vm, "drawmode", &priv->drawmode);

	return 0;
}

int lv_superscope_cleanup (VisPluginData *plugin)
{
	SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_superscope_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_superscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_superscope_events (VisPluginData *plugin, VisEventQueue *events)
{
	SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_superscope_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
                printf("superscope_events param %p %p\n", param, param->name);

				if (visual_param_entry_is (param, VIS_BSTR ("point"))) {

					priv->point = visual_param_entry_get_string (param);
					scope_load_runnable(priv, SCOPE_RUNNABLE_POINT, priv->point);

				} else if (visual_param_entry_is (param, VIS_BSTR ("frame"))) {

					priv->frame = visual_param_entry_get_string (param);
					scope_load_runnable(priv, SCOPE_RUNNABLE_FRAME, priv->frame);

				} else if (visual_param_entry_is (param, VIS_BSTR ("beat"))) {

					priv->beat = visual_param_entry_get_string (param);
					scope_load_runnable(priv, SCOPE_RUNNABLE_BEAT, priv->beat);

				} else if (visual_param_entry_is (param, VIS_BSTR ("init"))) {

					priv->init = visual_param_entry_get_string (param);
					scope_load_runnable(priv, SCOPE_RUNNABLE_INIT, priv->init);
                    priv->needs_init = TRUE;

				} else if (visual_param_entry_is (param, VIS_BSTR ("channel source")))
					priv->channel_source = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("draw type")))
					priv->draw_type = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, VIS_BSTR ("palette"))) {
					VisPalette *pal;

					pal = visual_param_entry_get_palette (param);

					visual_palette_free_colors (&priv->pal);
					visual_palette_allocate_colors (&priv->pal, pal->ncolors);
					visual_palette_copy (&priv->pal, pal);

					if (priv->cycler != NULL)
						visual_object_unref (VISUAL_OBJECT (priv->cycler));

					priv->cycler = avs_gfx_color_cycler_new (&priv->pal);
					avs_gfx_color_cycler_set_mode (priv->cycler, AVS_GFX_COLOR_CYCLER_TYPE_TIME);
					avs_gfx_color_cycler_set_time (priv->cycler, avs_config_standard_color_cycler_time ());
				}

				break;

			default:
				break;
		}
	}

	return 0;
}
/*
static int makeint(AvsNumber t)
{
	if (t <= 0.0)
		return 0;
	if (t >= 1.0)
		return 255;
	return (int)(t*255.0);
}
*/
VisPalette *lv_superscope_palette (VisPluginData *plugin)
{
	//SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_superscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint32_t *buf = visual_video_get_pixels (video);
	VisBuffer pcm;
	float pcmbuf[576*4];
	int isBeat = 0;

    visual_buffer_set_data_pair (&pcm, pcmbuf, sizeof (pcmbuf));

	visual_audio_get_sample_mixed (audio, &pcm, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

    visual_mem_copy(vispcmdata, pcmbuf, 576*4);

    if(priv->needs_init) {
        priv->needs_init = FALSE;
	    scope_run(priv, SCOPE_RUNNABLE_INIT);
    }

//	visual_video_fill_color(video, visual_color_black()); 
    buf = visual_video_get_pixels (video);

	int a, l, lx = 0, ly = 0, x = 0, y = 0;
	
	scope_run(priv, SCOPE_RUNNABLE_FRAME);
//	priv->beat = isBeat;
	if (isBeat)
		scope_run(priv, SCOPE_RUNNABLE_BEAT);


	l = priv->n;
	if (l > 128*1024)
		l = 128*1024;

	priv->drawmode = 1.0; /* 0 = dots, 1 = lines */
	for (a=0; a < l; a++, lx = x, ly = y) {
		priv->v = pcmbuf[a * 288 / l];
		priv->i = (AvsNumber)a/(AvsNumber)(l-1);
		priv->skip = 0.0;
		scope_run(priv, SCOPE_RUNNABLE_POINT);

		x = (int)((priv->x + 1.0) * (AvsNumber)video->width * 0.5);
		y = (int)((priv->y + 1.0) * (AvsNumber)video->height * 0.5);

		if (priv->skip >= 0.00001)
			continue;

		//int this_color = makeint(priv->blue) |
		//		 (makeint(priv->green) << 8) |
		//		 (makeint(priv->red) << 16);

		if (priv->drawmode < 0.00001) {
			if (y >= 0 && y < video->height && x >= 0 && x < video->width)
				*((buf) + y * video->pitch / 4 + x) = 0xffffff;
		} else {
			if (a > 0) {
				if (y >= 0 && y < video->height && x >= 0 && x < video->width &&
				    ly >= 0 && ly < video->height && lx >= 0 && lx < video->width)
						avs_gfx_line_ints(video, lx, ly, x, y, visual_color_white());
			}
		}
				
	}

	return 0;
}

short get_data (SuperScopePrivate *priv, VisAudio *audio, int index)
{
#if 0
	if (priv->type == 0)
		return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SCOPE, priv->source, index) >> 9;
	else
		return avs_sound_get_from_source (audio, AVS_SOUND_SOURCE_TYPE_SPECTRUM, priv->source, index);
#endif
	return 0;
}

