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

int lv_morph_flash_init (VisMorphPlugin *plugin);
int lv_morph_flash_cleanup (VisMorphPlugin *plugin);
int lv_morph_flash_palette (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2);
int lv_morph_flash_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisMorphPlugin *morph;
	FlashPrivate *priv;
	
	plugin = visual_plugin_new ();
	morph = visual_plugin_morph_new ();

	morph->name = "flash";
	morph->info = visual_plugin_info_new ("flash morph", "Dennis Smit <ds@nerds-incorporated.org>", "0.1",
			"An flash in and out morph plugin", "This morph plugin morphs between two video sources using a bright flash");

	morph->init =		lv_morph_flash_init;
	morph->cleanup =	lv_morph_flash_cleanup;
	morph->palette =	lv_morph_flash_palette;
	morph->apply =		lv_morph_flash_apply;

	morph->depth =
		VISUAL_VIDEO_DEPTH_8BIT  |
		VISUAL_VIDEO_DEPTH_16BIT |
		VISUAL_VIDEO_DEPTH_24BIT |
		VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (FlashPrivate));
	memset (priv, 0, sizeof (FlashPrivate));

	morph->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_MORPH;
	plugin->plugin.morphplugin = morph;

	return plugin;
}

int lv_morph_flash_init (VisMorphPlugin *plugin)
{
	FlashPrivate *priv = plugin->private;

	memset (&priv->whitepal, 0xff, sizeof (VisPalette));
	
	return 0;
}

int lv_morph_flash_cleanup (VisMorphPlugin *plugin)
{
	FlashPrivate *priv = plugin->private;

	free (priv);

	return 0;
}

int lv_morph_flash_palette (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisPalette *pal, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = plugin->private;

	if (rate < 0.5)
		visual_palette_blend (pal, src1->pal, &priv->whitepal, rate * 2);
	else
		visual_palette_blend (pal, &priv->whitepal, src2->pal, (rate - 0.5) * 2);

	return 0;
}

int lv_morph_flash_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	FlashPrivate *priv = plugin->private;

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
		memcpy (dest->screenbuffer, src1->screenbuffer, src1->size);
	else
		memcpy (dest->screenbuffer, src2->screenbuffer, src2->size);
}

static void flash_32 (FlashPrivate *priv, float rate, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	uint8_t *scrbuf;
	uint8_t *destbuf = dest->screenbuffer;
	int size;
	int i;

	if (rate < 0.5) {
		scrbuf = src1->screenbuffer;
		size = src1->size;
	} else {
		scrbuf = src2->screenbuffer;
		size = src2->size;
	}
	
	for (i = 0; i < size; i++)
		destbuf[i] = priv->replacetable[scrbuf[i]];
}

