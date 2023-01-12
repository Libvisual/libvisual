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

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

/** default amount of bars */
#define BARS_DEFAULT 25
/** default space between bars in pixels */
#define BARS_DEFAULT_SPACE 0


/* helper macro */
#define QTY(array)  (sizeof(array) / sizeof(*(array)))

typedef struct
{
	VisPalette *pal;
	int bars;
	int bar_space;
	int width, height;
	VisBuffer *pcm_buffer;
	VisBuffer *freq_buffer;
} AnalyzerPrivate;

static int         lv_analyzer_init        (VisPluginData *plugin);
static void        lv_analyzer_cleanup     (VisPluginData *plugin);
static void        lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height);
static int         lv_analyzer_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_analyzer_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_analyzer_palette     (VisPluginData *plugin);




const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_analyzer_requisition,
		.palette     = lv_analyzer_palette,
		.render      = lv_analyzer_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_analyzer",
		.name     = "libvisual analyzer",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>",
		.version  = "1.1",
		.about    = N_("Libvisual analyzer plugin"),
		.help     = N_("A nice simple spectrum analyzer plugin."),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init    = lv_analyzer_init,
		.cleanup = lv_analyzer_cleanup,
		.events  = lv_analyzer_events,
		.plugin  = &actor
	};

	return &info;
}


static int lv_analyzer_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	AnalyzerPrivate *priv = visual_mem_new0 (AnalyzerPrivate, 1);
	visual_plugin_set_private (plugin, priv);

	/* get plugins param-container */
	VisParamList *params = visual_plugin_get_params (plugin);
	visual_param_list_add_many (params,
                                visual_param_new_integer ("bars", "Number of bars in graph", BARS_DEFAULT, NULL),
	                            visual_param_new_integer ("bar-space", "Amount of pixels between bars in graph", BARS_DEFAULT_SPACE, NULL),
                                NULL);

	/* default values */
	priv->bars = BARS_DEFAULT;
	priv->bar_space = BARS_DEFAULT_SPACE;

	/* allocate buffers */
	priv->pcm_buffer = visual_buffer_new_allocate (priv->bars*2 * sizeof (float));
	priv->freq_buffer = visual_buffer_new_allocate (priv->bars * sizeof (float));
		
	/* allocate space for palette */
	priv->pal = visual_palette_new (256);

	/* initialize palette */
	VisColor *pal_colors = visual_palette_get_colors (priv->pal);
	int i;

	for (i = 0; i < 256; i++) {
		pal_colors[i].r = 0;
		pal_colors[i].g = 0;
		pal_colors[i].b = 0;
	}

	for (i = 1; i < 64; i++) {
		pal_colors[i].r = i * 4;
		pal_colors[i].g = 255;
		pal_colors[i].b = 0;

		pal_colors[i + 63].r = 255;
		pal_colors[i + 63].g = (63 - i) * 4;
		pal_colors[i + 63].b = 0;
	}
		
	return TRUE;
}

static void lv_analyzer_cleanup (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

	visual_buffer_unref (priv->freq_buffer);
	visual_buffer_unref (priv->pcm_buffer);
		
	visual_palette_free (priv->pal);

	visual_mem_free (priv);
}

static void lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height)
{
    AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

	int total_space = priv->bar_space * (priv->bars - 1);

    int min_width = priv->bars + total_space;

    if (*width < min_width) {
        *width = min_width;
    }
}

static int _validate_bar_space(VisPluginData *plugin, int *bar_space)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

	if(*bar_space > 0 && 
	   priv->bars + *bar_space * (priv->bars - 1) < priv->width)
		return 1;

	return 0;
}

static int _validate_bars(VisPluginData *plugin, int *bars)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

	if(*bars > 0 && *bars < priv->width)
		return 1;

	return 0;
}

static void _change_bar_space(VisPluginData *plugin,
                         VisParam *p,
                         int (*validator)(VisPluginData *plugin, void *value))
{
	AnalyzerPrivate *priv = visual_plugin_get_private(plugin);

	int integer = visual_param_get_value_integer(p);

	if(!validator || validator(plugin, &integer))
    {
		priv->bar_space = integer;
        return;
    }
    /* reset to previous value */
    else
        visual_param_set_value_integer(p, priv->bar_space);
}

static void _change_bars(VisPluginData *plugin,
                         VisParam *p,
                         int (*validator)(VisPluginData *plugin, void *value))
{
    AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

    int integer = visual_param_get_value_integer(p);

    if(!validator || validator(plugin, &integer))
    {
        priv->bars = integer;

        /* resize buffers */

        visual_buffer_unref(priv->pcm_buffer);
        visual_buffer_unref(priv->freq_buffer);

        priv->pcm_buffer = visual_buffer_new_allocate(priv->bars*2 * sizeof (float));
        priv->freq_buffer = visual_buffer_new_allocate(priv->bars * sizeof (float));
        return;
    }
    /* reset to previous value */
    else
        visual_param_set_value_integer(p, priv->bars);
}

static void _change_param(VisPluginData *plugin, VisParam *p)
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
                       VisParam *parameter, int (*validator)(void *value));
        /* function called after parameter change */
        void (*postchange)(VisPluginData *plugin);
    } parms[] =
    {
        {"bars", (void *) _validate_bars, (void *) _change_bars, NULL},
		{"bar-space", (void *) _validate_bar_space, (void *) _change_bar_space, NULL},
    };

    /** look for parameter in our structure */
    int i;
    for(i = 0; i < QTY(parms); i++)
    {
        /* not our parameter? -> continue the quest */
        if(!visual_param_has_name(p, parms[i].name))
            continue;

        /* call this parameters' change handler */
        if(parms[i].change)
            parms[i].change(plugin, p, parms[i].validator);

        /* call this parameters' post-change handler */
        if(parms[i].postchange)
            parms[i].postchange(plugin);

        return;
    }

    visual_log(VISUAL_LOG_WARNING, "Unknown param '%s'", visual_param_get_name(p));
}

static void lv_analyzer_resize (VisPluginData *plugin, int width, int height)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);

	priv->width  = width;
	priv->height = height;
}

static int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev))
	{
		switch (ev.type)
		{
			case VISUAL_EVENT_PARAM:
			{
				VisParam *param = ev.event.param.param;
				/* change config parameter */
				_change_param(plugin, param);
				break;
			}

			case VISUAL_EVENT_RESIZE:
			{
				lv_analyzer_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;
			}

			default: /* to avoid warnings */
			{
				break;
			}
		}
	}

	return TRUE;
}

static VisPalette *lv_analyzer_palette (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);
	return priv->pal;
}

static inline void draw_bar (VisVideo *video, int x, int width, float amplitude)
{
	/* NOTES:
	 * - We use 16:16 fixed point to incrementally calculate the color at each y
	 * - Bar row color must be in [1,126]
	*/
	int video_height = visual_video_get_height (video);
	int video_pitch	 = visual_video_get_pitch (video);
	int y = (1.0 - amplitude) * video_height;

	if (y >= video_height) 
		return;
		
	int color  = (1 << 16) + (amplitude * (125 << 16));
	int dcolor = (125 << 16) / video_height;

	uint8_t *row = (uint8_t *) visual_video_get_pixel_ptr (video, x, y);

	while (y < video_height) {
		visual_mem_set (row, color >> 16, width);

		y++; row += video_pitch;
		color -= dcolor;
	}

}

/**
 * render analyzer - calledback
 */
static void lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	AnalyzerPrivate *priv = visual_plugin_get_private (plugin);
	
	visual_audio_get_sample_mixed_simple (audio, priv->pcm_buffer, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);
	visual_audio_get_spectrum_for_sample (priv->freq_buffer, priv->pcm_buffer, TRUE);

	
	int spaces = priv->bar_space * (priv->bars - 1);
	int width  = (visual_video_get_width (video) - spaces) / priv->bars;
	int x	   = ((visual_video_get_width (video) - spaces) % priv->bars) / 2;

	visual_video_fill_color (video, NULL);

	float *freq = (float *) visual_buffer_get_data (priv->freq_buffer);

	for(int i = 0; i < priv->bars; i++) {
		draw_bar (video, x, width, freq[i]);
		x += width + priv->bar_space;
	}
}
