/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include "actor_bumpscope.h"
#include "bump_scope.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_bumpscope_init        (VisPluginData *plugin);
static void        act_bumpscope_cleanup     (VisPluginData *plugin);
static void        act_bumpscope_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_bumpscope_resize      (VisPluginData *plugin, int width, int height);
static int         act_bumpscope_events      (VisPluginData *plugin, VisEventQueue *events);
static void        act_bumpscope_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *act_bumpscope_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_bumpscope_requisition,
		.palette     = act_bumpscope_palette,
		.render      = act_bumpscope_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "bumpscope",
		.name     = "Bumpscope plugin",
		.author   = N_("Original by: Zinx Verituse <zinx@xmms.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.0.1",
		.about    = N_("Bumpscope visual plugin"),
		.help     = N_("This is the libvisual port of the xmms Bumpscope plugin"),
		.license  = VISUAL_PLUGIN_LICENSE_GPL,

		.init     = act_bumpscope_init,
		.cleanup  = act_bumpscope_cleanup,
		.events   = act_bumpscope_events,
		.plugin   = &actor
	};

	return &info;
}

static int act_bumpscope_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	VisParamList *params = visual_plugin_get_params (plugin);
	visual_param_list_add_many (params,
                                visual_param_new_color_rgb ("color",
                                                            N_("The light's color"),
                                                            122, 204, 255,
                                                            NULL),
                                visual_param_new_integer   ("light_size",
                                                            N_("The size of the light"),
                                                            256,
                                                            visual_param_in_range_integer (0, 1000)),
                                visual_param_new_bool      ("color_cycle",
                                                            N_("Whether to cycle colors"),
                                                            TRUE,
                                                            NULL),
                                visual_param_new_bool      ("moving_light",
                                                            N_("Whether the light moves with the mouse"),
                                                            TRUE,
                                                            NULL),
                                visual_param_new_bool      ("diamond",
                                                            N_("Whether to use a diamond shape light"),
                                                            FALSE,
                                                            NULL),
                                NULL);

	BumpscopePrivate *priv = visual_mem_new0 (BumpscopePrivate, 1);
	visual_plugin_set_private (plugin, priv);

	priv->phongres = 256;
	priv->rcontext = visual_plugin_get_random_context (plugin);
	priv->pal      = visual_palette_new (256);
	priv->pcmbuf   = visual_buffer_new_allocate (512 * sizeof (float));

	return TRUE;
}

static void act_bumpscope_cleanup (VisPluginData *plugin)
{
	BumpscopePrivate *priv = visual_plugin_get_private (plugin);

	__bumpscope_cleanup (priv);

	visual_palette_free (priv->pal);

	visual_buffer_unref (priv->pcmbuf);

	visual_mem_free (priv);
}

static void act_bumpscope_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;
}

static void act_bumpscope_resize (VisPluginData *plugin, int width, int height)
{
	BumpscopePrivate *priv = visual_plugin_get_private (plugin);

	priv->width  = width;
	priv->height = height;

	__bumpscope_cleanup (priv);
	__bumpscope_init (priv);
}

static int act_bumpscope_events (VisPluginData *plugin, VisEventQueue *events)
{
	BumpscopePrivate *priv = visual_plugin_get_private (plugin);
	VisEvent ev;
	VisParam *param;
	VisColor *tmp;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_bumpscope_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				if (ev.event.mousemotion.state == VISUAL_MOUSE_DOWN) {
					priv->light_x = ev.event.mousemotion.x;
					priv->light_y = ev.event.mousemotion.y;
				}

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_has_name (param, "color")) {
					tmp = visual_param_get_value_color (param);
					visual_color_copy (&priv->color, tmp);

					__bumpscope_generate_palette (priv, &priv->color);

				} else if (visual_param_has_name (param, "light size")) {
					priv->phongres = visual_param_get_value_integer (param);

					__bumpscope_cleanup (priv);
					__bumpscope_init (priv);

				} else if (visual_param_has_name (param, "color_cycle")) {
					priv->color_cycle = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "moving_light")) {
					priv->moving_light = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "diamond")) {
					priv->diamond = visual_param_get_value_bool (param);

					__bumpscope_generate_phongdat (priv);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_bumpscope_palette (VisPluginData *plugin)
{
	BumpscopePrivate *priv = visual_plugin_get_private (plugin);

	return priv->pal;
}

static void act_bumpscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	BumpscopePrivate *priv = visual_plugin_get_private (plugin);
	priv->video = video;

	visual_audio_get_sample_mixed_simple (audio, priv->pcmbuf, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	__bumpscope_render_pcm (priv, visual_buffer_get_data (priv->pcmbuf));

	visual_mem_copy (visual_video_get_pixels (video), priv->rgb_buf2, visual_video_get_size (video));

	priv->colorupdate++;

	/* Let's not overload the color selector */
	if (priv->colorupdate > 1)
		priv->colorupdate = 0;

	if (priv->colorchanged == TRUE && priv->colorupdate == 0) {
		/* I couldn't hold myself */
		visual_param_set_value_color (
			visual_param_list_get (
				visual_plugin_get_params (plugin), "color"), &priv->color);
	}
}
