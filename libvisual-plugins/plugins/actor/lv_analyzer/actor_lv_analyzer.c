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

int lv_analyzer_init (VisPluginData *plugin);
int lv_analyzer_cleanup (VisPluginData *plugin);
int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height);
int lv_analyzer_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_analyzer_palette (VisPluginData *plugin);
int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_analyzer_requisition,
		.palette = lv_analyzer_palette,
		.render = lv_analyzer_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_analyzer",
		.name = "libvisual analyzer",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The Libvisual analzer plugin",
		.help = "This is a test plugin that'll display a simple analyzer",

		.init = lv_analyzer_init,
		.cleanup = lv_analyzer_cleanup,
		.events = lv_analyzer_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_analyzer_init (VisPluginData *plugin)
{
	AnalyzerPrivate *priv;

	priv = visual_mem_new0 (AnalyzerPrivate, 1);
	plugin->priv = priv;

	visual_palette_allocate_colors (&priv->pal, 256);

	return 0;
}

int lv_analyzer_cleanup (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = plugin->priv;

	visual_palette_free_colors (&priv->pal);

	visual_mem_free (priv);

	return 0;
}

int lv_analyzer_requisition (VisPluginData *plugin, int *width, int *height)
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

int lv_analyzer_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_analyzer_events (VisPluginData *plugin, VisEventQueue *events)
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

VisPalette *lv_analyzer_palette (VisPluginData *plugin)
{
	AnalyzerPrivate *priv = plugin->priv;
	int i;
	
	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = i;
		priv->pal.colors[i].g = i;
		priv->pal.colors[i].b = i;
	}

	return &priv->pal;
}

int lv_analyzer_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	AnalyzerPrivate *priv = plugin->priv;
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

