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
#include "lvavs_pipeline.h"
#include "avs.h"

AvsNumber PI = M_PI;

typedef enum trans_runnable TransRunnable;

enum trans_runnable {
    RUNNABLE_INIT,
    RUNNABLE_BEAT,
    RUNNABLE_FRAME
};

typedef struct {
    LVAVSPipeline *pipeline;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsNumber var_x, var_y, var_isBeat, var_isLongBeat, var_bi;

    // params
    int enabled, depth, depth2, onbeat, durFrames, thisDepth, blend, blendavg, nF, showlight, initted, invert, oldstyle, buffern;

} BumpPrivate;

int lv_bump_init (VisPluginData *plugin);
int lv_bump_cleanup (VisPluginData *plugin);
int lv_bump_events (VisPluginData *plugin, VisEventQueue *events);
int lv_bump_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_bump_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

int bump_render(BumpPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_bump_palette,
        .video = lv_bump_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_bumpmap",
        .name = "Libvisual AVS Transform: bump element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: bump element",
        .help = "This is the bump element for the libvisual AVS system",

        .init = lv_bump_init,
        .cleanup = lv_bump_cleanup,
        .events = lv_bump_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_bump_init (VisPluginData *plugin)
{
printf("---------------------\n");
    BumpPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1), //bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeat", 0), //bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("durFrames", 15), //bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("depth", 30),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("depth2", 100),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("showlight", 0),//bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("invert", 0),//bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("oldstyle", 0),//bool
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("buffern", 0),// VISUAL_PARAM_LIMIT_INTEGER(0, 9), "Depth buffer"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("init", "t=0;"),//, "Init code will be executed each time the window size is changed or when the effect loads"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("frame", ""),//, "Frame code is executed before rendering a new frame"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("beat", "x=0.5+cos(t)*0.3;y=0.5+sin(t)*0.3;t=t+0.1;"),//, "Beat code is executed when a beat is detected"),

        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (BumpPrivate, 1);

    priv->pipeline = LVAVS_PIPELINE(visual_object_get_private(VISUAL_OBJECT(plugin)));
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many (paramcontainer, params);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();

    avs_runnable_variable_bind(priv->vm, "x", &priv->var_x);
    avs_runnable_variable_bind(priv->vm, "y", &priv->var_y);
    avs_runnable_variable_bind(priv->vm, "isbeat", &priv->var_isBeat);
    avs_runnable_variable_bind(priv->vm, "islbeat", &priv->var_isLongBeat);
    avs_runnable_variable_bind(priv->vm, "bi", &priv->var_bi);

    return 0;
}

int lv_bump_cleanup (VisPluginData *plugin)
{
    BumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->pipeline));

    visual_mem_free (priv);

    return 0;
}

static int trans_load_runnable(BumpPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

static int trans_run_runnable(BumpPrivate *priv, TransRunnable runnable)
{
    avs_runnable_execute(priv->runnable[runnable]);
    return 0;
}

int lv_bump_events (VisPluginData *plugin, VisEventQueue *events)
{
    BumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;
                if (visual_param_entry_is (param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "durFrames"))
                    priv->durFrames = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "depth"))
                    priv->depth = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "depth2"))
                    priv->depth2 = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "showlight"))
                    priv->showlight = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "invert"))
                    priv->invert = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "oldstyle"))
                    priv->oldstyle = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "buffern"))
                    priv->buffern = visual_param_entry_get_integer(param);
                if (visual_param_entry_is (param, "init"))
                    trans_load_runnable(priv, RUNNABLE_INIT, visual_param_entry_get_string(param));
                if (visual_param_entry_is (param, "frame"))
                    trans_load_runnable(priv, RUNNABLE_FRAME, visual_param_entry_get_string(param));
                if (visual_param_entry_is (param, "beat"))
                    trans_load_runnable(priv, RUNNABLE_BEAT, visual_param_entry_get_string(param));
                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_bump_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

#define min(a, b) ( a < b )? a : b
#define max(a, b) ( a > b )? a : b
#define abs(x) (( x ) >= 0 ? ( x ) : - ( x ))

static int __inline depthof(int c, int i)
{
    int r= max(max((c & 0xFF), ((c & 0xFF00)>>8)), (c & 0xFF0000)>>16);
    return i ? 255 - r : r;
}

static int __inline setdepth(int l, int c)
{
    int r;
    r=min((c&0xFF)+l, 254);
    r|=min(((c&0xFF00))+(l<<8),254<<8);
    r|=min(((c&0xFF0000))+(l<<16),254<<16);
    return r;
}

static int __inline setdepth0(int c)
{
    int r;
    r=min((c&0xFF), 254);
    r|=min(((c&0xFF00)),254<<8);
    r|=min(((c&0xFF0000)),254<<16);
    return r;
}

int lv_bump_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    BumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    int *framebuffer = priv->pipeline->framebuffer;
    int *fbout = priv->pipeline->fbout;
    uint8_t isBeat = priv->pipeline->isBeat;
    void *visdata = priv->pipeline->audiodata;
    int w = video->width, h = video->height;

    bump_render(priv, visdata, isBeat, framebuffer, fbout, w, h);

    priv->pipeline->swap = 1;
    return 0;
}

int bump_render(BumpPrivate *priv, float visdata[2][2][1024], int isBeat, int *framebuffer, int *fbout, int w, int h)
{
    int cx,cy;
    int curbuf;

    if (!priv->enabled) return 0;

    if (isBeat&0x80000000) return 0;

    int *depthbuffer = !priv->buffern ? framebuffer : visual_video_get_pixels(priv->pipeline->buffers[priv->buffern - 1]);
    if (!depthbuffer) return 0;

    curbuf = (depthbuffer==framebuffer);

    if (!priv->initted)
    {
        trans_run_runnable(priv, RUNNABLE_INIT);
        priv->initted=1;
    }

    trans_run_runnable(priv, RUNNABLE_FRAME);
    if (isBeat) trans_run_runnable(priv, RUNNABLE_BEAT);

    if (isBeat)
        priv->var_isBeat=-1;
    else
        priv->var_isBeat=1;
    if (priv->nF)
        priv->var_isLongBeat=-1;
    else
        priv->var_isLongBeat=1;

    if (priv->onbeat && isBeat)
    {
        priv->thisDepth=priv->depth2;
        priv->nF = priv->durFrames;
    }
    else if (!priv->nF) priv->thisDepth = priv->depth;

    memset(fbout, 0, w*h*sizeof(int)); // previous effects may have left fbout in a mess

    if (priv->oldstyle)
    {
        cx = (int)(priv->var_x/100.0*w);
        cy = (int)(priv->var_y/100.0*h);
    }
    else
    {
        cx = (int)(priv->var_x*w);
        cy = (int)(priv->var_y*h);
    }
    cx = max(0, min(w, cx));
    cy = max(0, min(h, cy));
    if (priv->showlight) fbout[cx+cy*w]=0xFFFFFF;

    if (priv->var_bi) 
    {
        priv->var_bi = min(max(priv->var_bi, 0), 1);
        priv->thisDepth = (int)(priv->thisDepth * priv->var_bi);
    }

  int thisDepth_scaled=(priv->thisDepth<<8)/100;
    depthbuffer += w+1;
    framebuffer += w+1;
    fbout += w+1;

    int ly=1-cy;
  int i=h-2;
  while (i--)
    {
    int j=w-2;
        int lx=1-cx;
    if (priv->blend)
    {
      while (j--)
          {
        int m1,p1,mw,pw;
        m1=depthbuffer[-1];
        p1=depthbuffer[1];
        mw=depthbuffer[-w];
        pw=depthbuffer[w];
              if (!curbuf || (curbuf && (m1||p1||mw||pw)))
              {
          int coul1,coul2;
                  coul1=depthof(p1, priv->invert)-depthof(m1, priv->invert)-lx;
                  coul2=depthof(pw, priv->invert)-depthof(mw, priv->invert)-ly;
                  coul1=127-abs(coul1);
                  coul2=127-abs(coul2);
                  if (coul1<=0||coul2<=0)
            coul1=setdepth0(framebuffer[0]);
          else 
            coul1=setdepth((coul1*coul2*thisDepth_scaled)>>(8+6), framebuffer[0]);
                  fbout[0]=BLEND(framebuffer[0], coul1);
              }
              depthbuffer++;
              framebuffer++;
        fbout++;
              lx++;
          }
    }
    else if (priv->blendavg)
    {
      while (j--)
          {
        int m1,p1,mw,pw;
        m1=depthbuffer[-1];
        p1=depthbuffer[1];
        mw=depthbuffer[-w];
        pw=depthbuffer[w];
              if (!curbuf || (curbuf && (m1||p1||mw||pw)))
              {
          int coul1,coul2;
                  coul1=depthof(p1, priv->invert)-depthof(m1, priv->invert)-lx;
                  coul2=depthof(pw, priv->invert)-depthof(mw, priv->invert)-ly;
                  coul1=127-abs(coul1);
                  coul2=127-abs(coul2);
                  if (coul1<=0||coul2<=0)
            coul1=setdepth0(framebuffer[0]);
          else 
            coul1=setdepth((coul1*coul2*thisDepth_scaled)>>(8+6), framebuffer[0]);
                  fbout[0]=BLEND_AVG(framebuffer[0], coul1);
              }
              depthbuffer++;
              framebuffer++;
        fbout++;
              lx++;
          }
    }
    else
    {
      while (j--)
          {
        int m1,p1,mw,pw;
        m1=depthbuffer[-1];
        p1=depthbuffer[1];
        mw=depthbuffer[-w];
        pw=depthbuffer[w];
              if (!curbuf || (curbuf && (m1||p1||mw||pw)))
              {
          int coul1,coul2;
                  coul1=depthof(p1, priv->invert)-depthof(m1, priv->invert)-lx;
                  coul2=depthof(pw, priv->invert)-depthof(mw, priv->invert)-ly;
                  coul1=127-abs(coul1);
                  coul2=127-abs(coul2);
                  if (coul1<=0||coul2<=0)
            coul1=setdepth0(framebuffer[0]);
          else 
            coul1=setdepth((coul1*coul2*thisDepth_scaled)>>(8+6), framebuffer[0]);
                  fbout[0]=coul1;
              }
              depthbuffer++;
              framebuffer++;
        fbout++;
              lx++;
          }
    }
        depthbuffer+=2;
        framebuffer+=2;
    fbout+=2;
        ly++;
    }

  if (priv->nF)
  {
      priv->nF--;
      if (priv->nF)
      {
          int a = abs(priv->depth - priv->depth2) / priv->durFrames;
          priv->thisDepth += a * (priv->depth2 > priv->depth ? -1 : 1);
      }
  }

  return 0;
}

