/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_stars.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <omp.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "lvavs_pipeline.h"

typedef struct {
    int X, Y;
    float Z;
    float Speed;
    int OX, OY;
} StarFormat;

typedef struct {
    LVAVSPipeline *pipeline;
    VisPalette pal;

    int enabled;
    int MaxStars, MaxStars_set;
    int Xoff;
    int Yoff;
    int Zoff;
    float warpSpeed;
    int blend;
    int blendavg;
    StarFormat Stars[4096];
    int Width, Height;
    int onbeat;
    float spdBeat;
    float incBeat;
    int durFrames;
    float CurrentSpeed;
    int nc;

    AVSGfxColorCycler *cycler;

} StarsPrivate;

void initialize_stars(StarsPrivate *priv);
void create_star(StarsPrivate *priv, int A);
static unsigned int __inline BLEND_ADAPT(unsigned int a, unsigned int b, /*float*/int divisor);

int lv_stars_init (VisPluginData *plugin);
int lv_stars_cleanup (VisPluginData *plugin);
int lv_stars_requisition (VisPluginData *plugin, int *width, int *height);
int lv_stars_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_stars_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_stars_palette (VisPluginData *plugin);
int lv_stars_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisActorPlugin actor[] = {{
        .requisition = lv_stars_requisition,
        .palette = lv_stars_palette,
        .render = lv_stars_render,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_8BIT |
            VISUAL_VIDEO_DEPTH_32BIT

    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "avs_stars",
        .name = "Libvisual AVS Render: Starfield element",
        .author = "Dennis Smit <ds@nerds-incorporated.org>",
        .version = "0.1",
        .about = "The Libvisual AVS Render: Starfield element",
        .help = "This is the Starfield scope element for the libvisual AVS system",

        .init = lv_stars_init,
        .cleanup = lv_stars_cleanup,
        .events = lv_stars_events,

        .plugin = VISUAL_OBJECT (&actor[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_stars_init (VisPluginData *plugin)
{
    StarsPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0),
        VISUAL_PARAM_LIST_ENTRY ("palette"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("warpSpeed", 6),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("MaxStars_set", 350),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeat", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("spdBeat", 4),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("durFrames", 15),
        VISUAL_PARAM_LIST_END
    };

    
    priv = visual_mem_new0 (StarsPrivate, 1);
    priv->pipeline = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->pipeline == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return VISUAL_ERROR_GENERAL;
    }
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_palette_allocate_colors (&priv->pal, 1);

    for (i = 0; i < priv->pal.ncolors; i++) {
        priv->pal.colors[i].r = 0xff;
        priv->pal.colors[i].g = 0xff;
        priv->pal.colors[i].b = 0xff;
    }

    priv->MaxStars = 350;
    priv->Zoff = 255;
    priv->CurrentSpeed = 6;

    visual_param_container_add_many (paramcontainer, params);

    visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

    visual_palette_free_colors (&priv->pal);

    return 0;
}

int lv_stars_cleanup (VisPluginData *plugin)
{
    StarsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_mem_free (priv);

    return 0;
}

int lv_stars_requisition (VisPluginData *plugin, int *width, int *height)
{
    return 0;
}

int lv_stars_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
    visual_video_set_dimension (video, width, height);

    return 0;
}

int lv_stars_events (VisPluginData *plugin, VisEventQueue *events)
{
    StarsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                lv_stars_dimension (plugin, ev.event.resize.video,
                        ev.event.resize.width, ev.event.resize.height);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "warpSpeed"))
                    priv->warpSpeed = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "MaxStars_set"))
                    priv->MaxStars_set = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "spdBeat"))
                    priv->spdBeat = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "durFrames"))
                    priv->durFrames = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "palette")) {
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

VisPalette *lv_stars_palette (VisPluginData *plugin)
{
    StarsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    return NULL;
}

#define max(a, b) (a>b?a:b)
int lv_stars_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    StarsPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    LVAVSPipeline *pipeline = priv->pipeline;
    uint32_t *framebuffer = visual_video_get_pixels(video);
    int w = video->width;
    int h = video->height;
    uint32_t color = 0xffffffff;//visual_color_to_uint32(&priv->pal.colors[0]);
    int isBeat = pipeline->isBeat;

    int i=w*h;
    int NX,NY;
    int c;

    if (!priv->enabled) return 0;

    if (priv->onbeat && isBeat)
    {
        priv->CurrentSpeed = priv->spdBeat;
        priv->incBeat = (priv->warpSpeed - priv->CurrentSpeed) / (float)priv->durFrames;
        priv->nc = priv->durFrames;
    }

    if (priv->Width != w || priv->Height != h)
    {
        priv->Width = w;
        priv->Height = h;
        priv->Xoff = priv->Width/2;
        priv->Yoff = priv->Height/2;
        initialize_stars(priv);
    }
    if (isBeat&0x80000000) return 0;

#pragma omp parallel for private(i)
    for (i=0;i<priv->MaxStars;i++)
    {
        #pragma omp critical(nextstar)
        if ((int)priv->Stars[i].Z > 0)
        {
            NX = ((priv->Stars[i].X << 7) / (int)priv->Stars[i].Z) + priv->Xoff;
            NY = ((priv->Stars[i].Y << 7) / (int)priv->Stars[i].Z) + priv->Yoff;
            if ((NX > 0) && (NX < w) && (NY > 0) && (NY < h))
            {
                c = (int)((255-(int)priv->Stars[i].Z)*priv->Stars[i].Speed);
                if (color != 0xFFFFFF) c = BLEND_ADAPT((c|(c<<8)|(c<<16)), color, c>>4); else c = (c|(c<<8)|(c<<16));
		{
                framebuffer[NY*w+NX] = priv->blend ? BLEND(framebuffer[NY*w+NX], c) : priv->blendavg ? BLEND_AVG(framebuffer[NY*w+NX], c) : c;
                priv->Stars[i].OX = NX;
                priv->Stars[i].OY = NY;
                priv->Stars[i].Z-=priv->Stars[i].Speed*priv->CurrentSpeed;
		}
            }
            else
                create_star(priv, i);
        }
        else
            create_star(priv, i);
    }

    if (!priv->nc)
        priv->CurrentSpeed = priv->warpSpeed;
    else
    {
        priv->CurrentSpeed = max(0, priv->CurrentSpeed + priv->incBeat);
        priv->nc--;
    }


    return 0;
}

static int MulDiv(int nMultiplicand, int nMultiplier, int nDivisor) {
    int ret;
    if(nDivisor == 0) return -1;
    if ( ( (nMultiplicand <  0) && (nMultiplier <  0) ) ||
         ( (nMultiplicand >= 0) && (nMultiplier >= 0) ) )
      ret = (((long long)nMultiplicand * nMultiplier) + (nDivisor/2)) / nDivisor;
    else
      ret = (((long long)nMultiplicand * nMultiplier) - (nDivisor/2)) / nDivisor;
    return ret;
}

void initialize_stars(StarsPrivate *priv)
{
  int i;
  srand(time(NULL));
  priv->MaxStars = MulDiv(priv->MaxStars_set,priv->Width*priv->Height,512*384);
  if (priv->MaxStars > 4095) priv->MaxStars=4095;
  for (i=0;i<priv->MaxStars;i++)
    {
    priv->Stars[i].X=(rand()%priv->Width)-priv->Xoff;
    priv->Stars[i].Y=(rand()%priv->Height)-priv->Yoff;
    priv->Stars[i].Z=(float)(rand()%255);
    priv->Stars[i].Speed = (float)(rand()%9+1)/10;
    }
}

void create_star(StarsPrivate *priv, int A)
{
  priv->Stars[A].X = (rand()%priv->Width)-priv->Xoff;
  priv->Stars[A].Y = (rand()%priv->Height)-priv->Yoff;
  priv->Stars[A].Z = (float)priv->Zoff;
}

static unsigned int __inline BLEND_ADAPT(unsigned int a, unsigned int b, /*float*/int divisor)
{
return ((((a >> 4) & 0x0F0F0F) * (16-divisor) + (((b >> 4) & 0x0F0F0F) * divisor)));
}

