/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_picture.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <stpicture.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;

    int enabled;
    int width, height;
    int lastWidth, lastHeight;
    int blend, blendavg, adapt, persist;
    int ration, axis_ratio;
    char ascName[MAX_PATH];
    int persistCount;

} PicturePrivate;

int lv_picture_init (VisPluginData *plugin);
int lv_picture_cleanup (VisPluginData *plugin);
int lv_picture_requisition (VisPluginData *plugin, int *width, int *height);
int lv_picture_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_picture_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_picture_palette (VisPluginData *plugin);
int lv_picture_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (PicturePrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_picture_requisition,
		.palette = lv_picture_palette,
		.render = lv_picture_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT

	}};

	static const VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "avs_picture",
		.name = "Libvisual AVS Render: picture element",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual AVS Render: picture element",
		.help = "This is the picture scope element for the libvisual AVS system",

		.init = lv_picture_init,
		.cleanup = lv_picture_cleanup,
		.events = lv_picture_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_picture_init (VisPluginData *plugin)
{
	PicturePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	int i;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_NONE, "Enable Picture Rendering"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 2, VISUAL_PARAM_LIMIT_NONE, "Blend"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 0x10, VISUAL_PARAM_LIMIT_NONE, "Blend average"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("adapt", 0, VISUAL_PARAM_LIMIT_NONE, "Adapt"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("ascName", 0, VISUAL_PARAM_LIMIT_NONE, "File Name"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("ratio", 0, VISUAL_PARAM_LIMIT_NONE, "Keep aspect ratio"),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("axis_ratio", 0, VISUAL_PARAM_LIMIT_NONE, "Axis Ratio"),

		VISUAL_PARAM_LIST_END
	};

    
	priv = visual_mem_new0 (PicturePrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many_proxy (paramcontainer, params);

	return 0;
}

int lv_picture_cleanup (VisPluginData *plugin)
{
	PicturePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_picture_requisition (VisPluginData *plugin, int *width, int *height)
{
	return 0;
}

int lv_picture_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_picture_events (VisPluginData *plugin, VisEventQueue *events)
{
	PicturePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_picture_dimension (plugin, ev.event.resize.video,
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
				else if (visual_param_entry_is (param, "adapt"))
					priv->adapt = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "persist"))
					priv->persist = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "ascName"))
					priv->ascName = visual_param_entry_get_string (param);
				else if (visual_param_entry_is (param, "ratio"))
					priv->ratio = visual_param_entry_get_integer (param);
				else if (visual_param_entry_is (param, "axis_ratio"))
					priv->axis_ratio = visual_param_entry_get_integer (param);


				break;

			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_picture_palette (VisPluginData *plugin)
{
	PicturePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return NULL;
}

int lv_picture_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PicturePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
	int w = video->width;
	int h = video->height;

  if (!enabled) return 0;

  if (!width || !height) return 0;


    if (lastWidth != w || lastHeight != h) 
  {
        lastWidth = w;
        lastHeight = h;

        if(hBitmapDC2) {
            DeleteDC(hBitmapDC2);
            DeleteObject(hb2);
            hBitmapDC2=0;
        }

        // Copy the bitmap from hBitmapDC to hBitmapDC2 and stretch it
      hBitmapDC = CreateCompatibleDC (NULL); 
      hOldBitmap = (HBITMAP) SelectObject (hBitmapDC, hb); 
        hBitmapDC2 = CreateCompatibleDC (NULL);
        hb2=CreateCompatibleBitmap (hBitmapDC, w, h);
        SelectObject(hBitmapDC2,hb2);
        {
            HBRUSH b=CreateSolidBrush(0);
            HPEN p=CreatePen(PS_SOLID,0,0);
            HBRUSH bold;
            HPEN pold;
            bold=(HBRUSH)SelectObject(hBitmapDC2,b);
            pold=(HPEN)SelectObject(hBitmapDC2,p);
            Rectangle(hBitmapDC2,0,0,w,h);
            SelectObject(hBitmapDC2,bold);
            SelectObject(hBitmapDC2,pold);
            DeleteObject(b);
            DeleteObject(p);
        }
        SetStretchBltMode(hBitmapDC2,COLORONCOLOR);
        int final_height=h,start_height=0;
        int final_width=w,start_width=0;
        if (ratio)
    {
            if(axis_ratio==0) {
                // ratio on X axis
                final_height=height*w/width;
                start_height=(h/2)-(final_height/2);
            } else {
                // ratio on Y axis
                final_width=width*h/height;
                start_width=(w/2)-(final_width/2);
            }
    }
        StretchBlt(hBitmapDC2,start_width,start_height,final_width,final_height,hBitmapDC,0,0,width,height,SRCCOPY);
        DeleteDC(hBitmapDC);
        hBitmapDC=0;
    }
  if (isBeat&0x80000000) return 0;

    // Copy the stretched bitmap to fbout
    BITMAPINFO bi;
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = w;
  bi.bmiHeader.biHeight = h;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = 0;
  bi.bmiHeader.biXPelsPerMeter = 0;
  bi.bmiHeader.biYPelsPerMeter = 0;
  bi.bmiHeader.biClrUsed = 0;
  bi.bmiHeader.biClrImportant = 0;
    GetDIBits(hBitmapDC2, hb2, 0, h, (void *)fbout, &bi, DIB_RGB_COLORS);

    // Copy the bitmap from fbout to framebuffer applying replace/blend/etc...
    if (isBeat)
        persistCount=persist;
  else
    if (persistCount>0) persistCount--;

    int *p,*d;
    int i,j;

  p = fbout;
  d = framebuffer+w*(h-1);
  if (blend || (adapt && (isBeat || persistCount)))
   for (i=0;i<h;i++)
    {
    for (j=0;j<w;j++)
        {
        *d=BLEND(*p, *d);
        d++;
        p++;
        }
    d -= w*2;
    }
  else
  if (blendavg || adapt)
   for (i=0;i<h;i++)
    {
    for (j=0;j<w;j++)
        {
        *d=BLEND_AVG(*p, *d);
        d++;
        p++;
        }
    d -= w*2;
    }
  else
   for (i=0;i<h;i++)
    {
    memcpy(d, p, w*4);
    p+=w;
    d-=w;
    }


    return 0;
}

