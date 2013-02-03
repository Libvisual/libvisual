/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2000, 2001 Richard Ashburn <richard.asbury@btinternet.com>
 *
 * Authors: Richard Ashburn <richard.asbury@btinternet.com>
 * 	    Jean-Christophe Hoelt <jeko@ios-software.com>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_corona.cpp,v 1.17 2006/01/27 20:19:14 synap Exp $
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
#include "corona.h"
#include "palette.h"
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

namespace {

  struct CoronaPrivate {
	  LV::Time       oldtime;
	  LV::Palette    pal;
	  Corona*        corona; /* The corona internal private struct */
	  PaletteCycler* pcyl;
  	  TimedLevel     tl;
  };

  int         lv_corona_init        (VisPluginData *plugin);
  void        lv_corona_cleanup     (VisPluginData *plugin);
  void        lv_corona_requisition (VisPluginData *plugin, int *width, int *height);
  void        lv_corona_resize      (VisPluginData *plugin, int width, int height);
  int         lv_corona_events      (VisPluginData *plugin, VisEventQueue *events);
  void        lv_corona_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
  VisPalette *lv_corona_palette     (VisPluginData *plugin);

  const int PALETTEDATA[][NB_PALETTES] = {
      { 4, 85, 0xff0000, 170, 0xffff00, 224, 0xffffff, 240, 0xf8f8e0 },     // 0. fire
      { 3, 85, 0xff, 170, 0xffff, 256, 0xffffff },                          // 1. ice
      { 1, 256, 0xffffff },                                                 // 2. white
      { 2, 128, 0xffffff, 256, 0xff },                                      // 3. white to blue
      { 2, 128, 0x808080, 256, 0xff },                                      // 4. gray to blue
      { 2, 128, 0xffff80, 256, 0xff },                                      // 5. yellow to blue
      { 2, 128, 0xffffff, 256, 0xff0000 },                                  // 6. white to red
      { 2, 128, 0xff, 256, 0xff0000 },                                      // 7. blue to red
      { 2, 128, 0xff0000, 256, 0xff },                                      // 8. red to blue
      { 2, 128, 0xff, 256, 0xffffff },                                      // 9. blue to white
      { 2, 128, 0xffffff, 256, 0 },                                         // 10. white to black
      { 2, 128, 0xff, 256, 0 },                                             // 11. blue to black
      { 2, 128, 0xff0000, 256, 0 },                                         // 12. red to black
      { 8, 32, 0xff, 64, 0, 96, 0xff, 128, 0, 192, 0xff, 256, 0 },          // 13. blue lines on black
      { 8, 32, 0xffffff, 64, 0, 96, 0xffffff, 128, 0, 192, 0xffffff, 256, 0 }, // 14. white lines on black
      { 3, 0, 0x000020, 128, 0x40, 256, 0xffffff },                         // 15. night sky
      { 2, 128, 0xff8080, 256, 0xff },                                      // 16. pink to blue
      { 2, 0, 0xffffff, 256, 0 },                                           // 17. black on white
      { 3, 0, 0xffffff, 128, 0, 256, 0xffffff },                            // 18. black to white on white
      { 3, 0, 0xffffff, 128, 0, 256, 0xc0c0ff },                            // 19. heavenly blue
      { 4, 120, 0x78, 128, 0xff0000, 136, 0x88, 256, 0xff },                // 20. blue, red line
      { 2, 0, 0xffe0a0, 256, 0 },                                           // 21. twilight yellow
      { 3, 0, 0xc0c0ff, 128, 0xa0a0a0, 256, 0xffffff }                      // 22. clouds
  };
}

const VisPluginInfo *get_plugin_info ()
{
	static VisActorPlugin actor;
	static VisPluginInfo info;

	actor.requisition = lv_corona_requisition;
	actor.palette     = lv_corona_palette;
	actor.render      = lv_corona_render;
	actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT;

	info.type    = VISUAL_PLUGIN_TYPE_ACTOR;

	info.plugname = "corona";
	info.name     = "libvisual corona plugin";
	info.author   = "Jean-Christophe Hoelt <jeko@ios-software.com> and Richard Ashburn <richard.asbury@btinternet.com>";
	info.version  = "0.1";
	info.about    = N_("Libvisual corona plugin");
	info.help     = N_("This plugin adds support for the neat corona plugin");
	info.license  = VISUAL_PLUGIN_LICENSE_GPL,

	info.init     = lv_corona_init;
	info.cleanup  = lv_corona_cleanup;
	info.events   = lv_corona_events;
	info.plugin   = &actor;

	return &info;
}


namespace {

int lv_corona_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	auto priv = new CoronaPrivate;
	visual_mem_set (priv, 0, sizeof (CoronaPrivate));

	visual_plugin_set_private (plugin, priv);

	priv->corona       = new Corona();
	priv->pcyl         = new PaletteCycler(PALETTEDATA, NB_PALETTES);
	priv->tl.timeStamp = 0;
	priv->tl.lastbeat  = 0;
	priv->tl.state     = normal_state;

	priv->oldtime = LV::Time::now ();

	priv->pal.colors.resize (256);

	return true;
}

void lv_corona_cleanup (VisPluginData *plugin)
{
	auto priv = static_cast<CoronaPrivate*> (visual_plugin_get_private (plugin));

	delete priv->corona;
	delete priv->pcyl;
	delete priv;
}

void lv_corona_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;
}

void lv_corona_resize (VisPluginData *plugin, int width, int height)
{
	auto priv = static_cast<CoronaPrivate*> (visual_plugin_get_private (plugin));

	delete priv->corona;
	delete priv->pcyl;

	priv->corona       = new Corona();
	priv->pcyl         = new PaletteCycler(PALETTEDATA, NB_PALETTES);
	priv->tl.timeStamp = 0;
	priv->tl.lastbeat  = 0;
	priv->tl.state     = normal_state;

	priv->corona->setUpSurface(width, height);
}

int lv_corona_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_corona_resize (plugin, ev.event.resize.width, ev.event.resize.height);

				break;

			default:
				break;
		}
	}

	return true;
}

VisPalette *lv_corona_palette (VisPluginData *plugin)
{
	auto priv = static_cast<CoronaPrivate*> (visual_plugin_get_private (plugin));

	priv->pcyl->updateVisPalette (&priv->pal);

	return &priv->pal;
}

void lv_corona_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	auto priv = static_cast<CoronaPrivate*> (visual_plugin_get_private (plugin));

	float freq[2][256];
	float pcm[256];
	short freqdata[2][512]; // FIXME Move to floats
	unsigned long timemilli = 0;

	LV::BufferPtr buffer = LV::Buffer::create ();
	LV::BufferPtr pcmb   = LV::Buffer::create ();

	pcmb->set (pcm, sizeof (pcm));

	audio->get_sample (pcmb, VISUAL_AUDIO_CHANNEL_LEFT);
	buffer->set (freq[0], sizeof (freq[0]));
	audio->get_spectrum_for_sample (buffer, pcmb, true);

	audio->get_sample (pcmb, VISUAL_AUDIO_CHANNEL_RIGHT);
	buffer->set (freq[1], sizeof (freq[1]));
	audio->get_spectrum_for_sample (buffer, pcmb, true);

	for (unsigned int i = 0; i < 256; ++i) {
		freqdata[0][i*2]   = freq[0][i];
		freqdata[1][i*2]   = freq[1][i];
		freqdata[0][i*2+1] = freq[0][i];
		freqdata[1][i*2+1] = freq[1][i];
	}

	LV::Time curtime  = LV::Time::now ();
	LV::Time difftime = curtime - priv->oldtime;

	timemilli = difftime.to_msecs ();

	priv->tl.timeStamp += timemilli;
	priv->oldtime = curtime;

	for (unsigned int i = 0; i < 512; ++i) {
		priv->tl.frequency[0][i] = freqdata[0][i] * 32768;
		priv->tl.frequency[1][i] = freqdata[1][i] * 32768;
	}

	priv->corona->update(&priv->tl); // Update Corona
	priv->pcyl->update(&priv->tl);    // Update Palette Cycler

	LV::VideoPtr vidcorona = LV::Video::wrap (priv->corona->getSurface (),
	                                          false,
	                                          video->get_width (),
	                                          video->get_height (),
	                                          VISUAL_VIDEO_DEPTH_8BIT);
	video->mirror (vidcorona, VISUAL_VIDEO_MIRROR_Y);
}

} // anonymous namespace

