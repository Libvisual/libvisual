#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
                                                                                                                                               
#include <libvisual/libvisual.h>

typedef enum {
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_BOTTOM,
	SLIDE_UPPER
} SlideType;

typedef struct {
	SlideType	slide_type;
} SlidePrivate; 

int lv_morph_slide_init_left (VisPluginData *plugin);
int lv_morph_slide_init_right (VisPluginData *plugin);
int lv_morph_slide_init_bottom (VisPluginData *plugin);
int lv_morph_slide_init_upper (VisPluginData *plugin);
int lv_morph_slide_cleanup (VisPluginData *plugin);
int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisMorphPlugin morph[] = {{
		.apply = lv_morph_slide_apply,
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

		.plugname = "slide_left",
		.name = "Slide left morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",

		.init = lv_morph_slide_init_left,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_right",
		.name = "Slide right morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",

		.init = lv_morph_slide_init_right,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_bottom",
		.name = "Slide bottom morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",

		.init = lv_morph_slide_init_bottom,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	},
	{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_MORPH,

		.plugname = "slide_upper",
		.name = "Slide upper morph",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "A slide in/out morph plugin",
		.help = "This morph plugin morphs between two video sources by sliding one in and the other out",

		.init = lv_morph_slide_init_upper,
		.cleanup = lv_morph_slide_cleanup,

		.plugin = VISUAL_OBJECT (&morph[0])
	}};

	*count = sizeof (info) / sizeof (*info);
	
	return info;
}

int lv_morph_slide_init_left (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_LEFT;
	
	return 0;
}

int lv_morph_slide_init_right (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_RIGHT;
	
	return 0;
}

int lv_morph_slide_init_bottom (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_BOTTOM;
	
	return 0;
}

int lv_morph_slide_init_upper (VisPluginData *plugin)
{
	SlidePrivate *priv;

	priv = visual_mem_new0 (SlidePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->slide_type = SLIDE_UPPER;
	
	return 0;
}

int lv_morph_slide_cleanup (VisPluginData *plugin)
{
	SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	SlidePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
        uint8_t *destbuf = dest->pixels;
	uint8_t *srcbuf1 = src1->pixels;
	uint8_t *srcbuf2 = src2->pixels;
	int i;
	int diff1;
	int diff2;
	int hadd;

	memset (destbuf, 0, dest->size);
	
	if (priv->slide_type == SLIDE_RIGHT || priv->slide_type == SLIDE_UPPER)
		rate = 1.0 - rate;
	
	diff1 = dest->pitch * rate;
	diff1 -= diff1 % dest->bpp;

	if (diff1 > dest->pitch)
		diff1 = dest->pitch;

	diff2 = dest->pitch - diff1;

	hadd = dest->height * rate;
	
	switch (priv->slide_type) {
		case SLIDE_LEFT:
			for (i = 0; i < dest->height; i++) {
				memcpy (destbuf + (i * dest->pitch), srcbuf2 + (i * dest->pitch) + diff2, diff1);
				memcpy (destbuf + (i * dest->pitch) + (diff1), srcbuf1 + (i * dest->pitch), diff2);
			}

			break;

		case SLIDE_RIGHT:
			for (i = 0; i < dest->height; i++) {
				memcpy (destbuf + (i * dest->pitch), srcbuf1 + (i * dest->pitch) + diff2, diff1);
				memcpy (destbuf + (i * dest->pitch) + (diff1), srcbuf2 + (i * dest->pitch), diff2);
			}
			
			break;

		case SLIDE_BOTTOM:
			memcpy (destbuf, srcbuf1 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
			memcpy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf2, hadd * dest->pitch);

			break;

		case SLIDE_UPPER:
			memcpy (destbuf, srcbuf2 + (hadd * dest->pitch), (dest->height - hadd) * dest->pitch);
			memcpy (destbuf + ((dest->height - hadd) * dest->pitch), srcbuf1, hadd * dest->pitch);

			break;

		default:
			visual_log (VISUAL_LOG_WARNING,
					"Slide plugin is initialized with an impossible slide direction, this should never happen");

			break;
	}
	
	return 0;
}

