#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
                                                                                                                                               
#include <libvisual/libvisual.h>

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;
        
static inline int alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha);

/* alpha blenders */
static inline int alpha_blend_8_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);
static inline int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha);

int lv_morph_alpha_init (VisMorphPlugin *plugin);
int lv_morph_alpha_cleanup (VisMorphPlugin *plugin);
int lv_morph_alpha_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisMorphPlugin *morph;

	plugin = visual_plugin_new ();
	if (plugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"Could not create a new plugin");
		return NULL;
	}

	morph = visual_plugin_morph_new ();
	if (morph == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"Could not create a new morph plugin");
		return NULL;
	}

	morph->name = "alphablend";
	morph->info = visual_plugin_info_new ("alphablend morph", "Dennis Smit <ds@nerds-incorporated.org>", "0.1",
			"An alphablend morph plugin", "This morph plugin morphs between two video sources using the alphablend method");

	morph->init =		lv_morph_alpha_init;
	morph->cleanup =	lv_morph_alpha_cleanup;
	morph->apply =		lv_morph_alpha_apply;

	morph->depth =
		VISUAL_VIDEO_DEPTH_8BIT  |
		VISUAL_VIDEO_DEPTH_16BIT |
		VISUAL_VIDEO_DEPTH_24BIT |
		VISUAL_VIDEO_DEPTH_32BIT;

	plugin->type = VISUAL_PLUGIN_TYPE_MORPH;
	plugin->plugin.morphplugin = morph;

	return plugin;
}

int lv_morph_alpha_init (VisMorphPlugin *plugin)
{
	return 0;
}

int lv_morph_alpha_cleanup (VisMorphPlugin *plugin)
{
	return 0;
}

int lv_morph_alpha_apply (VisMorphPlugin *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2)
{
	visual_log_return_val_if_fail (dest != NULL, -1);
	visual_log_return_val_if_fail (src1 != NULL, -1);
	visual_log_return_val_if_fail (src2 != NULL, -1);

	alpha_blend_buffer (dest->screenbuffer, src1->screenbuffer, src2->screenbuffer, dest->size, dest->depth, rate);

	return 0;
}

static inline int alpha_blend_buffer (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, int depth, float alpha)
{
	if (depth == VISUAL_VIDEO_DEPTH_8BIT)
		return alpha_blend_8_c (dest, src1, src2, size, alpha);

	if (depth == VISUAL_VIDEO_DEPTH_16BIT)
		return alpha_blend_16_c (dest, src1, src2, size, alpha);

	if (depth == VISUAL_VIDEO_DEPTH_24BIT)
		return alpha_blend_24_c (dest, src1, src2, size, alpha);

	if (depth == VISUAL_VIDEO_DEPTH_32BIT)
		return alpha_blend_32_c (dest, src1, src2, size, alpha);

	return -1;
}

/* FIXME TODO blends:   c       sse     mmx
 * 8                    x
 * 16                   x
 * 24                   x
 * 32                   x
 */

static inline int alpha_blend_8_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_16_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	_color16 *destr = (_color16 *) dest;
	_color16 *src1r = (_color16 *) src1;
	_color16 *src2r = (_color16 *) src2;
	int i;

	for (i = 0; i < size / 2; i++) {
		destr[i].r = ((ialpha * (src2r[i].r - src1r[i].r)) / 255 + src1r[i].r);
		destr[i].g = ((ialpha * (src2r[i].g - src1r[i].g)) / 255 + src1r[i].g);
		destr[i].b = ((ialpha * (src2r[i].b - src1r[i].b)) / 255 + src1r[i].b);
	}

	return 0;
}

static inline int alpha_blend_24_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

static inline int alpha_blend_32_c (uint8_t *dest, uint8_t *src1, uint8_t *src2, int size, float alpha)
{
	uint8_t ialpha = (alpha * 255);
	int i;

	for (i = 0; i < size; i++) {
		dest[i] = (ialpha * (src2[i] - src1[i])) / 255 + src1[i];
	}

	return 0;
}

