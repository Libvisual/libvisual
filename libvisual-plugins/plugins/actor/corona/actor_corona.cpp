#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

extern "C" const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[1];
	static VisPluginInfo info[1];
			
	actor[0].requisition = lv_corona_requisition;
	actor[0].palette = lv_corona_palette;
	actor[0].render  = lv_corona_render;
	actor[0].depth   = VISUAL_VIDEO_DEPTH_8BIT;

	info[0].struct_size = sizeof (VisPluginInfo);
	info[0].api_version = VISUAL_PLUGIN_API_VERSION;
	info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;

	info[0].plugname = "corona";
	info[0].name     = "libvisual corona plugin";
	info[0].author   = "Jean-Christophe Hoelt <jeko@ios-software.com> and Richard Ashburn <richard.asbury@btinternet.com>";
	info[0].version  = "0.1";
	info[0].about    = "The Libvisual corona plugin";
	info[0].help     = "This plugin adds support for the neat corona plugin";

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

	priv = new CoronaPrivate;
	memset (priv, 0, sizeof (CoronaPrivate));

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

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

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
				lv_corona_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);

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
	VisTime curtime;
	VisTime difftime;
	short freqdata[2][512];
	unsigned long timemilli = 0;
	int i;
	
	for (i = 0; i < 256; ++i) {
		freqdata[0][i*2]   = audio->freqnorm[0][i];
		freqdata[1][i*2]   = audio->freqnorm[1][i];
		freqdata[0][i*2+1] = audio->freqnorm[0][i];
		freqdata[1][i*2+1] = audio->freqnorm[1][i];
	}

	visual_time_get (&curtime);
	
	visual_time_difference (&difftime, &priv->oldtime, &curtime);

	timemilli = difftime.tv_sec * 1000 + difftime.tv_usec / 1000;

	priv->tl.timeStamp += timemilli;

	visual_time_copy (&priv->oldtime, &curtime);
	
	for (i = 0; i < 512; ++i) {
		priv->tl.frequency[0][i] = freqdata[0][i];
		priv->tl.frequency[1][i] = freqdata[1][i];
	}
	
	priv->corona->update(&priv->tl); // Update Corona
	priv->pcyl->update(&priv->tl);    // Update Palette Cycler

	for (i = 0; i < video->height; i++) {
		memcpy ((uint8_t *)(video->pixels) + i * video->pitch,
				priv->corona->getSurface() + (video->height - 1 - i) * video->width,
				video->width);
	}
	
	return 0;
}

