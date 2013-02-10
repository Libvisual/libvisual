/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2012       Libvisual team
 *               2004-2006  Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define SAMPLES_PER_FRAME_DEFAULT 512

#define MIN(a,b) ((a) <= (b) ? (a) : (b))

typedef struct {
    unsigned int samples_per_frame;
} LVDumpPrivate;

static int         lv_dump_init        (VisPluginData *plugin);
static void        lv_dump_cleanup     (VisPluginData *plugin);
static void        lv_dump_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_dump_resize      (VisPluginData *plugin, int width, int height);
static int         lv_dump_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_dump_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_dump_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
    static VisActorPlugin actor = {
        .requisition      = lv_dump_requisition,
        .palette          = lv_dump_palette,
        .render           = lv_dump_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT
    };

    static VisPluginInfo info = {
        .type     = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "lv_dump",
        .name     = "libvisual PCM data dump",
        .author   = "Chong Kai Xiong <kaixiong@codeleft.sg>",
        .version  = "1.0",
        .about    = N_("Libvisual data dump plugin"),
        .help     = N_("A plugin that simply dumps floating-point PCM data for debugging"),
        .license  = VISUAL_PLUGIN_LICENSE_LGPL,

        .init    = lv_dump_init,
        .cleanup = lv_dump_cleanup,
        .events  = lv_dump_events,

        .plugin  = &actor
    };

    return &info;
}

static int lv_dump_init (VisPluginData *plugin)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    LVDumpPrivate *priv = visual_mem_new0 (LVDumpPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    /* Parameter specifications */
    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_integer ("samples_per_frame", "Samples per frame",
                                                          SAMPLES_PER_FRAME_DEFAULT,
                                                          NULL),
                                NULL);

    /* Default values */
    priv->samples_per_frame = SAMPLES_PER_FRAME_DEFAULT;

    return TRUE;
}

static void lv_dump_cleanup (VisPluginData *plugin)
{
    LVDumpPrivate *priv = visual_plugin_get_private (plugin);

    visual_mem_free (priv);
}

static void lv_dump_requisition (VisPluginData *plugin, int *width, int *height)
{
    LVDumpPrivate *priv = visual_plugin_get_private (plugin);

    /* We will dump all data into a single row, 1 float sample per 32-bit pixel */
    *width  = priv->samples_per_frame;
    *height = 1;
}

static void lv_dump_resize (VisPluginData *plugin, int width, int height)
{
    /* Nothing to resize. We will pad, clip and wrap the dump
     * according to the given dimensions */
}

static int lv_dump_events (VisPluginData *plugin, VisEventQueue *events)
{
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev))
    {
        switch (ev.type)
        {
            case VISUAL_EVENT_PARAM:
            {
                /* TODO: Handle this */
                break;
            }

            case VISUAL_EVENT_RESIZE:
            {
                lv_dump_resize (plugin, ev.event.resize.width, ev.event.resize.height);
                break;
            }

            default:;
                /* Ignore the rest */
        }
    }

    return TRUE;
}

static VisPalette *lv_dump_palette (VisPluginData *plugin)
{
    return NULL;
}

static void lv_dump_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    LVDumpPrivate *priv = visual_plugin_get_private (plugin);

    VisBuffer *pcm_buffer = visual_buffer_new_allocate (priv->samples_per_frame * sizeof (float));

    visual_audio_get_sample_mixed_simple (audio, pcm_buffer, 2,
        VISUAL_AUDIO_CHANNEL_LEFT,
        VISUAL_AUDIO_CHANNEL_RIGHT);

    int width  = visual_video_get_width (video);
    int height = visual_video_get_height (video);

    unsigned int samples_to_render = MIN(width * height, priv->samples_per_frame);

    visual_video_fill_color (video, NULL);

    visual_mem_copy (visual_video_get_pixels(video),
                     visual_buffer_get_data (pcm_buffer),
                     samples_to_render * sizeof(float));

    visual_buffer_unref (pcm_buffer);
}
