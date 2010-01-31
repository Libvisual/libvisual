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
    AVSGlobalProxy *proxy;

    AvsRunnableContext *ctx;
    AvsRunnableVariableManager *vm;
    AvsRunnable *runnable[4];
    AvsVariable var_x, int var_y, int var_isBeat, int var_isLongBeat, int var_bi;

    // params
    int enabled, onbeat, durFrames, depth, depth2, blend, blendavg, showlight, invert, oldstyle, buffern;
    char *code1, code2, code3;

} BumpPrivate;

int lv_bump_init (VisPluginData *plugin);
int lv_bump_cleanup (VisPluginData *plugin);
int lv_bump_events (VisPluginData *plugin, VisEventQueue *events);
int lv_bump_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_bump_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

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

		.plugname = "avs_bump",
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
	BumpPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable Bump"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeat", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "OnBeat"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("durFrames", 15, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("depth", 30, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("depth2", 100, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 0, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("showlight", 0, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("invert", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Invert depth"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("oldstyle", 0, VISUAL_PARAM_LIMIT_BOOLEAN, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("buffern", 0, VISUAL_PARAM_LIMIT_INTEGER(0, 9), "Depth buffer"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("init", "t=0;", "Init code will be executed each time the window size is changed or when the effect loads"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("frame", "", "Frame code is executed before rendering a new frame"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("beat", "x=0.5+cos(t)*0.3;y=0.5+sin(t)*0.3;t=t+0.1;", "Beat code is executed when a beat is detected"),

		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (BumpPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

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

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int trans_load_runnable(BlitPrivate *priv, TransRunnable runnable, char *buf)
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable[runnable] = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int trans_run_runnable(BlitPrivate *priv, TransRunnable runnable)
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
                else if (visual_param_entry_is (param, "roundmode"))
                    priv->roundmode = visual_param_entry_get_integer(param);

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

#define min(a, b) if( a < b )? a : b;
#define max(a, b) if( a > b )? a : b;
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
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

    return 0;
}

int bump_render(BumpPrivate *priv, float *data, int *framebuffer, int *fbout, int w, int h, int onBeat)
{
  int cx,cy;
    int curbuf;

    if (!enabled) return 0;

  if (need_recompile) 
  {
    EnterCriticalSection(&rcs);
    if (!var_bi || g_reset_vars_on_recompile)
    {
      clearVars();
      var_x = registerVar("x");
      var_y = registerVar("y");
      var_isBeat = registerVar("isbeat");
      var_isLongBeat = registerVar("islbeat");
      var_bi = registerVar("bi");
      *var_bi = 1.0;
      initted=0;
    }

    need_recompile=0;

    freeCode(codeHandle);
    freeCode(codeHandleBeat);
    freeCode(codeHandleInit);
    codeHandle = compileCode(code1.get());
    codeHandleBeat = compileCode(code2.get());
    codeHandleInit = compileCode(code3.get());

    LeaveCriticalSection(&rcs);
  }
  if (isBeat&0x80000000) return 0;

    int *depthbuffer = !buffern ? framebuffer : (int *)getGlobalBuffer(w,h,buffern-1,0);
    if (!depthbuffer) return 0;

    curbuf = (depthbuffer==framebuffer);

    if (!initted)
  {
        executeCode(codeHandleInit,visdata);
    initted=1;
    }

    executeCode(codeHandle,visdata);
    if (isBeat) executeCode(codeHandleBeat,visdata);

    if (isBeat)
        *var_isBeat=-1;
    else
        *var_isBeat=1;
    if (nF)
        *var_isLongBeat=-1;
    else
        *var_isLongBeat=1;

    if (onbeat && isBeat)
    {
    thisDepth=depth2;
    nF = durFrames;
    }
    else if (!nF) thisDepth = depth;

    memset(fbout, 0, w*h*4); // previous effects may have left fbout in a mess

    if (oldstyle)
        {
        cx = (int)(*var_x/100.0*w);
        cy = (int)(*var_y/100.0*h);
        }
    else
        {
        cx = (int)(*var_x*w);
        cy = (int)(*var_y*h);
        }
    cx = max(0, min(w, cx));
    cy = max(0, min(h, cy));
    if (showlight) fbout[cx+cy*w]=0xFFFFFF;

    if (var_bi) 
        {
        *var_bi = min(max(*var_bi, 0), 1);
        thisDepth = (int)(thisDepth * *var_bi);
        }

  int thisDepth_scaled=(thisDepth<<8)/100;
    depthbuffer += w+1;
    framebuffer += w+1;
    fbout += w+1;

    int ly=1-cy;
  int i=h-2;
  while (i--)
    {
    int j=w-2;
        int lx=1-cx;
    if (blend)
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
                  coul1=depthof(p1, invert)-depthof(m1, invert)-lx;
                  coul2=depthof(pw, invert)-depthof(mw, invert)-ly;
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
    else if (blendavg)
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
                  coul1=depthof(p1, invert)-depthof(m1, invert)-lx;
                  coul2=depthof(pw, invert)-depthof(mw, invert)-ly;
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
                  coul1=depthof(p1, invert)-depthof(m1, invert)-lx;
                  coul2=depthof(pw, invert)-depthof(mw, invert)-ly;
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

  if (nF)
    {
      nF--;
      if (nF)
        {
          int a = abs(depth - depth2) / durFrames;
          thisDepth += a * (depth2 > depth ? -1 : 1);
        }
    }

}

