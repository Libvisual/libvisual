#ifndef _LV_MORPH_SLIDE_COMMON_H
#define _LV_MORPH_SLIDE_COMMON_H

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>

typedef enum {
	SLIDE_LEFT,
	SLIDE_RIGHT,
	SLIDE_DOWN,
	SLIDE_UP
} SlideType;

typedef struct {
	SlideType slide_type;
} SlidePrivate;

int  lv_morph_slide_init    (VisPluginData *plugin, SlideType type);
void lv_morph_slide_cleanup (VisPluginData *plugin);
void lv_morph_slide_apply   (VisPluginData *plugin, float progress, VisAudio *audio, VisVideo *dest, VisVideo *src1, VisVideo *src2);

extern VisMorphPlugin morph;

#endif /* _LV_MORPH_SLIDE_COMMON_H */
