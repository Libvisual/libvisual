#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#define NR_BANDS 16

typedef struct {
	VisPalette pal;
	int bands[NR_BANDS];
} AnalyzerPrivate;

static int calc_bands (VisAudio *audio, int band);
static void hline (uint8_t *buf, VisVideo *video, int x1, int x2, int y, int col);

int lv_analyzer_init (VisActorPlugin *plugin);
int lv_analyzer_cleanup (VisActorPlugin *plugin);
int lv_analyzer_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_analyzer_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_analyzer_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_analyzer_palette (VisActorPlugin *plugin);
int lv_analyzer_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *lv_analyzer;
	AnalyzerPrivate *priv;

	plugin = visual_plugin_new ();
	lv_analyzer = visual_plugin_actor_new ();

	lv_analyzer->name = "lv_analyzer";
	lv_analyzer->info = visual_plugin_info_new (
			"libvisual analyzer",
			"Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual analzer plugin",
			"This is a test plugin that'll display a simple analyzer");

	lv_analyzer->init =		lv_analyzer_init;
	lv_analyzer->cleanup =		lv_analyzer_cleanup;
	lv_analyzer->requisition =	lv_analyzer_requisition;
	lv_analyzer->events =		lv_analyzer_events;
	lv_analyzer->palette =		lv_analyzer_palette;
	lv_analyzer->render =		lv_analyzer_render;

	lv_analyzer->depth = VISUAL_VIDEO_DEPTH_8BIT;

	priv = malloc (sizeof (AnalyzerPrivate));
	memset (priv, 0, sizeof (AnalyzerPrivate));

	lv_analyzer->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = lv_analyzer;

	return plugin;
}

int lv_analyzer_init (VisActorPlugin *plugin)
{
	return 0;
}

int lv_analyzer_cleanup (VisActorPlugin *plugin)
{
	AnalyzerPrivate *priv = plugin->private;

	free (priv);

	return 0;
}

int lv_analyzer_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw;

	reqw = *width;

	while (reqw % 2 || (reqw / 2) % 2) {
		reqw--;
	}

	if (reqw < 32)
		reqw = 32;

	*width = reqw;

	return 0;
}

int lv_analyzer_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_analyzer_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_analyzer_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_analyzer_palette (VisActorPlugin *plugin)
{
	AnalyzerPrivate *priv = plugin->private;
	int i;
	
	for (i = 0; i < 256; i++) {
		priv->pal.r[i] = i;
		priv->pal.g[i] = i;
		priv->pal.b[i] = i;
	}

	return &priv->pal;
}

int lv_analyzer_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	AnalyzerPrivate *priv = plugin->private;
	int size;
	int i, j;
	float height;
	uint8_t *buf;

	for (i = 0; i < NR_BANDS; i++)
		priv->bands[i] = calc_bands (audio, i);

	buf = (uint8_t *) video->screenbuffer;

	memset (buf, 0, video->width * video->height);

	size = video->width / NR_BANDS;
	size -= 2;

	for (i = 0; i < NR_BANDS; i++) {

		height = (float) priv->bands[i] / 100.00;
//		printf ("height: %f\n", height);
		
		for (j = video->height - 1; j > video->height / 2; j--) {
			hline (buf, video, size * i, size * (i + 1), j, 100);
		}
	}
	
	for (i = 0; i < NR_BANDS; i++) {
//		printf ("BAND: %d = %d\n", i, priv->bands[i]);
	}
	
	return 0;
}

static int calc_bands (VisAudio *audio, int band)
{
	int i;
	int bandsize = (256 / NR_BANDS);
	int mean = 0;
	
	for (i = bandsize * band; i < bandsize * (band + 1); i++)
		mean += audio->freq[2][i];

	return mean / bandsize;
}

static void hline (uint8_t *buf, VisVideo *video, int x1, int x2, int y, int col)
{
	int i;

	/* FIXME, memcpy me */
	for (i = x1; i < x2; i++)
		buf[(y * video->width) + i] = col;
}

