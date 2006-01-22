/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_bumpscope.c,v 1.28 2006-01-22 13:25:23 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <libvisual/libvisual.h>

#include "actor_bumpscope.h"
#include "bump_scope.h"

int act_bumpscope_init (VisPluginData *plugin);
int act_bumpscope_cleanup (VisPluginData *plugin);
int act_bumpscope_requisition (VisPluginData *plugin, int *width, int *height);
int act_bumpscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_bumpscope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_bumpscope_palette (VisPluginData *plugin);
int act_bumpscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = act_bumpscope_requisition,
		.palette = act_bumpscope_palette,
		.render = act_bumpscope_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "bumpscope",
		.name = "Bumpscope plugin",
		.author = N_("Original by: Zinx Verituse <zinx@xmms.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version = "0.0.1",
		.about = N_("Bumpscope visual plugin"),
		.help = N_("This is the libvisual port of the xmms Bumpscope plugin"),
		.license = VISUAL_PLUGIN_LICENSE_GPL,

		.init = act_bumpscope_init,
		.cleanup = act_bumpscope_cleanup,
		.events = act_bumpscope_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_bumpscope_init (VisPluginData *plugin)
{
	BumpscopePrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_COLOR	("color",		122, 204, 255),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("light size",		256),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("color cycle",		TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("moving light",	TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("diamond",		FALSE),
		VISUAL_PARAM_LIST_END
	};

	/* UI vars */
	VisUIWidget *vbox;
	VisUIWidget *hbox;
	VisUIWidget *label1;
	VisUIWidget *color;
	VisUIWidget *separator;
	VisUIWidget *slider;
	VisUIWidget *numeric;
	VisUIWidget *check1;
	VisUIWidget *check2;
	VisUIWidget *check3;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (BumpscopePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);
	priv->phongres = 256;

	priv->rcontext = visual_plugin_get_random_context (plugin);

	visual_palette_allocate_colors (&priv->pal, 256);

	visual_param_container_add_many (paramcontainer, params);

	vbox = visual_ui_box_new (VISUAL_ORIENT_TYPE_VERTICAL);
	hbox = visual_ui_box_new (VISUAL_ORIENT_TYPE_HORIZONTAL);

	label1 = visual_ui_label_new ("Light size:", FALSE);

	color = visual_ui_color_new ();
	visual_ui_widget_set_tooltip (color, _("The color of the light"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (color), visual_param_container_get (paramcontainer, "color"));

	separator = visual_ui_separator_new (VISUAL_ORIENT_TYPE_HORIZONTAL);

	numeric = visual_ui_numeric_new ();
	visual_ui_widget_set_tooltip (numeric, _("The size of the light"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (numeric), visual_param_container_get (paramcontainer, "light size"));
	visual_ui_range_set_min (VISUAL_UI_RANGE (numeric), 8);
	visual_ui_range_set_max (VISUAL_UI_RANGE (numeric), 512);
	visual_ui_range_set_step (VISUAL_UI_RANGE (numeric), 8);
	visual_ui_range_set_precision (VISUAL_UI_RANGE (numeric), 0);

	slider = visual_ui_slider_new (FALSE);
	visual_ui_widget_set_tooltip (slider, _("The size of the light"));
	visual_ui_widget_set_size_request (slider, 200, -1);
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (slider), visual_param_container_get (paramcontainer, "light size"));
	visual_ui_range_set_min (VISUAL_UI_RANGE (slider), 8);
	visual_ui_range_set_max (VISUAL_UI_RANGE (slider), 512);
	visual_ui_range_set_step (VISUAL_UI_RANGE (slider), 8);
	visual_ui_range_set_precision (VISUAL_UI_RANGE (slider), 0);

	check1 = visual_ui_checkbox_new (_("Cycling colors"), TRUE);
	visual_ui_widget_set_tooltip (check1, _("Automatic cycling through colors"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (check1), visual_param_container_get (paramcontainer, "color cycle"));

	check2 = visual_ui_checkbox_new (_("Moving light"), TRUE);
	visual_ui_widget_set_tooltip (check2,
			_("Automatic movement of the light, when disabled it's possible to select it" \
			"using the mouse cursor"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (check2), visual_param_container_get (paramcontainer, "moving light"));

	check3 = visual_ui_checkbox_new (_("Diamond"), TRUE);
	visual_ui_widget_set_tooltip (check3, _("Diamond shaped light"));
	visual_ui_mutator_set_param (VISUAL_UI_MUTATOR (check3), visual_param_container_get (paramcontainer, "diamond"));

	visual_ui_box_pack (VISUAL_UI_BOX (hbox), label1);
	visual_ui_box_pack (VISUAL_UI_BOX (hbox), slider);
	visual_ui_box_pack (VISUAL_UI_BOX (hbox), numeric);

	visual_ui_box_pack (VISUAL_UI_BOX (vbox), color);
	visual_ui_box_pack (VISUAL_UI_BOX (vbox), separator);
	visual_ui_box_pack (VISUAL_UI_BOX (vbox), hbox);
	visual_ui_box_pack (VISUAL_UI_BOX (vbox), check1);
	visual_ui_box_pack (VISUAL_UI_BOX (vbox), check2);
	visual_ui_box_pack (VISUAL_UI_BOX (vbox), check3);

	visual_plugin_set_userinterface (plugin, vbox);

	priv->pcmbuf = visual_buffer_new_allocate (512 * sizeof (float), visual_buffer_destroyer_free);

	return 0;
}

int act_bumpscope_cleanup (VisPluginData *plugin)
{
	BumpscopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
        VisUIWidget *ui;

	ui = visual_plugin_get_userinterface (plugin);
	visual_object_unref (VISUAL_OBJECT (ui));

	__bumpscope_cleanup (priv);

	visual_palette_free_colors (&priv->pal);

	visual_object_unref (VISUAL_OBJECT (priv->pcmbuf));

	visual_mem_free (priv);

	return 0;
}

int act_bumpscope_requisition (VisPluginData *plugin, int *width, int *height)
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

	return 0;
}

int act_bumpscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	BumpscopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_video_set_dimension (video, width, height);

	priv->video = video;

	priv->width = width;
	priv->height = height;

	__bumpscope_cleanup (priv);
	__bumpscope_init (priv);

	return 0;
}

int act_bumpscope_events (VisPluginData *plugin, VisEventQueue *events)
{
	BumpscopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;
	VisColor *tmp;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_bumpscope_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				if (ev.event.mousemotion.state == VISUAL_MOUSE_DOWN) {
					priv->light_x = ev.event.mousemotion.x;
					priv->light_y = ev.event.mousemotion.y;
				}

				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "color")) {
					tmp = visual_param_entry_get_color (param);
					visual_color_copy (&priv->color, tmp);

					__bumpscope_generate_palette (priv, &priv->color);
				} else if (visual_param_entry_is (param, "light size")) {
					priv->phongres = visual_param_entry_get_integer (param);

					__bumpscope_cleanup (priv);
					__bumpscope_init (priv);

				} else if (visual_param_entry_is (param, "color cycle")) {
					priv->color_cycle = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "moving light")) {
					priv->moving_light = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "diamond")) {
					priv->diamond = visual_param_entry_get_integer (param);

					__bumpscope_generate_phongdat (priv);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_bumpscope_palette (VisPluginData *plugin)
{
	BumpscopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	return &priv->pal;
}

int act_bumpscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	BumpscopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	priv->video = video;

	visual_audio_get_sample_mixed (audio, priv->pcmbuf, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

	__bumpscope_render_pcm (priv, visual_buffer_get_data (priv->pcmbuf));

	visual_mem_copy (visual_video_get_pixels (video), priv->rgb_buf2, visual_video_get_size (video));

	priv->colorupdate++;

	/* Let's not overload the color selector */
	if (priv->colorupdate > 1)
		priv->colorupdate = 0;

	if (priv->colorchanged == TRUE && priv->colorupdate == 0) {
		/* I couldn't hold myself */
		visual_param_entry_set_color_by_color (
			visual_param_container_get (
				visual_plugin_get_params (plugin), "color"), &priv->color);
	}

	return 0;
}

