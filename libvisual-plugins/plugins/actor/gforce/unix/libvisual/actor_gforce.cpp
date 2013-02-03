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

#include "config.h"
#include "gettext.h"

#include <libvisual/libvisual.h>

#include "G-Force_Proj.h"
#include "G-Force.h"
#include "EgOSUtils.h"
#include "RectUtils.h"
#include "CEgFileSpec.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define SND_BUF_SIZE 512
#define FFT_BUF_SIZE 256
#define NUMSAMPLES 200
#define SAMPSKIP 1

typedef struct {
	LV::Palette	*pal;
	GForce		*gGF;
} GForcePrivate;

static int         lv_gforce_init        (VisPluginData *plugin);
static void        lv_gforce_cleanup     (VisPluginData *plugin);
static void        lv_gforce_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_gforce_resize      (VisPluginData *plugin, int width, int height);
static int         lv_gforce_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_gforce_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_gforce_palette     (VisPluginData *plugin);

const VisPluginInfo *get_plugin_info ()
{
	static VisActorPlugin actor;

	actor.requisition = lv_gforce_requisition;
	actor.palette     = lv_gforce_palette;
	actor.render      = lv_gforce_render;
	actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT;

	static VisPluginInfo info;

	info.type     = VISUAL_PLUGIN_TYPE_ACTOR;
	info.plugname = "gforce";
	info.name     = "libvisual G-Force plugin";
	info.author   = "Winamp version: Andy O'Meara, Unix port: Boris Gjenero, Libvisual port and cleanups: Dennis Smit <ds@nerds-incorporated.org";
	info.version  = "0.1.0";
	info.about    = N_("Libvisual G-Force plugin");
	info.help     = N_("This plugin is a port of the well known G-Force winamp plugin, based on an old unix port");
	info.license  = "Unknown",

	info.init     = lv_gforce_init;
	info.cleanup  = lv_gforce_cleanup;
	info.events   = lv_gforce_events;
	info.plugin   = &actor;

	return &info;
}

int lv_gforce_init (VisPluginData *plugin)
{
	auto priv = new GForcePrivate;
	visual_mem_set (priv, 0, sizeof (GForcePrivate));

	visual_plugin_set_private (plugin, priv);

	priv->pal = new LV::Palette (256);

	EgOSUtils::Initialize (0);
	ScreenDevice::sMinDepth = 8;

	/* Randomize the seed */
	srand (EgOSUtils::CurTimeMS ());

	priv->gGF = new GForce;

	Rect r;
	SetRect (&r, 0, 0, 64, 64);

	priv->gGF->SetWinPort (0, &r);
	priv->gGF->StoreWinRect ();

    return true;
}

void lv_gforce_cleanup (VisPluginData *plugin)
{
	auto priv = static_cast<GForcePrivate*> (visual_plugin_get_private (plugin));

	if (priv->gGF)
		delete priv->gGF;

	EgOSUtils::Shutdown ();

	delete priv->pal;

	delete priv;
}

void lv_gforce_requisition (VisPluginData *plugin, int *width, int *height)
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
}

void lv_gforce_resize (VisPluginData *plugin, int width, int height)
{
	auto priv = static_cast<GForcePrivate*> (visual_plugin_get_private (plugin));

	Rect r;
	SetRect (&r, 0, 0, width, height);
	priv->gGF->SetWinPort (0, &r);
}

int lv_gforce_events (VisPluginData *plugin, VisEventQueue *events)
{
	auto priv = static_cast<GForcePrivate*> (visual_plugin_get_private (plugin));

	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_gforce_resize (plugin, ev.event.resize.width, ev.event.resize.height);
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

	return true;
}

VisPalette *lv_gforce_palette (VisPluginData *plugin)
{
	auto priv = static_cast<GForcePrivate*> (visual_plugin_get_private (plugin));

	PixPalEntry *GFpal;
	int i;

	GFpal = priv->gGF->GetPalette ();

	for (i = 0; i < 256; i++) {
		priv->pal->colors[i].r = GFpal[i].red;
		priv->pal->colors[i].g = GFpal[i].green;
		priv->pal->colors[i].b = GFpal[i].blue;
	}

	return priv->pal;
}

void lv_gforce_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	auto priv = static_cast<GForcePrivate*> (visual_plugin_get_private (plugin));

	int i;
	long time;
	float gSoundBuf[SND_BUF_SIZE];
	float gFFTBuf[FFT_BUF_SIZE];

	LV::BufferPtr pcmbuf = LV::Buffer::create ();
	LV::BufferPtr freqbuf = LV::Buffer::create ();

	pcmbuf->set (gSoundBuf, sizeof (gSoundBuf));
	audio->get_sample_mixed_simple (pcmbuf, 2, VISUAL_AUDIO_CHANNEL_LEFT, VISUAL_AUDIO_CHANNEL_RIGHT);

	freqbuf->set (gFFTBuf, sizeof (gFFTBuf));
	audio->get_spectrum_for_sample (freqbuf, pcmbuf, true, 3.0);

	// Increase volume
	for (i = 0; i < SND_BUF_SIZE; i++)
		gSoundBuf[i] *= 32768;

	// Set the video buffer
	priv->gGF->SetOutVideoBuffer ((unsigned char *) video->get_pixels ());

	time = EgOSUtils::CurTimeMS ();
	priv->gGF->RecordSample (time, gSoundBuf, .000043, NUMSAMPLES, gFFTBuf, 1, FFT_BUF_SIZE);
}
