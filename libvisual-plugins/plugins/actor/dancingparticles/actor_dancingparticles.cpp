/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_dancingparticles.cpp,v 1.12 2006-09-19 18:41:41 synap Exp $
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
#include <math.h>
#include <gettext.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "gl.h"
#include "fastmath.h"
#include "etoile.h"

#include "actor_dancingparticles.h"

unsigned int fast_sqrt_table[0x10000];
int titleHasChanged = 0;
char *curtitle = "Moeders";

extern "C" int lv_dancingparticles_init (VisPluginData *plugin);
extern "C" int lv_dancingparticles_cleanup (VisPluginData *plugin);
extern "C" int lv_dancingparticles_requisition (VisPluginData *plugin, int *width, int *height);
extern "C" int lv_dancingparticles_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
extern "C" int lv_dancingparticles_events (VisPluginData *plugin, VisEventQueue *events);
extern "C" VisPalette *lv_dancingparticles_palette (VisPluginData *plugin);
extern "C" int lv_dancingparticles_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

/* Main plugin stuff */
extern "C" const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[1];
	static VisPluginInfo info[1];

	actor[0].requisition = lv_dancingparticles_requisition;
	actor[0].palette = lv_dancingparticles_palette;
	actor[0].render = lv_dancingparticles_render;
	actor[0].vidoptions.depth = VISUAL_VIDEO_DEPTH_GL;

	info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;

	info[0].plugname = "dancingparticles";
	info[0].name = "libvisual Dancing Particles plugin";
	info[0].author = N_("Original by: Pierre Tardy <tardyp@free.fr>, Port by: Dennis Smit <ds@nerds-incorporated.org>");
	info[0].version = "0.1";
	info[0].about = N_("Libvisual Dancing Particles plugin");
	info[0].help =  N_("This plugin shows dancing particles");
	info[0].license = VISUAL_PLUGIN_LICENSE_GPL,

	info[0].init = lv_dancingparticles_init;
	info[0].cleanup = lv_dancingparticles_cleanup;
	info[0].events = lv_dancingparticles_events;

	info[0].plugin = VISUAL_OBJECT (&actor[0]);

	*count = sizeof (info) / sizeof (*info);

	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTRIBUTE_OPTIONS_GL_ENTRY(actor[0].vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return info;
}

extern "C" int lv_dancingparticles_init (VisPluginData *plugin)
{
	DancingParticlesPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_INTEGER ("transparant bars", FALSE),
		VISUAL_PARAM_LIST_END
	};

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (DancingParticlesPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_param_container_add_many (paramcontainer, params);

	build_sqrt_table ();

	init_gl ();

	return 0;
}

extern "C" int lv_dancingparticles_cleanup (VisPluginData *plugin)
{
	DancingParticlesPrivate *priv = (DancingParticlesPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	VisUIWidget *ui;

	/* Destroy the VisUI tree */
	ui = visual_plugin_get_userinterface (plugin);
	visual_object_unref (VISUAL_OBJECT (ui));

	visual_mem_free (priv);

	return 0;
}

extern "C" int lv_dancingparticles_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;

	return 0;
}

extern "C" int lv_dancingparticles_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	glViewport(0, 0, width, height);

	build_sqrt_table ();

	return 0;
}

extern "C" int lv_dancingparticles_events (VisPluginData *plugin, VisEventQueue *events)
{
	DancingParticlesPrivate *priv = (DancingParticlesPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dancingparticles_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = static_cast<VisParamEntry *> (ev.event.param.param);

				if (visual_param_entry_is (param, "transparant bars")) {
					priv->transparant = visual_param_entry_get_integer (param);

					if (priv->transparant == FALSE)
						glDisable (GL_BLEND);
					else
						glEnable (GL_BLEND);
				}

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

extern "C" VisPalette *lv_dancingparticles_palette (VisPluginData *plugin)
{
	return 0;
}

extern "C" int lv_dancingparticles_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	VisBuffer fbuf;
	float freq[3][256];
	DancingParticlesPrivate *priv = (DancingParticlesPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	visual_buffer_set_data_pair (&fbuf, freq[0], sizeof(freq[0]));
	visual_audio_get_spectrum (audio, &fbuf, 256, VISUAL_AUDIO_CHANNEL_LEFT, FALSE);

	visual_buffer_set_data_pair (&fbuf, freq[1], sizeof(freq[1]));
	visual_audio_get_spectrum (audio, &fbuf, 256, VISUAL_AUDIO_CHANNEL_RIGHT, FALSE);

	for (i = 0; i < sizeof(freq[2]); i++)
		freq[2][i] = (freq[0][i] + freq[1][i]) / 2;

	/* FIXME on title change, do something */
	dp_render_freq (freq);
//	update_playlist_info ();
	etoileLoop ();
	draw_gl ();

	return 0;
}

