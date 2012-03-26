/* Libvisual-gforce - GForce interface plugin for libvisual
 *
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_gforce.cpp,v 1.11 2006/01/27 20:19:13 synap Exp $
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

/*
 * Note that the above license is only for the glue layer between G-Force
 * and libvisual, the license around G-Force remains very unclear.
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

#include "G-Force_Proj.h"

#include "G-Force.h"
#include "EgOSUtils.h"
#include "RectUtils.h"
#include "CEgFileSpec.h"

#define SND_BUF_SIZE 550
// Not doing 256, because the end of the fft buffer is kinda jumpy for some reason
#define FFT_BUF_SIZE 180
#define NUMSAMPLES 200
#define SAMPSKIP 1

typedef struct {
	VisPalette	pal;
	GForce		*gGF;
} GForcePrivate;

VISUAL_C_LINKAGE int lv_gforce_init (VisPluginData *plugin);
VISUAL_C_LINKAGE int lv_gforce_cleanup (VisPluginData *plugin);
VISUAL_C_LINKAGE int lv_gforce_requisition (VisPluginData *plugin, int *width, int *height);
VISUAL_C_LINKAGE int lv_gforce_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
VISUAL_C_LINKAGE int lv_gforce_events (VisPluginData *plugin, VisEventQueue *events);
VISUAL_C_LINKAGE VisPalette *lv_gforce_palette (VisPluginData *plugin);
VISUAL_C_LINKAGE int lv_gforce_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

VISUAL_C_LINKAGE
const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[1];
	static VisPluginInfo info[1];

	actor[0].requisition = lv_gforce_requisition;
	actor[0].palette = lv_gforce_palette;
	actor[0].render = lv_gforce_render;
	actor[0].vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT;

	info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;

	info[0].plugname = "gforce";
	info[0].name = "libvisual G-Force plugin";
	info[0].author = "Winamp version: Andy O'Meara, Unix port: Boris Gjenero, Libvisual port and cleanups: Dennis Smit <ds@nerds-incorporated.org";
	info[0].version = "0.1.0";
	info[0].about = N_("Libvisual G-Force plugin");
	info[0].help = N_("This plugin is a port of the well known G-Force winamp plugin, based on an old unix port");
	info[0].license = "Unknown",

	info[0].init = lv_gforce_init;
	info[0].cleanup = lv_gforce_cleanup;
	info[0].events = lv_gforce_events;

	info[0].plugin = VISUAL_OBJECT (&actor[0]);

	*count = sizeof (info) / sizeof (*info);

	return (const VisPluginInfo *) info;
}

VISUAL_C_LINKAGE
int lv_gforce_init (VisPluginData *plugin)
{
	GForcePrivate *priv;
	Rect r;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = new GForcePrivate;
	visual_mem_set (priv, 0, sizeof (GForcePrivate));

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	EgOSUtils::Initialize (0);
	ScreenDevice::sMinDepth = 8;

	/* Randomize the seed */
	srand (EgOSUtils::CurTimeMS ());


	priv->gGF = new GForce;

	SetRect (&r, 0, 0, 64, 64);

	priv->gGF->SetWinPort (0, &r);
	priv->gGF->StoreWinRect ();

	return 0;
}

VISUAL_C_LINKAGE
int lv_gforce_cleanup (VisPluginData *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));

	if (priv->gGF)
		delete priv->gGF;

	EgOSUtils::Shutdown ();

	visual_palette_free_colors (&priv->pal);

	delete priv;

	return 0;
}

VISUAL_C_LINKAGE
int lv_gforce_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

VISUAL_C_LINKAGE
int lv_gforce_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	GForcePrivate *priv = (GForcePrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	Rect r;

	visual_video_set_dimension (video, width, height);

	SetRect (&r, 0, 0, width, height);
	priv->gGF->SetWinPort (0, &r);

	return 0;
}

VISUAL_C_LINKAGE
int lv_gforce_events (VisPluginData *plugin, VisEventQueue *events)
{
	GForcePrivate *priv = (GForcePrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_gforce_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				break;

			case VISUAL_EVENT_MOUSEBUTTONDOWN:
			case VISUAL_EVENT_MOUSEBUTTONUP:
				break;

			case VISUAL_EVENT_KEYDOWN:
				priv->gGF->HandleKey (ev.event.keyboard.keysym.sym);
				break;

			default:
				break;
		}
	}

	return 0;
}

VISUAL_C_LINKAGE
VisPalette *lv_gforce_palette (VisPluginData *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	PixPalEntry *GFpal;
	int i;

	GFpal = priv->gGF->GetPalette ();

	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = GFpal[i].red;
		priv->pal.colors[i].g = GFpal[i].green;
		priv->pal.colors[i].b = GFpal[i].blue;
	}

	return &priv->pal;
}

VISUAL_C_LINKAGE
int lv_gforce_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	GForcePrivate *priv = (GForcePrivate *) visual_object_get_private (VISUAL_OBJECT (plugin));
	VisBuffer pcmbuf;
	VisBuffer freqbuf;
	int i;
	long time;
	float gSoundBuf[SND_BUF_SIZE];
	float gFFTBuf[FFT_BUF_SIZE];

	visual_buffer_set_data_pair (&pcmbuf, gSoundBuf, sizeof (gSoundBuf));
	visual_audio_get_sample_mixed_simple (audio, &pcmbuf, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_buffer_set_data_pair (&freqbuf, gFFTBuf, sizeof (gFFTBuf));

	visual_audio_get_spectrum_for_sample_multiplied (&freqbuf, &pcmbuf, TRUE, 3.0);

	// Increase volume
	for (i = 0; i < SND_BUF_SIZE; i++)
		gSoundBuf[i] *= 32768;

	// Set the video buffer
	priv->gGF->SetOutVideoBuffer ((unsigned char *) visual_video_get_pixels (video));

	time = EgOSUtils::CurTimeMS ();
	priv->gGF->RecordSample (time, gSoundBuf, .000043, NUMSAMPLES, gFFTBuf, 1, FFT_BUF_SIZE);

	return 0;
}

