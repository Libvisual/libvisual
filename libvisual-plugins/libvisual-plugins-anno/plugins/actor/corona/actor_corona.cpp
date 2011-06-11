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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <time.h>
#include <sys/time.h>

#include <libvisual/libvisual.h>

#include "corona.h"
#include "palette.h"

static const int PALETTEDATA[][NB_PALETTES] = {
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

typedef struct {
	VisTime		 oldtime;
	VisPalette	 pal;
	Corona		*corona; /* The corona internal private struct */
	PaletteCycler	*pcyl;
	TimedLevel	 tl;
} CoronaPrivate;

extern "C" int lv_corona_init (VisPluginData *plugin);
extern "C" int lv_corona_cleanup (VisPluginData *plugin);
extern "C" int lv_corona_requisition (VisPluginData *plugin, int *width, int *height);
extern "C" int lv_corona_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
extern "C" int lv_corona_events (VisPluginData *plugin, VisEventQueue *events);
extern "C" VisPalette *lv_corona_palette (VisPluginData *plugin);
extern "C" int lv_corona_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

extern "C" const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[1];
	static VisPluginInfo info[1];

	actor[0].requisition = lv_corona_requisition;
	actor[0].palette = lv_corona_palette;
	actor[0].render  = lv_corona_render;
	actor[0].vidoptions.depth   = VISUAL_VIDEO_DEPTH_8BIT;

	info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;

	info[0].plugname = "corona";
	info[0].name     = "libvisual corona plugin";
	info[0].author   = "Jean-Christophe Hoelt <jeko@ios-software.com> and Richard Ashburn <richard.asbury@btinternet.com>";
	info[0].version  = "0.1";
	info[0].about    = N_("Libvisual corona plugin");
	info[0].help     = N_("This plugin adds support for the neat corona plugin");
	info[0].license = VISUAL_PLUGIN_LICENSE_GPL,

	info[0].init     = lv_corona_init;
	info[0].cleanup  = lv_corona_cleanup;
	info[0].events   = lv_corona_events;

	info[0].plugin   = VISUAL_OBJECT (&actor[0]);

	*count = sizeof (info) / sizeof (*info);

	return info;
}

extern "C" int lv_corona_init (VisPluginData *plugin)
{
	CoronaPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = new CoronaPrivate;
	visual_mem_set (priv, 0, sizeof (CoronaPrivate));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->corona       = new Corona();
	priv->pcyl         = new PaletteCycler(PALETTEDATA, NB_PALETTES);
	priv->tl.timeStamp = 0;
	priv->tl.lastbeat  = 0;
	priv->tl.state     = normal_state;

	visual_time_get (&priv->oldtime);

	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

extern "C" int lv_corona_cleanup (VisPluginData *plugin)
{
	CoronaPrivate *priv = (CoronaPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	if (priv->corona != NULL)
		delete priv->corona;

	if (priv->pcyl != NULL)
		delete priv->pcyl;

	delete priv;

	return 0;
}

extern "C" int lv_corona_requisition (VisPluginData *plugin, int *width, int *height)
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

	return 0;
}

extern "C" int lv_corona_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	CoronaPrivate *priv = (CoronaPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_video_set_dimension (video, width, height);

	delete priv->corona;
	delete priv->pcyl;

	priv->corona       = new Corona();
	priv->pcyl         = new PaletteCycler(PALETTEDATA, NB_PALETTES);
	priv->tl.timeStamp = 0;
	priv->tl.lastbeat  = 0;
	priv->tl.state     = normal_state;

	priv->corona->setUpSurface(width, height);

	return 0;
}

extern "C" int lv_corona_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_corona_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);

				break;

			default:
				break;
		}
	}

	return 0;
}

extern "C" VisPalette *lv_corona_palette (VisPluginData *plugin)
{
	CoronaPrivate *priv = (CoronaPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));

	priv->pcyl->updateVisPalette (&priv->pal);

	return &priv->pal;
}

extern "C" int lv_corona_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	CoronaPrivate *priv = (CoronaPrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer buffer;
	VisBuffer pcmb;
	float freq[2][256];
	float pcm[256];
	VisTime curtime;
	VisTime difftime;
	VisVideo vidcorona;
	short freqdata[2][512]; // FIXME Move to floats
	unsigned long timemilli = 0;
	int i;

	visual_buffer_set_data_pair (&pcmb, pcm, sizeof (pcm));

	visual_audio_get_sample (audio, &pcmb, VISUAL_AUDIO_CHANNEL_LEFT);
	visual_buffer_set_data_pair (&buffer, freq[0], sizeof (freq[0]));
	visual_audio_get_spectrum_for_sample (&buffer, &pcmb, TRUE);

	visual_audio_get_sample (audio, &pcmb, VISUAL_AUDIO_CHANNEL_RIGHT);
	visual_buffer_set_data_pair (&buffer, freq[1], sizeof (freq[1]));
	visual_audio_get_spectrum_for_sample (&buffer, &pcmb, TRUE);

	for (i = 0; i < 256; ++i) {
		freqdata[0][i*2]   = freq[0][i];
		freqdata[1][i*2]   = freq[1][i];
		freqdata[0][i*2+1] = freq[0][i];
		freqdata[1][i*2+1] = freq[1][i];
	}

	visual_time_get (&curtime);

	visual_time_difference (&difftime, &priv->oldtime, &curtime);

	timemilli = difftime.tv_sec * 1000 + difftime.tv_usec / 1000;

	priv->tl.timeStamp += timemilli;

	visual_time_copy (&priv->oldtime, &curtime);

	for (i = 0; i < 512; ++i) {
		priv->tl.frequency[0][i] = freqdata[0][i] * 32768;
		priv->tl.frequency[1][i] = freqdata[1][i] * 32768;
	}

	priv->corona->update(&priv->tl); // Update Corona
	priv->pcyl->update(&priv->tl);    // Update Palette Cycler

	visual_video_init (&vidcorona);
	visual_video_set_depth (&vidcorona, VISUAL_VIDEO_DEPTH_8BIT);
	visual_video_set_dimension (&vidcorona, video->width, video->height);
	visual_video_set_buffer (&vidcorona, priv->corona->getSurface());

	visual_video_mirror (video, &vidcorona, VISUAL_VIDEO_MIRROR_Y);

	return 0;
}

