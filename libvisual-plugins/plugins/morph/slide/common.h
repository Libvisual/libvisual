#ifndef _LV_MORPH_SLIDE_COMMON_H
#define _LV_MORPH_SLIDE_COMMON_H

#include <libvisual/libvisual.h>

typedef enum {
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_DOWN,
	SLIDE_UP
} SlideType;

typedef struct {
	SlideType	slide_type;
} SlidePrivate;

int lv_morph_slide_init (VisPluginData *plugin, SlideType type);

int lv_morph_slide_cleanup (VisPluginData *plugin);

int lv_morph_slide_apply (VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

static VisMorphPlugin morph = {
	.apply = lv_morph_slide_apply,
	.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	                  | VISUAL_VIDEO_DEPTH_16BIT
	                  | VISUAL_VIDEO_DEPTH_24BIT
	                  | VISUAL_VIDEO_DEPTH_32BIT
};

#endif /* _LV_MORPH_SLIDE_COMMON_H */

