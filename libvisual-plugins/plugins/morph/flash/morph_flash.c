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
	VisPalette whitepal;
	uint8_t replacetable[256];
} FlashPrivate;

static void replacetable_generate_24 (FlashPrivate *priv, float rate);
static void flash_8 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2);
static void flash_24 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2);

int lv_morph_flash_init (VisPluginData *plugin);
int lv_morph_flash_cleanup (VisPluginData *plugin);
int lv_morph_flash_palette (VisPluginData *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2);
int lv_morph_flash_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisMorphPlugin morph[] = {{
		.palette = lv_morph_flash_palette,
		.apply = lv_morph_flash_apply,
		.depth =
			VISUAL_VIDEO_DEPTH_8BIT  |
			VISUAL_VIDEO_DEPTH_16BIT |
			VISUAL_VIDEO_DEPTH_24BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "flash",
		.name = "flash morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "An flash in and out morph plugin",
		.help = "This morph plugin morphs between two video sources using a bright flash",

		.init = lv_morph_flash_init,
		.cleanup = lv_morph_flash_cleanup,
		
		.plugin = VISUAL_OBJECT (&morph[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_morph_flash_init (VisPluginData *plugin)
{
	int i;
	FlashPrivate *priv;

	priv = visual_mem_new0 (FlashPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->whitepal, 256);

	for (i = 0; i < 256; i++) {
		priv->whitepal.colors[i].r = 0xff;
		priv->whitepal.colors[i].g = 0xff;
		priv->whitepal.colors[i].b = 0xff;
	}
	
	return 0;
}

int lv_morph_flash_cleanup (VisPluginData *plugin)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_morph_flash_palette (VisPluginData *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (rate < 0.5)
		visual_palette_blend (pal, src1->pal, &priv->whitepal, rate * 2);
	else
		visual_palette_blend (pal, &priv->whitepal, src2->pal, (rate - 0.5) * 2);
	
	return 0;
}

int lv_morph_flash_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	switch (dest->depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			flash_8 (priv, rate, dest, src1, src2);	
			break;

		case VISUAL_VIDEO_DEPTH_16BIT:
			
			break;

		case VISUAL_VIDEO_DEPTH_24BIT:
			replacetable_generate_24 (priv, rate);
			flash_24 (priv, rate, dest, src1, src2);
			break;

		case VISUAL_VIDEO_DEPTH_32BIT:
			replacetable_generate_24 (priv, rate);
			flash_24 (priv, rate, dest, src1, src2);
			break;

		default:
			break;
	}

	return 0;
}

static void replacetable_generate_24 (FlashPrivate *priv, float rate)
{
	int i;

	for (i = 0; i < 256; i++) {
		if (rate < 0.5)
			priv->replacetable[i] = i + (((255.00 - i) / 100.00) * ((rate * 2) * 100));
		else
			priv->replacetable[i] = i + (((255.00 - i) / 100.00) * ((1.0 - ((rate - 0.5) * 2)) * 100));
	}

}

static void flash_8 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	if (rate < 0.5)
		memcpy (dest->pixels, src1->pixels, src1->size);
	else
		memcpy (dest->pixels, src2->pixels, src2->size);
}

static void flash_24 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	uint8_t *scrbuf;
	uint8_t *destbuf = dest->pixels;
	int size;
	int i;

	if (rate < 0.5) {
		scrbuf = src1->pixels;
		size = src1->size;
	} else {
		scrbuf = src2->pixels;
		size = src2->size;
	}
	
	for (i = 0; i < size; i++)
		destbuf[i] = priv->replacetable[scrbuf[i]];
}

