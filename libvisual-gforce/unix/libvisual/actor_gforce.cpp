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

extern "C" int lv_gforce_init (VisPluginData *plugin);
extern "C" int lv_gforce_cleanup (VisPluginData *plugin);
extern "C" int lv_gforce_requisition (VisPluginData *plugin, int *width, int *height);
extern "C" int lv_gforce_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
extern "C" int lv_gforce_events (VisPluginData *plugin, VisEventQueue *events);
extern "C" VisPalette *lv_gforce_palette (VisPluginData *plugin);
extern "C" int lv_gforce_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

extern "C" const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[1];
	static VisPluginInfo info[1];

	actor[0].requisition = lv_gforce_requisition;
	actor[0].palette = lv_gforce_palette;
	actor[0].render = lv_gforce_render;
	actor[0].depth = VISUAL_VIDEO_DEPTH_8BIT;

	info[0].struct_size = sizeof (VisPluginInfo);
	info[0].api_version = VISUAL_PLUGIN_API_VERSION;
	info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;

	info[0].plugname = "G-Force";
	info[0].name = "libvisual G-Force plugin";
	info[0].author = "Winamp version: Andy O'Meara, Unix port: Boris Gjenero, Libvisual port and cleanups: Dennis Smit <ds@nerds-incorporated.org";
	info[0].version = "0.1.0";
	info[0].about = "The Libvisual G-Force plugin";
	info[0].help = "This plugin is a port of the well known G-Force winamp plugin, based on an old unix port.";

	info[0].init = lv_gforce_init;
	info[0].cleanup = lv_gforce_cleanup;
	info[0].events = lv_gforce_events;

	info[0].plugin = (void *) &actor[0];

	*count = sizeof (info) / sizeof (*info);

	return (const VisPluginInfo *) info;
}

extern "C" int lv_gforce_init (VisPluginData *plugin)
{
	GForcePrivate *priv;
	Rect r;

	priv = new GForcePrivate;
	memset (priv, 0, sizeof (GForcePrivate));
	plugin->priv = priv;

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

extern "C" int lv_gforce_cleanup (VisPluginData *plugin)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;

	if (priv->gGF)
		delete priv->gGF;
		
	EgOSUtils::Shutdown ();

	visual_palette_free_colors (&priv->pal);

	delete priv;

	return 0;
}

extern "C" int lv_gforce_requisition (VisPluginData *plugin, int *width, int *height)
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

extern "C" int lv_gforce_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	GForcePrivate *priv = (GForcePrivate *) plugin->priv;
	Rect r;
	
	visual_video_set_dimension (video, width, height);

	SetRect (&r, 0, 0, width, height);
	priv->gGF->SetWinPort (0, &r);

	return 0;
}

extern "C" int lv_gforce_events (VisPluginData *plugin, VisEventQueue *events)
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

extern "C" VisPalette *lv_gforce_palette (VisPluginData *plugin)
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

extern "C" int lv_gforce_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
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
	priv->gGF->SetOutVideoBuffer ((unsigned char *) video->pixels);

	time = EgOSUtils::CurTimeMS ();
	priv->gGF->RecordSample (time, gSoundBuf, .000043, NUMSAMPLES, gFFTBuf, 1, FFT_BUF_SIZE);

	/* Overlap with a color tab */

	/* Some color debug to study palettes */
/*
	for (i = 0; i < 256; i++) {
		uint8_t *buf = (uint8_t *) video->pixels;
		for (j = 0; j < 20; j++) {
			buf[(video->pitch * j) + i] = i;
		}

		buf[(video->pitch * ((priv->pal.colors[i].r / 4) + 20)) + i] = 255;
		buf[(video->pitch * ((priv->pal.colors[i].g / 4) + 20 + 64)) + i] = 255;
		buf[(video->pitch * ((priv->pal.colors[i].b / 4) + 20 + 128)) + i] = 255;
	}
*/
	return 0;
}

