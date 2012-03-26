#ifndef _LV_ALPHA_BLEND_H
#define _LV_ALPHA_BLEND_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

VISUAL_BEGIN_DECLS

typedef void (*VisAlphaBlendFunc) (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);

extern VisAlphaBlendFunc visual_alpha_blend_8;
extern VisAlphaBlendFunc visual_alpha_blend_16;
extern VisAlphaBlendFunc visual_alpha_blend_24;
extern VisAlphaBlendFunc visual_alpha_blend_32;

void visual_alpha_blend_initialize (void);

VISUAL_END_DECLS

#endif /* _LV_ALPHA_BLEND_H */
