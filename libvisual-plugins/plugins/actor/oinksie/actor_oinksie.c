#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "oinksie.h"

typedef struct {
	OinksiePrivate priv1;
	OinksiePrivate priv2;

	int depth;
	uint8_t *tbuf1;
	uint8_t *tbuf2;
	uint8_t *buf1;
	uint8_t *buf2;
} OinksiePrivContainer;

static int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);

int act_oinksie_init (VisActorPlugin *plugin);
int act_oinksie_cleanup (VisActorPlugin *plugin);
int act_oinksie_requisition (VisActorPlugin *plugin, int *width, int *height);
int act_oinksie_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int act_oinksie_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *act_oinksie_palette (VisActorPlugin *plugin);
int act_oinksie_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *oinksie;
	OinksiePrivContainer *priv;

	plugin = visual_plugin_new ();
	oinksie = visual_plugin_actor_new ();
	
	oinksie->name = "oinksie";
	oinksie->info = visual_plugin_info_new (
			"oinksie plugin",
			"Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The oinksie visual plugin",
			"This is the libvisual plugin for the oinksie visual");

	oinksie->init =		act_oinksie_init;
	oinksie->cleanup =	act_oinksie_cleanup;
	oinksie->requisition =	act_oinksie_requisition;
	oinksie->events =	act_oinksie_events;
	oinksie->palette =	act_oinksie_palette;
	oinksie->render =	act_oinksie_render;

	oinksie->depth =
		VISUAL_VIDEO_DEPTH_8BIT |
		VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (OinksiePrivContainer));
	memset (priv, 0, sizeof (OinksiePrivContainer));

	oinksie->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = oinksie;
	
	return plugin;
}

int act_oinksie_init (VisActorPlugin *plugin)
{
	OinksiePrivContainer *priv = plugin->priv;
	
	oinksie_init (&priv->priv1, 64, 64);
	oinksie_init (&priv->priv2, 64, 64);
	
	return 0;
}

int act_oinksie_cleanup (VisActorPlugin *plugin)
{
	OinksiePrivContainer *priv = plugin->priv;

	oinksie_quit (&priv->priv1);
	oinksie_quit (&priv->priv2);

	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		free (priv->buf1);
		free (priv->buf2);

		free (priv->tbuf1);
		free (priv->tbuf2);
	}

	free (priv);

	return 0;
}

int act_oinksie_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 2 || (reqw / 2) % 2)
		reqw--;

	while (reqh % 2 || (reqh / 2) % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	printf ("[OINKSIE] we've got a requisition hit YEAH BABY YEAH\n");
	
	return 0;
}

int act_oinksie_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	OinksiePrivContainer *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	oinksie_size_set (&priv->priv1, video->width, video->height);
	oinksie_size_set (&priv->priv2, video->width, video->height);

	printf ("[OINKSIE] we've got a dimension hit YEAH BABY YEAH\n");
	
	priv->depth = video->depth;
	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		if (priv->buf1)
			free (priv->buf1);

		if (priv->buf2)
			free (priv->buf2);

		if (priv->tbuf1);
			free (priv->tbuf1);

		if (priv->tbuf2);
			free (priv->tbuf2);

		priv->buf1 = malloc (video->size);
		memset (priv->buf1, 0, video->size);

		priv->buf2 = malloc (video->size);
		memset (priv->buf2, 0, video->size);

		priv->tbuf1 = malloc (video->size);
		memset (priv->tbuf1, 0, video->size);
		
		priv->tbuf2 = malloc (video->size);
		memset (priv->tbuf2, 0, video->size);
	}

	return 0;
}

int act_oinksie_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {

		printf ("[OINKSIE] event handler is being called\n");
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_oinksie_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_oinksie_palette (VisActorPlugin *plugin)
{
	OinksiePrivContainer *priv = plugin->priv;
	VisPalette *pal;
	
	pal = oinksie_palette_get (&priv->priv1);

	return pal;
}

int act_oinksie_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	OinksiePrivContainer *priv = plugin->priv;
	VisVideo transvid;
	int pitch;

	memcpy (&priv->priv1.audio.freq, &audio->freq, sizeof (short) * 3 * 256);
	memcpy (&priv->priv2.audio.freq, &audio->freq, sizeof (short) * 3 * 256);

	memcpy (&priv->priv1.audio.pcm, &audio->pcm, sizeof (short) * 3 * 512);
	memcpy (&priv->priv2.audio.pcm, &audio->pcm, sizeof (short) * 3 * 512);

	priv->priv1.audio.energy = audio->energy;
	priv->priv2.audio.energy = audio->energy;

	if (priv->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		oinksie_sample (&priv->priv1);

		priv->priv1.drawbuf = video->screenbuffer;
		oinksie_render (&priv->priv1);
	} else {
		oinksie_sample (&priv->priv1);
		oinksie_sample (&priv->priv2);
	
		priv->priv1.drawbuf = priv->buf1;
		priv->priv2.drawbuf = priv->buf2;

		oinksie_render (&priv->priv1);
		oinksie_render (&priv->priv2);

		visual_video_set_depth (&transvid, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_dimension (&transvid, video->width, video->height);
		
		pitch = video->width * video->bpp;

		visual_video_set_buffer (&transvid, priv->buf1);
		visual_video_depth_transform_to_buffer (priv->tbuf1, &transvid,
				oinksie_palette_get (&priv->priv1), priv->depth, video->pitch);

		visual_video_set_buffer (&transvid, priv->buf2);
		visual_video_depth_transform_to_buffer (priv->tbuf2, &transvid,
				oinksie_palette_get (&priv->priv2), priv->depth, video->pitch);

		alpha_blend_32_c (video->screenbuffer, priv->tbuf1, priv->tbuf2, transvid.size * 4, 0.5);
	}
	
	return 0;
}

static int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

