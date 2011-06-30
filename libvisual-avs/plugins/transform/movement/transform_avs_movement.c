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
#include "lvavs_pipeline.h"
#include "avs.h"

#define REFFECT_MIN 3
#define REFFECT_MAX 23

#define PI 3.14

#define MAKE_REFFECT(n,x) static void _ref##n(double *r, double *d, double max_d, int *xo, int *yo) { x }

void test(double *r);

typedef void t_reffect(double *r, double *d, double max_d, int *xo, int *yo);
         
MAKE_REFFECT(3, *r+=0.1-0.2*(*d/max_d); *d*=0.96;)
MAKE_REFFECT(4, *d*=0.99*(1.0-sin(*r)/32.0); *r+=0.03*sin(*d/max_d * M_PI * 4);)
MAKE_REFFECT(5, *d*=0.94+(cos(*r*32.0)*0.06);)
MAKE_REFFECT(6, *d*=1.01+(cos(*r*4.0)*0.04); *r+=0.03*sin(*d/max_d * M_PI * 4);)
MAKE_REFFECT(8, *r+=0.1*sin(*d/max_d*M_PI*5);)
MAKE_REFFECT(9, double t; t=sin(*d/max_d*M_PI); *d-=8*t*t*t*t*t; )
MAKE_REFFECT(10,double t; t=sin(*d/max_d*M_PI); *d-=8*t*t*t*t*t; t=cos((*d/max_d)*M_PI/2.0); *r+=0.1*t*t*t; )
MAKE_REFFECT(11, *d*=0.95+(cos(*r*5.0 - M_PI/2.50)*0.03); )
MAKE_REFFECT(12, *r+=0.04; *d*=0.96+cos(*d/max_d*M_PI)*0.05; )
MAKE_REFFECT(13, double t; t=cos(*d/max_d*M_PI); *r+=0.07*t; *d*=0.98+t*0.10; )
MAKE_REFFECT(14, *r+=0.1-0.2*(*d/max_d); *d*=0.96; *xo=8; )
MAKE_REFFECT(15, *d=max_d*0.15;)
MAKE_REFFECT(16, *r=cos(*r*3);)
MAKE_REFFECT(17, *d*=(1-(((*d/max_d)-.35)*.5)); *r+=.1;)

static t_reffect *radial_effects[REFFECT_MAX-REFFECT_MIN+1]=
{
  _ref3, _ref4, _ref5, _ref6, NULL, _ref8, _ref9, _ref10, _ref11, _ref12, _ref13,
  _ref14, _ref15, _ref16, _ref17, NULL/*18*/, NULL/*19*/, NULL/*20*/, NULL/*21*/,
  NULL/*22*/, NULL/*23*/,
};

typedef struct {
    LVAVSPipeline *pipeline;
    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable;
    AvsNumber d, r, x, y, sw, sh, ph, pw, px, py;

    uint8_t *swapbuf, *renderbuf;

    int *tab;
    int width, height;

    int *trans_tab, trans_tab_w, trans_tab_h, trans_tab_subpixel;
    int tab_w, tab_h, tab_subpixel;
    int trans_effect;
    char *effect_exp;
    int effect_exp_ch;
    int effect, blend;
    int sourcemapped;
    int rectangular;
    int subpixel;
    int wrap;

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

static inline int effect_uses_eval(int t)
{
  int retval = FALSE;
  if ((t >= REFFECT_MIN) && (t <= REFFECT_MAX))
  {
    if (__movement_descriptions[t].uses_eval)
    {
      retval = TRUE;
    }
  }
  return retval;
}

static int max(int x, int y)
{
    return x > y ? x : y;
}

static int  min(int x, int y)
{
    return x < y ? x : y;
}

int lv_movement_init (VisPluginData *plugin);
int lv_movement_cleanup (VisPluginData *plugin);
int lv_movement_events (VisPluginData *plugin, VisEventQueue *events);
int lv_movement_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_movement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static void trans_generate_blend_table(MovementPrivate *priv);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int load_runnable(MovementPrivate *priv, char *buf) {
        AvsRunnable *obj = avs_runnable_new(priv->ctx);
        avs_runnable_set_variable_manager(obj, priv->vm);
        priv->runnable = obj;
        avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
        return 0;
}

static int run_runnable(MovementPrivate *priv) {
        avs_runnable_execute(priv->runnable);
        return 0;
}

const VisPluginInfo *get_plugin_info (int *count);

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
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", 2),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("rectangular", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("sourcemapped", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("wrap", 0),
        VISUAL_PARAM_LIST_ENTRY_STRING ("code",
                "r = r + (0.1 * sin(d * PI * 5));"),
        VISUAL_PARAM_LIST_END
    };
	
    priv = visual_mem_new0 (MovementPrivate, 1);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();
    avs_runnable_variable_bind(priv->vm, "d", &priv->d);
    avs_runnable_variable_bind(priv->vm, "r", &priv->r);
    avs_runnable_variable_bind(priv->vm, "x", &priv->x);
    avs_runnable_variable_bind(priv->vm, "y", &priv->y);
    avs_runnable_variable_bind(priv->vm, "sw", &priv->sw);
    avs_runnable_variable_bind(priv->vm, "sh", &priv->sh);

    priv->pipeline = (LVAVSPipeline *)visual_object_get_private(VISUAL_OBJECT(plugin));
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many (paramcontainer, params);

    return 0;
}

int lv_movement_cleanup (VisPluginData *plugin)
{
    MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    if (priv->swapbuf != NULL)
        visual_mem_free (priv->swapbuf);

    visual_object_unref(VISUAL_OBJECT(priv->pipeline));

    if(priv->effect_exp)
        visual_mem_free(priv->effect_exp);

    visual_mem_free (priv);

    return 0;
}

int lv_movement_events (VisPluginData *plugin, VisEventQueue *events)
{
    MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param, *tmp;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "effect")) {
                    priv->effect = visual_param_entry_get_integer (param);

                    if(priv->effect == 0) priv->effect = 32767;

                    if (priv->effect != 32767) {
                        if (priv->effect >= 0 && priv->effect < 23) {
                            tmp = visual_param_container_get(param->parent, "code");
                            visual_param_entry_set_string (tmp, __movement_descriptions[priv->effect].eval_desc);
                            //char *wht = strdup(__movement_descriptions[priv->effect].eval_desc);
                        }
                    }

                } else if (visual_param_entry_is (param, "rectangular"))
                    priv->rectangular = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "sourcemapped"))
                    priv->sourcemapped = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "subpixel"))
                    priv->subpixel = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "wrap"))
                    priv->wrap = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "code"))  {
                    priv->effect_exp = strdup(visual_param_entry_get_string(param));
                    load_runnable(priv, priv->effect_exp);
		}
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


int smp_begin(MovementPrivate *priv, int max_threads, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h);
int smp_finish(MovementPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h);
int smp_render(MovementPrivate *priv, int this_thread, int max_threads, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h);

void trans_generate_blend_table(MovementPrivate *priv)
{
    int i,j;

    for (j=0; j < 256; j++)
        for (i=0; i < 256; i++)
            priv->pipeline->blendtable[i][j] = (unsigned char)((i / 255.0) * (float)j);

}

int lv_movement_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    MovementPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    int *framebuffer = priv->pipeline->framebuffer;// = visual_video_get_pixels (video);
    int *fbout = priv->pipeline->fbout;
    uint8_t *vidbuf, *vidoutbuf;
    int isBeat = priv->pipeline->isBeat;
    int i;
    int w = video->width, h = video->height;
    void *visdata = priv->pipeline->audiodata;
    trans_generate_blend_table(priv);
    int this_thread = 0, max_threads = 1;

    trans_generate_blend_table(priv);

#pragma omp parallel
{
    max_threads = omp_get_num_threads();
}
    smp_begin(priv, max_threads, visdata, isBeat, framebuffer, fbout, w, h);
    if(isBeat & 0x80000000) return 0;

#pragma omp parallel for
    for(this_thread = omp_get_num_threads() - 1; this_thread >= 0; this_thread--) {
        smp_render(priv, this_thread, max_threads, visdata, isBeat, framebuffer, fbout, w, h);
    }
    priv->pipeline->swap = smp_finish(priv, visdata,isBeat,framebuffer,fbout,w,h);
    return VISUAL_OK;
}

int smp_begin(MovementPrivate *priv, int max_threads, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
  if (!priv->effect) return 0;

  if (!priv->trans_tab || priv->trans_tab_w != w || priv->trans_tab_h != h || priv->effect != priv->trans_effect || 
       priv->effect_exp_ch)
  {
    int p;
    int *transp,x;
    //if (priv->trans_tab) visual_mem_free(priv->trans_tab); //GlobalFree(trans_tab);
    priv->trans_tab_w=w; 
    priv->trans_tab_h=h;
    priv->trans_tab=visual_mem_malloc0(priv->trans_tab_w*priv->trans_tab_h*sizeof(int));
    priv->trans_effect=priv->effect;
    priv->trans_tab_subpixel=(priv->subpixel && priv->trans_tab_w*priv->trans_tab_h < (1<<22) &&
                  ((priv->trans_effect >= REFFECT_MIN && priv->trans_effect <= REFFECT_MAX
                  && priv->trans_effect != 1 && priv->trans_effect != 2 && priv->trans_effect != 7
                  )||priv->trans_effect ==32767));

    /* generate trans_tab */
    transp=priv->trans_tab;
    x=w*h;
    p=0;

	  if (priv->trans_effect == 1)
	  {
		  while (x--)
		  {
		    int r=(p++)+(rand()%3)-1 + ((rand()%3)-1)*w;
		    *transp++ = min(w*h-1,max(r,0));
		  }
	  }
	  else if (priv->trans_effect == 2)
	  {
		  int y=h;
		  while (y--)
		  {
		    int x=w;
		    int lp=w/64;
		    while (x--)
		    {
			  *transp++ = p+lp++;
			  if (lp >= w) lp-=w;
		    }
		    p+=w;
		  }
    }
    else if (priv->trans_effect == 7)
    {
      int y;
      for (y = 0; y < h; y ++)
      {
        for (x = 0; x < w; x ++)
        {
          if (x&2 || y&2)
          {
            *transp++ = x+y*w;
          }
          else
          {
            int xp=w/2+(((x&~1)-w/2)*7)/8;
            int yp=h/2+(((y&~1)-h/2)*7)/8;
            *transp++=xp+yp*w;
          }
        }
      }
    }
    else if (priv->trans_effect >= REFFECT_MIN && priv->trans_effect <= REFFECT_MAX && !effect_uses_eval(priv->trans_effect))
    {
      double max_d=sqrt((w*w+h*h)/4.0);
      int y;
      t_reffect *ref=radial_effects[priv->trans_effect-REFFECT_MIN];
      if (ref) for (y = 0; y < h; y ++)
      {
        for (x = 0; x < w; x ++)
        {
          double r,d;
          double xd,yd;
          int ow,oh,xo=0,yo=0;
          xd=x-(w/2);
          yd=y-(h/2);
          d=sqrt(xd*xd+yd*yd);
          r=atan2(yd,xd);

          ref(&r,&d,max_d,&xo,&yo);

          double tmp1,tmp2;
          tmp1= ((h/2) + sin(r)*d + 0.5) + (yo*h)*(1.0/256.0);
          tmp2= ((w/2) + cos(r)*d + 0.5) + (xo*w)*(1.0/256.0);
          oh=(int)tmp1;
          ow=(int)tmp2;
          if (priv->trans_tab_subpixel)
          {
            int xpartial=(int)(32.0*(tmp2-ow));
            int ypartial=(int)(32.0*(tmp1-oh));
            if (priv->wrap)
            {
              ow%=(w-1);
              oh%=(h-1);
              if (ow<0)ow+=w-1;
              if (oh<0)oh+=h-1;
            }
            else
            {
              if (ow < 0) { xpartial=0; ow=0; }
              if (ow >= w-1) { xpartial=31; ow=w-2; }
              if (oh < 0) { ypartial=0; oh=0; }
              if (oh >= h-1) {ypartial=31; oh=h-2; }
            }
            *transp++ = (ow+oh*w) | (ypartial<<22) | (xpartial<<27);
          }
          else 
          {
            if (priv->wrap)
            {
              ow%=(w);
              oh%=(h);
              if (ow<0)ow+=w;
              if (oh<0)oh+=h;
            }
            else
            {
              if (ow < 0) ow=0;
              if (ow >= w) ow=w-1;
              if (oh < 0) oh=0;
              if (oh >= h) oh=h-1;
            }
            *transp++ = ow+oh*w;
          }
        }
      }
    }
    else if (priv->trans_effect == 32767 || effect_uses_eval(priv->trans_effect))
    {
      AvsNumber *var_d, *var_r, *var_px, *var_py, *var_pw, *var_ph;
      double max_d=sqrt((double)(w*w+h*h))/2.0;
      double divmax_d=1.0/max_d;
      int y;
      int offs=0;
      int is_rect = priv->trans_effect == 32767 ? priv->rectangular : __movement_descriptions[priv->trans_effect].uses_rect;
      priv->pw=w;
      priv->ph=h;

      var_d = &priv->d;
      var_r = &priv->r;
      var_px = &priv->px;
      var_py = &priv->py;
      var_ph = &priv->ph;
      var_pw = &priv->pw;

      priv->pw = priv->width;
      priv->ph = priv->height;

      char *t = priv->trans_effect == 32767 ? priv->effect_exp : __movement_descriptions[priv->trans_effect].eval_desc;
      load_runnable(priv, t);

      if (1)         
      {
        double w2=w/2;
        double h2=h/2;
        double xsc=1.0/w2,ysc=1.0/h2;

        for (y = 0; y < h; y ++)
        {
          for (x = 0; x < w; x ++)
          {
            double xd,yd;
            int ow,oh;
            xd=x-w2;
            yd=y-h2;
            *var_px=xd*xsc;
            *var_py=yd*ysc;
            *var_d=sqrt(xd*xd+yd*yd)*divmax_d;
            *var_r=atan2(yd,xd) + M_PI*0.5;

            run_runnable(priv);    
        
            double tmp1,tmp2;
            if (!is_rect)
            {
              priv->d *= max_d;
              priv->r -= M_PI/2.0;
              tmp1=((h/2) + sin(priv->r)* priv->d);
              tmp2=((w/2) + cos(priv->r)* priv->d);
            }
            else
            {
              tmp1=((*var_py+1.0)*h2);
              tmp2=((*var_px+1.0)*w2);
            }
            if (priv->trans_tab_subpixel)
            {
              oh=(int) tmp1;
              ow=(int) tmp2;
              int xpartial=(int)(32.0*(tmp2-ow));
              int ypartial=(int)(32.0*(tmp1-oh));
              if (priv->wrap)
              {
                ow%=(w-1);
                oh%=(h-1);
                if (ow<0)ow+=w-1;
                if (oh<0)oh+=h-1;
              }
              else
              {
                if (ow < 0) { xpartial=0; ow=0; }
                if (ow >= w-1) { xpartial=31; ow=w-2; }
                if (oh < 0) { ypartial=0; oh=0; }
                if (oh >= h-1) {ypartial=31; oh=h-2; }
              }
              *transp++ = (ow+oh*w) | (ypartial<<22) | (xpartial<<27);
            }
            else
            {
              tmp1+=0.5;
              tmp2+=0.5;
              oh=(int) tmp1;
              ow=(int) tmp2;
              if (priv->wrap)
              {
                ow%=(w);
                oh%=(h);
                if (ow<0)ow+=w;
                if (oh<0)oh+=h;
              }
              else
              {
                if (ow < 0) ow=0;
                if (ow >= w) ow=w-1;
                if (oh < 0) oh=0;
                if (oh >= h) oh=h-1;
              }
              *transp++ = ow+oh*w;
            }
          }
        }
      }
      else 
      {
        transp=priv->trans_tab;
        priv->trans_tab_subpixel=0;
        for (x = 0; x < w*h; x ++)
          *transp++=x;
      }
    }
    priv->effect_exp_ch=0;
  }

  if (!(isBeat & 0x80000000))
  {
    if ((priv->sourcemapped&2)&&isBeat) priv->sourcemapped^=1;
    if (priv->sourcemapped&1)
    {
      if (!priv->blend) visual_mem_set(fbout,0,w*h*sizeof(int));
      else visual_mem_copy(fbout,framebuffer,w*h*sizeof(int));
    }
  }
  return max_threads;
}

int smp_render(MovementPrivate *priv, int this_thread, int max_threads, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
  if (!priv->effect) return 0;
  
#define OFFSET_MASK ((1<<22)-1)

  int *inp = (int *) framebuffer;
  int *outp;
  int *transp, x;

  if (max_threads < 1) max_threads=1;

  int start_l = ( this_thread * h ) / max_threads;
  int end_l;

  if (this_thread >= max_threads - 1) end_l = h;
  else end_l = ( (this_thread+1) * h ) / max_threads;  

  int outh=end_l-start_l;
  if (outh<1) return 0;

  int skip_pix=start_l*w;
  transp=priv->trans_tab;

  outp = (int *) fbout;
  x=(w*outh)/4.0;

  if (priv->sourcemapped&1)
  {
    inp += skip_pix;
    transp += skip_pix;
    if (priv->trans_tab_subpixel)
    {
      //timingEnter(3);
      while (x--) 
      {
        fbout[transp[0]&OFFSET_MASK]=BLEND_MAX(inp[0],fbout[transp[0]&OFFSET_MASK]);
        fbout[transp[1]&OFFSET_MASK]=BLEND_MAX(inp[1],fbout[transp[1]&OFFSET_MASK]);
        fbout[transp[2]&OFFSET_MASK]=BLEND_MAX(inp[2],fbout[transp[2]&OFFSET_MASK]);
        fbout[transp[3]&OFFSET_MASK]=BLEND_MAX(inp[3],fbout[transp[3]&OFFSET_MASK]);
        inp+=4;
        transp+=4;
      }
      //timingLeave(3);
      x = (w*outh)&3;
      if (x>0) while (x--)
      {
        fbout[transp[0]&OFFSET_MASK]=BLEND_MAX(inp++[0],fbout[transp[0]&OFFSET_MASK]);
        transp++;
      }
    }
    else
    {
      {
        //timingEnter(3);
        while (x--) 
        {
          fbout[transp[0]]=BLEND_MAX(inp[0],fbout[transp[0]]);
          fbout[transp[1]]=BLEND_MAX(inp[1],fbout[transp[1]]);
          fbout[transp[2]]=BLEND_MAX(inp[2],fbout[transp[2]]);
          fbout[transp[3]]=BLEND_MAX(inp[3],fbout[transp[3]]);
          inp+=4;
          transp+=4;
        }
        //timingLeave(3);
        x = (w*outh)&3;
        if (x>0) while (x--)
        {
          fbout[transp[0]]=BLEND_MAX(inp++[0],fbout[transp[0]]);
          transp++;
        }
      }
    }
    if (priv->blend)
    {
      framebuffer += skip_pix;
      fbout += skip_pix;
      x=(w*outh)/4;
      while (x--)
      {
        fbout[0]=BLEND_AVG(fbout[0],framebuffer[0]);
        fbout[1]=BLEND_AVG(fbout[1],framebuffer[1]);
        fbout[2]=BLEND_AVG(fbout[2],framebuffer[2]);
        fbout[3]=BLEND_AVG(fbout[3],framebuffer[3]);
        fbout+=4;
        framebuffer+=4;
      }
      x=(w*outh)&3;
      while (x--)
      {
        fbout[0]=BLEND_AVG(fbout[0],framebuffer[0]);
        fbout++;
        framebuffer++;
      }
    }
  }
  else
  {
    inp += skip_pix;
    outp += skip_pix;
    transp += skip_pix;
    if (priv->trans_tab_subpixel&&priv->blend)
    {
      while (x--)
      {
        int offs=transp[0]&OFFSET_MASK;
        outp[0]=BLEND_AVG(inp[0],BLEND4(priv->pipeline->blendtable, framebuffer+offs,w,((transp[0]>>24)&(31<<3)),((transp[0]>>19)&(31<<3))));
        offs=transp[1]&OFFSET_MASK;
        outp[1]=BLEND_AVG(inp[1],BLEND4(priv->pipeline->blendtable, framebuffer+offs,w,((transp[1]>>24)&(31<<3)),((transp[1]>>19)&(31<<3))));
        offs=transp[2]&OFFSET_MASK;
        outp[2]=BLEND_AVG(inp[2],BLEND4(priv->pipeline->blendtable, framebuffer+offs,w,((transp[2]>>24)&(31<<3)),((transp[2]>>19)&(31<<3))));
        offs=transp[3]&OFFSET_MASK;
        outp[3]=BLEND_AVG(inp[3],BLEND4(priv->pipeline->blendtable, (int *)framebuffer+offs,w,((transp[3]>>24)&(31<<3)),((transp[3]>>19)&(31<<3))));
        transp+=4;
        outp+=4;
        inp+=4;
      }    
      x=(w*outh)&3;
      while (x--)
      {
        int offs=transp[0]&OFFSET_MASK;
        outp++[0]=BLEND_AVG(inp[0],BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[0]>>24)&(31<<3)),((transp[0]>>19)&(31<<3))));
        transp++;
        inp++;
      }    
    }
    else if (priv->trans_tab_subpixel)
    {
      while (x--)
      {
        int offs=transp[0]&OFFSET_MASK;
        outp[0]=BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[0]>>24)&(31<<3)),((transp[0]>>19)&(31<<3)));
        offs=transp[1]&OFFSET_MASK;
        outp[1]=BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[1]>>24)&(31<<3)),((transp[1]>>19)&(31<<3)));
        offs=transp[2]&OFFSET_MASK;
        outp[2]=BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[2]>>24)&(31<<3)),((transp[2]>>19)&(31<<3)));
        offs=transp[3]&OFFSET_MASK;
        outp[3]=BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[3]>>24)&(31<<3)),((transp[3]>>19)&(31<<3)));
        transp+=4;
        outp+=4;
      }    
      x=(w*outh)&3;
      while (x--)
      {
        int offs=transp[0]&OFFSET_MASK;
        outp++[0]=BLEND4(priv->pipeline->blendtable,(int *)framebuffer+offs,w,((transp[0]>>24)&(31<<3)),((transp[0]>>19)&(31<<3)));
        transp++;
      }    
    #ifndef NO_MMX
      __asm emms;
    #endif
    }
    else if (priv->blend)
    {
      //timingEnter(3);
      while (x--) 
      {
	int diff = outp - fbout;

        outp[0]=BLEND_AVG(inp[0],framebuffer[transp[0]]);
        outp[1]=BLEND_AVG(inp[1],framebuffer[transp[1]]);
        outp[2]=BLEND_AVG(inp[2],framebuffer[transp[2]]);
        outp[3]=BLEND_AVG(inp[3],framebuffer[transp[3]]);
        outp+=4;
        inp+=4;
        transp+=4;
      }
      //timingLeave(3);
      x = (w*outh)&3;
      if (x>0) while (x--)
      {
        outp++[0]=BLEND_AVG(inp++[0],framebuffer[transp++[0]]);
      }
    }
    else
    {
      //timingEnter(4);
      while (x--) 
      {
        outp[0]=framebuffer[transp[0]];
        outp[1]=framebuffer[transp[1]];
        outp[2]=framebuffer[transp[2]];
        outp[3]=framebuffer[transp[3]];
        outp+=4;
        transp+=4;
      }
      //timingLeave(4);
      x = (w*outh)&3;
      if (x>0) while (x--)
      {
        outp++[0]=framebuffer[transp++[0]];
      }
    }
  }
  return 0;
}

int smp_finish(MovementPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h) // return value is that of render() for fbstuff etc
{
  return !!priv->effect;
}



