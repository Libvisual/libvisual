/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <math.h>
#include <string.h>
#include <limits.h>
#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define PCM_SIZE 512

typedef struct {
    VisPalette *pal;
    VisBuffer  *pcm;
    double n, b, x, y, i, v, w, h, color, linesize, skip, drawmode, t, d;
    double zo, z1, r1, r2, r3;
    int channel_source, needs_init;
    VisTimer *timer;
} ScopePrivate;

static int         lv_scope_init        (VisPluginData *plugin);
static void        lv_scope_cleanup     (VisPluginData *plugin);
static void        lv_scope_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_scope_resize      (VisPluginData *plugin, int width, int height);
static int         lv_scope_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_scope_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_scope_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
    static VisActorPlugin actor = {
        .requisition = lv_scope_requisition,
        .palette     = lv_scope_palette,
        .render      = lv_scope_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
    };

    static VisPluginInfo info = {
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "lv_scope",
        .name     = "libvisual scope",
        .author   = "Dennis Smit <ds@nerds-incorporated.org>",
        .version  = "0.1",
        .about    = N_("Libvisual scope plugin"),
        .help     = N_("This is a test plugin that'll display a simple scope"),
        .license  = VISUAL_PLUGIN_LICENSE_LGPL,

        .init     = lv_scope_init,
        .cleanup  = lv_scope_cleanup,
        .events   = lv_scope_events,
        .plugin   = &actor
    };

    return &info;
}

static int lv_scope_init (VisPluginData *plugin)
{
    ScopePrivate *priv;

#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    priv = visual_mem_new0 (ScopePrivate, 1);
    visual_plugin_set_private (plugin, priv);

    priv->pal = visual_palette_new (256);

    priv->pcm = visual_buffer_new_allocate (sizeof (float) * PCM_SIZE);

    priv->needs_init = TRUE;

    priv->timer = visual_timer_new();

    return TRUE;
}

static void lv_scope_cleanup (VisPluginData *plugin)
{
    ScopePrivate *priv = visual_plugin_get_private (plugin);

    visual_palette_free (priv->pal);

    visual_buffer_unref (priv->pcm);

    visual_timer_free(priv->timer);

    visual_mem_free (priv);
}

static void lv_scope_requisition (VisPluginData *plugin, int *width, int *height)
{
    int reqw, reqh;

    reqw = *width;
    reqh = *height;

    while (reqw % 2 || (reqw / 2) % 2)
        reqw--;

    while (reqh % 2 || (reqh / 2) % 2)
        reqh--;

    if (reqw < 32)
        reqw = 32;

    if (reqh < 32)
        reqh = 32;

    *width = reqw;
    *height = reqh;
}

static void lv_scope_resize (VisPluginData *plugin, int width, int height)
{
}

static int lv_scope_events (VisPluginData *plugin, VisEventQueue *events)
{
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                lv_scope_resize (plugin, ev.event.resize.width, ev.event.resize.height);
                break;
            default: /* to avoid warnings */
                break;
        }
    }

    return TRUE;
}

static VisPalette *lv_scope_palette (VisPluginData *plugin)
{
    ScopePrivate *priv = visual_plugin_get_private (plugin);
    VisColor *pal_colors = visual_palette_get_colors (priv->pal);
    int i;

    for (i = 0; i < 256; i++) {
        pal_colors[i].r = i;
        pal_colors[i].g = i;
        pal_colors[i].b = i;
    }

    return priv->pal;
}

static void run_init(ScopePrivate *priv)
{
    priv->n = 128;
    priv->zo = 0;
    priv->d = 0;
    visual_timer_start(priv->timer);
}

static void run_frame(ScopePrivate *priv)
{
    priv->zo=visual_timer_elapsed_usecs(priv->timer) / 1000.0 / 1000.0;
    priv->r1=1/7.0;
    priv->r2=4/9.0;
    priv->r3=5/3.0;
    priv->d += priv->zo;
}

static void run_beat(ScopePrivate *priv)
{
    priv->zo = visual_timer_elapsed_msecs(priv->timer) + 1000;
}

static void run_point(ScopePrivate *priv)
{
    priv->r1=priv->r2*9333.2312311+priv->r3*33222.93329;
    priv->r1=priv->r1-floor(priv->r1);
    priv->r2=priv->r3*6233.73553+priv->r1*9423.1323219;
    priv->r2=priv->r2-floor(priv->r2);
    priv->r3=priv->r1*373.871324+priv->r2*43322.4323441;
    priv->r3=priv->r3-floor(priv->r3);

    priv->z1=priv->r3-priv->zo;
    priv->z1=.5/(priv->z1-floor(priv->z1)+.2);

    priv->x=(priv->r2*2-1)*priv->z1;
    priv->y=(priv->r1*2-1)*priv->z1;
    priv->color=(1-exp(-priv->z1*priv->z1)) * 255.0;
}

static void lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    ScopePrivate *priv = visual_plugin_get_private (plugin);
    VisColor col;
    float *pcmbuf;
    int y, x;
    int video_width, video_height, video_pitch;
    uint8_t *buf;
    int isBeat = 0;

    int a, l;
    int ws=(priv->channel_source&4)?1:0;
    int xorv = (ws*128)^128;
    int size = visual_video_get_width(video)/4.0;

    video_width  = visual_video_get_width  (video);
    video_height = visual_video_get_height (video);
    video_pitch  = visual_video_get_pitch  (video);

    visual_audio_get_sample_mixed_simple (audio, priv->pcm, 2,
            VISUAL_AUDIO_CHANNEL_LEFT,
            VISUAL_AUDIO_CHANNEL_RIGHT);

    pcmbuf = visual_buffer_get_data (priv->pcm);

    buf = (uint8_t *) visual_video_get_pixels (video);

    visual_color_set(&col, 0, 0, 0);

    visual_video_fill_color(video, &col);

    // Star field

    if(priv->needs_init)
    {
        run_init(priv);
        priv->needs_init = FALSE;
    }

    priv->h = video_height;
    priv->w = video_width;
    priv->b = 0;

    run_frame(priv);

    if(isBeat)
        run_beat(priv);

    l = priv->n;

    if(l >= 128*size)
        l = 128*size - 1;


    for(a = 0; a < l; a++)
    {
        double r=(a*size)/(double)l;
        double s1=r-(int)r;
        int val1 = (pcmbuf[(int)r]/(float)UCHAR_MAX + 1) / 2.0 * 128;
        int val2 = (pcmbuf[(int)r+1]/(float)UCHAR_MAX + 1) / 2.0 * 128;
        double yr = (val1^xorv)*(1.0-s1)+(val2^xorv)*(s1);
        priv->y = yr/128.0;
        priv->i = (double)a/(double)(l-1);

        run_point(priv);

        uint8_t this_color = priv->color;

        x = (int)((priv->x + 1) * (double)video_width * 0.5);
        y = (int)((priv->y + 1) * (double)video_height * 0.5);

        if(y >= 0 && y < video_height && x >= 0 && x < video_width)
        {
            buf[x+y*video_width] = this_color;
        }
    }

    // Scope
    const int max_displacement = video_height / 4;
    const int y_origin = video_height / 2;
    int i;

    for (i = 0; i < video_width; i++) {
        int j;

        const int y_tip = y_origin + (pcmbuf[(i >> 1) % PCM_SIZE] * (max_displacement));

        if (y_tip > y_origin) {
            for (j = y_origin; j < y_tip; j++)
                buf[(j * video_pitch) + i] = 255;
        } else {
            for (j = y_tip; j <= y_origin; j++)
                buf[(j * video_pitch) + i] = 255;
        }
    }
}

