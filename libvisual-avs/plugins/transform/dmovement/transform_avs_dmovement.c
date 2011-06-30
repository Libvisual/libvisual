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
#include <omp.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "avs.h"
#include "lvavs_pipeline.h"

AvsNumber PI = M_PI;

typedef enum trans_runnable TransRunnable;

enum trans_runnable {
    TRANS_RUNNABLE_INIT,
    TRANS_RUNNABLE_FRAME,
    TRANS_RUNNABLE_BEAT,
    TRANS_RUNNABLE_PIXEL,
};

typedef struct {
    LVAVSPipeline *pipeline;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsNumber var_d, var_b, var_r, var_x, var_y, var_w, var_h, var_alpha;

    int *m_tab;
    int *m_wmul;

    int32_t m_lastw, m_lasth;
    int32_t m_lastxres, m_lastyres, m_xres, m_yres;

    int32_t buffern;
    int32_t preset;

    int32_t __subpixel, __rectcoords, __blend, __wrap, __nomove;
    int32_t subpixel, rectcoords, blend, wrap, nomove;
    int32_t w_adj, h_adj;
    int yres, xres;
    int8_t needs_init;
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
  {"Random Rotate", 0, 1, 2, 2, "","r = r + dr;","","dr = (rand(100) / 100) * PI;d = d * .95;"},
  {"Random Direction", 1, 1, 2, 2, "speed=.05;dr = (rand(200) / 100) * PI;","x = x + dx;y = y + dy;","dx = cos(dr) * speed;y = sin(dr) * speed;","dr = (rand(200) / 100) * PI;"},
  {"In and Out", 0, 1, 2, 2, "speed=.2;c=0;","d = d * dd;","","c = c + (PI/2);dd = 1 - (sin(c) * speed);"},
  {"Unspun Kaleida", 0, 1, 33, 33, "c=200;f=0;dt=0;dl=0;beatdiv=8","r=cos(r*dr);","f = f + 1;t = ((f * PI * 2)/c)/beatdiv;dt = dl + t;dr = 4+(cos(dt)*2);","c=f;f=0;dl=dt"},
  {"Roiling Gridley", 1, 1, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","x=x+(sin(y*dx) * .03);y=y-(cos(x*dy) * .03);","f = f + 1;t = ((f * PI * 2)/c)/beatdiv;dt = dl + t;dx = 14+(cos(dt)*8);dy = 10+(sin(dt*2)*4);","c=f;f=0;dl=dt"},
  {"6-Way Outswirl", 0, 0, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","d=d*(1+(cos(r*6) * .05));r=r-(sin(d*dr) * .05);d = d * .98;","f = f + 1;t = ((f * PI * 2)/c)/beatdiv;dt = dl + t;dr = 18+(cos(dt)*12);","c=f;f=0;dl=dt"},
  {"Wavy", 1, 1, 6, 6, "c=200;f=0;dx=0;dl=0;beatdiv=16;speed=.05","y = y + ((sin((x+dx) * PI))*speed);x = x + .025","f = f + 1;t = ( (f * 2 * 3.1415) / c ) / beatdiv;dx = dl + t;","c = f;f = 0;dl = dx;"},
  {"Smooth Rotoblitter", 0, 1, 2, 2, "c=200;f=0;dt=0;dl=0;beatdiv=4;speed=.15","r = r + dr;d = d * dd;","f = f + 1;t = ((f * PI * 2)/c)/beatdiv;dt = dl + t;dr = cos(dt)*speed*2;dd = 1 - (sin(dt)*speed);","c=f;f=0;dl=dt"},
};

int lv_dmovement_init (VisPluginData *plugin);
int lv_dmovement_cleanup (VisPluginData *plugin);
int lv_dmovement_events (VisPluginData *plugin, VisEventQueue *events);
int lv_dmovement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_dmovement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static int trans_begin(DMovementPrivate *priv, int max_threads, char visdata[2][2][576], int isBeat, int *framebuffer, int *fbout, int w, int h);
static int trans_render(DMovementPrivate *priv, int this_thread, int max_threads, char visdata[2][2][576], int isBeat, int *framebuffer, int *fbout, int w, int h);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count);
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

static int trans_load_runnable(DMovementPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

static int trans_run_runnable(DMovementPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_dmovement_init (VisPluginData *plugin)
{
	DMovementPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING ("init", ""),//, "Set intial variable values here"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("frame", ""),//, "Used to define movement and transformations"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("beat", ""),//, "Expression that gets evaluated on the beat"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("pixel", ""),//, "This is where the shape is defined"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("preset", -1),//, VISUAL_PARAM_LIMIT_INTEGER(-1, 7), "Preset choice"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 1),//, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("rectcoords", 0),//, VISUAL_PARAM_LIMIT_BOOLEAN, "Rectangular coordinates"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("xres", 16),//, VISUAL_PARAM_LIMIT_INTEGER(2, 256), "Grid size x axis"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("yres", 16),//, VISUAL_PARAM_LIMIT_INTEGER(2, 256), "Grid size y axis"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0),//, VISUAL_PARAM_LIMIT_BOOLEAN, "Blend"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("wrap", 0),//, VISUAL_PARAM_LIMIT_BOOLEAN, "Wrap"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("buffern", 0),//, VISUAL_PARAM_LIMIT_INTEGER(0, 8), "Source buffer"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("nomove", 0),//, VISUAL_PARAM_LIMIT_BOOLEAN, "No movement (just blend)"),
		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (DMovementPrivate, 1);

    priv->pipeline = LVAVS_PIPELINE(visual_object_get_private(VISUAL_OBJECT(plugin)));
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many (paramcontainer, params);

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

	visual_mem_free (priv);

	return 0;
}

static void load_preset(VisPluginData *plugin)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    VisParamEntry *param;
    
    param = visual_param_container_get(paramcontainer, "rectcoords");
    visual_param_entry_set_integer(param, presets[priv->preset].rect);

    param = visual_param_container_get(paramcontainer, "wrap");
    visual_param_entry_set_integer(param, presets[priv->preset].wrap);

    param = visual_param_container_get(paramcontainer, "xres");
    visual_param_entry_set_integer(param, presets[priv->preset].grid1);

    param = visual_param_container_get(paramcontainer, "yres");
    visual_param_entry_set_integer(param, presets[priv->preset].grid2);

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

    visual_plugin_events_pump(plugin);
}

int lv_dmovement_events (VisPluginData *plugin, VisEventQueue *events)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;
    char *str;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
			    if (visual_param_entry_is (param, "init")) {
					char *init = visual_param_entry_get_string (param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_INIT, init);
                    priv->needs_init = TRUE;
				} else if (visual_param_entry_is (param, "frame")) {
					char *frame = visual_param_entry_get_string (param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_FRAME, frame);
				} else if (visual_param_entry_is (param, "beat")) {
					char *beat = visual_param_entry_get_string (param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_BEAT, beat);
				} else if (visual_param_entry_is (param, "pixel")) {
					char *pixel = visual_param_entry_get_string (param);
                    trans_load_runnable(priv, TRANS_RUNNABLE_PIXEL, pixel);
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

    if(priv->preset >= 0 && priv->preset < VISUAL_TABLESIZE(presets)) {
        load_preset(plugin);
    }

	return 0;
}

int lv_dmovement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_dmovement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DMovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t isBeat = priv->pipeline->isBeat;
    int max_threads = 4;
    int w = video->width, h = video->height;
    void *visdata = priv->pipeline->audiodata;
    int *framebuffer = priv->pipeline->framebuffer;
    int *fbout = priv->pipeline->fbout;
    int this_thread = 0;


#pragma omp parallel
{
    max_threads = omp_get_num_threads();
}
    trans_begin(priv, max_threads, visdata, isBeat, framebuffer, fbout, w, h);
    //if(!isBeat)
    //    return 0;

#pragma omp parallel
{
#pragma omp for private(this_thread)
    for( this_thread = max_threads - 1; this_thread>=0; this_thread--)
        trans_render(priv, this_thread, max_threads, visdata, isBeat, framebuffer,fbout, w, h);
}
    priv->pipeline->swap = !priv->__nomove;

    return 0;
}

///////////////////////////////////////


int trans_begin(DMovementPrivate *priv, int max_threads, char visdata[2][2][576], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
  priv->__subpixel=priv->subpixel;
  priv->__rectcoords=priv->rectcoords;
  priv->__blend=priv->blend;
  priv->__wrap=priv->wrap;
  priv->__nomove=priv->nomove;

  priv->w_adj=(w-2)<<16;
  priv->h_adj=(h-2)<<16;
  priv->xres=priv->m_xres+1;
  priv->yres=priv->m_yres+1;

  if (priv->xres < 2) priv->xres=2;
  if (priv->xres > 256) priv->xres=256;
  if (priv->yres < 2) priv->yres=2;
  if (priv->yres > 256) priv->yres=256;

  if (priv->m_lasth != h || priv->m_lastw != w || !priv->m_tab || !priv->m_wmul || 
    priv->m_lastxres != priv->xres || priv->m_lastyres != priv->yres)
  {
    int y;
    priv->m_lastxres = priv->xres;
    priv->m_lastyres = priv->yres;
    priv->m_lastw=w;
    priv->m_lasth=h;

    if(priv->m_wmul) 
        visual_mem_free(priv->m_wmul);

    priv->m_wmul = visual_mem_malloc(sizeof(int)*h);

    for (y = 0; y < h; y ++) 
        priv->m_wmul[y]=y*w;

    if (priv->m_tab) 
        visual_mem_free(priv->m_tab);

    priv->m_tab= visual_mem_malloc((priv->xres*priv->yres*3 + (priv->xres*6 + 6)*max_threads)*sizeof(int));
  }

  if (!priv->__subpixel)
  {
    priv->w_adj=(w-1)<<16;
    priv->h_adj=(h-1)<<16;
  }

  if (isBeat&0x80000000) return 0;

  int *fbin = framebuffer;//priv->buffern ? framebuffer : visual_video_get_pixels(priv->pipeline->buffers[priv->buffern-1]);
  if (!fbin) return 0;

  priv->var_w=(double)w;
  priv->var_h=(double)h;
  priv->var_b=isBeat?1.0:0.0;
  priv->var_alpha=0.5;
  if(priv->needs_init) {
    priv->needs_init = FALSE;
    trans_run_runnable(priv, TRANS_RUNNABLE_INIT);
  }
  trans_run_runnable(priv, TRANS_RUNNABLE_FRAME);

  if (isBeat)
    trans_run_runnable(priv, TRANS_RUNNABLE_BEAT);

  {
    int x;
    int y;
    int *tabptr=priv->m_tab;

    double xsc=2.0/w,ysc=2.0/h;
    double dw2=((double)w*32768.0);
    double dh2=((double)h*32768.0);
    double max_screen_d=sqrt((double)(w*w+h*h))*0.5;
    
    double divmax_d=1.0/max_screen_d;

    max_screen_d *= 65536.0;

    int yc_pos, yc_dpos, xc_pos, xc_dpos;
    yc_pos=0;
    xc_dpos = (w<<16)/(priv->xres-1);
    yc_dpos = (h<<16)/(priv->yres-1);
    for (y = 0; y < priv->yres; y ++)
    {
      xc_pos=0;
      for (x = 0; x < priv->xres; x ++)
      {
        double xd,yd;
        
        xd=((double)xc_pos-dw2)*(1.0/65536.0);
        yd=((double)yc_pos-dh2)*(1.0/65536.0);

        xc_pos+=xc_dpos;

        priv->var_x=xd*xsc;
        priv->var_y=yd*ysc;
        priv->var_d=sqrt(xd*xd+yd*yd)*divmax_d;
        priv->var_r=atan2(yd,xd) + M_PI*0.5;

        trans_run_runnable(priv, TRANS_RUNNABLE_PIXEL);

        int tmp1,tmp2;
        if (!priv->__rectcoords)
        {
          priv->var_d *= max_screen_d;
          priv->var_r -= M_PI*0.5;
          tmp1=(int) (dw2 + cos(priv->var_r) * priv->var_d);
          tmp2=(int) (dh2 + sin(priv->var_r) * priv->var_d);
        }
        else
        {
          tmp1=(int) ((priv->var_x+1.0)*dw2);
          tmp2=(int) ((priv->var_y+1.0)*dh2);
        }
        if (!priv->__wrap)
        {
          if (tmp1 < 0) tmp1=0;
          if (tmp1 > priv->w_adj) tmp1=priv->w_adj;
          if (tmp2 < 0) tmp2=0;
          if (tmp2 > priv->h_adj) tmp2=priv->h_adj;
        }
        *tabptr++ = tmp1;
        *tabptr++ = tmp2;
        double va=priv->var_alpha;
        if (va < 0.0) va=0.0;
        else if (va > 1.0) va=1.0;
        int a=(int)(va*255.0*65536.0);
        *tabptr++ = a;
      }
      yc_pos+=yc_dpos;
    }
  }

  return max_threads;
}


int trans_render(DMovementPrivate *priv, int this_thread, int max_threads, char visdata[2][2][576], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
  if (max_threads < 1) max_threads=1;

  int start_l = ( this_thread * h ) / max_threads;
  int end_l;
  int ypos=0;

  if (this_thread >= max_threads - 1) end_l = h;
  else end_l = ( (this_thread+1) * h ) / max_threads;  

  int outh=end_l-start_l;
  if (outh<1) return 0;

  int *fbin =  framebuffer;//!priv->buffern ? framebuffer : visual_video_get_pixels(priv->pipeline->buffers[priv->buffern-1]);
  if (!fbin) return 0;

  // yay, the table is generated. now we do a fixed point 
  // interpolation of the whole thing and pray.

  {
    LVAVSPipeline *pipeline = priv->pipeline;
    int *interptab=priv->m_tab+priv->xres*priv->yres*3 + this_thread * (priv->xres*6+6);
    int *rdtab=priv->m_tab;
    int *in=(int *)fbin;
    int *blendin=(int *)framebuffer;
    int *out=(int *)fbout;
    int yseek=1;
    int xc_dpos, yc_pos=0, yc_dpos;
    xc_dpos=(w<<16)/(priv->xres-1);
    yc_dpos=(h<<16)/(priv->yres-1);
    int lypos=0;
    int yl=end_l;
    while (yl>0)
    {
      yc_pos+=yc_dpos;   
      yseek=(yc_pos>>16)-lypos;
      if (!yseek) 
      {
        #ifndef NO_MMX
          //__asm emms;
        #endif
        return 0;
      }
      lypos=yc_pos>>16;
      int l=priv->xres;
      int *stab=interptab;
      int xr3=priv->xres*3;
      while (l--)
      {
        int tmp1, tmp2,tmp3;
        tmp1=rdtab[0];
        tmp2=rdtab[1];
        tmp3=rdtab[2];
        stab[0]=tmp1;
        stab[1]=tmp2;
        stab[2]=(rdtab[xr3]-tmp1)/yseek;
        stab[3]=(rdtab[xr3+1]-tmp2)/yseek;
        stab[4]=tmp3;
        stab[5]=(rdtab[xr3+2]-tmp3)/yseek;
        rdtab+=3;
        stab+=6;
      }

      if (yseek > yl) yseek=yl;
      yl-=yseek;

      if (yseek > 0) while (yseek--)
      {
        int d_x;
        int d_y;
        int d_a;
        int ap;
        int seek;
        int *seektab=interptab;
        int xp,yp;
        int l=w;
        int lpos=0;
        int xc_pos=0;
        ypos++;
        {
          while (l>0)
          {
            xc_pos+=xc_dpos;
            seek=(xc_pos>>16)-lpos;
            if (!seek) 
            {
              #ifndef NO_MMX
                //__asm emms;
              #endif
              return 0;
            }
            lpos=xc_pos>>16;
            xp=seektab[0];
            yp=seektab[1];
            ap=seektab[4];
            d_a=(seektab[10]-ap)/(seek);
            d_x=(seektab[6]-xp)/(seek);
            d_y=(seektab[7]-yp)/(seek);
            seektab[0] += seektab[2];
            seektab[1] += seektab[3];
            seektab[4] += seektab[5];
            seektab+=6;
        
            if (seek>l) seek=l;
            l-=seek;
            if (seek > 0 && ypos <= start_l)
            {
              blendin+=seek;
              if (priv->__nomove) in+=seek;
              else out+=seek;

              seek=0;
            }
            if (seek>0)
            {

  #define CHECK
              //__docheck(xp,yp,m_lastw,m_lasth,d_x,d_y);
  // normal loop
  #define NORMAL_LOOP(Z) while ((seek--)) { Z; xp+=d_x; yp+=d_y; }

  #if 0
              // this would be faster, but seems like it might be less reliable:
  #define WRAPPING_LOOPS(Z) \
    if (d_x <= 0 && d_y <= 0) NORMAL_LOOP(if (xp < 0) xp += priv->w_adj; if (yp < 0) yp += priv->h_adj; Z) \
    else if (d_x <= 0) NORMAL_LOOP(if (xp < 0) xp += priv->w_adj; if (yp >= priv->h_adj) yp-=priv->h_adj; Z) \
    else if (d_y <= 0) NORMAL_LOOP(if (xp >= priv->w_adj) xp-=priv->w_adj; if (yp < 0) yp += priv->h_adj; Z) \
    else NORMAL_LOOP(if (xp >= priv->w_adj) xp-=priv->w_adj; if (yp >= priv->h_adj) yp-=priv->h_adj; Z)

  #define CLAMPED_LOOPS(Z) \
    if (d_x <= 0 && d_y <= 0) NORMAL_LOOP(if (xp < 0) xp=0; if (yp < 0) yp=0; Z) \
    else if (d_x <= 0) NORMAL_LOOP(if (xp < 0) xp=0; if (yp >= priv->h_adj) yp=priv->h_adj-1; Z) \
    else if (d_y <= 0) NORMAL_LOOP(if (xp >= priv->w_adj) xp=priv->w_adj-1; if (yp < 0) yp=0; Z) \
    else NORMAL_LOOP(if (xp >= priv->w_adj) xp=priv->w_adj-1; if (yp >= priv->h_adj) yp=priv->h_adj-1; Z)

  #else     // slower, more reliable loops

  // wrapping loop
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

  #endif

  #define LOOPS(DO)  \
                if (priv->__blend && priv->__subpixel) DO(CHECK *out++=BLEND_ADJ(pipeline->blendtable, BLEND4_16(pipeline->blendtable, in+(xp>>16)+(priv->m_wmul[yp>>16]),w,xp,yp),*blendin++,ap>>16); ap+=d_a) \
                else if (priv->__blend) DO(CHECK *out++=BLEND_ADJ(pipeline->blendtable, in[(xp>>16)+(priv->m_wmul[yp>>16])],*blendin++,ap>>16); ap+=d_a) \
                else if (priv->__subpixel) DO(CHECK *out++=BLEND4_16(pipeline->blendtable, in+(xp>>16)+(priv->m_wmul[yp>>16]),w,xp,yp)) \
                else DO(CHECK *out++=in[(xp>>16)+(priv->m_wmul[yp>>16])])

              if (priv->__nomove)
              {
                if (fbin != framebuffer) while (seek--)
                {
                  *blendin=BLEND_ADJ(pipeline->blendtable, *in++,*blendin,ap>>16); ap+=d_a;
                  blendin++;
                }
                else while (seek--)
                {
                  *blendin=BLEND_ADJ(pipeline->blendtable, 0,*blendin,ap>>16); ap+=d_a;
                  blendin++;
                }
              }
              else if (!priv->__wrap)
              {
                // this might not really be necessary b/c of the clamping in the loop, but I'm sick of crashes
                if (xp < 0) xp=0;
                else if (xp >= priv->w_adj) xp=priv->w_adj-1;
                if (yp < 0) yp=0;
                else if (yp >= priv->h_adj) yp=priv->h_adj-1;

                LOOPS(CLAMPED_LOOPS)
              }
              else // __wrap
              {
                xp %= priv->w_adj; 
                yp %= priv->h_adj;

                if (xp < 0) xp+=priv->w_adj; 
                if (yp < 0) yp+=priv->h_adj;

                if (d_x <= -priv->w_adj) d_x=-priv->w_adj+1;
                else if (d_x >= priv->w_adj) d_x=priv->w_adj-1; 

                if (d_y <= -priv->h_adj) d_y=-priv->h_adj+1;
                else if (d_y >= priv->h_adj) d_y=priv->h_adj-1;

                LOOPS(WRAPPING_LOOPS)
              }
            } // if seek>0
          }
          // adjust final (rightmost elem) part of seektab
          seektab[0] += seektab[2];
          seektab[1] += seektab[3];
          seektab[4] += seektab[5];
        }
      }
    }
  }


#ifndef NO_MMX
  __asm emms;
#endif
    return 0;  
}


