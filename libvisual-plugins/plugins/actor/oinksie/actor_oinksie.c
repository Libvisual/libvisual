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

int act_oinksie_init (VisPluginData *plugin);
int act_oinksie_cleanup (VisPluginData *plugin);
int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height);
int act_oinksie_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_oinksie_palette (VisPluginData *plugin);
int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_oinksie_requisition,
		.palette = act_oinksie_palette,
		.render = act_oinksie_render,
		.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "oinksie",
		.name = "oinksie plugin",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The oinksie visual plugin",
		.help = "This is the libvisual plugin for the oinksie visual",

		.init = act_oinksie_init,
		.cleanup = act_oinksie_cleanup,
		.events = act_oinksie_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_oinksie_init (VisPluginData *plugin)
{
	OinksiePrivContainer *priv;

	priv = visual_mem_new0 (OinksiePrivContainer, 1);
	plugin->priv = priv;

	visual_palette_allocate_colors (&priv->priv1.pal_cur, 256);
	visual_palette_allocate_colors (&priv->priv1.pal_old, 256);

	visual_palette_allocate_colors (&priv->priv2.pal_cur, 256);
	visual_palette_allocate_colors (&priv->priv2.pal_old, 256);

	oinksie_init (&priv->priv1, 64, 64);
	oinksie_init (&priv->priv2, 64, 64);
	
	return 0;
}

int act_oinksie_cleanup (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = plugin->priv;

	oinksie_quit (&priv->priv1);
	oinksie_quit (&priv->priv2);

	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		visual_mem_free (priv->buf1);
		visual_mem_free (priv->buf2);

		visual_mem_free (priv->tbuf1);
		visual_mem_free (priv->tbuf2);
	}

	visual_palette_free_colors (&priv->priv1.pal_cur);
	visual_palette_free_colors (&priv->priv1.pal_old);

	visual_palette_free_colors (&priv->priv2.pal_cur);
	visual_palette_free_colors (&priv->priv2.pal_old);

	visual_mem_free (priv);

	return 0;
}

int act_oinksie_requisition (VisPluginData *plugin, int *width, int *height)
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

	return 0;
}

int act_oinksie_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	OinksiePrivContainer *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	oinksie_size_set (&priv->priv1, video->width, video->height);
	oinksie_size_set (&priv->priv2, video->width, video->height);

	priv->depth = video->depth;
	if (priv->depth != VISUAL_VIDEO_DEPTH_8BIT) {
		if (priv->buf1)
			visual_mem_free (priv->buf1);

		if (priv->buf2)
			visual_mem_free (priv->buf2);

		if (priv->tbuf1);
			visual_mem_free (priv->tbuf1);

		if (priv->tbuf2);
			visual_mem_free (priv->tbuf2);

		priv->buf1 = visual_mem_malloc0 (video->size);
		priv->buf2 = visual_mem_malloc0 (video->size);
		priv->tbuf1 = visual_mem_malloc0 (video->size);
		priv->tbuf2 = visual_mem_malloc0 (video->size);
	}

	return 0;
}

int act_oinksie_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
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

VisPalette *act_oinksie_palette (VisPluginData *plugin)
{
	OinksiePrivContainer *priv = plugin->priv;
	VisPalette *pal;
	
	pal = oinksie_palette_get (&priv->priv1);

	return pal;
}

int act_oinksie_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
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

