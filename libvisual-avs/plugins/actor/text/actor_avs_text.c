/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_text.c,v 1.6 2006-09-19 19:05:47 synap Exp $
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
#include <sttext.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"

typedef struct {
    AvsGlobalProxy *proxy;
    CHOOSEFONT cf;
    LOGFONT lf;
    HBITMAP hRetBitmap; 
    HBITMAP hOldBitmap; 
    HFONT hOldFont;
    HFONT myFont;
    HDC hDesktopDC; 
    HDC hBitmapDC; 
    BITMAPINFO bi;
    int enabled;
    int color;
    int blend;
    int blendavg;
    int onbeat;
    int insertBlank;
    int randomPos;
    int valign;
    int halign;
    int onbeatSpeed;
    int normSpeed;
    char *text;
    int lw,lh;
    bool updating;
    int outline;
    int shadow;
    int outlinecolor;
    int outlinesize;
    int curword;
    int nb;
    int forceBeat;
    int xshift, yshift;
    int _xshift, _yshift;
    int forceshift;
    int forcealign;
    int _valign, _halign;
    int oddeven;
    int nf;
    RECT r;
    int *myBuffer;
    int forceredraw;
    int old_valign, old_halign, old_outline, oldshadow;
    int old_curword, old_clipcolor;
    char oldtxt[256];
    int old_blend1, old_blend2, old_blend3;
    int oldxshift, oldyshift;
    int randomword;
    int shiftinit;
} TextPrivate;

int lv_text_init (VisPluginData *plugin);
int lv_text_cleanup (VisPluginData *plugin);
int lv_text_requisition (VisPluginData *plugin, int *width, int *height);
int lv_text_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_text_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_text_palette (VisPluginData *plugin);
int lv_text_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

short get_data (TextPrivate *priv, VisAudio *audio, int index);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisActorPlugin actor[] = {{
        .requisition = lv_text_requisition,
        .palette = lv_text_palette,
        .render = lv_text_render,
        .vidoptions.depth =
            VISUAL_VIDEO_DEPTH_8BIT |
            VISUAL_VIDEO_DEPTH_32BIT

    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "avs_text",
        .name = "Libvisual AVS Render: text element",
        .author = "Dennis Smit <ds@nerds-incorporated.org>",
        .version = "0.1",
        .about = "The Libvisual AVS Render: text element",
        .help = "This is the text scope element for the libvisual AVS system",

        .init = lv_text_init,
        .cleanup = lv_text_cleanup,
        .events = lv_text_events,

        .plugin = VISUAL_OBJECT (&actor[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int lv_text_init (VisPluginData *plugin)
{
    TextPrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntryProxy params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("color", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendavg", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeat", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("insertBlank", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("randomPos", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("valign", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("halign", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("onbeatSpeed", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("normSpeed", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_STRING  ("myFont", "", ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("size", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("outline", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("outlinecolor", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("xshift", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("yshift", 1, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("outlinesize", 2, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("randomword", 0x10, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("shadow", 0, VISUAL_PARAM_LIMIT_NONE, ""),
        VISUAL_PARAM_LIST_END
    };

    
    priv = visual_mem_new0 (TextPrivate, 1);
    priv->proxy = visual_object_get_private(VISUAL_OBJECT(plugin));

    if(priv->proxy == NULL) {
        visual_log(VISUAL_LOG_CRITICAL, "This element is part of the AVS plugin");
        return 0;
    }
    visual_object_ref(VISUAL_OBJECT(priv->proxy));
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_palette_allocate_colors (&priv->pal, 1);

    for (i = 0; i < priv->pal.ncolors; i++) {
        priv->pal.colors[i].r = 0xff;
        priv->pal.colors[i].g = 0xff;
        priv->pal.colors[i].b = 0xff;
    }

    visual_param_container_add_many_proxy (paramcontainer, params);

    visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

    visual_palette_free_colors (&priv->pal);

    priv->forcealign = 1;
    priv->forcedraw = 1;
    priv->forceshift = 1;
    priv->shiftinit = 1;

    return 0;
}

int lv_text_cleanup (VisPluginData *plugin)
{
    TextPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_mem_free (priv);

    return 0;
}

int lv_text_requisition (VisPluginData *plugin, int *width, int *height)
{
    return 0;
}

int lv_text_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
    visual_video_set_dimension (video, width, height);

    return 0;
}

int lv_text_events (VisPluginData *plugin, VisEventQueue *events)
{
    TextPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                lv_text_dimension (plugin, ev.event.resize.video,
                        ev.event.resize.width, ev.event.resize.height);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "enabled"))
                    priv->source = visual_param_entry_get_integer (param);
                    priv->place = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "color"))
                    priv->color = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "blend"))
                    priv->blend = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "blendavg"))
                    priv->blendavg = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "onbeat"))
                    priv->onbeat = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "insertBlank"))
                    priv->insertBlank = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "randomPos"))
                    priv->randomPos = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "valign"))
                    priv->valign = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "halign"))
                    priv->halign = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "onbeatSpeed"))
                    priv->onbeatSpeed = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "normSpeed"))
                    priv->normSpeed = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "lpLogFont"))
                    priv->lpLogFont = visual_param_entry_get_string (param);
                else if (visual_param_entry_is (param, "myFont"))
                    priv->myFont = visual_param_entry_get_string (param);
                else if (visual_param_entry_is (param, "size"))
                    priv->size = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "outline"))
                    priv->outline = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "outlinecolor"))
                    priv->outlinecolor = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "xshift"))
                    priv->xshift = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "yshift"))
                    priv->yshift = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "outlinesize"))
                    priv->outlinesize = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "randomword"))
                    priv->randomword = visual_param_entry_get_integer (param);
                else if (visual_param_entry_is (param, "shadow"))
                    priv->shadow = visual_param_entry_get_integer (param);

                break;

            default:
                break;
        }
    }

    return 0;
}

VisPalette *lv_text_palette (VisPluginData *plugin)
{
    TextPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    return NULL;
}

int lv_text_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    TextPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    AvsGlobalProxy *proxy = priv->proxy;
    uint8_t *framebuffer = visual_video_get_pixels(video);
    int w = video->width;
    int h = video->height;

    return 0;
}

