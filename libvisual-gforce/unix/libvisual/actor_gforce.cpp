#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

extern "C" int lv_gforce_init (VisActorPlugin *plugin);
extern "C" int lv_gforce_cleanup (VisActorPlugin *plugin);
extern "C" int lv_gforce_requisition (VisActorPlugin *plugin, int *width, int *height);
extern "C" int lv_gforce_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
extern "C" int lv_gforce_events (VisActorPlugin *plugin, VisEventQueue *events);
extern "C" VisPalette *lv_gforce_palette (VisActorPlugin *plugin);
extern "C" int lv_gforce_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

extern "C" LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *gforce;
	GForcePrivate *priv;

	plugin = visual_plugin_new ();
	gforce = visual_plugin_actor_new ();

	gforce->name = "G-Force";
	gforce->info = visual_plugin_info_new (
			"libvisual G-Force plugin",
			"Winamp version: Andy O'Meara, Unix port: Boris Gjenero, Libvisual port and cleanups: Dennis Smit <ds@nerds-incorporated.org>",
			"0.1.0",
			"The Libvisual G-Force plugin",
			"This plugin is a port of the well known G-Force winamp plugin, based on an old unix port.");

	gforce->init =		lv_gforce_init;
	gforce->cleanup =	lv_gforce_cleanup;
	gforce->requisition =	lv_gforce_requisition;
	gforce->events =	lv_gforce_events;
	gforce->palette =	lv_gforce_palette;
	gforce->render =	lv_gforce_render;

	gforce->depth = VISUAL_VIDEO_DEPTH_8BIT;

	priv = new GForcePrivate;
	memset (priv, 0, sizeof (GForcePrivate));

	gforce->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = gforce;

	return plugin;
}

extern "C" int lv_gforce_init (VisActorPlugin *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
	Rect r;

	visual_palette_allocate_colors (&priv->pal, 256);

	EgOSUtils::Initialize (0);
	ScreenDevice::sMinDepth = 8;

	priv->gGF = new GForce;

	SetRect (&r, 0, 0, 64, 64);

	priv->gGF->SetWinPort (0, &r);
	priv->gGF->StoreWinRect ();
	
	return 0;
}

extern "C" int lv_gforce_cleanup (VisActorPlugin *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;

	if (priv->gGF)
		delete priv->gGF;
		
	EgOSUtils::Shutdown ();

	visual_palette_free_colors (&priv->pal);

	delete priv;

	return 0;
}

extern "C" int lv_gforce_requisition (VisActorPlugin *plugin, int *width, int *height)
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

extern "C" int lv_gforce_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
	Rect r;
	
	visual_video_set_dimension (video, width, height);

	SetRect (&r, 0, 0, width, height);
	priv->gGF->SetWinPort (0, &r);

	return 0;
}

extern "C" int lv_gforce_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_gforce_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				break;

			case VISUAL_EVENT_MOUSEBUTTONDOWN:
			case VISUAL_EVENT_MOUSEBUTTONUP:
				break;
		
			case VISUAL_EVENT_KEYDOWN:
				priv->gGF->HandleKey (ev.keyboard.keysym.sym);
				break;

			default:
				break;
		}
	}

	return 0;
}

extern "C" VisPalette *lv_gforce_palette (VisActorPlugin *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
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

extern "C" int lv_gforce_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
	int i, j = 0, ns;
	short int sbuf[NUMSAMPLES];
	long time;
	float gSoundBuf[SND_BUF_SIZE];	
	float gFFTBuf[FFT_BUF_SIZE];
	
	// Make the sample ready for G-Force usage
	for (i = 0; i < NUMSAMPLES; i++) {
		sbuf[i] = audio->pcm[2][j];

#ifdef SAMPSKIP
		j += 1 + SAMPSKIP;
#else
		j++;
#endif
	}

	for (ns = 0; ns < NUMSAMPLES; ns++) {
		gSoundBuf[ns] = sbuf[ns];
	}

	for (ns = 0; ns < FFT_BUF_SIZE; ns++) {
		gFFTBuf[ns] = (float) audio->freqnorm[2][ns] / 500.0000;
	}
	
	// Set the video buffer
	priv->gGF->SetOutVideoBuffer ((unsigned char *) video->screenbuffer);

	time = EgOSUtils::CurTimeMS ();
	priv->gGF->RecordSample (time, gSoundBuf, .000043, NUMSAMPLES, gFFTBuf, 1, FFT_BUF_SIZE);
	
	return 0;
}

