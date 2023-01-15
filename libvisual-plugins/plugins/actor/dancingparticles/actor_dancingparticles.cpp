/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
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
#include "actor_dancingparticles.h"
#include "gl.h"
#include "fastmath.h"
#include "etoile.h"
#include "font.h"
#include <cmath>

#ifdef USE_OPENGL_ES
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif

VISUAL_PLUGIN_API_VERSION_VALIDATOR

namespace {

  int         lv_dancingparticles_init        (VisPluginData *plugin);
  void        lv_dancingparticles_cleanup     (VisPluginData *plugin);
  void        lv_dancingparticles_requisition (VisPluginData *plugin, int *width, int *height);
  void        lv_dancingparticles_resize      (VisPluginData *plugin, int width, int height);
  int         lv_dancingparticles_events      (VisPluginData *plugin, VisEventQueue *events);
  void        lv_dancingparticles_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
  VisPalette *lv_dancingparticles_palette     (VisPluginData *plugin);
}

unsigned int fast_sqrt_table[0x10000];
int titleHasChanged = false;
const char *curtitle = "Moeders";

/* Main plugin stuff */
const VisPluginInfo *get_plugin_info ()
{
	static VisActorPlugin actor;

	actor.requisition = lv_dancingparticles_requisition;
	actor.palette     = lv_dancingparticles_palette;
	actor.render      = lv_dancingparticles_render;
	actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL;

	static VisPluginInfo info;

	info.type     = VISUAL_PLUGIN_TYPE_ACTOR;
	info.plugname = "dancingparticles";
	info.name     = "libvisual Dancing Particles plugin";
	info.author   = N_("Original by: Pierre Tardy <tardyp@free.fr>, Port by: Dennis Smit <ds@nerds-incorporated.org>");
	info.version  = "0.1";
	info.about    = N_("Libvisual Dancing Particles plugin");
	info.help     = N_("This plugin shows dancing particles");
	info.license  = VISUAL_PLUGIN_LICENSE_GPL,

	info.init     = lv_dancingparticles_init;
	info.cleanup  = lv_dancingparticles_cleanup;
	info.events   = lv_dancingparticles_events;
	info.plugin   = &actor;

	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return &info;
}

namespace {

int lv_dancingparticles_init (VisPluginData *plugin)
{
#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    auto priv = visual_mem_new0 (DancingParticlesPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    VisParamList *params = visual_plugin_get_params (plugin);
    visual_param_list_add_many (params,
                                visual_param_new_bool ("transparent_bars",
                                                       N_("Transparent bars"),
                                                       FALSE,
                                                       NULL),
                                NULL);

    build_sqrt_table ();

    initFontRasterizer();

    init_gl ();

    return true;
}

void lv_dancingparticles_cleanup (VisPluginData *plugin)
{
	auto priv = static_cast<DancingParticlesPrivate*> (visual_plugin_get_private (plugin));

	destroyFontRasterizer();

	visual_mem_free (priv);
}

void lv_dancingparticles_requisition (VisPluginData *plugin, int *width, int *height)
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
}

void lv_dancingparticles_resize (VisPluginData *plugin, int width, int height)
{
	glViewport(0, 0, width, height);

	build_sqrt_table ();
}

int lv_dancingparticles_events (VisPluginData *plugin, VisEventQueue *events)
{
	auto priv = static_cast<DancingParticlesPrivate*> (visual_plugin_get_private (plugin));

	VisEvent ev;
	VisParam *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dancingparticles_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = static_cast<VisParam *> (ev.event.param.param);

				if (visual_param_has_name (param, "transparent_bars")) {
					priv->transparent = visual_param_get_value_bool (param);

					if (priv->transparent == FALSE)
						glDisable (GL_BLEND);
					else
						glEnable (GL_BLEND);
				}

			default: /* to avoid warnings */
				break;
		}
	}

	return true;
}

VisPalette *lv_dancingparticles_palette (VisPluginData *plugin)
{
	return nullptr;
}

void lv_dancingparticles_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	const unsigned int size = 256;

	float freq[3][size];

	LV::BufferPtr fbuf = LV::Buffer::create ();
	fbuf->set (freq[0], size * sizeof(float));
	audio->get_spectrum (fbuf, size, VISUAL_AUDIO_CHANNEL_LEFT, false);

	fbuf->set (freq[1], size * sizeof(float));
	audio->get_spectrum (fbuf, size, VISUAL_AUDIO_CHANNEL_RIGHT, false);

	for (unsigned int i = 0; i < size; i++)
		freq[2][i] = (freq[0][i] + freq[1][i]) / 2;

	/* FIXME on title change, do something */
	dp_render_freq (freq);
//	update_playlist_info ();
	etoileLoop ();
	draw_gl ();
}

} // anonymous namespace
