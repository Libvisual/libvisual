#include <stdio.h>
#include <stdlib.h>
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

int lv_morph_tentacle_init (VisMorphPlugin *plugin);
int lv_morph_tentacle_cleanup (VisMorphPlugin *plugin);
int lv_morph_tentacle_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisMorphPlugin *morph;
	TentaclePrivate *priv;
	
	plugin = visual_plugin_new ();
	morph = visual_plugin_morph_new ();

	morph->name = "tentacle";
	morph->info = visual_plugin_info_new ("tentacle morph", "Dennis Smit <ds@nerds-incorporated.org>", "0.1",
			"An sine wave morph plugin", "This morph plugin morphs between two video sources using some sort of wave that grows in size");

	morph->init =		lv_morph_tentacle_init;
	morph->cleanup =	lv_morph_tentacle_cleanup;
	morph->apply =		lv_morph_tentacle_apply;

	morph->depth =
		VISUAL_VIDEO_DEPTH_8BIT |
		VISUAL_VIDEO_DEPTH_16BIT |
		VISUAL_VIDEO_DEPTH_24BIT |
		VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (TentaclePrivate));
	memset (priv, 0, sizeof (TentaclePrivate));

	morph->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_MORPH;
	plugin->plugin.morphplugin = morph;

	return plugin;
}

int lv_morph_tentacle_init (VisMorphPlugin *plugin)
{
	return 0;
}

int lv_morph_tentacle_cleanup (VisMorphPlugin *plugin)
{
	TentaclePrivate *priv = plugin->private;

	free (priv);

	return 0;
}

int lv_morph_tentacle_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	TentaclePrivate *priv = plugin->private;
	uint8_t *destbuf = dest->screenbuffer;
	uint8_t *src1buf = src1->screenbuffer;
	uint8_t *src2buf = src2->screenbuffer;

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
	uint8_t *destbuf = dest->screenbuffer;
	uint8_t *srcbuf = src->screenbuffer;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++)
		destbuf[(i * dest->pitch) + x] = srcbuf[(i * src->pitch) + x];	
}

static void vline_from_video_16 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint16_t *destbuf = dest->screenbuffer;
	uint16_t *srcbuf = src->screenbuffer;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++)
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];	
}

static void vline_from_video_24 (VisVideo *dest, VisVideo *src, int x, int y1, int y2)
{
	uint8_t *destbuf = dest->screenbuffer;
	uint8_t *srcbuf = src->screenbuffer;
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
	uint32_t *destbuf = dest->screenbuffer;
	uint32_t *srcbuf = src->screenbuffer;
	int i;

	sane_coords (dest, &x, &y1, &y2);
	
	for (i = y1; i < y2; i++) {
		destbuf[(i * (dest->pitch / dest->bpp)) + x] = srcbuf[(i * (src->pitch / src->bpp)) + x];	
	}
}
