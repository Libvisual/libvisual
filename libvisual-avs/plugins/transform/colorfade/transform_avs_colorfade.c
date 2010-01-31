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

    // params
    int enabled;
    int faders[3];
    int beatfaders[3];
    int faderpos[3];

    // Other elements
    uint8_t c_tab[512][512];
    uint8_t clip[256+40+40];
    static int ft[4][3];

} ColorfadePrivate;

int lv_colorfade_init (VisPluginData *plugin);
int lv_colorfade_cleanup (VisPluginData *plugin);
int lv_colorfade_events (VisPluginData *plugin, VisEventQueue *events);
int lv_colorfade_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio);
int lv_colorfade_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisTransformPlugin transform[] = {{
		.palette = lv_colorfade_palette,
		.video = lv_colorfade_video,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_32BIT,
		.requests_audio = TRUE
	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_TRANSFORM,

		.plugname = "avs_colorfade",
		.name = "Libvisual AVS Transform: colorfade element",
		.author = "",
		.version = "0.1",
		.about = "The Libvisual AVS Transform: colorfade element",
		.help = "This is the colorfade element for the libvisual AVS system",

		.init = lv_colorfade_init,
		.cleanup = lv_colorfade_cleanup,
		.events = lv_colorfade_events,

		.plugin = VISUAL_OBJECT (&transform[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_colorfade_init (VisPluginData *plugin)
{
	ColorfadePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_BOOLEAN, "Enabled"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("faders0", -8, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("faders1", -8, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("faders2", -8, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beatfaders0", -8, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beatfaders1", -8, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("beatfaders2", -8, VISUAL_PARAM_LIMIT_NONE, ""),

		VISUAL_PARAM_LIST_END
	};

	priv = visual_mem_new0 (ColorfadePrivate, 1);

    priv->proxy = AVS_GLOBAL_PROXY(visual_object_get_private(VISUAL_OBJECT(plugin)));
    
    if(priv->proxy == NULL)
    {
        visual_log(VISUAL_LOG_CRITICAL, "This plugin is part of the AVS plugin.");
        return -VISUAL_ERROR_GENERAL;
    }
  
    visual_object_ref(VISUAL_OBJECT(priv->proxy));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

    for(x = 0; x < 512; x++)
    {
        for(y = 0; y < 512; y++)
        {
            int xp = x - 255;
            int yp = y - 255;
            if(xp > 0 && xp > -yp )
                priv->c_tab[x][y]=0;
            else if(yp < 0 && xp < -yp )
                priv->c_tab[x][y]=1;
            else if(xp < 0 && yp > 0)
                priv->c_tab[x][y]=2;
            else
                priv->c_tab[x][y]=3;
        }
    }
    for(x = 0; x < 256+40+40; x++)
        priv->clip[x] = min(max(x-40, 0), 255);
	return 0;
}

int lv_colorfade_cleanup (VisPluginData *plugin)
{
	ColorfadePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_object_unref(VISUAL_OBJECT(priv->proxy));

	visual_mem_free (priv);

	return 0;
}

int lv_colorfade_events (VisPluginData *plugin, VisEventQueue *events)
{
	ColorfadePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;
                if (visual_param_entry_is (param, "enabled"))
                    priv->enabled = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is (param, "faders0")) {
                    priv->faders[0] = visual_param_entry_get_integer(param);
                    priv->faderpos[0] = priv->faders[0];
                } else if(visual_param_entry_is (param, "faders1")) {
                    priv->faders[1] = visual_param_entry_get_integer(param);
                    priv->faderpos[1] = priv->faders[1];
                } else if(visual_param_entry_is (param, "faders2")) {
                    priv->faders[2] = visual_param_entry_get_integer(param);
                    priv->faderpos[2] = priv->faders[2];
                } else if(visual_param_entry_is (param, "beatfaders0"))
                    priv->beatfaders[0] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is (param, "beatfaders1"))
                    priv->beatfaders[1] = visual_param_entry_get_integer(param);
                else if(visual_param_entry_is (param, "beatfaders2"))
                    priv->beatfaders[2] = visual_param_entry_get_integer(param);
                    
				break;

			default:
				break;
		}
	}

	return 0;
}

int lv_colorfade_palette (VisPluginData *plugin, VisPalette *pal, VisAudio *audio)
{
	return 0;
}

int lv_colorfade_video (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ColorfadePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	uint8_t *pixels = visual_video_get_pixels (video);
    uint8_t isBeat = priv->proxy->isBeat;

    colorfade_begin(priv, data, framebuffer, fbout, video->width, video->height, isBeat);

    if(isBeat & 0x80000000) return 0;

    colorfade_render(priv, data, framebuffer, fbout, video->width, video->height, isBeat);

    return 0;
}


int colorfade_begin(ColorfadePrivate *priv, float *data, uint8_t framebuffer, uint8_t fbout, int w, int h, int isBeat)
{
  if (!priv->enabled || (isBeat&0x80000000)) return 0;

  if (priv->faderpos[0] < priv->faders[0]) priv->faderpos[0]++;
  if (priv->faderpos[1] < priv->faders[2]) priv->faderpos[1]++;
  if (priv->faderpos[2] < priv->faders[1]) priv->faderpos[2]++;
  if (priv->faderpos[0] > priv->faders[0]) priv->faderpos[0]--;
  if (priv->faderpos[1] > priv->faders[2]) priv->faderpos[1]--;
  if (priv->faderpos[2] > priv->faders[1]) priv->faderpos[2]--;

  if (!(priv->enabled&4))
  {
    priv->faderpos[0]=priv->faders[0];
    priv->faderpos[1]=priv->faders[1];
    priv->faderpos[2]=priv->faders[2];
  }
  else if (isBeat && (priv->enabled&2))
  {
    priv->faderpos[0]=(rand()%32)-6;
    priv->faderpos[1]=(rand()%64)-32;
    if (priv->faderpos[1] < 0 && priv->faderpos[1] > -16) priv->faderpos[1]=-32;
    if (priv->faderpos[1] >= 0 && priv->faderpos[1] < 16) priv->faderpos[1]=32;
    priv->faderpos[2]=(rand()%32)-6;
  }
  else if (isBeat)
  {
    priv->faderpos[0]=priv->beatfaders[0];
    priv->faderpos[1]=priv->beatfaders[1];
    priv->faderpos[2]=priv->beatfaders[2];
  }

  {
    int fs1,fs2,fs3;
    fs1=priv->faderpos[0];
    fs2=priv->faderpos[1];
    fs3=priv->faderpos[2];

    priv->ft[0][0]=fs3;
    priv->ft[0][1]=fs2;
    priv->ft[0][2]=fs1;

    priv->ft[1][0]=fs2;
    priv->ft[1][1]=fs1;
    priv->ft[1][2]=fs3;

    priv->ft[2][0]=fs1;
    priv->ft[2][1]=fs3;
    priv->ft[2][2]=fs2;

    priv->ft[3][0]=fs3;
    priv->ft[3][1]=fs3;
    priv->ft[3][2]=fs3;
  }

  return 0;
}

int colorfade_render(ColorfadePrivate *priv, float *data, uint8_t framebuffer, uint8_t fbout, int w, int h, int isBeat)
{
  if (!priv->enabled) return;

  unsigned char *q=framebuffer;

  unsigned char *ctab_ptr=(unsigned char *)priv->c_tab[0]+255+(255<<9);
  unsigned char *clip_ptr=(unsigned char *)priv->clip+40;

  int x=0;

  if (priv->enabled) 
  {
    int lx=x&1;
    x>>=1;
    while (x--)
    {
      int r=q[0];
      int g=q[1];
      int b=q[2];
      int r2=q[4];
      int g2=q[5];
      int b2=q[6];
      int i=((g-b)<<9) + b - r;
      int i2=((g2-b2)<<9) + b2 - r2;
      int p=ctab_ptr[i];
      int p2=ctab_ptr[i2];

      q[0]=clip_ptr[r+ft[p][0]];
      q[1]=clip_ptr[g+ft[p][1]];
      q[2]=clip_ptr[b+ft[p][2]];
      q[4]=clip_ptr[r2+ft[p2][0]];
      q[5]=clip_ptr[g2+ft[p2][1]];
      q[6]=clip_ptr[b2+ft[p2][2]];
      q+=8;
    }
    if (lx)
    {
      int r=q[0];
      int g=q[1];
      int b=q[2];
      int i=((g-b)<<9) + b - r;
      int p=ctab_ptr[i];
      q[0]=clip_ptr[r+ft[p][0]];
      q[1]=clip_ptr[g+ft[p][1]];
      q[2]=clip_ptr[b+ft[p][2]];
    }
  }

}

