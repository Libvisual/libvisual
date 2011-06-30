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

/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

    // params
    int enabled, density, depth, random_drop;
    int drop_position_x, drop_position_y, drop_radius, method;

    // Others
    int *buffers[2];
    int buffer_w, buffer_h;
    int page;


} WaterbumpPrivate;

int lv_waterbump_init (VisPluginData *plugin);
int lv_waterbump_cleanup (VisPluginData *plugin);
int lv_waterbump_events (VisPluginData *plugin, VisEventQueue *events);
int lv_waterbump_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_waterbump_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisTransformPlugin transform[] = {{
        .palette = lv_waterbump_palette,
        .video = lv_waterbump_video,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_32BIT,
        .requests_audio = TRUE
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_TRANSFORM,

        .plugname = "avs_waterbump",
        .name = "Libvisual AVS Transform: waterbump element",
        .author = "",
        .version = "0.1",
        .about = "The Libvisual AVS Transform: waterbump element",
        .help = "This is the waterbump element for the libvisual AVS system",

        .init = lv_waterbump_init,
        .cleanup = lv_waterbump_cleanup,
        .events = lv_waterbump_events,

        .plugin = VISUAL_OBJECT (&transform[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_waterbump_init (VisPluginData *plugin)
{
    WaterbumpPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_INTEGER("enabled", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Enable the waterbump effect"),
        VISUAL_PARAM_LIST_INTEGER("density", 0, VISUAL_PARAM_LIMIT_NONE, "Water density"),
        VISUAL_PARAM_LIST_INTEGER("depth", 0, VISUAL_PARAM_LIMIT_NONE, "Drop depth"),
        VISUAL_PARAM_LIST_INTEGER("random_drop", 0, VISUAL_PARAM_LIMIT_BOOLEAN, "Random drop position"),
        VISUAL_PARAM_LIST_INTEGER("drop_position_x", 0, VISUAL_PARAM_LIMIT_NONE, "Drop position x coordinate"),
        VISUAL_PARAM_LIST_INTEGER("drop_position_y", 0, VISUAL_PARAM_LIMIT_NONE, "Drop position y coordinate"),
        VISUAL_PARAM_LIST_INTEGER("drop_radius", 0, VISUAL_PARAM_LIMIT_NONE, "Drop radius"),
        VISUAL_PARAM_LIST_INTEGER("method", 0, VISUAL_PARAM_LIMIT_NONE, "Method"),
        VISUAL_PARAM_LIST_END
    };

    priv = visual_mem_new0 (WaterbumpPrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_param_container_add_many_proxy (paramcontainer, params);

    return 0;
}

int lv_waterbump_cleanup (VisPluginData *plugin)
{
    WaterbumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

    visual_mem_free (priv);

    return 0;
}

int lv_waterbump_events (VisPluginData *plugin, VisEventQueue *events)
{
    WaterbumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "density"))
                    priv->density = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "depth"))
                    priv->depth = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "random_drop"))
                    priv->random_drop = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "drop_position_x"))
                    priv->drop_position_x = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "drop_position_y"))
                    priv->drop_position_y = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "drop_radius"))
                    priv->drop_radius = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is(param, "method"))
                    priv->method = visual_param_entry_get_integer(param);

                break;

            default:
                break;
        }
    }

    return 0;
}

int lv_waterbump_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
    return 0;
}

void sine_blob(WaterbumpPrivate *priv, int x, int y, int radius, int height, int page)
{
  int cx, cy;
  int left,top,right,bottom;
  int square;
  double dist;
  int radsquare = priv->radius * priv->radius;
  double length = (1024.0/(float)priv->radius)*(1024.0/(float)priv->radius);

  if(x<0) x = 1+priv->radius+ rand()%(priv->buffer_w-2*priv->radius-1);
  if(y<0) y = 1+priv->radius+ rand()%(priv->buffer_h-2*priv->radius-1);

  left=-priv->radius; right = priv->radius;
  top=-priv->radius; bottom = radius;

  // Perform edge clipping...
  if(x - priv->radius < 1) left -= (x-priv->radius-1);
  if(y - priv->radius < 1) top  -= (y-priv->radius-1);
  if(x + priv->radius > priv->buffer_w-1) right -= (x+priv->radius-priv->buffer_w+1);
  if(y + priv->radius > priv->buffer_h-1) bottom-= (y+priv->radius-priv->buffer_h+1);

  for(cy = top; cy < bottom; cy++)
  {
    for(cx = left; cx < right; cx++)
    {
      square = cy*cy + cx*cx;
      if(square < radsquare)
      {
        dist = sqrt(square*length);
        priv->buffers[page][buffer_w*(cy+y) + cx+x]
          += (int)((cos(dist)+0xffff)*(height)) >> 19;
      }
    }
  }
}

void height_blob(WaterbumpPrivate *priv, int x, int y, int radius, int height, int page)
{
  int rquad;
  int cx, cy, cyq;
  int left, top, right, bottom;

  rquad = priv->radius * priv->radius;

  // Make a randomly-placed blob...
  if(x<0) x = 1+priv->radius+ rand()%(priv->buffer_w-2*priv->radius-1);
  if(y<0) y = 1+priv->radius+ rand()%(priv->buffer_h-2*priv->radius-1);

  left=-priv->radius; right = priv->radius;
  top=-priv->radius; bottom = priv->radius;

  // Perform edge clipping...
  if(x - priv->radius < 1) left -= (x-priv->radius-1);
  if(y - priv->radius < 1) top  -= (y-radius-1);
  if(x + priv->radius > priv->buffer_w-1) right -= (x+priv->radius-priv->buffer_w+1);
  if(y + priv->radius > priv->buffer_h-1) bottom-= (y+priv->radius-priv->buffer_h+1);


  for(cy = top; cy < bottom; cy++)
  {
    cyq = cy*cy;
    for(cx = left; cx < right; cx++)
    {
      if(cx*cx + cyq < rquad)
        priv->buffers[page][buffer_w*(cy+y) + (cx+x)] += height;
    }
  }

}


void calc_water(WaterbumpPrivate *priv, int npage, int density)
{
  int newh;
  int count = priv->buffer_w + 1;

  int *newptr = priv->buffers[npage];
  int *oldptr = priv->buffers[!npage];

  int x, y;

  for (y = (priv->buffer_h-1)*priv->buffer_w; count < y; count += 2)
  {
    for (x = count+priv->buffer_w-2; count < x; count++)
    {
// This does the eight-pixel method.  It looks much better.

      newh          = ((oldptr[count + priv->buffer_w]
                      + oldptr[count - priv->buffer_w]
                      + oldptr[count + 1]
                      + oldptr[count - 1]
                      + oldptr[count - priv->buffer_w - 1]
                      + oldptr[count - priv->buffer_w + 1]
                      + oldptr[count + priv->buffer_w - 1]
                      + oldptr[count + priv->buffer_w + 1]
                       ) >> 2 )
                      - newptr[count];


      newptr[count] =  newh - (newh >> priv->density);
    }
  }
}

/*
void C_THISCLASS::CalcWaterSludge(int npage, int density)
{
  int newh;
  int count = buffer_w + 1;

  int *newptr = buffers[npage];
  int *oldptr = buffers[!npage];

  int x, y;

  for (y = (buffer_h-1)*buffer_w; count < y; count += 2)
  {
    for (x = count+buffer_w-2; count < x; count++)
    {
// This is the "sludge" method...
      newh = (oldptr[count]<<2)
           +  oldptr[count-1-buffer_w]
           +  oldptr[count+1-buffer_w]
           +  oldptr[count-1+buffer_w]
           +  oldptr[count+1+buffer_w]
           + ((oldptr[count-1]
           +   oldptr[count+1]
           +   oldptr[count-buffer_w]
           +   oldptr[count+buffer_w])<<1);

      newptr[count] = (newh-(newh>>6)) >> density;
    }
  }
}
*/

int lv_waterbump_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    WaterbumpPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    uint8_t isBeat = priv->proxy->isBeat;
    int w = video->width;
    int h = video->height;
    uint8_t *framebuffer = visual_mem_malloc0(w * h * video->pitch);
    uint8_t *fbout = visual_video_get_pixels (video);

    if (!priv->enabled) return 0;
    int l,i;
    l=w*h;

    if(priv->buffer_w!=w||priv->buffer_h!=h) {
        for(i=0;i<2;i++) {
            if(priv->buffers[i]) visual_mem_free(buffers[i]);
            priv->buffers[i]=NULL;
        }
    }
    if(priv->buffers[0]==NULL) {
        for(i=0;i<2;i++) {
            priv->buffers[i]=visual_mem_malloc0(w*h*sizeof(int));
        }
        priv->buffer_w=w;
        priv->buffer_h=h;
    }
    if (isBeat&0x80000000) return 0;

    if(isBeat) {
        if(priv->random_drop) {
            int max=w;
            if(h>w) max=h;
            sine_blob(priv, -1,-1,priv->drop_radius*max/100,-depth,priv->page);
        } else {
            int x,y;
            switch(priv->drop_position_x) {
                case 0: x=w/4; break;
                case 1: x=w/2; break;
                case 2: x=w*3/4; break;
            }
            switch(priv->drop_position_y) {
                case 0: y=h/4; break;
                case 1: y=h/2; break;
                case 2: y=h*3/4; break;
            }
            sine_blob(priv, x,y,priv->drop_radius,-depth,priv->page);
        }
//  HeightBlob(-1,-1,80/2,1400,page);
    }

    {
        int dx, dy;
        int x, y;
        int ofs,len=priv->buffer_h*priv->buffer_w;

        int offset=priv->buffer_w + 1;

        int *ptr = priv->buffers[priv->page];

        for (y = (priv->buffer_h-1)*priv->buffer_w; offset < y; offset += 2)
        {
            for (x = offset+priv->buffer_w-2; offset < x; offset++)
            {
                dx = ptr[offset] - ptr[offset+1];
                dy = ptr[offset] - ptr[offset+priv->buffer_w];
                ofs=offset + priv->buffer_w*(dy>>3) + (dx>>3);
                if((ofs<len)&&(ofs>-1))
                    fbout[offset] = framebuffer[ofs];
                else
                    fbout[offset] = framebuffer[offset];

                offset++;
                dx = ptr[offset] - ptr[offset+1];
                dy = ptr[offset] - ptr[offset+priv->buffer_w];
                ofs=offset + priv->buffer_w*(dy>>3) + (dx>>3);
                if((ofs<len)&&(ofs>-1))
                    fbout[offset] = framebuffer[ofs];
                else
                    fbout[offset] = framebuffer[offset];
 
            }
        }
    }

    calc_water(priv, !priv->page,priv->density);

    priv->page=!priv->page;

    return 0;
}

