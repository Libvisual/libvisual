/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_lv_analyzer.c,v 1.28 2006/01/27 20:19:16 synap Exp $
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

#include <config.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <limits.h>

#include <libvisual/libvisual.h>

/** default amount of bars */
#define BARS_DEFAULT 25
#define BARS_DEFAULT_SPACE 1


/* helper macro */
#define QTY(array)  (sizeof(array) / sizeof(*(array)))

const VisPluginInfo *get_plugin_info (int *count);

typedef struct
{
	VisPalette pal;
	VisParamContainer *paramcontainer;
	int bars;
	int width, height;
} AnalyzerPrivate;

static void draw_bar (VisVideo *video, int index, int nbars, float amplitude);
static inline void draw_vline (VisVideo *video, int x1, int x2, int y, uint8_t color);

static int lv_analyzer_init (VisPluginData *plugin);
static int lv_analyzer_cleanup (VisPluginData *plugin);
static int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height);
static int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *lv_analyzer_palette (VisPluginData *plugin);
static int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = lv_analyzer_requisition,
		.palette = lv_analyzer_palette,
		.render = lv_analyzer_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_analyzer",
		.name = "libvisual analyzer",
		.author = N_("Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "1.0",
		.about = N_("Libvisual analyzer plugin"),
		.help = N_("A nice simple spectrum analyzer plugin."),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = lv_analyzer_init,
		.cleanup = lv_analyzer_cleanup,
		.events = lv_analyzer_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

static int _bars(VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	return priv->bars;
}

static int lv_analyzer_init (VisPluginData *plugin)
{

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	AnalyzerPrivate *priv = visual_mem_new0 (AnalyzerPrivate, 1);
	visual_return_val_if_fail(priv != NULL, -1);

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	/* default values */
	priv->bars = BARS_DEFAULT;

	/* get plugins param-container */
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	visual_return_val_if_fail(paramcontainer != NULL, -1);

	/* save paramcontainer */
	priv->paramcontainer = paramcontainer;

	/* parameter-description */
	static VisParamEntry params[] =
	{
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("bars", BARS_DEFAULT),
        VISUAL_PARAM_LIST_END
    };

    /* register parameters */
	visual_param_container_add_many (paramcontainer, params);

	/* allocate space for palette */
	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

static int lv_analyzer_cleanup (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_mem_free (priv);

	return 0;
}

static int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw;

	reqw = *width;

	while (reqw % 2 || reqw % 4)
		reqw--;

	/*if (reqw < 32)
		reqw = 32;*/

	*width = reqw;

	return 0;
}

static int _validate_bars(VisPluginData *plugin, int *bars)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if(*bars > 0 && *bars < priv->width)
		return 1;

	return 0;
}

static void _change_bars(VisPluginData *plugin,
                       VisParamEntry *p, int (*validator)(VisPluginData *plugin, void *value))
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	int integer = visual_param_entry_get_integer(p);

	if(!validator || validator(plugin, &integer))
    {
		priv->bars = integer;
        return;
    }
    /* reset to previous value */
    else
        visual_param_entry_set_integer(p, priv->bars);
}

static void _change_param(VisPluginData *plugin, VisParamEntry *p)
{
	/**
     * structure defining handler functions for configuration values
     */
    struct
    {
        /* parameter-name */
        char *name;
        /* validator function */
        int (*validator)(void *value);
        /* function called to change parameter */
        void (*change)(VisPluginData *plugin,
                       VisParamEntry *parameter, int (*validator)(void *value));
        /* function called after parameter change */
        void (*postchange)(VisPluginData *plugin);
    } parms[] =
    {
        {"bars", (void *) _validate_bars, (void *) _change_bars, NULL},
    };



    /** look for parameter in our structure */
    int i;
    for(i = 0; i < QTY(parms); i++)
    {
        /* not our parameter? -> continue the quest */
        if(!visual_param_entry_is(p, parms[i].name))
            continue;

        /* call this parameters' change handler */
        if(parms[i].change)
            parms[i].change(plugin, p, parms[i].validator);

        /* call this parameters' post-change handler */
        if(parms[i].postchange)
            parms[i].postchange(plugin);

        return;
    }

    visual_log(VISUAL_LOG_WARNING, "Unknown param '%s'", visual_param_entry_get_name(p));
}

static int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));


	while (visual_event_queue_poll (events, &ev))
	{
		switch (ev.type)
		{
			case VISUAL_EVENT_PARAM:
			{
                VisParamEntry *param = ev.event.param.param;
                /* change config parameter */
                _change_param(plugin, param);
                break;
			}

			case VISUAL_EVENT_RESIZE:
			{
				visual_video_set_dimension (ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				priv->width = ev.event.resize.video->width;
				priv->height = ev.event.resize.video->height;

				if(priv->width > priv->bars)
					priv->bars = priv->width;

				break;
			}

			default: /* to avoid warnings */
			{
				break;
			}
		}
	}

	return 0;
}

static VisPalette *lv_analyzer_palette (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = 0;
		priv->pal.colors[i].g = 0;
		priv->pal.colors[i].r = 0;
	}

	for (i = 1; i < 64; i++) {
		priv->pal.colors[i].r = i * 4;
		priv->pal.colors[i].g = 255;
		priv->pal.colors[i].b = 0;

		priv->pal.colors[i + 63].r = 255;
		priv->pal.colors[i + 63].g = (63 - i) * 4;
		priv->pal.colors[i + 63].b = 0;
	}

	return &priv->pal;
}

/**
 * draw vertical line to VisVideo
 * @p[in] video - VisVideo to draw on
 * @p[in] x1 - start coordinate
 * @p[in] x2 - end coordinate
 * @p[in] y - height
 * @p[in] color - color to draw bar
 */
static inline void draw_vline (VisVideo *video, int x1, int x2, int y, uint8_t color)
{
	uint8_t *pixels = visual_video_get_pixels (video);

	if (video->depth != VISUAL_VIDEO_DEPTH_8BIT)
		return;

	pixels += (y * video->pitch) + x1;
	visual_mem_set (pixels, color, x2 - x1);
}

/**
 * draw one vertical bar
 * @p[in] video - VisVideo to draw on
 * @p[in] index - index of this bar
 * @p[in] nbars - total amount of bars
 * @p[in] amplitude - amplitude of waveform
 */
static void draw_bar (VisVideo *video, int index, int nbars, float amplitude)
{
	int height = video->height * amplitude;
	int i;
	float scale = 128.0 / video->height;
	int width = (video->width-1)/nbars;

	for (i = video->height - 1; i > (video->height - height); i--)
	{
		draw_vline(video, index*width, index*width + width - BARS_DEFAULT_SPACE, i, (video->height - i) * scale);
	}
}

/**
 * render analyzer - calledback
 */
static int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	VisBuffer buffer;
	VisBuffer pcmb;
	int bars = _bars(plugin);

	/* no value configured? */
	if(bars < 0)
		bars = video->width/2;

	float freq[bars];
	float pcm[bars * 2];
	int i;

	visual_video_fill_color (video, NULL);

	visual_buffer_set_data_pair (&buffer, freq, sizeof (freq));
	visual_buffer_set_data_pair (&pcmb, pcm, sizeof (pcm));

	visual_audio_get_sample_mixed_simple (audio, &pcmb, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (&buffer, &pcmb, TRUE);

	for (i = 0; i < bars; i++)
		draw_bar (video, i, bars, freq[i]);

	return 0;
}





