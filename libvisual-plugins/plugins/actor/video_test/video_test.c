#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

typedef struct {
	VisPalette pal;
} VideoTestPrivate;

int video_test_init (VisActorPlugin *plugin);
int video_test_cleanup (VisActorPlugin *plugin);
int video_test_requisition (VisActorPlugin *plugin, int *width, int *height);
int video_test_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int video_test_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *video_test_palette (VisActorPlugin *plugin);
int video_test_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *video_test;
	VideoTestPrivate *priv;

	plugin = visual_plugin_new ();
	video_test = visual_plugin_actor_new ();

	video_test->name = "video_test";
	video_test->info = visual_plugin_info_new (
			"libvisual video test",
			"Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual video test plugin",
			"This is a test plugin to help with testing library internals");

	video_test->init =		video_test_init;
	video_test->cleanup =		video_test_cleanup;
	video_test->requisition =	video_test_requisition;
	video_test->events =		video_test_events;
	video_test->palette =		video_test_palette;
	video_test->render =		video_test_render;

	video_test->depth =
		VISUAL_VIDEO_DEPTH_8BIT  |
		VISUAL_VIDEO_DEPTH_16BIT |
		VISUAL_VIDEO_DEPTH_24BIT |
		VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (VideoTestPrivate));
	memset (priv, 0, sizeof (VideoTestPrivate));

	video_test->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = video_test;

	return plugin;
}

int video_test_init (VisActorPlugin *plugin)
{
	return 0;
}

int video_test_cleanup (VisActorPlugin *plugin)
{
	VideoTestPrivate *priv = plugin->priv;

	free (priv);

	return 0;
}

int video_test_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 2 || (reqw / 2) % 2)
		reqw--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 4)
		reqh = 4;

	*width = reqw;
	*height = reqh;

	return 0;
}

int video_test_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int video_test_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				video_test_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
		}
	}

	return 0;
}

VisPalette *video_test_palette (VisActorPlugin *plugin)
{
	VideoTestPrivate *priv = plugin->priv;
	int i, j = 256;
	
	for (i = 0; i < 256; i++) {
		priv->pal.r[i] = i;
	
		/* Prevent the background from being non black */
		if (i == 0)
			priv->pal.g[i] = 0;
		else
			priv->pal.g[i] = j--;
	
		priv->pal.b[i] = i;
	}

	return &priv->pal;
}

int video_test_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	int i, j;
	int col, col2;
	uint8_t *buf;
	uint16_t *sbuf;
	uint32_t *ibuf;
	
	buf = (uint8_t *) video->screenbuffer;
	sbuf = (uint16_t *) video->screenbuffer;
	ibuf = (uint32_t *) video->screenbuffer;	

	memset (buf, 0, video->width * video->height * video->bpp);
	
	for (i = 0; i < video->width; i += (rand () % 10) + 10) {
		if (i >= video->width - 1)
			return 0;

		col = rand ();
		col2 = rand ();
		
		for (j = 0; j < video->height; j++) {
			if (video->depth == VISUAL_VIDEO_DEPTH_8BIT) {
				buf[(j * video->width) + i] = col;
			} else if (video->depth == VISUAL_VIDEO_DEPTH_16BIT) {
				sbuf[(j * video->width) + i] = col;
			} else if (video->depth == VISUAL_VIDEO_DEPTH_24BIT) {
				buf[(j * video->width * 3) + (i * 3)] = col; 
				buf[(j * video->width * 3) + (i * 3) + 1] = col2;
				buf[(j * video->width * 3) + (i * 3) + 2] = col2;
			} else if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {
				ibuf[(j * video->width) + i] = col;
			}
		}
	}

	return 0;
}

