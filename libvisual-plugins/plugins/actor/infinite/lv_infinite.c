/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_infinite.c,v 1.24 2006/01/30 18:22:50 synap Exp $
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

#include "config.h"
#include "gettext.h"
#include "main.h"
#include "renderer.h"
#include "display.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_infinite_init        (VisPluginData *plugin);
static void        act_infinite_cleanup     (VisPluginData *plugin);
static void        act_infinite_resize      (VisPluginData *plugin, int width, int height);
static void        act_infinite_requisition (VisPluginData *plugin, int *width, int *height);
static int         act_infinite_events      (VisPluginData *plugin, VisEventQueue *events);
static void        act_infinite_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *act_infinite_palette     (VisPluginData *plugin);


const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_infinite_requisition,
		.palette     = act_infinite_palette,
		.render      = act_infinite_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "infinite",
		.name     = "infinite plugin",
		.author   = N_("Original by: Julien Carme <julien.carme@acm.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.1",
		.about    = N_("Infinite visual plugin"),
		.help     = N_("This is the libvisual plugin for the infinite visual"),
		.license  = VISUAL_PLUGIN_LICENSE_GPL,

		.init     = act_infinite_init,
		.cleanup  = act_infinite_cleanup,
		.events   = act_infinite_events,
		.plugin   = &actor
	};

	return &info;
}

int act_infinite_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	InfinitePrivate *priv = visual_mem_new0 (InfinitePrivate, 1);
	visual_plugin_set_private (plugin, priv);

	priv->rcontext = visual_plugin_get_random_context (plugin);

	priv->plugwidth = 32;
	priv->plugheight = 32;

	priv->pal = visual_palette_new (256);

	_inf_init_renderer (priv);
	_inf_load_random_effect(priv, &priv->current_effect);

	priv->color = visual_random_context_int_range (priv->rcontext, 0, NB_PALETTES - 1);
	_inf_change_color(priv, priv->old_color, priv->color, 256);
	priv->old_color = priv->color;

	priv->color = visual_random_context_int_range (priv->rcontext, 0, NB_PALETTES - 1);

	return TRUE;
}

static void act_infinite_cleanup (VisPluginData *plugin)
{
	InfinitePrivate *priv;

	priv = visual_plugin_get_private (plugin);

	_inf_close_renderer (priv);

	visual_palette_free (priv->pal);
	visual_mem_free (priv);
}

static void act_infinite_requisition (VisPluginData *plugin, int *width, int *height)
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

static void act_infinite_resize (VisPluginData *plugin, int width, int height)
{
	InfinitePrivate *priv = visual_plugin_get_private (plugin);
	priv->plugwidth = width;
	priv->plugheight = height;

	_inf_close_renderer (priv);
	_inf_init_renderer (priv);
}

static int act_infinite_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_infinite_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_infinite_palette (VisPluginData *plugin)
{
	InfinitePrivate *priv;

	visual_return_val_if_fail (plugin != NULL, NULL);

	priv = visual_plugin_get_private (plugin);

	return priv->pal;
}

static void act_infinite_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	InfinitePrivate *priv = visual_plugin_get_private (plugin);

	VisBuffer *pcm_left = visual_buffer_new_wrap_data(priv->pcm_data[0], sizeof (float) * 512, 0);
	visual_audio_get_sample (audio, pcm_left, VISUAL_AUDIO_CHANNEL_LEFT);
	visual_buffer_unref(pcm_left);

	VisBuffer *pcm_right = visual_buffer_new_wrap_data(priv->pcm_data[1], sizeof (float) * 512, 0);
	visual_audio_get_sample (audio, pcm_right, VISUAL_AUDIO_CHANNEL_RIGHT);
	visual_buffer_unref(pcm_right);

	_inf_renderer (priv);
	_inf_display (priv, (uint8_t *) visual_video_get_pixels (video), visual_video_get_pitch (video));
}
