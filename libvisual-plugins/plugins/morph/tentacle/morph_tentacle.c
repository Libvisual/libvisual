#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
                                                                                                                                               
#include <libvisual/libvisual.h>

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

typedef struct {
	float	move;
} TentaclePrivate; 

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2);

static void vline_from_video_8 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);
static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2);

int lv_morph_tentacle_init (VisPluginData *plugin);
int lv_morph_tentacle_cleanup (VisPluginData *plugin);
int lv_morph_tentacle_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisMorphPlugin morph[] = {{
		.apply = lv_morph_tentacle_apply,
		.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "tentacle",
		.name = "tentacle morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An sine wave morph plugin",
		.help = "This morph plugin morphs between two video sources using some sort of wave that grows in size",

		.init = lv_morph_tentacle_init,
		.cleanup = lv_morph_tentacle_cleanup,

		.plugin = (void *) &morph[0]
	}};

	*count = sizeof (info) / sizeof (*info);
	
	return info;
}

int lv_morph_tentacle_init (VisPluginData *plugin)
{
	TentaclePrivate *priv;

	priv = visual_mem_new0 (TentaclePrivate, 1);
	plugin->priv = priv;

	return 0;
}

int lv_morph_tentacle_cleanup (VisPluginData *plugin)
{
	TentaclePrivate *priv = plugin->priv;

	visual_mem_free (priv);

	return 0;
}

int lv_morph_tentacle_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	TentaclePrivate *priv = plugin->priv;
	uint8_t *destbuf = dest->pixels;
	uint8_t *src1buf = src1->pixels;

	int height1;
	int height2;
	int add1;
	int add2;
	
	float sinrate = priv->move;
	float multiplier = 0;
	float multiadd = 1.000 / dest->width;
	
	int i;
	
	memcpy (destbuf, src1buf, src1->size);

	for (i = 0; i < dest->width; i++) {
		add1 = (dest->height / 2) - ((dest->height / 2) * (rate * 1.5));
		add2 = (dest->height / 2) + ((dest->height / 2) * (rate * 1.5));
		
		height1 = (sin (sinrate) * ((dest->height / 4) * multiplier)) + add1;
		height2 = (sin (sinrate) * ((dest->height / 4) * multiplier)) + add2;
		multiplier += multiadd;
	
		switch (dest->depth) {
			case VISUAL_VIDEO_DEPTH_8BIT:
				vline_from_video_8 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_16BIT:
				vline_from_video_16 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_24BIT:
				vline_from_video_24 (dest, src2, i, height1, height2);
				break;

			case VISUAL_VIDEO_DEPTH_32BIT:
				vline_from_video_32 (dest, src2, i, height1, height2);
				break;
				
			default:
				break;
		}

		sinrate += 0.02;
		priv->move += 0.0002;
	}

	return 0;
}

static void sane_coords (VisVideo *dest, int *x, int *y1, int *y2)
{
	if (*x > dest->width)
		*x = dest->width;
	else if (*x < 0)
		*x = 0;
	
	if (*y1 > dest->height)
		*y1 = dest->height;
	else if (*y1 < 0)
		*y1 = 0;

	if (*y2 > dest->height)
		*y2 = dest->height;
	else if (*y2 < 0)
		*y2 = 0;
}

static void vline_from_video_8 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++)
		destbuf[(i * dest->pitch) + x] = srcbuf[(i * src->pitch) + x];	
}

static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint16_t *destbuf = dest->pixels;
	uint16_t *srcbuf = src->pixels;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++)
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];	
}

static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf = src->pixels;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	x *= 3;
	
	for (i = y1; i < y2; i++) {
		destbuf[(i * dest->pitch) + x] = srcbuf[(i * src->pitch) + x];	
		destbuf[(i * dest->pitch) + x + 1] = srcbuf[(i * src->pitch) + x + 1];	
		destbuf[(i * dest->pitch) + x + 2] = srcbuf[(i * src->pitch) + x + 2];	
	}	
}

static void vline_from_video_32 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint32_t *destbuf = dest->pixels;
	uint32_t *srcbuf = src->pixels;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++) {
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];	
	}
}
